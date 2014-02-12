#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>


#include "dmlite/cpp/utils/security.h"
#include <cppunit/extensions/HelperMacros.h>
#include <dmlite/cpp/authn.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/io.h>
#include <iostream>
#include <ios>
#include <iosfwd>



class TestIO: public CppUnit::TestFixture
{
private:
  dmlite::PluginManager*  manager;
  dmlite::IODriver*       io;
  dmlite::StackInstance*  si;
  dmlite::Extensible      extras;
  dmlite::SecurityContext root;

public:
  static const char* config;

  void setUp()
  {
    manager = new dmlite::PluginManager();
    manager->loadConfiguration(config);
    manager->configure("TokenPassword", "test");
    manager->configure("TokenId", "DN");
    si = new dmlite::StackInstance(manager);
    
    dmlite::GroupInfo group;
    group.name   = "root";
    group["gid"] = 0u;
  
    root.user["uid"] = 0u;
    root.groups.push_back(group);
    
    si->setSecurityContext(root);
    
    io = si->getIODriver();
  }

  void tearDown()
  {
    if (si)
      delete si;
    if (manager)
      delete manager;
    extras.clear();
  }

  void testOpen(void)
  {
    char b;
    
    extras["token"] = dmlite::generateToken("", "/dev/zero", "test",
                                            1000, false);
    dmlite::IOHandler* s = io->createIOHandler("/dev/zero",
                                               O_RDONLY,
                                               extras);

    s->read(&b, sizeof(char));
    CPPUNIT_ASSERT_EQUAL((char)0, b);
    
    delete s;
  }

  void testNotExist(void)
  {
    extras["token"] = dmlite::generateToken("", "/this/should/not/exist", "test",
                                            1000, false);
    CPPUNIT_ASSERT_THROW(io->createIOHandler("/this/should/not/exist",
                                             O_RDONLY,
                                             extras),
                         dmlite::DmException);
  }

  void testWriteAndRead(void)
  {
    const char ostring[] = "This-is-the-string-to-be-checked!";
    
    extras["token"] = dmlite::generateToken("", "/tmp/test-io-wr", "test",
                                            1000, true);

    // Open to write
    dmlite::IOHandler* os = io->createIOHandler("/tmp/test-io-wr",
                                                O_WRONLY,
                                                extras);
    os->write(ostring, strlen(ostring));
    delete os;

    // Open to read
    char buffer[512] = "";
    dmlite::IOHandler* is = io->createIOHandler("/tmp/test-io-wr",
                                                O_RDONLY,
                                                extras);
    size_t nb = is->read(buffer, sizeof(buffer));

    CPPUNIT_ASSERT_EQUAL(strlen(ostring), nb);
    CPPUNIT_ASSERT_EQUAL(std::string(ostring), std::string(buffer));

    delete is;
  }

  
  CPPUNIT_TEST_SUITE(TestIO);
  CPPUNIT_TEST(testOpen);
  CPPUNIT_TEST(testNotExist);
  CPPUNIT_TEST(testWriteAndRead);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestIO);

const char* TestIO::config = 0x00;

int main(int argn, char **argv)
{
  if (argn < 2) {
    std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
    return -1;
  }

  TestIO::config = argv[1];

  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  return runner.run()?0:1;
}
