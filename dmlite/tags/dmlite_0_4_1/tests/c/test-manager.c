#include <dmlite/c/catalog.h>
#include <dmlite/c/dmlite.h>
#include <dmlite/common/errno.h>
#include "utils.h"


int main(int argn, char** argv)
{
  dmlite_manager* manager;
  dmlite_context* context;
  
  printf("DMLite API Version: %d\n", dmlite_api_version());
  
  /* Creation */
  SECTION("Creation");
  manager = dmlite_manager_new();
  TEST_ASSERT(manager != NULL);
  
  SECTION("Loading errors")
  TEST_ASSERT_EQUAL(DM_NO_SUCH_FILE, 
                    dmlite_manager_load_plugin(manager, "/does/not/exist", "plugin_something"));
  TEST_ASSERT_EQUAL(DM_NO_SUCH_SYMBOL,
                    dmlite_manager_load_plugin(manager, "./plugin_mock.so", "plugin_fake"));
  
  /* Load the mock plug-in */  
  TEST_MANAGER_CALL(manager, dmlite_manager_load_plugin, "./plugin_mock.so", "plugin_mock");
  
  /* Set an unknown option */
  TEST_ASSERT_EQUAL(DM_UNKNOWN_OPTION, dmlite_manager_set(manager, "Option", "Value"));
  
  /* Instantiate */
  context = dmlite_context_new(manager);
  TEST_ASSERT(context != NULL);
  if (context == NULL) {
    log_failure("%s", dmlite_manager_error(manager));
    exit(1);
  }
  
  /* Doing anything must fail because the credentials weren't set */
  struct stat s;
  TEST_ASSERT_EQUAL(DM_NO_SECURITY_CONTEXT, dmlite_stat(context, "/", &s));

  /* Clean-up */
  dmlite_context_free(context);
  dmlite_manager_free(manager);
  
  return TEST_FAILURES;
}
