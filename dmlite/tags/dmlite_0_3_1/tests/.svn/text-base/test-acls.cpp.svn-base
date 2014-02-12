#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <sys/stat.h>
#include <vector>
#include "test-base.h"
#include "dmlite/cpp/utils/dm_security.h"

class TestAcls: public TestBase
{
protected:
  const static char *FOLDER;
  const static char *NESTED;
  const static char *FILE;

  std::string ACL_STRING;

  const dmlite::SecurityContext* ctx;

public:

  void setUp()
  {
    TestBase::setUp();

    this->stackInstance->setSecurityCredentials(cred1);

    ctx = this->stackInstance->getSecurityContext();

    std::stringstream ss;
    ss << "A7" << ctx->getUser().uid << ",C0" << ctx->getGroup(0).gid << ",E70,F50";
    ACL_STRING = ss.str();

    this->catalog->makeDir(FOLDER, 0700);
    this->catalog->setAcl(FOLDER, dmlite::deserializeAcl(ACL_STRING));
  }

  void tearDown()
  {
    if (this->catalog != 0x00) {
      IGNORE_NOT_EXIST(this->catalog->changeMode(FOLDER, 0700));
      IGNORE_NOT_EXIST(this->catalog->unlink(FILE));
      IGNORE_NOT_EXIST(this->catalog->removeDir(NESTED));
      IGNORE_NOT_EXIST(this->catalog->removeDir(FOLDER));
    }
    TestBase::tearDown();
  }
  
  void testAcl()
  {
    ExtendedStat stat = this->catalog->extendedStat(FOLDER);
    CPPUNIT_ASSERT_EQUAL(ACL_STRING,
                         std::string(stat.acl));
  }

  void testChown()
  {
    this->catalog->changeOwner(FOLDER, -1, ctx->getGroup(1).gid);
    ExtendedStat stat = this->catalog->extendedStat(FOLDER);
    std::vector<Acl> acls = dmlite::deserializeAcl(stat.acl);

    CPPUNIT_ASSERT_EQUAL((size_t)4, acls.size());

    CPPUNIT_ASSERT_EQUAL(ACL_USER_OBJ,  (int)acls[0].type);
    CPPUNIT_ASSERT_EQUAL(ctx->getUser().uid, acls[0].id);
    CPPUNIT_ASSERT_EQUAL(7,             (int)acls[0].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_GROUP_OBJ,   (int)acls[1].type);
    CPPUNIT_ASSERT_EQUAL(ctx->getGroup(1).gid, acls[1].id);
    CPPUNIT_ASSERT_EQUAL(0,               (int)acls[1].perm);
  }

  void testChmod()
  {
    this->catalog->changeMode(FOLDER, 0555);
    ExtendedStat stat = this->catalog->extendedStat(FOLDER);
    std::vector<Acl> acls = dmlite::deserializeAcl(stat.acl);

    CPPUNIT_ASSERT_EQUAL((size_t)4, acls.size());

    CPPUNIT_ASSERT_EQUAL(ACL_USER_OBJ,  (int)acls[0].type);
    CPPUNIT_ASSERT_EQUAL(ctx->getUser().uid, acls[0].id);
    CPPUNIT_ASSERT_EQUAL(5,             (int)acls[0].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_GROUP_OBJ,   (int)acls[1].type);
    CPPUNIT_ASSERT_EQUAL(ctx->getGroup(0).gid, acls[1].id);
    CPPUNIT_ASSERT_EQUAL(5,               (int)acls[1].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_OTHER, (int)acls[3].type);
    CPPUNIT_ASSERT_EQUAL(5,         (int)acls[3].perm);
  }

  void testInherit()
  {
    // Set default ACL's
    this->catalog->setAcl(FOLDER, dmlite::deserializeAcl("A70,C00,E70,F50,a60,c50,f00"));
    // Make dir
    this->catalog->makeDir(NESTED, 0700);
    ExtendedStat stat = this->catalog->extendedStat(NESTED);

    // Assert resulting ACL
    std::stringstream acl;
    acl << "A6" << ctx->getUser().uid << ",C0" << ctx->getGroup(0).gid << ",F00,a60,c50,f00";
    CPPUNIT_ASSERT_EQUAL(acl.str(), std::string(stat.acl));
  }

  CPPUNIT_TEST_SUITE(TestAcls);
  CPPUNIT_TEST(testAcl);
  CPPUNIT_TEST(testChown);
  CPPUNIT_TEST(testChmod);
  CPPUNIT_TEST(testInherit);
  CPPUNIT_TEST_SUITE_END();
};

const char* TestAcls::FOLDER = "test-acls";
const char* TestAcls::NESTED = "test-acls/nested";
const char* TestAcls::FILE   = "test-acls/file";

CPPUNIT_TEST_SUITE_REGISTRATION(TestAcls);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
