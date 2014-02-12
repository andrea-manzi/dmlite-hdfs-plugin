/// @file   core/Inode-C.cpp
/// @brief  C wrapper for dmlite::INode.
/// @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
#include <dmlite/c/inode.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/inode.h>
#include <sys/stat.h>
#include "Private.h"



int dmlite_ibegin(dmlite_context* context)
{
  TRY(context, ibegin)
  context->stack->getINode()->begin();
  CATCH(context, ibegin)
}



int dmlite_icommit(dmlite_context* context)
{
  TRY(context, icommit)
  context->stack->getINode()->commit();
  CATCH(context, icommit)
}



int dmlite_irollback(dmlite_context* context)
{
  TRY(context, irollback)
  context->stack->getINode()->rollback();
  CATCH(context, irollback)
}



int dmlite_icreate(dmlite_context* context, const dmlite_xstat* f)
{
  TRY(context, icreate)
  NOT_NULL(f);
  dmlite::ExtendedStat ex;
  dmlite_cxstat_to_cppxstat(f, &ex);
  context->stack->getINode()->create(ex);
  CATCH(context, icreate)
}



int dmlite_isymlink(dmlite_context* context, ino_t inode, const char* link)
{
  TRY(context, isymlink)
  NOT_NULL(link);
  context->stack->getINode()->symlink(inode, link);
  CATCH(context, isymlink)
}



int dmlite_iunlink(dmlite_context* context, ino_t inode)
{
  TRY(context, iunlink)
  context->stack->getINode()->unlink(inode);
  CATCH(context, iunlink)
}



int dmlite_imove(dmlite_context* context, ino_t inode, ino_t dest)
{
  TRY(context, imove)
  context->stack->getINode()->move(inode, dest);
  CATCH(context, imove)
}



int dmlite_irename(dmlite_context* context, ino_t inode, const char* name)
{
  TRY(context, irename)
  context->stack->getINode()->rename(inode, name);
  CATCH(context, irename)
}



int dmlite_istat(dmlite_context* context, ino_t inode, struct stat* buf)
{
  TRY(context, istat)
  NOT_NULL(buf);
  *buf = context->stack->getINode()->extendedStat(inode).stat;
  CATCH(context, istat);
}



int dmlite_istatx(dmlite_context* context, ino_t inode, dmlite_xstat* buf)
{
  TRY(context, istatx)
  NOT_NULL(buf);
  dmlite::ExtendedStat ex = context->stack->getINode()->extendedStat(inode);
  dmlite_cppxstat_to_cxstat(ex, buf);
  CATCH(context, istatx);
}

int dmlite_istatx_by_name(dmlite_context* context, ino_t parent, const char* name,
                          dmlite_xstat* buf)
{
  TRY(context, istatx_by_name)
  NOT_NULL(name);
  NOT_NULL(buf);
  dmlite::ExtendedStat ex = context->stack->getINode()->
                              extendedStat(parent, name);
  dmlite_cppxstat_to_cxstat(ex, buf);
  CATCH(context, istax_by_name)
}

int dmlite_ireadlink(dmlite_context* context, ino_t inode,
                     char* path, size_t bufsize)
{
  TRY(context, ireadlink)
  NOT_NULL(path);
  dmlite::SymLink link = context->stack->getINode()->readLink(inode);
  strncpy(path, link.link.c_str(), bufsize);
  CATCH(context, ireadlink)
}



int dmlite_iaddreplica(dmlite_context* context, const dmlite_replica* replica)
{
  TRY(context, iaddreplica)
  NOT_NULL(replica);
  dmlite::Replica replicapp;
  dmlite_creplica_to_cppreplica(replica, &replicapp);
  context->stack->getINode()->addReplica(replicapp);
  CATCH(context, iaddreplica)
}



int dmlite_ideletereplica(dmlite_context* context, const dmlite_replica* replica)
{
  TRY(context, ideletereplica)
  NOT_NULL(replica);
  dmlite::Replica replicapp;
  dmlite_creplica_to_cppreplica(replica, &replicapp);
  context->stack->getINode()->deleteReplica(replicapp);
  CATCH(context, ideletereplica)
}



int dmlite_igetreplica(dmlite_context* context, int64_t rid, dmlite_replica* buf)
{
  TRY(context, igetreplica)
  NOT_NULL(buf);
  dmlite::Replica replicapp = context->stack->getINode()->getReplica(rid);
  dmlite_cppreplica_to_creplica(replicapp, buf);
  CATCH(context, igetreplica)
}



