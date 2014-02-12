/// @file   core/Private.h
/// @brief  Internal declarations.
/// @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
#ifndef PRIVATE_H
#define	PRIVATE_H

#include <dmlite/c/inode.h>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/utils/extensible.h>
#include <string>

/// Open the try statement.
#define TRY(context, method)\
try {\
  context->errorCode = DM_NO_ERROR;\
  context->errorString.clear();

/// Catch block.
#define CATCH(context, method)\
  return 0;\
} catch (dmlite::DmException& e) {\
  context->errorCode   = e.code();\
  context->errorString = e.what();\
  return e.code();\
} catch (...) {\
  context->errorCode   = DM_UNEXPECTED_EXCEPTION;\
  context->errorString = "An unexpected exception was thrown while executing "#method;\
  return DM_UNEXPECTED_EXCEPTION;\
}

/// Catch block for functions that return a pointer.
#define CATCH_POINTER(context, method)\
} catch (dmlite::DmException& e) {\
  context->errorCode   = e.code();\
  context->errorString = e.what();\
  return NULL;\
} catch (...) {\
  context->errorCode   = DM_UNEXPECTED_EXCEPTION;\
  context->errorString = "An unexpected exception was thrown while executing "#method;\
  return NULL;\
}

/// Try-catch in one macro
#define TRY_CATCH(handle, method, ...)\
try {\
  handle->manager->method(__VA_ARGS__);\
}\
catch (dmlite::DmException& e) {\
  handle->errorCode   = e.code();\
  handle->errorString = e.what();\
  return e.code();\
}\
catch (...) {\
  return DM_UNEXPECTED_EXCEPTION;\
}\
return DM_NO_ERROR;

/// Safe strings (empty instead of NULL)
#define SAFE_STRING(s) s==NULL?"":s

/// Throw an exception if the string is NULL
#define NOT_NULL(s) if (s==NULL) throw dmlite::DmException(DM_NULL_POINTER, #s" is a NULL pointer")

/// Plugin manager handle for C API.
struct dmlite_manager {
  dmlite::PluginManager* manager;
  int                    errorCode;
  std::string            errorString;
};

/// Context handle for C API.
struct dmlite_context {
  dmlite::StackInstance* stack;
  int                    errorCode;
  std::string            errorString;
};

/// dmlite_any
struct dmlite_any {
  boost::any value;
};

/// dmlite_any_dict
struct dmlite_any_dict {
  dmlite::Extensible extensible;
};

/// Directory handlers
struct dmlite_dir {
  dmlite::Directory* dir;
  dmlite_xstat       xstat;
};

struct dmlite_idir {
  dmlite::IDirectory* dir;
  dmlite_xstat        xstat;
};

/// Copies a ExtendedStat to a dmlite_xstat
inline void dmlite_cppxstat_to_cxstat(const dmlite::ExtendedStat& in,
                                      dmlite_xstat* out)
{
  std::strncpy(out->acl,       in.acl.serialize().c_str(), sizeof(out->acl));
  std::strncpy(out->csumtype,  in.csumtype.c_str(),  sizeof(out->csumtype));
  std::strncpy(out->csumvalue, in.csumvalue.c_str(), sizeof(out->csumvalue));
  std::strncpy(out->guid,      in.guid.c_str(),      sizeof(out->guid));
  std::strncpy(out->name,      in.name.c_str(),      sizeof(out->name));
  out->parent = in.parent;
  out->stat   = in.stat;
  out->status = static_cast<dmlite_file_status>(in.status);
  
  if (out->extra != NULL) {
    out->extra->extensible.clear();
    out->extra->extensible.copy(in);
  }
}

/// Copies a dmlite_xstat to a ExtendedStat
inline void dmlite_cxstat_to_cppxstat(const dmlite_xstat* in,
                                      dmlite::ExtendedStat* out)
{
  out->acl       = dmlite::Acl(in->acl);
  out->csumtype  = in->csumtype;
  out->csumvalue = in->csumvalue;
  out->guid      = in->guid;
  out->name      = in->name;
  out->parent    = in->parent;
  out->stat      = in->stat;
  out->status    = static_cast<dmlite::ExtendedStat::FileStatus>(in->status);
}

/// Copies a Replica to a dmlite_replica
inline void dmlite_cppreplica_to_creplica(const dmlite::Replica& in,
                                          dmlite_replica* out)
{
    out->atime  = in.atime;
    out->fileid = in.fileid;
    out->ltime  = in.ltime;
    out->nbaccesses = in.nbaccesses;
    out->ptime  = in.ptime;
    out->replicaid  = in.replicaid;
    out->status = static_cast<dmlite_replica_status>(in.status);
    out->type   = static_cast<dmlite_replica_type>(in.type);
    strncpy(out->rfn, in.rfn.c_str(), sizeof(out->rfn));
    strncpy(out->server, in.server.c_str(), sizeof(out->server));
    
    if (out->extra != NULL) {
      out->extra->extensible.clear();
      out->extra->extensible.copy(in);
    }
}

/// Copies a dmlite_replica to a Replica
inline void dmlite_creplica_to_cppreplica(const dmlite_replica* in,
                                          dmlite::Replica* out)
{
  out->atime      = in->atime;
  out->fileid     = in->fileid;
  out->ltime      = in->ltime;
  out->nbaccesses = in->nbaccesses;
  out->ptime      = in->ptime;
  out->replicaid  = in->replicaid;
  out->rfn    = in->rfn;
  out->server = in->server;
  out->status = static_cast<dmlite::Replica::ReplicaStatus>(in->status);
  out->type   = static_cast<dmlite::Replica::ReplicaType>(in->type);
  
  if (in->extra != NULL)
    out->copy(in->extra->extensible);  
}

#endif	/* PRIVATE_H */
