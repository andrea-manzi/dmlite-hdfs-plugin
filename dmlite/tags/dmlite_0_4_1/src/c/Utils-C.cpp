/// @file   c/PluginManager-C.cpp
/// @brief  C wrapper for dmlite::PluginManager.
/// @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
#include <cstring>
#include <dmlite/c/utils.h>
#include <dmlite/cpp/utils/security.h>
#include <dmlite/cpp/utils/urls.h>



void dmlite_parse_url(const char* source, dmlite_url* dest)
{
  dmlite::Url url(source);
  
  std::strncpy(dest->domain, url.domain.c_str(), sizeof(dest->domain));
  std::strncpy(dest->path,   url.path.c_str(),   sizeof(dest->path));
  std::strncpy(dest->query,  url.query.c_str(),  sizeof(dest->query));
  std::strncpy(dest->scheme, url.scheme.c_str(), sizeof(dest->scheme));
  dest->port = url.port;
}



void dmlite_serialize_acls(unsigned nEntries, dmlite_aclentry* acl,
                           char* buffer, size_t bsize)
{
  dmlite::Acl aclpp;
  
  for (unsigned i = 0; i < nEntries; ++i) {
    dmlite::AclEntry e;
    
    e.id   = acl[i].id;
    e.perm = acl[i].perm;
    e.type = acl[i].type;
    
    aclpp.push_back(e);
  }
  
  std::strncpy(buffer, aclpp.serialize().c_str(), bsize);
}

void dmlite_deserialize_acl(const char* buffer, unsigned* nEntries,
                             dmlite_aclentry** acl)
{
  dmlite::Acl aclpp(buffer);
  *nEntries = aclpp.size();
  
  *acl = new dmlite_aclentry[*nEntries];
  
  for (unsigned i = 0; i < *nEntries; ++i) {
    (*acl)[i].id   = aclpp[i].id;
    (*acl)[i].perm = aclpp[i].perm;
    (*acl)[i].type = aclpp[i].type;
  }
}



void dmlite_acl_free(unsigned nEntries, struct dmlite_aclentry* acl)
{
  delete [] acl;
}
