#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <sstream>
#include <vector>
#include "test-base.h"

class TestXAttr: public TestBase
{
protected:
  static const char* FILE;
  
public:
  
  void setUp()
  {
    TestBase::setUp();
  }
  
  void tearDown()
  {
    if (this->catalog) {
      this->stackInstance->setSecurityContext(this->root);
      IGNORE_NOT_EXIST(this->catalog->unlink(FILE));
    }
    TestBase::tearDown();
  }
  
  void testCreate()
  {
    // This test only make sense for inode tests
    dmlite::INode* inode = this->stackInstance->getINode();
    
    CPPUNIT_ASSERT(inode != NULL);

    dmlite::ExtendedStat xs;
    
    // Need the parent
    xs = this->catalog->extendedStat(".");
    ino_t parent = xs.stat.st_ino;
    
    // Create with extended attributes
    xs.parent = parent;
    xs.name   = FILE;
    xs.status = dmlite::ExtendedStat::kOnline;
    memset(&xs.stat, 0, sizeof(xs.stat));
    xs.stat.st_mode = S_IFREG | 0755;
    
    xs["string"] = std::string("additional");
    xs["int"]    = 42;
    
    inode->create(xs);
    
    // Get it back and check
    dmlite::ExtendedStat check = this->catalog->extendedStat(FILE);
    
    CPPUNIT_ASSERT_EQUAL(std::string("additional"), check.getString("string"));
    CPPUNIT_ASSERT_EQUAL(42l, check.getLong("int"));
  }
  
  void testSet()
  {
    this->catalog->create(FILE, 0755);
    
    dmlite::ExtendedStat xs = this->catalog->extendedStat(FILE);
    
    xs["string2"] = std::string("testSet");
    xs["bool"] = true;
    
    this->catalog->updateExtendedAttributes(FILE, xs);
    
    // Check
    dmlite::ExtendedStat check = this->catalog->extendedStat(FILE);
    
    CPPUNIT_ASSERT_EQUAL(std::string("testSet"), check.getString("string2"));
    CPPUNIT_ASSERT_EQUAL(true, check.getBool("bool"));
  }
  
  CPPUNIT_TEST_SUITE(TestXAttr);
  CPPUNIT_TEST(testCreate);
  CPPUNIT_TEST(testSet);
  CPPUNIT_TEST_SUITE_END();
};

const char* TestXAttr::FILE = "test-xattr";

CPPUNIT_TEST_SUITE_REGISTRATION(TestXAttr);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
