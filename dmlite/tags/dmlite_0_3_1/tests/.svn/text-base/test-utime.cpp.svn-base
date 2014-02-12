#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#include "test-base.h"

class TestUtime: public TestBase
{
protected:
  const static char *FILE;

public:

  void setUp()
  {
    TestBase::setUp();
    this->catalog->create(FILE, 0700);
  }

  void tearDown()
  {
    if (this->catalog != 0x00)
      this->catalog->unlink(FILE);
    TestBase::tearDown();
  }

  void testValue()
  {
    struct utimbuf tim;

    tim.actime  = time(NULL);
    tim.modtime = tim.actime - 1;

    this->catalog->utime(FILE, &tim);

    struct stat s = this->catalog->extendedStat(FILE).stat;

    CPPUNIT_ASSERT_EQUAL(tim.actime, s.st_atime);
    CPPUNIT_ASSERT_EQUAL(tim.modtime, s.st_mtime);
  }

  void testDefault()
  {
    struct stat before, after;

    before = this->catalog->extendedStat(FILE).stat;
    sleep(1);
    this->catalog->utime(FILE, 0x00);
    after = this->catalog->extendedStat(FILE).stat;

    CPPUNIT_ASSERT(before.st_atime < after.st_atime);
    CPPUNIT_ASSERT(before.st_mtime < after.st_mtime);
    CPPUNIT_ASSERT(before.st_ctime < after.st_ctime);
  }


  CPPUNIT_TEST_SUITE(TestUtime);
  CPPUNIT_TEST(testValue);
  CPPUNIT_TEST(testDefault);
  CPPUNIT_TEST_SUITE_END();
};

const char* TestUtime::FILE  = "test-utime";

CPPUNIT_TEST_SUITE_REGISTRATION(TestUtime);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}

