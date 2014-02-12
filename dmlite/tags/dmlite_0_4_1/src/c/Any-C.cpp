/** @file   c/any.cpp
 *  @brief  Opaque handler to pass different types of values to the API.
 *  @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
 *  @note   Basically it wraps boost::any and dmlite::Extensible.
 */
#include <cstring>
#include <dmlite/c/any.h>
#include <dmlite/cpp/utils/extensible.h>
#include <string>
#include <vector>
#include "Private.h"



dmlite_any* dmlite_any_new_string(const char* str)
{
  dmlite_any* any = new dmlite_any();
  any->value = std::string(str);
  return any;
}



dmlite_any* dmlite_any_new_string_array(unsigned n, const char** strv)
{
  dmlite_any* any = new dmlite_any();
  std::vector<boost::any> av;
  
  for (unsigned i = 0; i < n; ++i)
    av.push_back(std::string(strv[i]));
  
  any->value = av;
  return any;
}



dmlite_any* dmlite_any_new_long(long l)
{
  dmlite_any* any = new dmlite_any();
  any->value = l;
  return any;
}



dmlite_any* dmlite_any_new_long_array(unsigned n, long* lv)
{
  dmlite_any* any = new dmlite_any();
  std::vector<boost::any> av;
  
  for (unsigned i = 0; i < n; ++i)
    av.push_back(lv[i]);
  
  any->value = av;
  return any;
}



void dmlite_any_free(dmlite_any* any)
{
  delete any;
}



void dmlite_any_to_string(const dmlite_any* any, char* buffer, size_t bsize)
{
  std::string str;
  
  try {
    str = dmlite::Extensible::anyToString(any->value);
    std::strncpy(buffer, str.c_str(), bsize);
  }
  catch (...) {
    buffer[0] = '\0';
  }
}



long dmlite_any_to_long(const dmlite_any* any)
{
  try {
    return dmlite::Extensible::anyToLong(any->value);
  }
  catch (...) {
    return 0;
  }
}



dmlite_any_dict* dmlite_any_dict_new()
{
  return new dmlite_any_dict();
}



dmlite_any_dict* dmlite_any_dict_copy(const dmlite_any_dict* dict)
{
  dmlite_any_dict* copy = new dmlite_any_dict();
  copy->extensible.copy(dict->extensible);
  return copy;
}



void dmlite_any_dict_free(dmlite_any_dict* d)
{
  delete d;
}



void dmlite_any_dict_clear(dmlite_any_dict* d)
{
  d->extensible.clear();
}



void dmlite_any_dict_insert(dmlite_any_dict* d, const char* k, const dmlite_any* v)
{
  d->extensible[k] = v->value;
}



unsigned long dmlite_any_dict_count(const dmlite_any_dict* d)
{
  return d->extensible.size();
}



dmlite_any* dmlite_any_dict_get(const dmlite_any_dict* d, const char* k)
{
  if (!d->extensible.hasField(k))
    return NULL;

  dmlite_any* any;
  any = new dmlite_any();
  any->value = d->extensible[k];
  return any;
}



void dmlite_any_dict_erase(dmlite_any_dict* d, const char* k)
{
  d->extensible.erase(k);
}



char* dmlite_any_dict_to_json(const dmlite_any_dict* d, char* buffer, size_t bsize)
{
  strncpy(buffer, d->extensible.serialize().c_str(), bsize);
  return buffer;
}



dmlite_any_dict* dmlite_any_dict_from_json(const char* json)
{
  dmlite_any_dict* dict;
  dict = new dmlite_any_dict();
  try {
    dict->extensible.deserialize(json);
    return dict;
  }
  catch (...) {
    return NULL;
  }
}



void dmlite_any_dict_keys(const dmlite_any_dict* d, unsigned* nkeys, char*** keys)
{
  std::vector<std::string> v = d->extensible.getKeys();
  
  if (v.size() == 0) {
    *nkeys = 0;
    *keys  = NULL;
    return;
  }
  
  *keys = new char*[v.size()];
  for (unsigned i = 0; i < v.size(); ++i) {
    (*keys)[i] = new char[v[i].length() + 1];
    strcpy((*keys)[i], v[i].c_str());
  }
  *nkeys = v.size();
}



void dmlite_any_dict_keys_free(unsigned n, char** keys)
{
  if (keys == NULL)
    return;
  
  for (unsigned i = 0; i < n; ++i)
    delete [] keys[i];
  delete [] keys;
}
