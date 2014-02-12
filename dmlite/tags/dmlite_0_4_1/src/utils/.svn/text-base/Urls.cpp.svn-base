/// @file   utils/Urls.cpp
/// @brief  Common methods and functions for URI's.
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#include <boost/regex.hpp>
#include <cstdlib>
#include <cstring>
#include <dmlite/cpp/utils/urls.h>

using namespace dmlite;



Url::Url(const std::string& url) throw (): port(0) 
{
  boost::regex regexp("(([[:alnum:]]+):/{2})?([[:alnum:]][-_[:alnum:]]*(\\.[-_[:alnum:]]+)*)?(:[[:digit:]]*)?([^?]*)?(.*)",
                      boost::regex::extended | boost::regex::icase);
  boost::smatch what;
  
  if (boost::regex_match(url, what, regexp, boost::match_posix)) {
    this->scheme = what[2];
    this->domain = what[3];
    
    std::string portStr(what[5]);
    if (portStr.length() > 1)
      this->port = std::atol(portStr.c_str() + 1);
    
    this->path   = what[6];
    this->query  = what[7];
    if (!this->query.empty())
      this->query = this->query.substr(1);
  }
}



bool Url::operator == (const Url& u) const
{
  return this->domain == u.domain &&
         this->path   == u.path &&
         this->port   == u.port &&
         this->query  == u.query &&
         this->scheme == u.scheme;
}



bool Url::operator != (const Url& u) const
{
  return !(*this == u);
}



bool Url::operator <  (const Url& u) const
{
  if (this->scheme < u.scheme)
    return true;
  else if (this->scheme > u.scheme)
    return false;
  else {
    if (this->domain < u.domain)
      return true;
    else if (this->domain > u.domain)
      return false;
    else {
      if (this->port < u.port)
        return true;
      else if (this->port > u.port)
        return false;
      else {
        if (this->path < u.path)
          return true;
        else if (this->path > u.path)
          return false;
        else {
          if (this->query < u.query)
            return true;
          else if (this->query > u.query)
            return false;
          else {
            return this->scheme < u.scheme;
          }
        }
      }
    }
  }
}



bool Url::operator >  (const Url& u) const
{
  return (*this < u);
}



std::vector<std::string> Url::splitPath(const std::string& path) throw()
{
  std::vector<std::string> components;
  size_t s, e;
  
  if (!path.empty() && path[0] == '/')
    components.push_back("/");

  s = path.find_first_not_of('/');
  while (s != std::string::npos) {
    e = path.find('/', s);
    if (e != std::string::npos) {
      components.push_back(path.substr(s, e - s));
      s = path.find_first_not_of('/', e);
    }
    else {
      components.push_back(path.substr(s));
      s = e;
    }
  }

  return components;
}



std::string Url::joinPath(const std::vector<std::string>& components) throw()
{
  std::vector<std::string>::const_iterator i;
  std::string path;

  for (i = components.begin(); i != components.end(); ++i) {
    if (*i != "/")
      path += *i + "/";
    else
      path += "/";
  }
  
  if (!path.empty())
    path.erase(--path.end());
    
  return path;
}



std::string Url::normalizePath(const std::string& path) throw ()
{
  std::vector<std::string> components = Url::splitPath(path);
  std::string              result;
  
  result.reserve(path.length());
  
  unsigned i;
  if (components[0] == "/") {
    i      = 1;
    result = "/";
  } else {
    i = 0;
  }
  
  for ( ; i < components.size(); ++i) {
    result.append(components[i]);
    if (i < components.size() - 1)
      result.append("/");
  }
  
  if (components.size() > 1 && path[path.length() - 1] == '/')
    result.append("/");
  
  return result;
}
