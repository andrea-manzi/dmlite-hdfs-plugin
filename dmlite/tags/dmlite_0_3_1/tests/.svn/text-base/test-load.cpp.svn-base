#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dmlite/cpp/dmlite.h>
#include <iostream>
#include <sys/stat.h>

class PluginLoaded: public CppUnit::TestFixture {
protected:
  dmlite::PluginManager *pluginManager;
  dmlite::SecurityContext secCtx;

public:
  void setUp()
  {
    pluginManager = new dmlite::PluginManager();
  }

  void tearDown()
  {
    if (pluginManager)
      delete pluginManager;
  }

  void testNested()
  {
    dmlite::StackInstance *stack;
    dmlite::Catalog       *catalog;
    struct stat  buf;

    // Load first
    try {
      pluginManager->loadPlugin("../plugins/adapter/plugin_adapter.so", "plugin_adapter_ns");
      // Load second
      pluginManager->loadPlugin("../plugins/profiler/plugin_profiler.so", "plugin_profiler");
      // Configure (adapter should recognise)
      pluginManager->configure("Host", "arioch.cern.ch");
      // Get stack
      stack = new dmlite::StackInstance(pluginManager);
      stack->setSecurityContext(secCtx);
      // Get interface
      catalog = stack->getCatalog();
      CPPUNIT_ASSERT(catalog != 0x00);
    }
    catch (dmlite::DmException& exc) {
      CPPUNIT_FAIL(exc.what());
    }
    // Call stat (it is irrelevant, but errno can not be DM_NOT_IMPLEMENTED)
    try {
      buf = catalog->extendedStat("/").stat;
      CPPUNIT_ASSERT_EQUAL((unsigned)0, buf.st_uid);
    }
    catch (dmlite::DmException& exc)
    {
      CPPUNIT_ASSERT(exc.code() != DM_NOT_IMPLEMENTED);
    }
    // Free
    if (stack) delete stack;
  }

  CPPUNIT_TEST_SUITE(PluginLoaded);
  CPPUNIT_TEST(testNested);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PluginLoaded);

int main(int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  return runner.run()?0:-1;
}
