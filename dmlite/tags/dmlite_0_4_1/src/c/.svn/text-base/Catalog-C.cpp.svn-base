/// @file   core/Catalog-C.cpp
/// @brief  C wrapper for dmlite::Catalog.
/// @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
#include <cstring>
#include <dmlite/c/dmlite.h>
#include <dmlite/c/catalog.h>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/exceptions.h>
#include <dmlite/cpp/utils/urls.h>
#include <set>
#include <vector>

#include "Private.h"



int dmlite_chdir(dmlite_context* context, const char* path)
{
  TRY(context, chdir)
  NOT_NULL(path);
  context->stack->getCatalog()->changeDir(path);
  CATCH(context, chdir)
}



char* dmlite_getcwd(dmlite_context* context, char* buffer, size_t size)
{
  TRY(context, getcwd)
  std::string wd = context->stack->getCatalog()->getWorkingDir();
  if (buffer != NULL)
    return strncpy(buffer, wd.c_str(), size);
  else
    return strdup(wd.c_str());
  CATCH_POINTER(context, getcwd)
}



mode_t dmlite_umask(dmlite_context* context, mode_t mask)
{
  TRY(context, umask)
  return context->stack->getCatalog()->umask(mask);
  }
  catch(...) {
    return 0;
  }
}



int dmlite_stat(dmlite_context *context, const char* path, struct stat* buf)
{
  TRY(context, stat)
  NOT_NULL(path);
  *buf = context->stack->getCatalog()->extendedStat(path).stat;
  CATCH(context, stat)
}



int dmlite_statl(dmlite_context* context, const char* path, struct stat* buf)
{
  TRY(context, statl)
  NOT_NULL(path);
  NOT_NULL(buf);
  *buf = context->stack->getCatalog()->extendedStat(path, false).stat;
  CATCH(context, statl);
}



int dmlite_statx(dmlite_context* context, const char* path, dmlite_xstat* buf)
{
  TRY(context, statx)
  NOT_NULL(path);
  NOT_NULL(buf);
  dmlite::ExtendedStat ex = context->stack->getCatalog()->extendedStat(path);
  dmlite_cppxstat_to_cxstat(ex, buf);
  CATCH(context, statx);
}



int dmlite_addreplica(dmlite_context* context, const dmlite_replica* replica)
{
  TRY(context, addreplica)
  NOT_NULL(replica);  
  dmlite::Replica replicapp;
  dmlite_creplica_to_cppreplica(replica, &replicapp);
  context->stack->getCatalog()->addReplica(replicapp);
  CATCH(context, addreplica)
}



int dmlite_delreplica(dmlite_context* context, const dmlite_replica* replica)
{
  TRY(context, delreplica)
  NOT_NULL(replica);
  dmlite::Replica replicapp;
  dmlite_creplica_to_cppreplica(replica, &replicapp);
  context->stack->getCatalog()->deleteReplica(replicapp);
  CATCH(context, delreplica)
}



int dmlite_getreplicas(dmlite_context* context, const char* path, unsigned *nReplicas,
                       dmlite_replica** fileReplicas)
{
  TRY(context, getreplicas)
  NOT_NULL(path);
  NOT_NULL(nReplicas);
  NOT_NULL(fileReplicas);

  std::vector<dmlite::Replica> replicaSet = context->stack->getCatalog()->getReplicas(path);

  *nReplicas = replicaSet.size();
  if (*nReplicas > 0)
    *fileReplicas = new dmlite_replica[replicaSet.size()];
  else
    *fileReplicas = NULL;

  for (unsigned i = 0; i < *nReplicas; ++i) {
    (*fileReplicas)[i].extra = new dmlite_any_dict();
    dmlite_cppreplica_to_creplica(replicaSet[i], &((*fileReplicas)[i]));
  }  

  CATCH(context, getreplicas)
}



int dmlite_replicas_free(dmlite_context* context, unsigned nReplicas,
                         dmlite_replica* fileReplicas)
{
  for (unsigned i = 0; i < nReplicas; ++i) {
    if (fileReplicas[i].extra != NULL)
      delete fileReplicas[i].extra;
  }
  delete [] fileReplicas;
  return 0;
}



