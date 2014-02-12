#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include "test-base.h"

class TestChecksum: public TestBase
{
protected:
  const static char *FILE;

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

  void testBasic()
  {
    const char* csum = "1a31009319c99ebdefd23055b20ff034";
    
    // Change checksum to something else
    this->catalog->changeChecksum(FILE, "MD", csum);
    
    // Compare
    ExtendedStat meta = this->catalog->extendedStat(FILE);
    
    CPPUNIT_ASSERT_MESSAGE(meta.csumtype, strcmp("MD", meta.csumtype) == 0);
    CPPUNIT_ASSERT_MESSAGE(meta.csumvalue, strncmp(csum, meta.csumvalue, 33) == 0);
  }

  CPPUNIT_TEST_SUITE(TestChecksum);
  CPPUNIT_TEST(testBasic);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestChecksum);

const char* TestChecksum::FILE        = "test-checksum";

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}

