#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <sys/stat.h>
#include "test-base.h"

class TestChDir: public TestBase
{
protected:
  struct stat statBuf;
  const static char *FOLDER;

public:

  void setUp()
  {
    TestBase::setUp();
  }

  void tearDown()
  {
    if (this->catalog != 0x00) {
      this->catalog->changeDir(BASE_DIR);
      IGNORE_NOT_EXIST(this->catalog->removeDir(FOLDER));
    }
    TestBase::tearDown();
  }

  void testRegular()
  {
    this->catalog->changeDir(BASE_DIR);

    // Beware: some plugins may strip the last '/', so tolerate that!
    std::string cwd = this->catalog->getWorkingDir();
    if (cwd.length() == BASE_DIR.length() - 1)
      CPPUNIT_ASSERT_EQUAL(BASE_DIR, cwd + "/");
    else
      CPPUNIT_ASSERT_EQUAL(BASE_DIR, cwd);

    statBuf = this->catalog->extendedStat(".").stat;
    CPPUNIT_ASSERT_EQUAL(S_IFDIR, (int)statBuf.st_mode & S_IFMT);

    // Adapter doesn't implement
    try {
      CPPUNIT_ASSERT_EQUAL(statBuf.st_ino, this->catalog->getWorkingDirI());
    }
    catch (dmlite::DmException& e) {
      if (e.code() != DM_NOT_IMPLEMENTED)
        throw;
      CPPUNIT_NS::Message("Access by inode not implemented. Ignoring.");
    }
  }

  void testRemoveCwd()
  {
    this->catalog->makeDir(FOLDER, 0775);
    this->catalog->changeDir(FOLDER);

    try {
      this->catalog->removeDir(std::string(BASE_DIR) + "/" + FOLDER);
      CPPUNIT_FAIL("Should have failed");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_IS_CWD, e.code());
    }
  }

  CPPUNIT_TEST_SUITE(TestChDir);
  CPPUNIT_TEST(testRegular);
  CPPUNIT_TEST(testRemoveCwd);
  CPPUNIT_TEST_SUITE_END();
};

const char* TestChDir::FOLDER  = "test-chdir";

CPPUNIT_TEST_SUITE_REGISTRATION(TestChDir);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}

