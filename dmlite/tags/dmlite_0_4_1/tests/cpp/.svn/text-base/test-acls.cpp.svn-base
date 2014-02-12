#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <dmlite/cpp/utils/security.h>
#include <sstream>
#include <vector>
#include "test-base.h"


static bool operator == (const dmlite::Acl& a, const dmlite::Acl& b)
{
  return a.serialize() == b.serialize();
}



static std::ostringstream& operator << (std::ostringstream& of, const dmlite::Acl& acl)
{
  of << acl.serialize();
  return of;
}



class TestAcls: public TestBase
{
protected:
  const static char *FOLDER;
  const static char *NESTED;
  const static char *FILE;

  dmlite::Acl ACL;

  const dmlite::SecurityContext* ctx;

public:

  void setUp()
  {
    TestBase::setUp();

    this->stackInstance->setSecurityCredentials(cred1);

    ctx = this->stackInstance->getSecurityContext();

    std::stringstream ss;
    ss << "A7" << getUid(ctx) << ",C0" << getGid(ctx) << ",E70,F50";
    ACL = dmlite::Acl(ss.str());

    this->catalog->makeDir(FOLDER, 0700);
    this->catalog->setAcl(FOLDER, ACL);
  }

  void tearDown()
  {
    if (this->catalog != 0x00) {
      IGNORE_NOT_EXIST(this->catalog->setMode(FOLDER, 0700));
      IGNORE_NOT_EXIST(this->catalog->unlink(FILE));
      IGNORE_NOT_EXIST(this->catalog->removeDir(NESTED));
      IGNORE_NOT_EXIST(this->catalog->removeDir(FOLDER));
    }
    TestBase::tearDown();
  }
  
  void testAcl()
  {
    dmlite::ExtendedStat stat = this->catalog->extendedStat(FOLDER);
    CPPUNIT_ASSERT_EQUAL(ACL, stat.acl);
  }

  void testChown()
  {
    this->catalog->setOwner(FOLDER, -1, getGid(ctx, 1));
    dmlite::ExtendedStat stat = this->catalog->extendedStat(FOLDER);

    CPPUNIT_ASSERT_EQUAL((size_t)4, stat.acl.size());

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kUserObj, stat.acl[0].type);
    CPPUNIT_ASSERT_EQUAL(getUid(ctx),  stat.acl[0].id);
    CPPUNIT_ASSERT_EQUAL(7u, (unsigned)stat.acl[0].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kGroupObj, stat.acl[1].type);
    CPPUNIT_ASSERT_EQUAL(getGid(ctx, 1), stat.acl[1].id);
    CPPUNIT_ASSERT_EQUAL(0u, (unsigned)stat.acl[1].perm);
  }

  void testChmod()
  {
    this->catalog->setMode(FOLDER, 0555);
    dmlite::ExtendedStat stat = this->catalog->extendedStat(FOLDER);

    CPPUNIT_ASSERT_EQUAL((size_t)4, stat.acl.size());

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kUserObj, stat.acl[0].type);
    CPPUNIT_ASSERT_EQUAL(getUid(ctx),  stat.acl[0].id);
    CPPUNIT_ASSERT_EQUAL(5u, (unsigned)stat.acl[0].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kGroupObj, stat.acl[1].type);
    CPPUNIT_ASSERT_EQUAL(getGid(ctx, 0), stat.acl[1].id);
    CPPUNIT_ASSERT_EQUAL(5u,   (unsigned)stat.acl[1].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kOther, stat.acl[3].type);
    CPPUNIT_ASSERT_EQUAL(5u, (unsigned)stat.acl[3].perm);
  }

  void testInherit()
  {
    // Set default ACL's
    dmlite::Acl parentAcl("A70,C00,E70,F50,a60,c50,f00");
    this->catalog->setAcl(FOLDER, parentAcl);
    dmlite::ExtendedStat parent = this->catalog->extendedStat(FOLDER);
    
    // Make dir
    this->catalog->makeDir(NESTED, 0700);
    dmlite::ExtendedStat stat = this->catalog->extendedStat(NESTED);

    // Assert resulting ACL
    std::stringstream aclStr;
    aclStr << "A6" << getUid(ctx) << ",C0" << getGid(ctx) << ",F00,a60,c50,f00";
    CPPUNIT_ASSERT_EQUAL(dmlite::Acl(aclStr.str()), stat.acl);
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