int dmlite_symlink(dmlite_context* context,
                   const char* oldPath, const char* newPath)
{
  TRY(context, symlink)
  context->stack->getCatalog()->symlink(oldPath, newPath);
  CATCH(context, symlink)
}  



int dmlite_readlink(dmlite_context* context, const char* path,
                    char* buf, size_t bufsize)
{
  TRY(context, readlink)
  NOT_NULL(path);
  NOT_NULL(buf);
  std::string target = context->stack->getCatalog()->readLink(path);
  std::strncpy(buf, target.c_str(), bufsize);
  CATCH(context, readlink)
}



int dmlite_create(dmlite_context* context, const char* path, mode_t mode)
{
  TRY(context, create)
  NOT_NULL(path);
  context->stack->getCatalog()->create(path, mode);
  CATCH(context, create)
}



int dmlite_unlink(dmlite_context* context, const char* path)
{
  TRY(context, unlink)
  NOT_NULL(path);
  context->stack->getCatalog()->unlink(path);
  CATCH(context, unlink)
}



int dmlite_chmod(dmlite_context* context, const char* path, mode_t mode)
{
  TRY(context, chmod)
  NOT_NULL(path);
  context->stack->getCatalog()->setMode(path, mode);
  CATCH(context, chmod)
}



int dmlite_chown(dmlite_context* context, const char* path, uid_t newUid, gid_t newGid)
{
  TRY(context, chown)
  NOT_NULL(path);
  context->stack->getCatalog()->setOwner(path, newUid, newGid);
  CATCH(context, chown)
}



int dmlite_lchown(dmlite_context* context, const char* path, uid_t newUid, gid_t newGid)
{
  TRY(context, lchown)
  NOT_NULL(path);
  context->stack->getCatalog()->setOwner(path, newUid, newGid, false);
  CATCH(context, lchown)
}



int dmlite_setfsize(dmlite_context* context, const char* path, uint64_t filesize)
{
  TRY(context, setfsize)
  NOT_NULL(path);
  context->stack->getCatalog()->setSize(path, filesize);
  CATCH(context, setfsize)
}



int dmlite_setfsizec(dmlite_context* context, const char* path, uint64_t filesize,
                 const char* csumtype, const char* csumvalue)
{
  TRY(context, setfsizec)
  NOT_NULL(path);
  NOT_NULL(csumtype);
  NOT_NULL(csumvalue);
  context->stack->getCatalog()->setSize(path, filesize);
  context->stack->getCatalog()->setChecksum(path, csumtype, csumvalue);
  CATCH(context, setfsizec)
}



int dmlite_setacl(dmlite_context* context, const char* path, unsigned nEntries,
                  dmlite_aclentry* acl)
{
  TRY(context, setacl)
  NOT_NULL(path);
  NOT_NULL(acl);

  dmlite::Acl acl;
  for (unsigned i = 0; i < nEntries; ++i) {
    dmlite::AclEntry e;
    
    e.id   = acl[i].id;
    e.perm = acl[i].perm;
    e.type = acl[i].type;
    
    acl.push_back(e);
  }

  context->stack->getCatalog()->setAcl(path, acl);

  CATCH(context, setacl)
}



int dmlite_utime(dmlite_context* context, const char* path, const struct utimbuf* buf)
{
  TRY(context, utime)
  NOT_NULL(path);
  context->stack->getCatalog()->utime(path, buf);
  CATCH(context, utime)
}



int dmlite_getcomment(dmlite_context* context, const char* path,
                      char* comment, size_t bufsize)
{
  TRY(context, getcomment)
  NOT_NULL(path);
  NOT_NULL(comment);
  std::string c = context->stack->getCatalog()->getComment(path);
  strncpy(comment, c.c_str(), bufsize);
  CATCH(context, getcomment)
}



int dmlite_setcomment(dmlite_context* context, const char* path, const char* comment)
{
  TRY(context, setcomment)
  NOT_NULL(path);
  NOT_NULL(comment);
  context->stack->getCatalog()->setComment(path, comment);
  CATCH(context, setcomment)
}



int dmlite_setguid(dmlite_context* context, const char* path, const char* guid)
{
  TRY(context, setguid)
  NOT_NULL(path);
  NOT_NULL(guid);
  context->stack->getCatalog()->setGuid(path, guid);
  CATCH(context, seguid)
}



