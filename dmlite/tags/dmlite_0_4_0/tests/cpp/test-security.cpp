#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <cstring>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/common/config.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <sys/stat.h>
#include "test-base.h"

class TestSecurity: public TestBase
{
public:

  void setUp()
  {
    TestBase::setUp();
  }

  void tearDown()
  {
    TestBase::tearDown();
  }

  void testCred()
  {
    const dmlite::SecurityContext *ctx;

    this->stackInstance->setSecurityCredentials(cred1);
    ctx = this->stackInstance->getSecurityContext();

    CPPUNIT_ASSERT_EQUAL(std::string(TEST_USER),
                         std::string(ctx->user.name));
  }

  void testDiff()
  {
    dmlite::SecurityContext ctx1, ctx2;

    this->stackInstance->setSecurityCredentials(cred1);
    ctx1 = *this->stackInstance->getSecurityContext();

    this->stackInstance->setSecurityCredentials(cred2);
    ctx2 = *this->stackInstance->getSecurityContext();

    CPPUNIT_ASSERT(boost::any_cast<uid_t>(ctx1.user["uid"]) !=
                   boost::any_cast<uid_t>(ctx2.user["uid"]));
  }
  
  void testHostCert()
  {
    // Initialize
    dmlite::SecurityCredentials hostCreds;
    
    BIO*  in = BIO_new(BIO_s_file());
    X509* hostX509 = NULL;
    if (BIO_read_filename(in, HOST_CERTIFICATE) > 0) {
      hostX509 = PEM_read_bio_X509_AUX(in, NULL, NULL, NULL);
      BIO_free_all(in);
      CPPUNIT_ASSERT(hostX509 != NULL);
      hostCreds.clientName = hostX509->name;
    }
    else {
      // Obviously is going to fail, but still, need to call
      hostCreds.clientName = "/DC=ch/DC=cern/OU=computers/CN=host.cern.ch";
    }
    
    // Check
    const dmlite::SecurityContext* ctx;
    this->stackInstance->setSecurityCredentials(hostCreds);
    ctx = this->stackInstance->getSecurityContext();
    
    CPPUNIT_ASSERT_EQUAL(0u, boost::any_cast<uid_t>(ctx->user["uid"]));
    
    // Free
    if (hostX509) X509_free(hostX509);
  }

  CPPUNIT_TEST_SUITE(TestSecurity);
  CPPUNIT_TEST(testCred);
  CPPUNIT_TEST(testDiff);
  CPPUNIT_TEST(testHostCert);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestSecurity);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
