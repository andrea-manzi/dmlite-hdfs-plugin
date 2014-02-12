#include <cppunit/extensions/HelperMacros.h>
#include "test-base.h"

class TestComment: public TestBase
{
private:
  static const char* FILE;

public:

  void setUp()
  {
    TestBase::setUp();
    this->catalog->create(FILE, 0755);
  }

  void tearDown()
  {
    this->catalog->unlink(FILE);
    TestBase::tearDown();
  }

  void testComment()
  {
    const std::string comment("This is a comment!@1234XX");

    this->catalog->setComment(FILE, comment);
    CPPUNIT_ASSERT_EQUAL(comment, this->catalog->getComment(FILE));
  }


  CPPUNIT_TEST_SUITE(TestComment);
  CPPUNIT_TEST(testComment);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestComment);

const char* TestComment::FILE = "test-comment";

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
