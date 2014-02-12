#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dmlite/cpp/utils/dm_security.h>

class ChkPerm: public CppUnit::TestFixture {
protected:
  struct stat stat_;
  dmlite::SecurityContext* context;
public:

  void setUp()
  {
    context = new dmlite::SecurityContext();
    // Set the security fields
    stat_.st_uid  = 200;
    stat_.st_gid  = 200;
    stat_.st_mode = 0750;
  }

  void tearDown()
  {
    delete context;
  }

  void testOwner()
  {
    // Owner and group
    context->resizeGroup(1);
    context->getUser().uid   = 200;
    context->getGroup(0).gid = 200;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    // Owner but no group
    context->getGroup(0).gid = 500;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
  }

  void testGroup()
  {
    std::vector<GroupInfo> gids;
    // Main group matches
    context->resizeGroup(1);
    context->getUser().uid   = 500;
    context->getGroup(0).gid = 200;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IEXEC));
    // Main doesn't match, but a secondary does
    context->resizeGroup(2);
    context->getGroup(0).gid = 500;
    context->getGroup(1).gid = 200;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testOther()
  {
    std::vector<GroupInfo> gids;
    // No match
    context->resizeGroup(1);
    context->getUser().uid   = 500;
    context->getGroup(0).gid = 500;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD));
  }

  void testAcl()
  {
    const char *acl = "A70,B7101,C7102,D5103,E70,F00,a70,c70,f50";
    // No match
    context->resizeGroup(1);
    context->getUser().uid   = 500;
    context->getGroup(0).gid = 500;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD));
    // Match user
    context->getUser().uid = 101;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));

    // Match main group
    context->getUser().uid   = 500;
    context->getGroup(0).gid = 103;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));

    // Match secondary
    context->resizeGroup(2);
    context->getGroup(0).gid = 500;
    context->getGroup(1).gid = 103;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testUserBanned()
  {
    // Owner and group
    context->resizeGroup(1);
    context->getUser().uid    = 200;
    context->getGroup(0).gid  = 200;
    context->getUser().banned = 1;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
  }

  void testMainGroupBanned()
  {
    // Owner matches, group banned
    context->resizeGroup(1);
    context->getUser().uid      = 200;
    context->getGroup(0).gid    = 200;
    context->getGroup(0).banned = 1;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    // Owner doesn't match. Group does, but banned.
    context->getUser().uid = 500;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testSecondaryBanned()
  {
    // Owner doesn't match, main group does, but secondary banned.
    context->resizeGroup(2);
    context->getUser().uid      = 500;
    context->getGroup(0).gid    = 200;
    context->getGroup(1).gid    = 500;
    context->getGroup(1).banned = 1;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IEXEC));
    // Owner doesn't match, main group neither, secondary does but banned.
    context->getGroup(0).gid    = 500;
    context->getGroup(1).gid    = 200;
    context->getGroup(1).banned = 1;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IEXEC));

    // Owner doesn't match, main group neither and banned. Secondary OK.
    context->getGroup(0).banned = 1;
    context->getGroup(1).banned = 0;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     0x00, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testAclBanned()
  {
    std::vector<GroupInfo> gids;
    const char *acl = "A70,B7101,C7102,D5103,E70,F00,a70,c70,f50";

    // Match user
    context->resizeGroup(1);
    context->getUser().uid    = 101;
    context->getUser().banned = 1;
    context->getGroup(0).gid  = 500;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));

    // Match main group
    context->getUser().uid      = 500;
    context->getUser().banned   = 0;
    context->getGroup(0).gid    = 103;
    context->getGroup(0).banned = 1;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));

    // Main group banned, but secondary matches (and banned)
    context->resizeGroup(2);
    context->getGroup(0).gid    = 500;
    context->getGroup(1).gid    = 103;
    context->getGroup(1).banned = 1;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testGetVoFromRole()
  {
    std::string vo;
    vo = dmlite::voFromRole(std::string("/dteam/Role=NULL/Capability=NULL"));
    CPPUNIT_ASSERT_EQUAL(std::string("dteam"), vo);
  }


  void testAclSerialization()
  {
    // Test deserialization
    std::vector<Acl> acls = dmlite::deserializeAcl("A6101,B6101,C4101,D7101,E70,F40");

    CPPUNIT_ASSERT_EQUAL((size_t)6, acls.size());

    CPPUNIT_ASSERT_EQUAL(ACL_USER_OBJ, (int)acls[0].type);
    CPPUNIT_ASSERT_EQUAL(101u,         acls[0].id);
    CPPUNIT_ASSERT_EQUAL(6,            (int)acls[0].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_USER, (int)acls[1].type);
    CPPUNIT_ASSERT_EQUAL(101u,     acls[1].id);
    CPPUNIT_ASSERT_EQUAL(6,        (int)acls[1].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_GROUP_OBJ, (int)acls[2].type);
    CPPUNIT_ASSERT_EQUAL(101u,          acls[2].id);
    CPPUNIT_ASSERT_EQUAL(4,             (int)acls[2].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_GROUP, (int)acls[3].type);
    CPPUNIT_ASSERT_EQUAL(101u,      acls[3].id);
    CPPUNIT_ASSERT_EQUAL(7,         (int)acls[3].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_MASK, (int)acls[4].type);
    CPPUNIT_ASSERT_EQUAL(0u,       acls[4].id);
    CPPUNIT_ASSERT_EQUAL(7,        (int)acls[4].perm);

    CPPUNIT_ASSERT_EQUAL(ACL_OTHER, (int)acls[5].type);
    CPPUNIT_ASSERT_EQUAL(0u,        acls[5].id);
    CPPUNIT_ASSERT_EQUAL(4,         (int)acls[5].perm);

    // Test serialization
    acls.insert(acls.begin(), acls.back());
    acls.pop_back();

    std::string serial = dmlite::serializeAcl(acls);
    CPPUNIT_ASSERT_EQUAL(std::string("A6101,B6101,C4101,D7101,E70,F40"), serial);
  }

  void testAclValidation()
  {
    // Empty is good
    dmlite::validateAcl("");
    // Good
    dmlite::validateAcl("A6101,B6101,C4101,D7101,E70,F40");
    // Good too
    dmlite::validateAcl("A6101,B6101,B6100,C4101,D7101,D4200,E70,F40");
    // With defaults
    dmlite::validateAcl("A6101,B6101,C4101,D7101,E70,F40,a00,c50,f60");
    // Invalid value
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A8101,A6200,B6101,C4101,D7101,E70,F40"), dmlite::DmException);
    // Invalid type
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,A6200,B6101,C4101,D7101,E70,F40,Z40"), dmlite::DmException);
    // Duplicated ACL_USER_OBJ
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,A6200,B6101,C4101,D7101,E70,F40"), dmlite::DmException);
    // Duplicated ACL_GROUP_OBJ
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,B6101,C4101,C5200,D7101,E70,F40"), dmlite::DmException);
    // Twice the same user ID
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,B6101,B6101,C4101,D7101,E70,F40"), dmlite::DmException);
    // Twice the same group ID
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,B6101,C4101,D7101,D5101,E70,F40"), dmlite::DmException);
    // No mask
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,B6101,C4101,D7101,F40"), dmlite::DmException);
    // No other
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,B6101,C4101,D7101,E70"), dmlite::DmException);
    // Missing other defaults
    CPPUNIT_ASSERT_THROW(dmlite::validateAcl("A6101,B6101,C4101,D7101,E70,F40,a00,c50"), dmlite::DmException);
  }

  void testTokenRead()
  {
    std::string token = dmlite::generateToken("myid", "/pfn", "dummy", 1000);
    
    // Everything matches
    CPPUNIT_ASSERT_EQUAL(true, dmlite::validateToken(token, "myid", "/pfn", "dummy"));
    
    // Id does not match
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "owned", "/pfn", "dummy"));
    
    // PFN does not match
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "myid", "/etc/root", "dummy"));
    
    /// Password does not match
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "myid", "/pfn", "forgot"));
        
    /// Token expired
    token = dmlite::generateToken("myid", "/pfn", "dummy", 1);
    sleep(2);
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "myid", "/pfn", "dummy"));
  }
  
  void testTokenWrite()
  {
    std::string token = dmlite::generateToken("myid", "/pfn", "dummy", 1000, true);
    
    // Everything matches
    CPPUNIT_ASSERT_EQUAL(true, dmlite::validateToken(token, "myid", "/pfn", "dummy", true));
    
    // Trying to use it for read. It should be OK (i.e. for stats)
    CPPUNIT_ASSERT_EQUAL(true, dmlite::validateToken(token, "myid", "/pfn", "dummy", false));
    
    // Id does not match
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "owned", "/pfn", "dummy", true));
    
    // PFN does not match
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "myid", "/etc/root", "dummy", true));
    
    // Password does not match
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "myid", "/pfn", "forgot", true));
    
    // Token expired
    token = dmlite::generateToken("myid", "/pfn", "dummy", 1, true);
    sleep(2);
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "myid", "/pfn", "dummy", true));
    
    // Trying to write using read
    token = dmlite::generateToken("myid", "/pfn", "dummy", 1000, false);
    CPPUNIT_ASSERT_EQUAL(false, dmlite::validateToken(token, "myid", "/pfn", "dummy", true));
  }

  CPPUNIT_TEST_SUITE(ChkPerm);
  CPPUNIT_TEST(testOwner);
  CPPUNIT_TEST(testGroup);
  CPPUNIT_TEST(testOther);
  CPPUNIT_TEST(testAcl);
  CPPUNIT_TEST(testUserBanned);
  CPPUNIT_TEST(testMainGroupBanned);
  CPPUNIT_TEST(testSecondaryBanned);
  CPPUNIT_TEST(testAclBanned);
  CPPUNIT_TEST(testGetVoFromRole);
  CPPUNIT_TEST(testAclSerialization);
  CPPUNIT_TEST(testAclValidation);
  CPPUNIT_TEST(testTokenRead);
  CPPUNIT_TEST(testTokenWrite);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChkPerm);

int main(int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  return runner.run()?0:-1;
}
