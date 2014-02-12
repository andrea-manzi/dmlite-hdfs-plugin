/// @file   include/dmlite/cpp/exceptions.h
/// @brief  Exceptions used by the API
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#ifndef DMLITE_CPP_EXCEPTIONS_H
#define DMLITE_CPP_EXCEPTIONS_H

#include <cstdarg>
#include <exception>
#include <string>
#include "../common/errno.h"

namespace dmlite {

/// Base exception class
class DmException: public std::exception {
public:
  DmException();
  DmException(int code);
  DmException(int code, const std::string &string);
  DmException(int code, const char* fmt, va_list args);
  DmException(int code, const char* fmt, ...);

  DmException(const DmException &de);
  
  virtual ~DmException() throw();

  int         code() const throw();
  const char* what() const throw();

protected:
  int         errorCode_;
  std::string errorMsg_;

  void setMessage(const char* fmt, va_list args);
};

};

#endif // DMLITE_CPP_EXCEPTIONS_H
