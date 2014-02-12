#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <dmlite/cpp/utils/security.h>
#include "test-base.h"



class TestSymlink: public TestBase
{
protected:
  static const char* FOLDER;
  static const char* NESTED;
  static const char* NESTEDLNK;
  static const char* SYMLINK;
  
public:
  
  void setUp()
  {
    TestBase::setUp();
  }
  
  void tearDown()
  {
    if (this->catalog) {
      IGNORE_NOT_EXIST(this->catalog->unlink(SYMLINK));
      IGNORE_NOT_EXIST(this->catalog->unlink(NESTED));
      IGNORE_NOT_EXIST(this->catalog->unlink(NESTEDLNK));
      IGNORE_NOT_EXIST(this->catalog->removeDir(FOLDER));
    }
    TestBase::tearDown();
  }
  
  void testNoFollow()
  {
    struct stat original, test;
    
    // No follow, pointing to a file that exist
    this->catalog->symlink("/", SYMLINK);
    original = this->catalog->extendedStat("/").stat;
    test     = this->catalog->extendedStat(SYMLINK, false).stat;
    
    CPPUNIT_ASSERT(S_ISLNK(test.st_mode));
    this->catalog->unlink(SYMLINK);
    
    // No follow, pointing to a file that does not exist
    // (Must not throw exception)
    this->catalog->symlink("/should/not/exist", SYMLINK);
    test = this->catalog->extendedStat(SYMLINK, false).stat;
    CPPUNIT_ASSERT(S_ISLNK(test.st_mode));
    this->catalog->unlink(SYMLINK);
    
    // No follow, pointing to a relative that does not exist
    this->catalog->symlink("../wrong", SYMLINK);
    test = this->catalog->extendedStat(SYMLINK, false).stat;
    CPPUNIT_ASSERT(S_ISLNK(test.st_mode));
    this->catalog->unlink(SYMLINK);
  }
  
  void testCwd()
  {
    const char* nestedname = strchr(NESTED, '/') + 1;
    
    this->catalog->makeDir(FOLDER, 0755);
    this->catalog->create(NESTED, 0755);
    this->catalog->symlink(nestedname, NESTEDLNK);
    
    // Make sure it is pointing as it should
    std::string link = this->catalog->readLink(NESTEDLNK);
    CPPUNIT_ASSERT_EQUAL(std::string(nestedname), link);
    
    // Is it able to stat it without following?
    this->catalog->extendedStat(NESTEDLNK, false);

    // Now, if we stat NESTEDLNK from outside the folder
    // it must be OK
    this->catalog->extendedStat(NESTEDLNK);
  }
  
  CPPUNIT_TEST_SUITE(TestSymlink);
  CPPUNIT_TEST(testNoFollow);
  CPPUNIT_TEST(testCwd);
  CPPUNIT_TEST_SUITE_END();
};

const char* TestSymlink::FOLDER     = "test-symlink.d";
const char* TestSymlink::NESTED     = "test-symlink.d/test-nested";
const char* TestSymlink::NESTEDLNK  = "test-symlink.d/test-symlink-nested";
const char* TestSymlink::SYMLINK    = "test-symlink";

CPPUNIT_TEST_SUITE_REGISTRATION(TestSymlink);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
