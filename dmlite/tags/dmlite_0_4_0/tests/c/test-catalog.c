#include <dmlite/c/dmlite.h>
#include <dmlite/c/catalog.h>
#include <errno.h>
#include <unistd.h>
#include "utils.h"



void testChDir(dmlite_context* context)
{
  char buffer[64];
  memset(buffer, 0, sizeof(buffer));
  
  SECTION("Test chdir/getcwd");
  
  TEST_CONTEXT_CALL(context, dmlite_chdir, "/usr");
  TEST_ASSERT_EQUAL(buffer, dmlite_getcwd(context, buffer, sizeof(buffer)));
  TEST_ASSERT_STR_EQUAL("/usr", buffer);
  TEST_CONTEXT_CALL(context, dmlite_chdir, "/");
  TEST_ASSERT_EQUAL(buffer, dmlite_getcwd(context, buffer, sizeof(buffer)));
  TEST_ASSERT_STR_EQUAL("/", buffer);
}



void testStat(dmlite_context* context)
{
  struct stat  s;
  dmlite_xstat sx;
  char         buffer[64];
  
  memset(&s,  0, sizeof(s));
  memset(&sx, 0, sizeof(sx));
  
  SECTION("Test stat");
  
  TEST_CONTEXT_CALL(context, dmlite_stat, "/etc/hosts", &s);
  TEST_ASSERT_EQUAL(0, s.st_uid);
  TEST_ASSERT_EQUAL(0, s.st_gid);
  
  SECTION("Extended stat with no metadata");
  
  TEST_CONTEXT_CALL(context, dmlite_statx, "/etc/hosts", &sx);
  TEST_ASSERT_EQUAL(s.st_ino, sx.stat.st_ino);
  TEST_ASSERT_STR_EQUAL("hosts", sx.name);
  TEST_ASSERT_EQUAL(NULL, sx.extra);
  
  SECTION("Extended stat with metadata");
  
  sx.extra = dmlite_any_dict_new();
  TEST_CONTEXT_CALL(context, dmlite_statx, "/etc/hosts", &sx);
  dmlite_any* extra = dmlite_any_dict_get(sx.extra, "easter");
  if (extra) {
    dmlite_any_to_string(extra, buffer, sizeof(buffer));
    TEST_ASSERT_STR_EQUAL("egg", buffer);
    dmlite_any_free(extra);
  }
  dmlite_any_dict_free(sx.extra);
}



void testReadDir(dmlite_context* context)
{
  dmlite_dir*   d;
  dmlite_xstat* sx;
  char          buffer[64];
  
  SECTION("Test read dir")
  
  TEST_CONTEXT_CALL_PTR(d, context, dmlite_opendir, "/usr/");
  
  while ((sx = dmlite_readdirx(context, d)) != NULL) {
    log_dump("/usr/%s", sx->name);
    dmlite_any* extra = dmlite_any_dict_get(sx->extra, "easter");
    if (extra) {
      dmlite_any_to_string(extra, buffer, sizeof(buffer));
      TEST_ASSERT_STR_EQUAL("egg", buffer);
      dmlite_any_free(extra);
    }
  }
  TEST_ASSERT_EQUAL(0, dmlite_errno(context));
  
  TEST_CONTEXT_CALL(context, dmlite_closedir, d);
}



void testCreateAndModify(dmlite_context* context)
{
  dmlite_xstat sx;
  
  memset(&sx, 0, sizeof(sx));
  
  SECTION("Create");
  dmlite_umask(context, 0000);
  
  TEST_CONTEXT_CALL(context, dmlite_mkdir, "/tmp/test-catalog", 0755);
  TEST_ASSERT_EQUAL(ENOENT,
                    dmlite_statx(context, "/tmp/test-catalog/file", &sx))
  
  TEST_CONTEXT_CALL(context, dmlite_create, "/tmp/test-catalog/file", 0755);
  TEST_CONTEXT_CALL(context, dmlite_statx, "/tmp/test-catalog/file", &sx);
  
  TEST_ASSERT_STR_EQUAL("file", sx.name);
  TEST_ASSERT_EQUAL(getuid(), sx.stat.st_uid);
  TEST_ASSERT_EQUAL(0755, (sx.stat.st_mode & ~S_IFMT));
  
  SECTION("Change mode");
  
  TEST_CONTEXT_CALL(context, dmlite_chmod, "/tmp/test-catalog/file", 0700);
  TEST_CONTEXT_CALL(context, dmlite_statx, "/tmp/test-catalog/file", &sx);
  TEST_ASSERT_EQUAL(0700, (sx.stat.st_mode & ~S_IFMT));
  
  SECTION("Rename");
  TEST_CONTEXT_CALL(context, dmlite_rename, "/tmp/test-catalog/file",
                                            "/tmp/test-catalog/file-2");
  
  SECTION("Remove");
  
  TEST_ASSERT_EQUAL(ENOTEMPTY, dmlite_rmdir(context, "/tmp/test-catalog"));
  TEST_ASSERT_EQUAL(ENOENT,    dmlite_unlink(context, "/tmp/test-catalog/file"));
  TEST_CONTEXT_CALL(context, dmlite_unlink, "/tmp/test-catalog/file-2");
  TEST_CONTEXT_CALL(context, dmlite_rmdir, "/tmp/test-catalog");
}



