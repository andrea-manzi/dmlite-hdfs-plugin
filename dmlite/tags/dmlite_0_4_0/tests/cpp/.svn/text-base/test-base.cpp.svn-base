#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <iostream>
#include "test-base.h"

std::string                    TestBase::config("");



TestBase::TestBase(): pluginManager(0x00), catalog(0x00),
  TEST_USER("/C=CH/O=CERN/OU=GD/CN=Test user 0"),
  TEST_USER_2("/C=CH/O=CERN/OU=GD/CN=Test user 1")
{
  const char *testHome;
  
  // Populate groups
  TEST_USER_GROUPS.push_back("dteam");
  TEST_USER_GROUPS.push_back("org.glite.voms-test");
  
  TEST_USER_2_GROUPS.push_back("atlas");

  // Get the test root directory
  if ((testHome = getenv("DPNS_HOME")) != NULL)
    BASE_DIR = std::string(testHome);
  else if ((testHome = getenv("LFC_HOME")) != NULL)
    BASE_DIR = std::string(testHome);
  else
    throw dmlite::DmException(DM_INVALID_VALUE,
                              "Could not guess the test directory. Try setting DPNS_HOME or LFC_HOME");

  dmlite::GroupInfo group;
  group.name   = "root";
  group["gid"] = 0u;
  
  root.user["uid"] = 0u;
  root.groups.push_back(group);
}


void TestBase::setUp()
{
  pluginManager = new dmlite::PluginManager();
  pluginManager->loadConfiguration(TestBase::config);
  
  stackInstance = new dmlite::StackInstance(pluginManager);
  
  // Initialize security context
  stackInstance->setSecurityContext(root);

  // Catalog
  catalog = stackInstance->getCatalog();

  // Credentials 1
  this->cred1.clientName = TEST_USER;
  this->cred1.fqans      = TEST_USER_GROUPS;
  this->cred1.mech       = "NONE";

  // Credentials 2
  this->cred2.clientName = TEST_USER_2;
  this->cred2.fqans      = TEST_USER_2_GROUPS;
  this->cred2.mech       = "NONE";

  // Change dir
  this->catalog->changeDir(BASE_DIR);
}



void TestBase::tearDown()
{
  if (stackInstance)
    delete stackInstance;
  if (pluginManager)
    delete pluginManager;
}



int testBaseMain(int argn, char **argv)
{
  if (argn < 2) {
    std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
    return -1;
  }

  TestBase::config = argv[1];

  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  return runner.run()?0:1;
}
