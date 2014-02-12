/// @file    core/builtin/Catalog.cpp
/// @brief   Implementation of a Catalog using other plugins, as INode.
/// @details Intended to ease the development of database backends.
/// @author  Alejandro Álvarez Ayllon <aalvarez@cern.ch>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/utils/security.h>
#include <dmlite/cpp/utils/urls.h>
#include <vector>

#include "Catalog.h"

using namespace dmlite;



inline uid_t getUid(const SecurityContext* ctx)
{
  return ctx->user.getUnsigned("uid");
}



inline gid_t getGid(const SecurityContext* ctx, unsigned index)
{
  return ctx->groups[index].getUnsigned("gid");
}



inline bool hasGroup(const SecurityContext* ctx, gid_t gid)
{
  for (unsigned i = 0; i < ctx->groups.size(); ++i)
    if (getGid(ctx, i) == gid)
      return true;
  return false;
}



BuiltInCatalogFactory::BuiltInCatalogFactory():
  updateATime_(true), symLinkLimit_(3)
{
  // Nothing
}



BuiltInCatalogFactory::~BuiltInCatalogFactory()
{
  
}
  


void BuiltInCatalogFactory::configure(const std::string& key, const std::string& value) throw (DmException)
{
  if (key == "SymLinkLimit")
    this->symLinkLimit_ = atoi(value.c_str());
  else if (key == "UpdateAccessTime") {
    std::string lower;
    std::transform(value.begin(), value.end(), lower.begin(), tolower);
    this->updateATime_ = (value == "yes");
  }
  else
    throw DmException(DM_UNKNOWN_OPTION, std::string("Unknown option ") + key);
}



Catalog* BuiltInCatalogFactory::createCatalog(PluginManager*) throw (DmException)
{
  return new BuiltInCatalog(this->updateATime_, this->symLinkLimit_);
}



BuiltInCatalog::BuiltInCatalog(bool updateATime, unsigned symLinkLimit) throw (DmException):
    si_(NULL), secCtx_(), cwd_(0), umask_(022),
    updateATime_(updateATime), symLinkLimit_(symLinkLimit)
{
  // Nothing
}



BuiltInCatalog::~BuiltInCatalog()
{
  // Nothing
}



std::string BuiltInCatalog::getImplId(void) const throw()
{
  return std::string("BuiltInCatalog");
}



void BuiltInCatalog::setStackInstance(StackInstance* si) throw (DmException)
{
  this->si_ = si;
}



void BuiltInCatalog::setSecurityContext(const SecurityContext* ctx) throw (DmException)
{
  this->secCtx_ = ctx;
}



void BuiltInCatalog::changeDir(const std::string& path) throw (DmException)
{
  if (path.empty()) {
    this->cwdPath_.clear();
    this->cwd_ = 0;
    return;
  }
  
  ExtendedStat cwd = this->extendedStat(path);
  this->cwd_       = cwd.stat.st_ino;
  if (path[0] == '/')
    this->cwdPath_ = path;
  else
    this->cwdPath_ = Url::normalizePath(this->cwdPath_ + "/" + path);
}



std::string BuiltInCatalog::getWorkingDir (void) throw (DmException)
{
  return this->cwdPath_;
}



