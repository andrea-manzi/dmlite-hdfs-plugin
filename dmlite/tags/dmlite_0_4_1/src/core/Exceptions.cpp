/// @file   core/Exceptions.cpp
/// @brief  Exceptions used by the API
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <dmlite/cpp/exceptions.h>

using namespace dmlite;

DmException::DmException(): std::exception(), errorCode_(0), errorMsg_()
{
  // Nothing
}



DmException::DmException(int code): errorCode_(code), errorMsg_()
{
  // Nothing
}



DmException::DmException(int code, const std::string& string): errorCode_(code), errorMsg_(string)
{
  // Nothing
}



DmException::DmException(int code, const char* fmt, ...): errorCode_(code)
{
  va_list args;

  va_start(args, fmt);
  this->setMessage(fmt, args);
  va_end(args);
}



DmException::DmException(int code, const char* fmt, va_list args): errorCode_(code)
{
  this->setMessage(fmt, args);
}



DmException::DmException(const DmException &base)
{
  this->errorCode_ = base.errorCode_;
  this->errorMsg_  = base.errorMsg_;
}



DmException::~DmException() throw()
{
  // Nothing
}



int DmException::code() const throw()
{
  return this->errorCode_;
}



const char* DmException::what() const throw()
{
  return this->errorMsg_.c_str();
}



void DmException::setMessage(const char* fmt, va_list args)
{
  char buffer[512];
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  this->errorMsg_ = std::string(buffer);
}
