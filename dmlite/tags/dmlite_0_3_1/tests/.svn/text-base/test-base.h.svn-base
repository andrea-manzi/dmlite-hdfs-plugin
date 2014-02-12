#ifndef TEST_BASE_H
#define	TEST_BASE_H

#include <cppunit/TestFixture.h>
#include <dmlite/cpp/dmlite.h>

#define IGNORE_NOT_EXIST(f) try { f; } catch (dmlite::DmException& e) { if (e.code() != DM_NO_SUCH_FILE) throw; }


class TestBase: public CppUnit::TestFixture {
public:
  TestBase();
  void setUp();
  void tearDown();

protected:
  dmlite::PluginManager *pluginManager;
  dmlite::StackInstance *stackInstance;
  dmlite::Catalog       *catalog;

  const static char *TEST_USER;
  const static char *TEST_USER_GROUPS[];
  static unsigned    TEST_USER_NGROUPS;

  const static char *TEST_USER_2;
  const static char *TEST_USER_2_GROUPS[];
  static unsigned    TEST_USER_2_NGROUPS;

  Credentials cred1, cred2;
  
  dmlite::SecurityContext root;

  std::string BASE_DIR;

private:
  friend int testBaseMain(int, char**);
  static const char *config;
};


int testBaseMain(int argn, char **argv);

#endif
