#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dmlite/cpp/authn.h>
#include <dmlite/cpp/utils/security.h>

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
    dmlite::GroupInfo group;
    dmlite::Acl       acl;
    
    // Owner and group
    group["gid"] = 200u;
    
    context->user["uid"] = 200u;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    // Owner but no group
    context->groups[0]["gid"] = 500u;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
  }

  void testGroup()
  {
    dmlite::GroupInfo group;
    dmlite::Acl       acl;
    
    // Main group matches
    group["gid"] = 200u;
    
    context->user["uid"] = 500u;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
    // Main doesn't match, but a secondary does
    group["gid"] = 200u;
    
    context->groups[0]["gid"] = 500u;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testOther()
  {
    dmlite::GroupInfo group;
    dmlite::Acl       acl;
    
    // No match
    group["gid"] = 500u;
    
    context->user["uid"] = 500u;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD));
  }

  void testAcl()
  {
    dmlite::Acl       acl("A70,B7101,C7102,D5103,E70,F00,a70,c70,f50");
    dmlite::GroupInfo group;
    
    // No match
    group["gid"] = 500u;
    context->user["uid"] = 500u;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD));
    // Match user
    context->user["uid"] = 101u;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));

    // Match main group
    context->user["uid"]      = 500u;
    context->groups[0]["gid"] = 103u;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));

    // Match secondary
    group["gid"] = 103u;
    context->groups[0]["gid"] = 500u;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testUserBanned()
  {
    dmlite::GroupInfo group;
    dmlite::Acl       acl;
    
    // Owner and group
    group["gid"] = 200u;
    context->user["uid"]    = 200u;
    context->user["banned"] = 1;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
  }

  void testMainGroupBanned()
  {
    dmlite::GroupInfo group;
    dmlite::Acl       acl;
    
    // Owner matches, group banned
    group["gid"]    = 200u;
    group["banned"] = 1;
    context->user["uid"] = 200u;
    context->groups.push_back(group);
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));
    // Owner doesn't match. Group does, but banned.
    context->user["uid"] = 500u;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testSecondaryBanned()
  {
    dmlite::GroupInfo group;
    dmlite::Acl       acl;
    
    // Owner doesn't match, main group does, but secondary banned.
    group["gid"] = 200u;
    context->groups.push_back(group);
    group["gid"]    = 500u;
    group["banned"] = 1;
    context->groups.push_back(group);
    context->user["uid"] = 500u;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
    // Owner doesn't match, main group neither, secondary does but banned.
    context->groups[0]["gid"] = 500u;
    context->groups[1]["gid"] = 200u;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));

    // Owner doesn't match, main group neither and banned. Secondary OK.
    context->groups[0]["banned"] = 1;
    context->groups[1]["banned"] = 0;
    CPPUNIT_ASSERT_EQUAL(0, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));
  }

  void testAclBanned()
  {
    dmlite::Acl acl("A70,B7101,C7102,D5103,E70,F00,a70,c70,f50");
    dmlite::GroupInfo group;

    // Match user
    group["gid"] = 500u;
    context->groups.push_back(group);
    context->user["uid"] = 101u;
    context->user["banned"] = 1;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IWRITE | S_IEXEC));

    // Match main group
    context->user["uid"] = 500u;
    context->user["banned"] = 0;
    context->groups[0]["gid"] = 103u;
    context->groups[0]["banned"] = 1;
    CPPUNIT_ASSERT_EQUAL(1, dmlite::checkPermissions(context,
                                                     acl, stat_,
                                                     S_IREAD | S_IEXEC));

    // Main group banned, but secondary matches (and banned)
    group["gid"]    = 103u;
    group["banned"] = 1;
    context->groups.push_back(group);
    context->groups[0]["gid"] = 500u;
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
    dmlite::Acl acl("A6101,B6101,C4101,D7101,E70,F40");
    
    CPPUNIT_ASSERT_EQUAL((size_t)6, acl.size());

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kUserObj, acl[0].type);
    CPPUNIT_ASSERT_EQUAL(101u, acl[0].id);
    CPPUNIT_ASSERT_EQUAL(6,    (int)acl[0].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kUser, acl[1].type);
    CPPUNIT_ASSERT_EQUAL(101u, acl[1].id);
    CPPUNIT_ASSERT_EQUAL(6,    (int)acl[1].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kGroupObj, acl[2].type);
    CPPUNIT_ASSERT_EQUAL(101u, acl[2].id);
    CPPUNIT_ASSERT_EQUAL(4,    (int)acl[2].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kGroup, acl[3].type);
    CPPUNIT_ASSERT_EQUAL(101u, acl[3].id);
    CPPUNIT_ASSERT_EQUAL(7,    (int)acl[3].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kMask, acl[4].type);
    CPPUNIT_ASSERT_EQUAL(0u, acl[4].id);
    CPPUNIT_ASSERT_EQUAL(7,  (int)acl[4].perm);

    CPPUNIT_ASSERT_EQUAL(dmlite::AclEntry::kOther, acl[5].type);
    CPPUNIT_ASSERT_EQUAL(0u,        acl[5].id);
    CPPUNIT_ASSERT_EQUAL(4,         (int)acl[5].perm);

    // Test serialization
    acl.insert(acl.begin(), acl.back());
    acl.pop_back();

    std::string serial = acl.serialize();
    CPPUNIT_ASSERT_EQUAL(std::string("A6101,B6101,C4101,D7101,E70,F40"), serial);
  }

  void testAclValidation()
  {
    // Empty is good
    dmlite::Acl("").validate();
    // Good
    dmlite::Acl("A6101,B6101,C4101,D7101,E70,F40").validate();
    // Good too
    dmlite::Acl("A6101,B6101,B6100,C4101,D7101,D4200,E70,F40").validate();
    // With defaults
    dmlite::Acl("A6101,B6101,C4101,D7101,E70,F40,a00,c50,f60").validate();
    // Invalid value
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A8101,A6200,B6101,C4101,D7101,E70,F40").validate(),
                         dmlite::DmException);
    // Invalid type
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,A6200,B6101,C4101,D7101,E70,F40,Z40").validate(),
                         dmlite::DmException);
    // Duplicated ACL_USER_OBJ
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,A6200,B6101,C4101,D7101,E70,F40").validate(),
                         dmlite::DmException);
    // Duplicated ACL_GROUP_OBJ
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,B6101,C4101,C5200,D7101,E70,F40").validate(),
                         dmlite::DmException);
    // Twice the same user ID
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,B6101,B6101,C4101,D7101,E70,F40").validate(),
                         dmlite::DmException);
    // Twice the same group ID
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,B6101,C4101,D7101,D5101,E70,F40").validate(),
                         dmlite::DmException);
    // No mask
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,B6101,C4101,D7101,F40").validate(),
                         dmlite::DmException);
    // No other
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,B6101,C4101,D7101,E70").validate(),
                         dmlite::DmException);
    // Missing other defaults
    CPPUNIT_ASSERT_THROW(dmlite::Acl("A6101,B6101,C4101,D7101,E70,F40,a00,c50").validate(),
                         dmlite::DmException);
  }
  
  void testAclInheritance()
  {
    mode_t      newMode;
    dmlite::Acl parent("A70,C00,E70,F50,a60,c50,f00");
    dmlite::Acl inherited(parent, 3u, 1u, 0700, &newMode);
    
    CPPUNIT_ASSERT_EQUAL(std::string("A63,C01,F00,a60,c50,f00"),
                         inherited.serialize());
  }

  void testTokenRead()
  {
    std::string token = dmlite::generateToken("myid", "/pfn", "dummy", 1000);
    
    // Everything matches
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenOK,
                         dmlite::validateToken(token, "myid", "/pfn", "dummy"));
    
    // Id does not match
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenInvalid,
                         dmlite::validateToken(token, "owned", "/pfn", "dummy"));
    
    // PFN does not match
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenInvalid,
                         dmlite::validateToken(token, "myid", "/etc/root", "dummy"));
    
    /// Password does not match
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenInvalid,
                         dmlite::validateToken(token, "myid", "/pfn", "forgot"));
        
    /// Token expired
    token = dmlite::generateToken("myid", "/pfn", "dummy", 1);
    sleep(2);
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenExpired,
                         dmlite::validateToken(token, "myid", "/pfn", "dummy"));
  }
  
  void testTokenWrite()
  {
    std::string token = dmlite::generateToken("myid", "/pfn", "dummy", 1000, true);
    
    // Everything matches
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenOK,
                         dmlite::validateToken(token, "myid", "/pfn", "dummy", true));
    
    // Trying to use it for read. It should be OK (i.e. for stats)
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenOK,
                         dmlite::validateToken(token, "myid", "/pfn", "dummy", false));
    
    // Id does not match
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenInvalid,
                         dmlite::validateToken(token, "owned", "/pfn", "dummy", true));
    
    // PFN does not match
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenInvalid,
                         dmlite::validateToken(token, "myid", "/etc/root", "dummy", true));
    
    // Password does not match
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenInvalid,
                         dmlite::validateToken(token, "myid", "/pfn", "forgot", true));
    
    // Token expired
    token = dmlite::generateToken("myid", "/pfn", "dummy", 1, true);
    sleep(2);
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenExpired,
                         dmlite::validateToken(token, "myid", "/pfn", "dummy", true));
    
    // Trying to write using read
    token = dmlite::generateToken("myid", "/pfn", "dummy", 1000, false);
    CPPUNIT_ASSERT_EQUAL(dmlite::kTokenInvalidMode,
                         dmlite::validateToken(token, "myid", "/pfn", "dummy", true));
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
  CPPUNIT_TEST(testAclInheritance);
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
  return runner.run()?0:1;
}
