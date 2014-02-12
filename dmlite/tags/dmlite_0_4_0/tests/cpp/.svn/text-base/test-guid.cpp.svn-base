#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include "test-base.h"

class TestGuid: public TestBase
{
protected:
  const static char* FILE;
  const static char* GUID;
  
public:

  void setUp()
  {
    TestBase::setUp();

    try {
      this->catalog->unlink(FILE);
    }
    catch (...) {
      // Nothing
    }
  }

  void tearDown()
  {
    if (this->catalog != 0x00) {
      try {
        this->catalog->unlink(FILE);
      }
      catch (dmlite::DmException& e) {
        if (e.code() != DM_NO_SUCH_FILE)
          throw;
      }
    }
    TestBase::tearDown();
  }

  void testPut()
  {
    /*struct xstat    xStat;
    struct url      location;
    char            hostname[HOST_NAME_MAX];
    struct hostent* host;

    gethostname(hostname, sizeof(hostname));
    host = gethostbyname(hostname);

    this-> ->put(FILE, &location, GUID);

    if (strcmp(location.host, host->h_name) != 0)
      CPPUNIT_FAIL(std::string("This test only support local PUT's!! ") +
                   host->h_name + " != " + location.host);

    fclose(fopen(location.path, "w"));

    this->catalog->putDone(location);

    xStat = this->catalog->extendedStat(FILE);

    CPPUNIT_ASSERT_EQUAL(std::string(GUID), std::string(xStat.guid));*/
  }

  CPPUNIT_TEST_SUITE(TestGuid);
  CPPUNIT_TEST(testPut);
  CPPUNIT_TEST_SUITE_END();
};

const char* TestGuid::FILE = "test-guid";
const char* TestGuid::GUID = "1234-5678-FF-4321";

CPPUNIT_TEST_SUITE_REGISTRATION(TestGuid);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
