#include <dmlite/c/any.h>
#include "utils.h"

int main(int argn, char** argv)
{
  char             buffer[512];
  dmlite_any*      value;
  dmlite_any_dict* dict;
  
  /* Test integer */
  SECTION("Integer => Any => Integer")
  value = dmlite_any_new_long(42l);
  TEST_ASSERT(value != NULL);
  TEST_ASSERT_EQUAL(42l, dmlite_any_to_long(value));
  dmlite_any_free(value);
  
  /* Test string */
  SECTION("String => Any => String");
  value = dmlite_any_new_string("this is a string");
  TEST_ASSERT(value != NULL);
  dmlite_any_to_string(value, buffer, sizeof(buffer));
  TEST_ASSERT_STR_EQUAL("this is a string", buffer);
  dmlite_any_free(value);

  /* Test dictionary */
  SECTION("Creating dictionary");
  dict = dmlite_any_dict_new();
  TEST_ASSERT(dict != NULL);
  
  SECTION("Pushing values");
  
  value = dmlite_any_new_long(1492l);
  dmlite_any_dict_insert(dict, "long", value);
  dmlite_any_free(value);
  TEST_ASSERT_EQUAL(1, dmlite_any_dict_count(dict));
  
  value = dmlite_any_new_string("something something");
  dmlite_any_dict_insert(dict, "string", value);
  dmlite_any_free(value);
  TEST_ASSERT_EQUAL(2, dmlite_any_dict_count(dict));
  
  SECTION("Getting values");
  
  value = dmlite_any_dict_get(dict, "long");
  TEST_ASSERT_EQUAL(1492l, dmlite_any_to_long(value));
  dmlite_any_free(value);
  
  value = dmlite_any_dict_get(dict, "string");
  dmlite_any_to_string(value, buffer, sizeof(buffer));
  TEST_ASSERT_STR_EQUAL("something something", buffer);
  dmlite_any_free(value);
  
  SECTION("Get keys");
  char**   keys;
  unsigned nKeys;
  dmlite_any_dict_keys(dict, &nKeys, &keys);
  
  TEST_ASSERT_EQUAL(2, nKeys);
  TEST_ASSERT_STR_EQUAL("long", keys[0]);
  TEST_ASSERT_STR_EQUAL("string", keys[1]);
  
  dmlite_any_dict_keys_free(nKeys, keys);
  
  SECTION("JSON OUTPUT");
  dmlite_any_dict_to_json(dict, buffer, sizeof(buffer));
  log_dump("%s", buffer);
  
  
  dmlite_any_dict_free(dict);
  
  /* Deserialize */
  SECTION("Deserializing");
  const char* json = "{\"long\": 13, \"string\": \"dmlite C API\"}";
  log_dump("%s", json);
  
  dict = dmlite_any_dict_from_json(json);
  
  value = dmlite_any_dict_get(dict, "long");
  TEST_ASSERT_EQUAL(13l, dmlite_any_to_long(value));
  dmlite_any_free(value);
  
  value = dmlite_any_dict_get(dict, "string");
  dmlite_any_to_string(value, buffer, sizeof(buffer));
  TEST_ASSERT_STR_EQUAL("dmlite C API", buffer);
  dmlite_any_free(value);
  
  dmlite_any_dict_free(dict);
  
  return TEST_FAILURES;
}