int dmlite_igetreplicas(dmlite_context* context, ino_t inode,
                        unsigned* nreplicas, dmlite_replica** replicas)
{
  TRY(context, igetreplicas)
  std::vector<dmlite::Replica> replicasSet = context->stack->getINode()->
                                                getReplicas(inode);
  
  *nreplicas = replicasSet.size();
  *replicas = new dmlite_replica[*nreplicas];
  
  for (unsigned i = 0; i < *nreplicas; ++i) {
    (*replicas)[i].extra = new dmlite_any_dict();
    dmlite_cppreplica_to_creplica(replicasSet[i], &((*replicas)[i]));
  }
  
  CATCH(context, igetreplicas)
}



int dmlite_iutime(dmlite_context* context, ino_t inode,
                  const struct utimbuf* buf)
{
  TRY(context, iutime)
  context->stack->getINode()->utime(inode, buf);
  CATCH(context, iutime)
}



int dmlite_isetmode(dmlite_context* context, ino_t inode, uid_t uid, gid_t gid,
                    mode_t mode, unsigned nentries, dmlite_aclentry* acl)
{
  TRY(context, isetmode)
  dmlite::Acl aclpp;
  
  if (nentries > 0)
    NOT_NULL(acl);
  
  for (unsigned i = 0; i < nentries; ++i) {
    dmlite::AclEntry e;
    e.id   = acl[i].id;
    e.perm = acl[i].perm;
    e.type = acl[i].type;
    aclpp.push_back(e);
  }
  
  context->stack->getINode()->setMode(inode, uid, gid, mode, aclpp);
  CATCH(context, isetmode)
}



int dmlite_isetsize(dmlite_context* context, ino_t inode, size_t size)
{
  TRY(context, isetsize)
  context->stack->getINode()->setSize(inode, size);
  CATCH(context, isetsize)
}



int dmlite_isetchecksum(dmlite_context* context, ino_t inode,
                        const char* csumtype, const char* csumvalue)
{
  TRY(context, isetchecksum)
  context->stack->getINode()->setChecksum(inode, csumtype, csumvalue);
  CATCH(context, isetchecksum)
}



int dmlite_igetcomment(dmlite_context* context, ino_t inode,
                       char* comment, size_t bufsize)
{
  TRY(context, igetcomment)
  std::string c = context->stack->getINode()->getComment(inode);
  strncpy(comment, c.c_str(), bufsize);
  CATCH(context, igetcomment)
}



int dmlite_isetcomment(dmlite_context* context,  ino_t inode,
                       const char* comment)
{
  TRY(context, isetcomment)
  context->stack->getINode()->setComment(inode, comment);
  CATCH(context, isetcomment)
}



int dmlite_ideletecomment(dmlite_context* context, ino_t inode)
{
  TRY(context, ideletecomment)
  context->stack->getINode()->deleteComment(inode);
  CATCH(context, ideletecomment)
}



int dmlite_isetguid(dmlite_context* context, ino_t inode, const char* guid)
{
  TRY(context, isetguid)
  context->stack->getINode()->setGuid(inode, guid);
  CATCH(context, isetguid)
}



dmlite_idir* dmlite_iopendir(dmlite_context* context, ino_t inode)
{
  TRY(context, iopendir)
  dmlite::IDirectory* idir = context->stack->getINode()->openDir(inode);
  dmlite_idir* d = new dmlite_idir();
  d->dir = idir;
  return d;
  CATCH_POINTER(context, iopendir)
}



int dmlite_iupdate_xattr(dmlite_context* context, ino_t inode,
                         const dmlite_any_dict* xattr)
{
  TRY(context, iupdate_xattr)
  context->stack->getINode()->updateExtendedAttributes(inode, xattr->extensible);
  CATCH(context, iupdate_xattr)
}



int dmlite_iclosedir(dmlite_context* context, dmlite_idir* dir)
{
  TRY(context, iclosedir)
  NOT_NULL(dir);
  dmlite::IDirectory* idir = dir->dir;
  delete dir;
  context->stack->getINode()->closeDir(idir);
  CATCH(context, iclosedir)
}



dmlite_xstat* dmlite_ireaddirx(dmlite_context* context, dmlite_idir* dir)
{
  TRY(context, ireaddirx)
  NOT_NULL(dir);
  dmlite::ExtendedStat* ex = context->stack->getINode()->readDirx(dir->dir);
  if (ex == NULL)
    return NULL;
  dmlite_cppxstat_to_cxstat(*ex, &dir->xstat);
  return &dir->xstat;
  CATCH_POINTER(context, ireaddirx)
}



struct dirent* dmlite_ireaddir(dmlite_context* context, dmlite_idir* dir)
{
  TRY(context, ireaddir)
  return context->stack->getINode()->readDir(dir->dir);
  CATCH_POINTER(context, ireaddir)
}
