#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <sys/stat.h>
#include "test-base.h"

class TestChDir: public TestBase
{
protected:
  struct stat statBuf;
  const static char *FOLDER;
  const static char *FILE;

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
      IGNORE_NOT_EXIST(this->catalog->unlink(FILE));
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
      if (DM_IS_CWD != e.code())
        throw;
    }
  }
  
  void testRemoveRelative()
  {
    this->catalog->makeDir(FOLDER, 0755);
    this->catalog->changeDir(BASE_DIR);
    this->catalog->removeDir(FOLDER);
    
    this->catalog->create(FILE, 0755);
    this->catalog->unlink(FILE);
    
    this->catalog->create(FILE, 0755);
    this->catalog->unlink(std::string("./") + FILE);
    
    this->catalog->create(FILE, 0755);
    this->catalog->makeDir(FOLDER, 0755);
    this->catalog->changeDir(FOLDER);
    this->catalog->unlink(std::string("../") + FILE);
  }

  CPPUNIT_TEST_SUITE(TestChDir);
  CPPUNIT_TEST(testRegular);
  CPPUNIT_TEST(testRemoveCwd);
  CPPUNIT_TEST(testRemoveRelative);
  CPPUNIT_TEST_SUITE_END();
};

const char* TestChDir::FOLDER = "test-chdir";
const char* TestChDir::FILE   = "test-chdir.file";

CPPUNIT_TEST_SUITE_REGISTRATION(TestChDir);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}