ExtendedStat BuiltInCatalog::extendedStat(const std::string& path, bool followSym) throw (DmException)
{
 // Split the path always assuming absolute
  std::vector<std::string> components = Url::splitPath(path);

  // Iterate starting from absolute root (parent of /) (0)
  uint64_t     parent       = 0;
  unsigned     symLinkLevel = 0;
  ExtendedStat meta;
  std::string  c;

  // If path is absolute OR cwd is empty, start in root
  if (path[0] == '/' || this->cwdPath_.empty()) {
    // Root parent "is" a dir and world-readable :)
    memset(&meta.stat, 0, sizeof(meta.stat));
    meta.stat.st_mode = S_IFDIR | 0555 ;
  }
  // Relative, and cwd set, so start there
  else {
    parent = this->cwd_;
    meta   = this->si_->getINode()->extendedStat(parent);
  }
  

  for (unsigned i = 0; i < components.size(); ) {
    // Check that the parent is a directory first
    if (!S_ISDIR(meta.stat.st_mode) && !S_ISLNK(meta.stat.st_mode))
      throw DmException(DM_NOT_DIRECTORY,
                        "%s is not a directory", meta.name.c_str());
    // New element traversed! Need to check if it is possible to keep going.
    if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IEXEC) != 0)
      throw DmException(DM_FORBIDDEN,
                        "Not enough permissions to list %s", meta.name.c_str());

    // Pop next component
    c = components[i];

    // Stay here
    if (c == ".") {
      // Nothing
    }
    // Up one level
    else if (c == "..") {
      parent = meta.parent;
      meta   = this->si_->getINode()->extendedStat(parent);
    }
    // Regular entry
    else {
      meta = this->si_->getINode()->extendedStat(parent, c);

      // Symbolic link!, follow that instead
      if (S_ISLNK(meta.stat.st_mode) && followSym) {
        SymLink link = this->si_->getINode()->readLink(meta.stat.st_ino);

        ++symLinkLevel;
        if (symLinkLevel > this->symLinkLimit_) {
          throw DmException(DM_TOO_MANY_SYMLINKS,
                           "Symbolic links limit exceeded: > %d",
                           this->symLinkLimit_);
        }

        // We have the symbolic link now. Split it and push
        // into the component
        std::vector<std::string> symPath = Url::splitPath(link.link);
        
        for (unsigned j = i + 1; j < components.size(); ++j)
          symPath.push_back(components[j]);
        
        components.swap(symPath);
        i = 0;
        
        // If absolute, need to reset parent
        if (link.link[0] == '/')
          parent = 0;
        
        continue; // Jump directly to the beginning of the loop
      }
      // Next one!
      else {
        parent = meta.stat.st_ino;
      }
    }
    ++i; // Next in array
  }

  return meta;
}



void BuiltInCatalog::addReplica(const Replica& replica) throw (DmException)
{
  ExtendedStat meta = this->si_->getINode()->extendedStat(replica.fileid);
   
  this->traverseBackwards(meta);
  if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Can not modify the file %d", replica.fileid);
  
  this->si_->getINode()->addReplica(replica);
}



void BuiltInCatalog::deleteReplica(const Replica& replica) throw (DmException)
{
  ExtendedStat meta = this->si_->getINode()->extendedStat(replica.fileid);
    
  this->traverseBackwards(meta);
  if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Can not modify the file %d", replica.fileid);
  
  this->si_->getINode()->deleteReplica(replica);
}



std::vector<Replica> BuiltInCatalog::getReplicas(const std::string& path) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path);
  
  // The file exists, plus we have permissions to go there. Check we can read
  if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IREAD) != 0)
    throw DmException(DM_FORBIDDEN,
                   "Not enough permissions to read " + path);

  this->updateAccessTime(meta);
  return this->si_->getINode()->getReplicas(meta.stat.st_ino);
}



