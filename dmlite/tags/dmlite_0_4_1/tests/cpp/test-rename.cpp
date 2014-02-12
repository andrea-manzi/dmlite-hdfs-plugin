#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include "test-base.h"

class TestRename: public TestBase
{
protected:
  const static char *SOURCE_FILE;
  const static char *DEST_FILE;
  const static char *SOURCE_DIR;
  const static char *DEST_DIR;
  const static char *NESTED_DIR;
  const static char *NESTED_FILE;
  const static char *NESTED_DEST_FILE;

public:

  void tearDown()
  {
    if (this->catalog) {
      this->stackInstance->setSecurityContext(root);
      IGNORE_NOT_EXIST(this->catalog->unlink(SOURCE_FILE));
      IGNORE_NOT_EXIST(this->catalog->unlink(DEST_FILE));
      IGNORE_NOT_EXIST(this->catalog->unlink(NESTED_FILE));
      IGNORE_NOT_EXIST(this->catalog->unlink(NESTED_DEST_FILE));
      IGNORE_NOT_EXIST(this->catalog->removeDir(NESTED_DIR));
      IGNORE_NOT_EXIST(this->catalog->removeDir(SOURCE_DIR));
      IGNORE_NOT_EXIST(this->catalog->removeDir(DEST_DIR));
    }

    TestBase::tearDown();
  }

  void testRenameFile()
  {
    try {
      this->catalog->rename(SOURCE_FILE, DEST_FILE);
      CPPUNIT_FAIL("Should have failed");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_NO_SUCH_FILE, e.code());
    }

    this->catalog->create(SOURCE_FILE, 0755);
    this->catalog->rename(SOURCE_FILE, DEST_FILE);

    CPPUNIT_ASSERT_THROW(this->catalog->extendedStat(SOURCE_FILE), dmlite::DmException);
    this->catalog->extendedStat(DEST_FILE);
  }

  void testRenameDir()
  {
    this->catalog->makeDir(SOURCE_DIR, 0755);
    this->catalog->rename(SOURCE_DIR, DEST_DIR);

    CPPUNIT_ASSERT_THROW(this->catalog->extendedStat(SOURCE_DIR), dmlite::DmException);
    this->catalog->extendedStat(DEST_DIR);
  }

  void testDescendant()
  {
    this->catalog->makeDir(SOURCE_DIR, 0755);
    try {
      this->catalog->rename(SOURCE_DIR, NESTED_DIR);
      CPPUNIT_FAIL("Should have failed");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_INVALID_VALUE, e.code());
    }
  }

  void testFileExistingDest()
  {
    this->catalog->create(SOURCE_FILE, 0755);
    this->catalog->create(DEST_FILE, 0755);
    this->catalog->rename(SOURCE_FILE, DEST_FILE);
  }

  void testDirExistingDest()
  {
    this->catalog->makeDir(SOURCE_DIR, 0755);
    this->catalog->create(NESTED_FILE, 0755);
    this->catalog->makeDir(DEST_DIR, 0755);

    // This must fail
    try {
      this->catalog->rename(DEST_DIR, SOURCE_DIR);
      CPPUNIT_FAIL("Should have failed");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_EXISTS, e.code());
    }

    // This must pass
    this->catalog->rename(SOURCE_DIR, DEST_DIR);
    struct stat s = this->catalog->extendedStat(DEST_DIR).stat;
    CPPUNIT_ASSERT_EQUAL(1, (int)s.st_nlink);
  }

  void testSticky()
  {
    this->stackInstance->setSecurityCredentials(cred1);

    this->catalog->makeDir(SOURCE_DIR, 0755 | S_ISVTX);
    this->catalog->create(NESTED_FILE, 0755);

    // Must fail
    this->stackInstance->setSecurityCredentials(cred2);
    try {
      this->catalog->rename(NESTED_FILE, DEST_FILE);
      CPPUNIT_FAIL("Should have failed");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_FORBIDDEN, e.code());
    }

    // Must succeed
    this->stackInstance->setSecurityCredentials(cred1);
    this->catalog->setMode(NESTED_FILE, 0777);
    this->catalog->rename(NESTED_FILE, DEST_FILE);

    this->catalog->extendedStat(DEST_FILE);
  }

  CPPUNIT_TEST_SUITE(TestRename);
  CPPUNIT_TEST(testRenameFile);
  CPPUNIT_TEST(testRenameDir);
  CPPUNIT_TEST(testDescendant);
  CPPUNIT_TEST(testFileExistingDest);
  CPPUNIT_TEST(testDirExistingDest);
  CPPUNIT_TEST(testSticky);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestRename);

const char* TestRename::SOURCE_FILE      = "test-rename-source";
const char* TestRename::DEST_FILE        = "test-rename-dest";
const char* TestRename::SOURCE_DIR       = "test-dir-source";
const char* TestRename::DEST_DIR         = "test-dir-dest";
const char* TestRename::NESTED_DIR       = "test-dir-source/something";
const char* TestRename::NESTED_FILE      = "test-dir-source/something-else";
const char* TestRename::NESTED_DEST_FILE = "test-dir-dest/something-else";

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
