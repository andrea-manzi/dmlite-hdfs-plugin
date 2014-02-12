#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <cstring>
#include <dmlite/common/dm_config.h>
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
                         std::string(ctx->getUser().name));
    CPPUNIT_ASSERT(0 != ctx->getUser().uid);
  }

  void testDiff()
  {
    dmlite::SecurityContext ctx1, ctx2;

    this->stackInstance->setSecurityCredentials(cred1);
    ctx1 = *this->stackInstance->getSecurityContext();

    this->stackInstance->setSecurityCredentials(cred2);
    ctx2 = *this->stackInstance->getSecurityContext();

    CPPUNIT_ASSERT(ctx1.getUser().uid != ctx2.getUser().uid);
  }
  
  void testHostCert()
  {
    // Initialize
    Credentials hostCreds;
    memset(&hostCreds, 0, sizeof(Credentials));
    
    X509* hostX509;
    BIO*  in = BIO_new(BIO_s_file());
    CPPUNIT_ASSERT(BIO_read_filename(in, HOST_CERTIFICATE) > 0);
    hostX509 = PEM_read_bio_X509_AUX(in, NULL, NULL, NULL);
    BIO_free_all(in);
    CPPUNIT_ASSERT(hostX509 != NULL);
    
    hostCreds.client_name = hostX509->name;
    
    // Check
    const dmlite::SecurityContext* ctx;
    this->stackInstance->setSecurityCredentials(hostCreds);
    ctx = this->stackInstance->getSecurityContext();
    
    CPPUNIT_ASSERT_EQUAL(0, (int)ctx->getUser().uid);
    
    // Free
    hostCreds.client_name = 0;
    X509_free(hostX509);
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
