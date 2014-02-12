/// @file   core/IO-C.cpp
/// @brief  C wrapper for dmlite::IOFactory.
/// @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
#include <dmlite/c/io.h>
#include <dmlite/c/dmlite.h>
#include <dmlite/cpp/io.h>
#include <fcntl.h>
#include "Private.h"

struct dmlite_fd {
  dmlite_context*        context;
  dmlite::IOHandler* stream;
};



dmlite_fd* dmlite_fopen(dmlite_context* context, const char* path, int flags,
                        const dmlite_any_dict* extra)
{
  TRY(context, fopen)
  NOT_NULL(path);
  
  dmlite::IOHandler* stream;
  if (extra != NULL)
    stream = context->stack->getIODriver()->
                              createIOHandler(path, flags, extra->extensible);
  else
    stream = context->stack->getIODriver()->
                              createIOHandler(path, flags, dmlite::Extensible());
  
  dmlite_fd* iofd = new dmlite_fd();
  iofd->context = context;
  iofd->stream  = stream;
  return iofd;
  CATCH_POINTER(context, fopen)
}



int dmlite_fclose(dmlite_fd* fd)
{
  if (!fd)
    return DM_NULL_POINTER;
  TRY(fd->context, fclose)
  NOT_NULL(fd);
  fd->stream->close();
  delete fd->stream;
  delete fd;
  CATCH(fd->context, fclose)
}



int dmlite_fseek(dmlite_fd* fd, long offset, int whence)
{
  if (!fd)
    return DM_NULL_POINTER;
  TRY(fd->context, fseek)
  NOT_NULL(fd);
  fd->stream->seek(offset, static_cast<dmlite::IOHandler::Whence>(whence));
  CATCH(fd->context, fseek)
}



long dmlite_ftell(dmlite_fd* fd)
{
  if (!fd)
    return DM_NULL_POINTER;
  TRY(fd->context, ftell)
  NOT_NULL(fd);
  return fd->stream->tell();
  CATCH(fd->context, ftell)
}



size_t dmlite_fread(dmlite_fd* fd, void* buffer, size_t count)
{
  if (!fd)
    return DM_NULL_POINTER;
  TRY(fd->context, fread)
  NOT_NULL(fd);
  NOT_NULL(buffer);
  return fd->stream->read((char*)buffer, count);
  CATCH(fd->context, fread)
}



size_t dmlite_fwrite(dmlite_fd* fd, const void* buffer, size_t count)
{
  if (!fd)
    return DM_NULL_POINTER;
  TRY(fd->context, fwrite)
  NOT_NULL(fd);
  NOT_NULL(buffer);
  return fd->stream->write((char*)buffer, count);
  CATCH(fd->context, fwrite)
}



int dmlite_feof(dmlite_fd* fd)
{
  if (!fd)
    return DM_NULL_POINTER;
  TRY(fd->context, feof)
  NOT_NULL(fd);
  return fd->stream->eof();
  CATCH(fd->context, feof)
}



int dmlite_donewriting(dmlite_context* context,
                       const char* pfn,
                       const dmlite_any_dict* extra)
{
  TRY(context, donewriting)
  NOT_NULL(pfn);  
  if (extra != NULL)
    context->stack->getIODriver()->doneWriting(pfn, extra->extensible);
  else
    context->stack->getIODriver()->doneWriting(pfn, extra->extensible);
  CATCH(context, donewriting)
}
