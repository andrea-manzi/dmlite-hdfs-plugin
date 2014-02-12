#include <dmlite/c/dmlite.h>
#include <dmlite/c/io.h>
#include <fcntl.h>
#include "utils.h"



void testRead(dmlite_context* context)
{
  dmlite_fd*       file;
  dmlite_any_dict* dict = dmlite_any_dict_new();
  char             buffer[64];
  memset(buffer, 0, sizeof(buffer));
  
  SECTION("Read");
  
  /* Bad token */
  file = dmlite_fopen(context, "/file", O_RDONLY, dict);
  TEST_ASSERT_EQUAL(NULL, file);
  TEST_ASSERT_EQUAL(DM_FORBIDDEN, dmlite_errno(context));
          
  /* Open non-existing */
  file = dmlite_fopen(context, "/does-not-exist", O_RDONLY, dict);
  TEST_ASSERT_EQUAL(NULL, file);
  TEST_ASSERT_EQUAL(DM_NO_SUCH_FILE, dmlite_errno(context));
  
  /* Good to open */
  dmlite_any* token = dmlite_any_new_string("123456789");
  dmlite_any_dict_insert(dict, "token", token);
  dmlite_any_free(token);
  TEST_CONTEXT_CALL_PTR(file, context, dmlite_fopen, "/file", O_RDONLY, dict);
  
  /* Read */
  TEST_ASSERT_EQUAL(4, dmlite_fread(file, buffer, 4));
  buffer[5] = '\0';
  TEST_ASSERT_STR_EQUAL("abcd", buffer);
  TEST_ASSERT_EQUAL(4, dmlite_fread(file, buffer, 4));
  buffer[5] = '\0';
  TEST_ASSERT_STR_EQUAL("efgh", buffer);
  
  /* Seek and read */
  TEST_ASSERT_EQUAL(0, dmlite_fseek(file, 10, SEEK_SET));
  TEST_ASSERT_EQUAL(4, dmlite_fread(file, buffer, 4));
  buffer[5] = '\0';
  TEST_ASSERT_STR_EQUAL("klmn", buffer);
  
  TEST_ASSERT_EQUAL(14, dmlite_ftell(file));
  
  TEST_ASSERT_EQUAL(0, dmlite_feof(file));
  
  /* Free */
  TEST_ASSERT_EQUAL(0, dmlite_fclose(file));
  dmlite_any_dict_free(dict);
}



void testWrite(dmlite_context* context)
{
  dmlite_fd*       file;
  dmlite_any_dict* dict = dmlite_any_dict_new();
  char             buffer[64];
  
  SECTION("Read");
  
  /* Good to open */
  dmlite_any* token = dmlite_any_new_string("987654321");
  dmlite_any_dict_insert(dict, "token", token);
  dmlite_any_free(token);
  TEST_CONTEXT_CALL_PTR(file, context, dmlite_fopen, "/file", O_RDWR, dict);
  
  /* Write a chunk */
  TEST_ASSERT_EQUAL(10, dmlite_fwrite(file, "123456789", 10));
  
  /* Seek and read it back */
  TEST_ASSERT_EQUAL(0, dmlite_fseek(file, 0, SEEK_SET));
  TEST_ASSERT_EQUAL(10, dmlite_fread(file, buffer, 10));
  TEST_ASSERT_STR_EQUAL("123456789", buffer);
  
  
  /* Free */
  TEST_ASSERT_EQUAL(0, dmlite_fclose(file));
  dmlite_any_dict_free(dict);
}



int main(int argn, char** argv)
{
  dmlite_manager* manager;
  dmlite_context* context;
  
  SECTION("Instantiation");
  
  /* Load the mock plug-in */
  manager = dmlite_manager_new();
  TEST_MANAGER_CALL(manager, dmlite_manager_load_plugin, "./plugin_mock.so", "plugin_mock");
  
  /* Instantiate */
  context = dmlite_context_new(manager);
  
  /* Set credentials (mock will ignore anyway) */
  dmlite_credentials creds;
  memset(&creds, 0, sizeof(creds));
  creds.client_name = "root";
  TEST_CONTEXT_CALL(context, dmlite_setcredentials, &creds);
  
  /* Do tests */
  testRead(context);
  testWrite(context);
  
  /* Clean-up */
  dmlite_context_free(context);
  dmlite_manager_free(manager);
  
  return TEST_FAILURES;
}

