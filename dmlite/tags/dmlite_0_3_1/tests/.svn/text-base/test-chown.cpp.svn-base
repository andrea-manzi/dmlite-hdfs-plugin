#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <unistd.h>
#include <sys/stat.h>
#include "test-base.h"

class TestChown: public TestBase
{
protected:
  const static char *FILE;
  const dmlite::SecurityContext* ctx;

public:

  void setUp()
  {
    TestBase::setUp();
    this->stackInstance->setSecurityCredentials(cred1);
    this->catalog->create(FILE, 0755);
  }

  void tearDown()
  {
    if (this->catalog) {
      this->stackInstance->setSecurityContext(root);
      IGNORE_NOT_EXIST(this->catalog->unlink(FILE));
    }

    TestBase::tearDown();
  }

  void testRoot()
  {
    // Root should be able to change the owner and the group
    this->stackInstance->setSecurityContext(root);
    this->catalog->changeOwner(FILE, 500, 200);

    struct stat s = this->catalog->extendedStat(FILE).stat;

    CPPUNIT_ASSERT_EQUAL(500, (int)s.st_uid);
    CPPUNIT_ASSERT_EQUAL(200, (int)s.st_gid);
  }

  void testOwner()
  {
    struct stat s;

    ctx = this->stackInstance->getSecurityContext();

    // It should be OK a -1, -1
    this->catalog->changeOwner(FILE, -1, -1);
    s = this->catalog->extendedStat(FILE).stat;
    CPPUNIT_ASSERT_EQUAL(ctx->getUser().uid, s.st_uid);
    CPPUNIT_ASSERT_EQUAL(ctx->getGroup().gid, s.st_gid);

    // Also, it should be ok to change to current value
    this->catalog->changeOwner(FILE, s.st_uid, s.st_gid);
    s = this->catalog->extendedStat(FILE).stat;

    // It should NOT be able to change the user
    try {
      this->catalog->changeOwner(FILE, ctx->getUser().uid, -1);
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_BAD_OPERATION, e.code());
    }

    // It should BE able to change the group to one it belongs to
    this->catalog->changeOwner(FILE, -1, ctx->getGroup(1).gid);
    s = this->catalog->extendedStat(FILE).stat;
    CPPUNIT_ASSERT_EQUAL(s.st_uid, s.st_uid);
    CPPUNIT_ASSERT_EQUAL(ctx->getGroup(1).gid, s.st_gid);

    // It should NOT be able to change the group to one it does not belong to
    try {
      this->catalog->changeOwner(FILE, -1, 0);
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_BAD_OPERATION, e.code());
    }
  }

  void testOther()
  {
    // Change the user
    this->stackInstance->setSecurityCredentials(cred2);
    ctx = this->stackInstance->getSecurityContext();

    // It should NOT be able to change the group or the owner
    try {
      this->catalog->changeOwner(FILE, ctx->getUser().uid, -1);
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_BAD_OPERATION, e.code());
    }

    try {
      this->catalog->changeOwner(FILE, -1, ctx->getGroup(0).gid);
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_BAD_OPERATION, e.code());
    }
  }

  void testCTime()
  {
    this->stackInstance->setSecurityContext(root);
    // First stat
    struct stat before = this->catalog->extendedStat(FILE).stat;
    // Make sure the clock moves!
    sleep(1);
    // Change the owner
    this->catalog->changeOwner(FILE, 500, 200);
    // Second stat
    struct stat after = this->catalog->extendedStat(FILE).stat;
    // ctime should have incremented
    CPPUNIT_ASSERT(before.st_ctime < after.st_ctime);
  }

  CPPUNIT_TEST_SUITE(TestChown);
  CPPUNIT_TEST(testRoot);
  CPPUNIT_TEST(testOwner);
  CPPUNIT_TEST(testOther);
  CPPUNIT_TEST(testCTime);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestChown);

const char* TestChown::FILE        = "test-chown";

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}

