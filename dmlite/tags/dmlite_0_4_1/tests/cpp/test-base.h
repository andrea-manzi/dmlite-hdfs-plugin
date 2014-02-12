#ifndef TEST_BASE_H
#define	TEST_BASE_H

#include <boost/any.hpp>
#include <cppunit/TestFixture.h>
#include <dmlite/cpp/authn.h>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/dmlite.h>
#include <string>
#include <vector>

#define IGNORE_NOT_EXIST(f) try { f; } catch (dmlite::DmException& e) { if (e.code() != DM_NO_SUCH_FILE) throw; }


inline uid_t getUid(const dmlite::SecurityContext* ctx)
{
  return boost::any_cast<uid_t>(ctx->user["uid"]);
}



inline gid_t getGid(const dmlite::SecurityContext* ctx, unsigned i = 0)
{
  return boost::any_cast<gid_t>(ctx->groups[i]["gid"]);
}



class TestBase: public CppUnit::TestFixture {
public:
  TestBase();
  void setUp();
  void tearDown();

protected:
  dmlite::PluginManager *pluginManager;
  dmlite::StackInstance *stackInstance;
  dmlite::Catalog       *catalog;

  std::string              TEST_USER;
  std::vector<std::string> TEST_USER_GROUPS;

  std::string              TEST_USER_2;
  std::vector<std::string> TEST_USER_2_GROUPS;

  dmlite::SecurityCredentials cred1, cred2;
  dmlite::SecurityContext     root;

  std::string BASE_DIR;

private:
  friend int testBaseMain(int, char**);
  static std::string config;
};



int testBaseMain(int argn, char **argv);

#endif