int dmlite_update_xattr(dmlite_context* context, const char* path,
                        const dmlite_any_dict* xattr)
{
  TRY(context, update_xattr)
  NOT_NULL(path);
  NOT_NULL(xattr);
  context->stack->getCatalog()->updateExtendedAttributes(path, xattr->extensible);
  CATCH(context, update_xattr)
}



int dmlite_getgrpbynam(dmlite_context* context, const char* groupName, gid_t* gid)
{
  TRY(context, getgrpbynam)
  NOT_NULL(groupName);
  NOT_NULL(gid);
  dmlite::GroupInfo group = context->stack->getAuthn()->getGroup(groupName);
  *gid = group.getUnsigned("gid");
  CATCH(context, getgrpbynam)
}



int dmlite_getusrbynam(dmlite_context* context, const char* userName, uid_t* uid)
{
  TRY(context, getusrbyuid)
  NOT_NULL(userName);
  dmlite::UserInfo user = context->stack->getAuthn()->getUser(userName);
  *uid = user.getUnsigned("uid");
  CATCH(context, getusrbyuid)
}



dmlite_dir* dmlite_opendir(dmlite_context* context, const char* path)
{
  TRY(context, opendir)
  NOT_NULL(path);
  dmlite::Directory* d = context->stack->getCatalog()->openDir(path);
  
  dmlite_dir* dirp = new dmlite_dir();
  dirp->dir = d;
  memset(&dirp->xstat, 0, sizeof(dirp->xstat));
  dirp->xstat.extra = dmlite_any_dict_new();
  
  return dirp;  
  CATCH_POINTER(context, opendir)
}



int dmlite_closedir(dmlite_context* context, dmlite_dir* dir)
{
  TRY(context, closedir)
  NOT_NULL(dir);
  context->stack->getCatalog()->closeDir(dir->dir);
  delete dir->xstat.extra;
  delete dir;
  CATCH(context, closedir)
}



struct dirent* dmlite_readdir(dmlite_context* context, dmlite_dir* dir)
{
  TRY(context, readdir)
  NOT_NULL(dir);
  return context->stack->getCatalog()->readDir(dir->dir);
  CATCH_POINTER(context, readdir)
}



struct dmlite_xstat* dmlite_readdirx(dmlite_context* context, dmlite_dir* dir)
{
  TRY(context, readdirx)
  NOT_NULL(dir);
  dmlite::ExtendedStat* ex = context->stack->getCatalog()->readDirx(dir->dir);
  if (ex == NULL)
    return NULL;
  
  dmlite_cppxstat_to_cxstat(*ex, &dir->xstat);
  return &dir->xstat;
  CATCH_POINTER(context, readdirx)
}



int dmlite_mkdir(dmlite_context* context, const char* path, mode_t mode)
{
  TRY(context, mkdir)
  NOT_NULL(path);
  context->stack->getCatalog()->makeDir(path, mode);
  CATCH(context, mkdir)
}



int dmlite_rename(dmlite_context* context, const char* oldPath, const char* newPath)
{
  TRY(context, rename)
  NOT_NULL(oldPath);
  NOT_NULL(newPath);
  context->stack->getCatalog()->rename(oldPath, newPath);
  CATCH(context, rename)
}



int dmlite_rmdir(dmlite_context* context, const char* path)
{
  TRY(context, rmdir)
  NOT_NULL(path);
  context->stack->getCatalog()->removeDir(path);
  CATCH(context, rmdir)
}



int dmlite_getreplica(dmlite_context* context, const char* rfn, dmlite_replica* replica)
{
  TRY(context, getreplica)
  NOT_NULL(rfn);
  NOT_NULL(replica);
  dmlite::Replica replicapp = context->stack->getCatalog()->getReplica(rfn);
  dmlite_cppreplica_to_creplica(replicapp, replica);
  CATCH(context, getreplica)
}



int dmlite_updatereplica(dmlite_context* context, const dmlite_replica* replica)
{
  TRY(context, updatereplica)
  NOT_NULL(replica);
  dmlite::Replica replicapp;
  dmlite_creplica_to_cppreplica(replica, &replicapp);
  context->stack->getCatalog()->updateReplica(replicapp);
  CATCH(context, updatereplica)
}
