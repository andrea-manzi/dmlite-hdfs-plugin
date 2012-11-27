/// @file   Throw.cpp
/// @brief  Maps the errno codes to DM_ERROR_*
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#include <dmlite/cpp/exceptions.h>
#include <errno.h>

#include "HdfsNS.h"



void dmlite::ThrowExceptionFromErrno(int err, const char* extra) throw(DmException)
{
  if (extra == 0) extra = "";
  throw DmException(err, "%s: %s", strerror(err), extra);
}



int dmlite::wrapCall(int r) throw (DmException)
{
  if (r < 0)
    ThrowExceptionFromErrno(errno);
  return r;
}



void* dmlite::wrapCall(void* p) throw (DmException)
{
  if (p == NULL && errno != 0)
    ThrowExceptionFromErrno(errno);
  return p;
}
