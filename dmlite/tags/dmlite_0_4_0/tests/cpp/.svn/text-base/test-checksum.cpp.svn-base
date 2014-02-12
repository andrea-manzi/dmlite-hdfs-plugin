#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <cstring>
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
    const std::string csumtype("MD");
    const std::string csumvalue("1a31009319c99ebdefd23055b20ff034");
    
    // Change checksum to something else
    this->catalog->setChecksum(FILE, csumtype, csumvalue);
    
    // Compare
    dmlite::ExtendedStat meta = this->catalog->extendedStat(FILE);
    
    CPPUNIT_ASSERT_EQUAL(csumtype,  meta.csumtype);
    CPPUNIT_ASSERT_EQUAL(csumvalue, meta.csumvalue);
  }

  CPPUNIT_TEST_SUITE(TestChecksum);
  CPPUNIT_TEST(testBasic);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestChecksum);

const char* TestChecksum::FILE  = "test-checksum";

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}