void testReplicas(dmlite_context* context)
{
  struct stat    s;
  dmlite_replica replica, *replicas;
  unsigned       nReplicas;
  
  /* Initialize */
  TEST_CONTEXT_CALL(context, dmlite_stat, "/etc/hosts", &s);
  
  replica.extra  = dmlite_any_dict_new();
  replica.fileid = s.st_ino;
  strcpy(replica.server, "disk.cern.ch");
  strcpy(replica.rfn,    "/storage/replica.01");
  replica.status = kAvailable;
  replica.type   = kPermanent;
  
  dmlite_any* any = dmlite_any_new_string("value");
  dmlite_any_dict_insert(replica.extra, "additional", any);
  dmlite_any_free(any);
  
  /* Create */
  SECTION("Add replica");
  TEST_CONTEXT_CALL(context, dmlite_addreplica, &replica);
  
  /* Get all */
  SECTION("Get replicas");
  TEST_CONTEXT_CALL(context, dmlite_getreplicas, "/etc/hosts",
                    &nReplicas, &replicas);
  
  TEST_ASSERT_EQUAL(1, nReplicas);
  TEST_ASSERT_STR_EQUAL("disk.cern.ch",        replicas[0].server);
  TEST_ASSERT_STR_EQUAL("/storage/replica.01", replicas[0].rfn);
  TEST_ASSERT_EQUAL(kAvailable, replicas[0].status);
  TEST_ASSERT_EQUAL(kPermanent, replicas[0].type);
  
  any = dmlite_any_dict_get(replicas[0].extra, "additional");
  TEST_ASSERT_NOT_EQUAL(NULL, any);
  if (any) {
    char buffer[64];
    dmlite_any_to_string(any, buffer, sizeof(buffer));
    TEST_ASSERT_STR_EQUAL("value", buffer);
    dmlite_any_free(any);
  }
  
  TEST_CONTEXT_CALL(context, dmlite_replicas_free, nReplicas, replicas);
  
  /* Modify */
  SECTION("Modify replica");
  
  TEST_CONTEXT_CALL(context, dmlite_getreplica, "/storage/replica.01", &replica);
  TEST_ASSERT_STR_EQUAL("disk.cern.ch",        replica.server);
  TEST_ASSERT_STR_EQUAL("/storage/replica.01", replica.rfn);
  TEST_ASSERT_EQUAL(kAvailable,    replica.status);
  TEST_ASSERT_EQUAL(kPermanent, replica.type);
  
  any = dmlite_any_dict_get(replica.extra, "additional");
  TEST_ASSERT_NOT_EQUAL(NULL, any);
  if (any) {
    char buffer[64];
    dmlite_any_to_string(any, buffer, sizeof(buffer));
    TEST_ASSERT_STR_EQUAL("value", buffer);
    dmlite_any_free(any);
  }
  
  replica.type  = kVolatile;
  replica.ltime = 55;
  any = dmlite_any_new_string("new one");
  dmlite_any_dict_insert(replica.extra, "new", any);
  dmlite_any_free(any);
  
  TEST_CONTEXT_CALL(context, dmlite_updatereplica, &replica);
  replica.type  = 0;
  replica.ltime = 0;
  dmlite_any_dict_clear(replica.extra);
  
  TEST_CONTEXT_CALL(context, dmlite_getreplica, "/storage/replica.01", &replica);
  TEST_ASSERT_STR_EQUAL("disk.cern.ch",        replica.server);
  TEST_ASSERT_STR_EQUAL("/storage/replica.01", replica.rfn);
  TEST_ASSERT_EQUAL(kAvailable, replica.status);
  TEST_ASSERT_EQUAL(kVolatile,  replica.type);
  TEST_ASSERT_EQUAL(55,         replica.ltime);
  
  any = dmlite_any_dict_get(replica.extra, "new");
  TEST_ASSERT_NOT_EQUAL(NULL, any);
  if (any) {
    char buffer[64];
    dmlite_any_to_string(any, buffer, sizeof(buffer));
    TEST_ASSERT_STR_EQUAL("new one", buffer);
    dmlite_any_free(any);
  }
  
  /* Remove */
  SECTION("Remove replica");
  
  TEST_CONTEXT_CALL(context, dmlite_delreplica, &replica);
  TEST_CONTEXT_CALL(context, dmlite_getreplicas, "/etc/hosts",
                    &nReplicas, &replicas);
  
  TEST_ASSERT_EQUAL(0, nReplicas);
  
  /* Free */
  dmlite_any_dict_free(replica.extra);
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
  testChDir(context);
  testStat(context);
  testReadDir(context);
  testCreateAndModify(context);
  testReplicas(context);
  
  /* Clean-up */
  dmlite_context_free(context);
  dmlite_manager_free(manager);
  
  return TEST_FAILURES;
}
