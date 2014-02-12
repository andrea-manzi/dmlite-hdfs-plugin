#include <dmlite/c/inode.h>
#include <dmlite/cpp/inode.h>
#include "../../src/c/Private.h"
#include "utils.h"

int main(int, char**)
{
  dmlite::ExtendedStat cppX;
  dmlite_xstat         cX;
  
  ////
  SECTION("Test dmlite::ExtendedStat => dmlite_xstat");
  
  cppX.name      = "test-file";
  cppX.parent    = 155;
  cppX.csumtype  = "MD";
  cppX.csumvalue = "123456789";
  cppX.guid      = "45-8900A-CX";
  cppX.status    = dmlite::ExtendedStat::kMigrated;
  cppX.stat.st_mode = S_IFDIR | 0750;
  cppX.stat.st_uid  = 101;
  cppX.stat.st_gid  = 102;
  
  dmlite_cppxstat_to_cxstat(cppX, &cX);
  
  TEST_ASSERT_STR_EQUAL(cppX.name.c_str(), cX.name);
  TEST_ASSERT_EQUAL(cppX.parent, cX.parent);
  TEST_ASSERT_STR_EQUAL(cppX.csumtype.c_str(), cX.csumtype);
  TEST_ASSERT_STR_EQUAL(cppX.csumvalue.c_str(), cX.csumvalue);
  TEST_ASSERT_STR_EQUAL(cppX.guid.c_str(), cX.guid);
  TEST_ASSERT_EQUAL((int)cppX.status, (int)cX.status);
  TEST_ASSERT_EQUAL((S_IFDIR | 0750), cX.stat.st_mode);  
  TEST_ASSERT_EQUAL(101u, cX.stat.st_uid);
  TEST_ASSERT_EQUAL(102u, cX.stat.st_gid);
  
  ////
  SECTION("Test dmlite_xstat => dmlite::ExtendedStat");
  
  strcpy(cX.name, "c-test-file");
  cX.parent    = 155;
  strcpy(cX.csumtype, "SH");
  strcpy(cX.csumvalue, "9876521");
  strcpy(cX.guid, "4995-DGX");
  cX.status = kOnline;
  cX.stat.st_mode = S_IFDIR | 0500;
  cX.stat.st_uid  = 105;
  cX.stat.st_gid  = 106;
  
  dmlite_cxstat_to_cppxstat(&cX, &cppX);
  
  TEST_ASSERT_STR_EQUAL("c-test-file", cppX.name.c_str());
  TEST_ASSERT_EQUAL(155, cppX.parent);
  TEST_ASSERT_STR_EQUAL("SH", cppX.csumtype.c_str());
  TEST_ASSERT_STR_EQUAL("9876521", cppX.csumvalue.c_str());
  TEST_ASSERT_STR_EQUAL("4995-DGX", cppX.guid.c_str());
  TEST_ASSERT_EQUAL(dmlite::ExtendedStat::kOnline, cppX.status);
  TEST_ASSERT_EQUAL((S_IFDIR | 0500), cppX.stat.st_mode);  
  TEST_ASSERT_EQUAL(105u, cppX.stat.st_uid);
  TEST_ASSERT_EQUAL(106u, cppX.stat.st_gid);
}