void BuiltInCatalog::symlink(const std::string& oldPath, const std::string& newPath) throw (DmException)
{
  std::string parentPath, symName;
  
  // Get the parent of the destination and file
  ExtendedStat parent = this->getParent(newPath, &parentPath, &symName);

  // Check we have write access for the parent
  if (checkPermissions(this->secCtx_, parent.acl, parent.stat, S_IWRITE | S_IEXEC) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions on " + parentPath);

  // Effective gid
  gid_t  egid;
  mode_t mode = 0777;
  
  if (parent.stat.st_mode & S_ISGID) {
    egid = parent.stat.st_gid;
    mode |= S_ISGID;
  }
  else {
    egid = getGid(this->secCtx_, 0);
  }
  
  this->si_->getINode()->begin();
  
  try {
    // Create file
    ExtendedStat newLink;
    
    newLink.parent = parent.stat.st_ino;
    newLink.name   = symName;
    newLink.stat.st_mode = mode | S_IFLNK;
    newLink.stat.st_size = 0;
    newLink.status       = ExtendedStat::kOnline;
    newLink.stat.st_uid  = getUid(this->secCtx_);
    newLink.stat.st_gid  = egid;
    
    ExtendedStat linkMeta = this->si_->getINode()->
                              create(newLink);
    // Create symlink
    this->si_->getINode()->symlink(linkMeta.stat.st_ino, oldPath);
  }
  catch (...) {
    this->si_->getINode()->rollback();
    throw;
  }
  
  this->si_->getINode()->commit();
}



std::string BuiltInCatalog::readLink(const std::string& path) throw (DmException)
{
  ExtendedStat xs = this->extendedStat(path, false);
  if (!S_ISLNK(xs.stat.st_mode))
    throw DmException(DM_INVALID_VALUE,
                      "%s is not a symbolic link", path.c_str());
  return this->si_->getINode()->readLink(xs.stat.st_ino).link;
}



void BuiltInCatalog::unlink(const std::string& path) throw (DmException)
{
  std::string  parentPath, name;

  // Get the parent
  ExtendedStat parent = this->getParent(path, &parentPath, &name);

  // Check we have exec access for the parent
  if (checkPermissions(this->secCtx_, parent.acl, parent.stat, S_IEXEC) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions to list " + parentPath);

  // The file itself
  ExtendedStat file = this->si_->getINode()->extendedStat(parent.stat.st_ino, name);

  // Directories can not be removed with this method!
  if (S_ISDIR(file.stat.st_mode))
    throw DmException(DM_IS_DIRECTORY, path + " is a directory, can not unlink");

  // Check we can remove it
  if ((parent.stat.st_mode & S_ISVTX) == S_ISVTX) {
    // Sticky bit set
    if (getUid(this->secCtx_) != file.stat.st_uid &&
        getUid(this->secCtx_) != parent.stat.st_uid &&
        checkPermissions(this->secCtx_, file.acl, file.stat, S_IWRITE) != 0)
      throw DmException(DM_FORBIDDEN, "Not enough permissions to unlink " +
                                      path + "( sticky bit set)");
  }
  else {
    // No sticky bit
    if (checkPermissions(this->secCtx_, parent.acl, parent.stat, S_IWRITE) != 0)
      throw DmException(DM_FORBIDDEN, "Not enough permissions to unlink " + path);
  }

  // Check there are no replicas
  if (!S_ISLNK(file.stat.st_mode)) {
    std::vector<Replica> replicas = this->si_->getINode()->getReplicas(file.stat.st_ino);
    
    // Pure catalogs must not remove files with replicas    
    if (!this->si_->isTherePoolManager() && replicas.size() != 0)
      throw DmException(DM_EXISTS, path + " has replicas, can not remove");
    
    // Try to remove replicas first
    for (unsigned i = 0; i < replicas.size(); ++i) {
      Pool pool   = this->si_->getPoolManager()->
                      getPool(replicas[i].getString("pool"));
      
      PoolHandler* handler = this->si_->getPoolDriver(pool.type)->
                               createPoolHandler(pool.name);
      
      handler->removeReplica(replicas[i]);
      
      delete handler;
    }
  }
  
  // All preconditions are good, so remove
  try {
    this->si_->getINode()->unlink(file.stat.st_ino);
  }
  catch (DmException& e) {
    if (e.code() != DM_NO_SUCH_FILE)
      throw;
    // If not found, that's good, as the pool driver probably
    // did it (i.e. legacy DPM)
  }
}



void BuiltInCatalog::create(const std::string& path, mode_t mode) throw (DmException)
{
  int          code;
  std::string  parentPath, name;
  ExtendedStat parent = this->getParent(path, &parentPath, &name);
  ExtendedStat file;
  
  // Need to be able to write to the parent
  if (checkPermissions(this->secCtx_, parent.acl, parent.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Need write access on %s", parentPath.c_str());

  // Check that the file does not exist, or it has no replicas
  try {
    file = this->si_->getINode()->extendedStat(parent.stat.st_ino, name);
    if (this->si_->getINode()->getReplicas(file.stat.st_ino).size() > 0)
      throw DmException(DM_EXISTS, path + " exists and has replicas. Can not truncate.");
  }
  catch (DmException& e) {
    code = e.code();
    if (code != DM_NO_SUCH_FILE)
      throw;
  }
  
  // Effective gid
  gid_t egid;
  if (parent.stat.st_mode & S_ISGID) {
    egid = parent.stat.st_gid;
    mode |= S_ISGID;
  }
  else {
    egid = getGid(this->secCtx_, 0);
  }

  // Create new
  if (code == DM_NO_SUCH_FILE) {
    ExtendedStat newFile;
    newFile.parent       = parent.stat.st_ino;
    newFile.name         = name;
    newFile.stat.st_mode = ((mode & ~S_IFMT) & ~this->umask_) | S_IFREG;
    newFile.stat.st_size = 0;
    newFile.stat.st_uid  = getUid(this->secCtx_);
    newFile.stat.st_gid  = egid;
    newFile.status       = ExtendedStat::kOnline;

    // Generate inherited ACL's if there are defaults
    if (parent.acl.has(AclEntry::kDefault | AclEntry::kUserObj))
      newFile.acl = Acl(parent.acl, getUid(this->secCtx_), egid,
                        mode, &newFile.stat.st_mode);

    this->si_->getINode()->create(newFile);
  }
  
  // Truncate
  else if (code == DM_NO_REPLICAS) {
    if (getUid(this->secCtx_) != file.stat.st_uid &&
        checkPermissions(this->secCtx_, file.acl, file.stat, S_IWRITE) != 0)
      throw DmException(DM_FORBIDDEN, "Not enough permissions to truncate " + path);
    this->si_->getINode()->setSize(file.stat.st_ino, 0);
  }
}



void BuiltInCatalog::makeDir(const std::string& path, mode_t mode) throw (DmException)
{
  std::string parentPath, name;

  // Get the parent of the new folder
  ExtendedStat parent = this->getParent(path, &parentPath, &name);

  // Check we have write access for the parent
  if (checkPermissions(this->secCtx_, parent.acl, parent.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Need write access for " + parentPath);
  
  // Create the folder
  ExtendedStat newFolder;
  
  newFolder.parent      = parent.stat.st_ino;
  newFolder.name        = name;
  newFolder.stat.st_uid = getUid(this->secCtx_);
  newFolder.status      = ExtendedStat::kOnline;
  
  // Mode
  newFolder.stat.st_mode = ((mode & ~S_IFMT) & ~this->umask_) | S_IFDIR;
  
  // Effective gid
  gid_t egid;
  if (parent.stat.st_mode & S_ISGID) {
    egid = parent.stat.st_gid;
    newFolder.stat.st_mode |= S_ISGID;
  }
  else {
    egid = getGid(this->secCtx_, 0);
  }
  newFolder.stat.st_gid = egid;

  // Generate inherited ACL's if there are defaults
  if (parent.acl.has(AclEntry::kDefault | AclEntry::kUserObj) > -1)
    newFolder.acl = Acl(parent.acl, getUid(this->secCtx_), egid,
                        mode, &newFolder.stat.st_mode);

  // Register
  this->si_->getINode()->create(newFolder);
}



void BuiltInCatalog::removeDir(const std::string& path) throw (DmException)
{
  std::string parentPath, name;

  // Fail inmediately with '/'
  if (path == "/")
    throw DmException(DM_INVALID_VALUE, "Can not remove '/'");

  // Get the parent of the new folder
  ExtendedStat parent = this->getParent(path, &parentPath, &name);

  // Get the file, and check it is a directory and it is empty
  ExtendedStat entry = this->si_->getINode()->extendedStat(parent.stat.st_ino, name);

  if (!S_ISDIR(entry.stat.st_mode))
    throw DmException(DM_NOT_DIRECTORY, path + " is not a directory. Can not remove.");

  if (this->cwd_ == entry.stat.st_ino)
    throw DmException(DM_IS_CWD, "Can not remove the current working dir");

  if (entry.stat.st_nlink > 0)
    throw DmException(DM_EXISTS, path + " is not empty. Can not remove.");

  // Check we can remove it
  if ((parent.stat.st_mode & S_ISVTX) == S_ISVTX) {
    // Sticky bit set
    if (getUid(this->secCtx_) != entry.stat.st_uid &&
        getUid(this->secCtx_) != parent.stat.st_uid &&
        checkPermissions(this->secCtx_, entry.acl, entry.stat, S_IWRITE) != 0)
      throw DmException(DM_FORBIDDEN, "Not enough permissions to remove " +
                                      path + "( sticky bit set)");
  }
  else {
    // No sticky bit
    if (checkPermissions(this->secCtx_, parent.acl, parent.stat, S_IWRITE) != 0)
      throw DmException(DM_FORBIDDEN, "Not enough permissions to remove " + path);
  }

  // All preconditions are good!
  this->si_->getINode()->begin();
  
  try {
    // Remove associated comments
    this->si_->getINode()->deleteComment(entry.stat.st_ino);
    // Remove directory itself
    this->si_->getINode()->unlink(entry.stat.st_ino);
  }
  catch (...) {
    this->si_->getINode()->rollback();
    throw;
  }
  
  this->si_->getINode()->commit();
}



void BuiltInCatalog::rename(const std::string& oldPath, const std::string& newPath) throw (DmException)
{
std::string oldParentPath, newParentPath;
  std::string oldName,       newName;

  // Do not even bother with '/'
  if (oldPath == "/" || newPath == "/")
    throw DmException(DM_INVALID_VALUE, "Not the source, neither the destination, can be '/'");

  // Get source and destination parent
  ExtendedStat oldParent = this->getParent(oldPath, &oldParentPath, &oldName);
  ExtendedStat newParent = this->getParent(newPath, &oldParentPath, &newName);

  // Source
  ExtendedStat old = this->si_->getINode()->extendedStat(oldParent.stat.st_ino, oldName);

  // Is the cwd?
  if (old.stat.st_ino == this->cwd_) {
    throw DmException(DM_IS_CWD, "Can not rename the current working directory");
  }

  // Need write permissions in both origin and destination
  if (checkPermissions(this->secCtx_, oldParent.acl, oldParent.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions on origin " + oldParentPath);

  if (checkPermissions(this->secCtx_, newParent.acl, newParent.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions on destination " + newParentPath);

  // If source is a directory, need write permissions there too
  if (S_ISDIR(old.stat.st_mode)) {
    if (checkPermissions(this->secCtx_, old.acl, old.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions on " + oldPath);

    // AND destination can not be a child
    ExtendedStat aux = newParent;

    while (aux.parent > 0) {
      if (aux.stat.st_ino == old.stat.st_ino)
        throw DmException(DM_INVALID_VALUE, "Destination is descendant of source");
      aux = this->si_->getINode()->extendedStat(aux.parent);
    }
  }

  // Check sticky
  if (oldParent.stat.st_mode & S_ISVTX &&
      getUid(this->secCtx_) != oldParent.stat.st_uid &&
      getUid(this->secCtx_) != old.stat.st_uid &&
      checkPermissions(this->secCtx_, old.acl, old.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Sticky bit set on the parent, and not enough permissions");

  // If the destination exists...
  try {
    ExtendedStat newF = this->si_->getINode()->extendedStat(newParent.stat.st_ino, newName);

    // If it is the same, leave the function
    if (newF.stat.st_ino == old.stat.st_ino)
      return;

    // It does! It has to be the same type
    if ((newF.stat.st_mode & S_IFMT) != (old.stat.st_mode & S_IFMT)) {
      if (S_ISDIR(old.stat.st_mode))
        throw DmException(DM_NOT_DIRECTORY, "Source is a directory and destination is not");
      else
        throw DmException(DM_IS_DIRECTORY, "Source is not directory and destination is");
    }

    // And it has to be empty. Just call remove or unlink
    // and they will fail if it is not
    if (S_ISDIR(newF.stat.st_mode))
      this->removeDir(newPath);
    else
      this->unlink(newPath);
  }
  catch (DmException& e) {
    if (e.code() != DM_NO_SUCH_FILE)
      throw;
  }

  // We are good, so we can move now
  this->si_->getINode()->begin();
  
  try {
    // Change the name if needed
    if (newName != oldName)
      this->si_->getINode()->rename(old.stat.st_ino, newName);

    // Change the parent if needed
    if (newParent.stat.st_ino != oldParent.stat.st_ino)
      this->si_->getINode()->move(old.stat.st_ino, newParent.stat.st_ino);
    else {
      // Parent is the same, but change its mtime
      struct utimbuf utim;
      utim.actime  = time(NULL);
      utim.modtime = utim.actime;
      this->si_->getINode()->utime(oldParent.stat.st_ino, &utim);
    }
  }
  catch (...) {
    this->si_->getINode()->rollback();
    throw;
  }

  // Done!
  this->si_->getINode()->commit();
}



mode_t BuiltInCatalog::umask(mode_t mask) throw ()
{
  mode_t prev = this->umask_;
  this->umask_ = mask;
  return prev;
}



void BuiltInCatalog::setMode(const std::string& path, mode_t mode) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path);

  // User has to be the owner, or root
  if (getUid(this->secCtx_) != meta.stat.st_uid &&
      getUid(this->secCtx_) != 0)
    throw DmException(DM_FORBIDDEN, "Only the owner can set the mode of " + path);

  // Clean up unwanted bits
  mode &= ~S_IFMT;
  if (!S_ISDIR(meta.stat.st_mode) && getUid(this->secCtx_) != 0)
    mode &= ~S_ISVTX;
  if (getUid(this->secCtx_) != 0 &&
      !hasGroup(this->secCtx_, meta.stat.st_gid))
    mode &= ~S_ISGID;

  // Update, keeping type bits from db.
  mode |= (meta.stat.st_mode & S_IFMT);

  // Update the ACL
  if (!meta.acl.empty()) {
    for (size_t i = 0; i < meta.acl.size(); ++i) {
      switch (meta.acl[i].type) {
        case AclEntry::kUserObj:
          meta.acl[i].perm = mode >> 6 & 07;
          break;
        case AclEntry::kGroupObj:
        case AclEntry::kMask:
          meta.acl[i].perm = mode >> 3 & 07;
          break;
        case AclEntry::kOther:
          meta.acl[i].perm = mode & 07;
          break;
        default:
          continue;
      }
    }
  }

  // Update entry
  this->si_->getINode()->setMode(meta.stat.st_ino,
                                 meta.stat.st_uid, meta.stat.st_gid, mode,
                                 meta.acl);
}



void BuiltInCatalog::setOwner(const std::string& path,
                              uid_t newUid, gid_t newGid,
                              bool followSymLink) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path, followSymLink);
  
  // If -1, no changes
  if (newUid == (uid_t)-1)
    newUid = meta.stat.st_uid;
  if (newGid == (gid_t)-1)
    newGid = meta.stat.st_gid;

  // Make sense to do anything?
  if (newUid == meta.stat.st_uid && newGid == meta.stat.st_gid)
    return;

  // If root, skip all checks
  if (getUid(this->secCtx_) != 0) {
    // Only root can change the owner
    if (meta.stat.st_uid != newUid)
      throw DmException(DM_BAD_OPERATION, "Only root can set the owner");
    // If the group is changing...
    if (meta.stat.st_gid != newGid) {
      // The user has to be the owner
      if (meta.stat.st_uid != getUid(this->secCtx_))
        throw DmException(DM_BAD_OPERATION, "Only root or the owner can set the group");
      // AND it has to belong to that group
      if (!hasGroup(this->secCtx_, newGid))
        throw DmException(DM_BAD_OPERATION, "The user does not belong to the group %d", newGid);
      // If it does, the group exists :)
    }
  }

  // Update the ACL's if there is any
  if (!meta.acl.empty()) {
    for (size_t i = 0; i < meta.acl.size(); ++i) {
      if (meta.acl[i].type == AclEntry::kUserObj)
        meta.acl[i].id = newUid;
      else if (meta.acl[i].type == AclEntry::kGroupObj)
        meta.acl[i].id = newGid;
    }
  }

  // Change!
  this->si_->getINode()->setMode(meta.stat.st_ino,
                                 newUid, newGid, meta.stat.st_mode,
                                 meta.acl);
}



void BuiltInCatalog::setSize(const std::string& path, size_t newSize) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path, false);
  if (getUid(this->secCtx_) != meta.stat.st_uid &&
      checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Can not set the size of " + path);
  
  this->si_->getINode()->setSize(meta.stat.st_ino, newSize);
}



void BuiltInCatalog::setChecksum(const std::string& path,
                                    const std::string& csumtype,
                                    const std::string& csumvalue) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path, false);
  if (getUid(this->secCtx_) != meta.stat.st_uid &&
      checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Can not set the checksum of " + path);
  
  if (csumtype != "MD" && csumtype != "AD" && csumtype != "CS")
    throw DmException(DM_INVALID_VALUE,
                      "%s is an invalid checksum type", csumtype.c_str());
  
  this->si_->getINode()->setChecksum(meta.stat.st_ino, csumtype, csumvalue);
}



void BuiltInCatalog::setAcl(const std::string& path, const Acl& acl) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path);

  // Check we can change it
  if (getUid(this->secCtx_) != meta.stat.st_uid &&
      getUid(this->secCtx_) != 0)
    throw DmException(DM_FORBIDDEN, "Only the owner can set the ACL of " + path);

  Acl aclCopy(acl);

  // Make sure the owner and group matches!
  for (size_t i = 0; i < aclCopy.size(); ++i) {
    if (aclCopy[i].type == AclEntry::kUserObj)
      aclCopy[i].id = meta.stat.st_uid;
    else if (aclCopy[i].type == AclEntry::kGroupObj)
      aclCopy[i].id = meta.stat.st_gid;
    else if (aclCopy[i].type & AclEntry::kDefault && !S_ISDIR(meta.stat.st_mode))
      throw DmException(DM_INVALID_ACL, "Defaults can be only applied to directories");
  }

  // Validate the ACL
  aclCopy.validate();

  // Update the file mode
  for (size_t i = 0; i < aclCopy.size(); ++i) {
    switch (aclCopy[i].type) {
      case AclEntry::kUserObj:
        meta.stat.st_mode = (meta.stat.st_mode & 0177077) |
                            (aclCopy[i].perm << 6);
        break;
      case AclEntry::kGroupObj:
        meta.stat.st_mode = (meta.stat.st_mode & 0177707) |
                            (aclCopy[i].perm << 3);
        break;
      case AclEntry::kMask:
        meta.stat.st_mode = (meta.stat.st_mode & ~070) |
                            (meta.stat.st_mode & aclCopy[i].perm << 3);
        break;
      case AclEntry::kOther:
        meta.stat.st_mode = (meta.stat.st_mode & 0177770) |
                            (aclCopy[i].perm);
        break;
      default:
        continue;
    }
  }

  // Update the file
  this->si_->getINode()->setMode(meta.stat.st_ino,
                                 meta.stat.st_uid, meta.stat.st_gid,
                                 meta.stat.st_mode,
                                 aclCopy);
}



void BuiltInCatalog::utime(const std::string& path, const struct utimbuf* buf) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path);

  // The user is the owner OR buf is NULL and has write permissions
  if (getUid(this->secCtx_) != meta.stat.st_uid &&
      checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions to modify the time of " + path);

  // Touch
  this->si_->getINode()->utime(meta.stat.st_ino, buf);
}



std::string BuiltInCatalog::getComment(const std::string& path) throw (DmException)
{
  // Get the file and check we can read
  ExtendedStat meta = this->extendedStat(path);
  
  if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IREAD) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions to read " + path);

  return this->si_->getINode()->getComment(meta.stat.st_ino);
}



void BuiltInCatalog::setComment(const std::string& path, const std::string& comment) throw (DmException)
{
  // Get the file and check we can write
  ExtendedStat meta = this->extendedStat(path);

  if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions to write " + path);

  this->si_->getINode()->setComment(meta.stat.st_ino, comment);
}



void BuiltInCatalog::setGuid(const std::string& path, const std::string &guid) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path);

  if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions to write " + path);

  this->si_->getINode()->setGuid(meta.stat.st_ino, guid);
}



void BuiltInCatalog::updateExtendedAttributes(const std::string& path,
                                              const Extensible& attr) throw (DmException)
{
  ExtendedStat meta = this->extendedStat(path);
  
  if (checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Not enough permissions to write " + path);
  
  this->si_->getINode()->updateExtendedAttributes(meta.stat.st_ino, attr);
}



Directory* BuiltInCatalog::openDir (const std::string& path) throw (DmException)
{
  BuiltInDir* dirp;
  
  dirp = new BuiltInDir;
  
  try {
    dirp->dir = this->extendedStat(path);

    if (checkPermissions(this->secCtx_, dirp->dir.acl, dirp->dir.stat, S_IREAD) != 0)
      throw DmException(DM_FORBIDDEN, "Not enough permissions to read " + path);

    dirp->idir = this->si_->getINode()->openDir(dirp->dir.stat.st_ino);
    
    return dirp;
  }
  catch (...) {
    delete dirp;
    throw;
  }  
}



void BuiltInCatalog::closeDir(Directory* dir) throw (DmException)
{
  BuiltInDir* dirp = (BuiltInDir*)dir;
  this->si_->getINode()->closeDir(dirp->idir);
  delete dirp;
}



struct dirent* BuiltInCatalog::readDir(Directory* dir) throw (DmException)
{
  BuiltInDir* dirp = (BuiltInDir*)dir;
  struct dirent* d = this->si_->getINode()->readDir(dirp->idir);
  this->updateAccessTime(dirp->dir);
  return d;
}



ExtendedStat* BuiltInCatalog::readDirx(Directory* dir) throw (DmException)
{
  BuiltInDir* dirp = (BuiltInDir*)dir;
  ExtendedStat* s = this->si_->getINode()->readDirx(dirp->idir);
  this->updateAccessTime(dirp->dir);
  return s;
}



Replica BuiltInCatalog::getReplica(const std::string& rfn) throw (DmException)
{
  Replica      rdata = this->si_->getINode()->getReplica(rfn);
  ExtendedStat meta  = this->si_->getINode()->extendedStat(rdata.fileid);
  
  this->traverseBackwards(meta);
  
  return rdata;
}



void BuiltInCatalog::updateReplica(const Replica& replica) throw (DmException)
{
  // Can not trust the fileid of replica!
  Replica      rdata = this->si_->getINode()->getReplica(replica.replicaid);
  ExtendedStat meta  = this->si_->getINode()->extendedStat(rdata.fileid);
  
  this->traverseBackwards(meta);
  
  if (dmlite::checkPermissions(this->secCtx_, meta.acl, meta.stat, S_IWRITE) != 0)
    throw DmException(DM_FORBIDDEN, "Can not modify the replica");
  
  this->si_->getINode()->updateReplica(replica);
}



void BuiltInCatalog::updateAccessTime(const ExtendedStat& meta) throw (DmException)
{
  if (this->updateATime_) {
    struct utimbuf tim;
    tim.actime  = time(NULL);
    tim.modtime = meta.stat.st_mtime;
    this->si_->getINode()->utime(meta.stat.st_ino, &tim);
  }
}



ExtendedStat BuiltInCatalog::getParent(const std::string& path,
                                       std::string* parentPath,
                                       std::string* name) throw (DmException)
{
  if (path.empty())
    throw DmException(DM_INVALID_VALUE, "Empty path");
  
  std::vector<std::string> components = Url::splitPath(path);

  *name = components.back();
  components.pop_back();
  
  *parentPath = Url::joinPath(components);

  // Get the files now
  if (!parentPath->empty()) {
    return this->extendedStat(*parentPath);
  }
  else if (!this->cwdPath_.empty()) {
    *parentPath = this->cwdPath_;
    return this->si_->getINode()->extendedStat(this->cwd_);
  }
  else {
    *parentPath = "/";
    return this->extendedStat("/");
  }
}



void BuiltInCatalog::traverseBackwards(const ExtendedStat& meta) throw (DmException)
{
  ExtendedStat current = meta;

  // We want to check if we can arrive here...
  while (current.parent != 0) {
    current = this->si_->getINode()->extendedStat(current.parent);
    if (checkPermissions(this->secCtx_, current.acl, current.stat, S_IEXEC))
      throw DmException(DM_FORBIDDEN, "Can not access #%ld",
                        current.stat.st_ino);
  }
}
