/// @file    core/dummy/DummyCatalog.cpp
/// @brief   DummyCatalog implementation.
/// @details It makes sense as a base for other decorator plug-ins.
/// @author  Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#include <dmlite/cpp/dummy/DummyCatalog.h>

using namespace dmlite;



/// Little of help here to avoid redundancy
#define DELEGATE(method, ...) \
if (this->decorated_ == NULL)\
  throw DmException(DM_NOT_IMPLEMENTED, "There is no plugin in the stack that implements "#method);\
this->decorated_->method(__VA_ARGS__);


/// Little of help here to avoid redundancy
#define DELEGATE_RETURN(method, ...) \
if (this->decorated_ == NULL)\
  throw DmException(DM_NOT_IMPLEMENTED, "There is no plugin in the stack that implements "#method);\
return this->decorated_->method(__VA_ARGS__);



DummyCatalog::DummyCatalog(Catalog* decorated) throw (DmException)
{
  this->decorated_ = decorated;
}



DummyCatalog::~DummyCatalog()
{
  delete this->decorated_;
}



void DummyCatalog::setStackInstance(StackInstance* si) throw (DmException)
{
  BaseInterface::setStackInstance(this->decorated_, si);
}



void DummyCatalog::setSecurityContext(const SecurityContext* ctx) throw (DmException)
{
  BaseInterface::setSecurityContext(this->decorated_, ctx);
}



void DummyCatalog::changeDir(const std::string& path) throw (DmException)
{
  DELEGATE(changeDir, path);
}



std::string DummyCatalog::getWorkingDir(void) throw (DmException)
{
  DELEGATE_RETURN(getWorkingDir);
}



ExtendedStat DummyCatalog::extendedStat(const std::string& path, bool follow) throw (DmException)
{
  DELEGATE_RETURN(extendedStat, path, follow);
}



void DummyCatalog::addReplica(const Replica& replica) throw (DmException)
{
  DELEGATE(addReplica, replica);
}



void DummyCatalog::deleteReplica(const Replica& replica) throw (DmException)
{
  DELEGATE(deleteReplica, replica);
}



std::vector<Replica> DummyCatalog::getReplicas(const std::string& path) throw (DmException)
{
  DELEGATE_RETURN(getReplicas, path);
}



void DummyCatalog::symlink(const std::string& oldpath, const std::string& newpath) throw (DmException)
{
  DELEGATE(symlink, oldpath, newpath);
}



std::string DummyCatalog::readLink(const std::string& path) throw (DmException)
{
  DELEGATE_RETURN(readLink, path);
}



void DummyCatalog::unlink(const std::string& path) throw (DmException)
{
  DELEGATE(unlink, path);
}



void DummyCatalog::create(const std::string& path, mode_t mode) throw (DmException)
{
  DELEGATE(create, path, mode);
}



mode_t DummyCatalog::umask(mode_t mask) throw ()
{
  DELEGATE_RETURN(umask, mask);
}



void DummyCatalog::setMode(const std::string& path, mode_t mode) throw (DmException)
{
  DELEGATE(setMode, path, mode);
}



void DummyCatalog::setOwner(const std::string& path, uid_t newUid, gid_t newGid, bool fs) throw (DmException)
{
  DELEGATE(setOwner, path, newUid, newGid, fs);
}



void DummyCatalog::setSize(const std::string& path, size_t newSize) throw (DmException)
{
  DELEGATE(setSize, path, newSize);
}



void DummyCatalog::setChecksum(const std::string& path,
                                  const std::string& csumtype,
                                  const std::string& csumvalue) throw (DmException)
{
  DELEGATE(setChecksum, path, csumtype, csumvalue);
}



void DummyCatalog::setAcl(const std::string& path, const Acl& acls) throw (DmException)
{
  DELEGATE(setAcl, path, acls);
}



void DummyCatalog::utime(const std::string& path, const struct utimbuf* buf) throw (DmException)
{
  DELEGATE(utime, path, buf);
}



std::string DummyCatalog::getComment(const std::string& path) throw (DmException)
{
  DELEGATE_RETURN(getComment, path);
}



void DummyCatalog::setComment(const std::string& path, const std::string& comment) throw (DmException)
{
  DELEGATE(setComment, path, comment);
}



void DummyCatalog::setGuid(const std::string& path, const std::string& guid) throw (DmException)
{
  DELEGATE(setGuid, path, guid);
}



void DummyCatalog::updateExtendedAttributes(const std::string& path,
                                            const Extensible& attr) throw (DmException)
{
  DELEGATE(updateExtendedAttributes, path, attr);
}



Directory* DummyCatalog::openDir(const std::string& path) throw (DmException)
{
  DELEGATE_RETURN(openDir, path);
}



void DummyCatalog::closeDir(Directory* dir) throw (DmException)
{
  DELEGATE(closeDir, dir);
}



struct dirent* DummyCatalog::readDir(Directory* dir) throw (DmException)
{
  DELEGATE_RETURN(readDir, dir);
}



ExtendedStat* DummyCatalog::readDirx(Directory* dir) throw (DmException)
{
  DELEGATE_RETURN(readDirx, dir);
}



void DummyCatalog::makeDir(const std::string& path, mode_t mode) throw (DmException)
{
  DELEGATE(makeDir, path, mode);
}



void DummyCatalog::rename(const std::string& oldPath, const std::string& newPath) throw (DmException)
{
  DELEGATE(rename, oldPath, newPath);
}



void DummyCatalog::removeDir(const std::string& path) throw (DmException)
{
  DELEGATE(removeDir, path);
}



Replica DummyCatalog::getReplica(const std::string& rfn) throw (DmException)
{
  DELEGATE_RETURN(getReplica, rfn);
}



void DummyCatalog::updateReplica(const Replica& replica) throw (DmException)
{
  DELEGATE(updateReplica, replica);
}
