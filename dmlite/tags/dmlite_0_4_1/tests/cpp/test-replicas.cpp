#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include "test-base.h"

class TestReplicas: public TestBase
{
protected:
  const static int   MODE;
  const static char *FOLDER;
  const static char *FILE;

public:

  void setUp()
  {
    TestBase::setUp();
    this->catalog->makeDir(FOLDER, MODE);
    this->catalog->create(FILE, MODE);
  }

  void tearDown()
  {
    if (this->catalog != 0x00) {
      try {
        this->catalog->extendedStat(FILE).stat;
        std::vector<dmlite::Replica> replicas = this->catalog->getReplicas(FILE);
        for (unsigned i = 0; i < replicas.size(); ++i) {
          this->catalog->deleteReplica(replicas[i]);
        }
      }
      catch (dmlite::DmException& e) {
        switch (e.code()) {
          case DM_NO_SUCH_FILE: case DM_NO_REPLICAS:
            break;
          default:
            throw;
        }
      }

      IGNORE_NOT_EXIST(this->catalog->unlink(FILE));
      IGNORE_NOT_EXIST(this->catalog->removeDir(FOLDER));
    }
    TestBase::tearDown();
  }

  void testAddAndRemove()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;
    
    dmlite::Replica replica;
    
    replica.fileid = s.st_ino;
    replica.server = "b.host.com";
    replica.rfn    = "http://a.host.com/replica";
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kPermanent;
    
    replica["pool"]       = std::string("the-pool");
    replica["filesystem"] = std::string("the-fs");
    replica["additional"] = std::string("metadata");
    
    this->catalog->addReplica(replica);

    replica.clear();
    replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL((unsigned)s.st_ino, (unsigned)replica.fileid);
    CPPUNIT_ASSERT_EQUAL(std::string("http://a.host.com/replica"),
                         std::string(replica.rfn));
    CPPUNIT_ASSERT_EQUAL(dmlite::Replica::kAvailable, replica.status);
    CPPUNIT_ASSERT_EQUAL(std::string("the-fs"),     replica.getString("filesystem"));
    CPPUNIT_ASSERT_EQUAL(std::string("the-pool"),   replica.getString("pool"));
    CPPUNIT_ASSERT_EQUAL(std::string("b.host.com"), std::string(replica.server));
    CPPUNIT_ASSERT_EQUAL(std::string("metadata"),   replica.getString("additional"));

    replica = this->catalog->getReplica("http://a.host.com/replica");
    
    this->catalog->deleteReplica(replica);

    CPPUNIT_ASSERT_EQUAL((size_t)0, (size_t)this->catalog->getReplicas(FILE).size());
  }

  void testAddNoHost()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;
    
    dmlite::Replica replica;
    
    replica.fileid = s.st_ino;
    replica.rfn    = "http://a.host.com/replica";
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kPermanent;
    
    replica["pool"]       = std::string("the-pool");
    replica["filesystem"] = std::string("the-fs");

    this->catalog->addReplica(replica);

    replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL(std::string("http://a.host.com/replica"),
                         std::string(replica.rfn));
    CPPUNIT_ASSERT_EQUAL(std::string("a.host.com"),
                         std::string(replica.server));

    this->catalog->deleteReplica(replica);
  }

  void testModify()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;
    
    dmlite::Replica replica;
    
    replica.fileid = s.st_ino;
    replica.server = "a.host.com";
    replica.rfn    = "http://a.host.com/replica";
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kPermanent;
    
    replica["pool"]       = std::string("the-pool");
    replica["filesystem"] = std::string("the-fs");
    replica["additional"] = std::string("metadata");

    this->catalog->addReplica(replica);

    replica = this->catalog->getReplica("http://a.host.com/replica");
    
    replica.ltime  = 12348;
    replica.status = dmlite::Replica::kToBeDeleted;
    replica.type   = dmlite::Replica::kVolatile;
    replica["additional"] = std::string("something else");
    
    this->catalog->updateReplica(replica);
    
    replica.clear();
    replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL(12348, (int)replica.ltime);
    CPPUNIT_ASSERT_EQUAL(dmlite::Replica::kToBeDeleted, replica.status);
    CPPUNIT_ASSERT_EQUAL(dmlite::Replica::kVolatile,    replica.type);
    CPPUNIT_ASSERT_EQUAL(std::string("something else"), replica.getString("additional"));
  }

  void testCachedEntries()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;
    
    dmlite::Replica replica;
    
    replica.fileid = s.st_ino;
    replica.server = "b.host.com";
    replica.rfn    = "http://a.host.com/replica";
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kPermanent;
    
    replica["pool"]       = std::string("the-pool");
    replica["filesystem"] = std::string("the-fs");

    this->catalog->addReplica(replica);

    replica = this->catalog->getReplicas(FILE)[0];
    dmlite::Replica replicaCached = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL((unsigned)replicaCached.fileid, (unsigned)replica.fileid);
    CPPUNIT_ASSERT_EQUAL(std::string(replicaCached.rfn),
                         std::string(replica.rfn));
    CPPUNIT_ASSERT_EQUAL(replicaCached.status, replica.status);
    CPPUNIT_ASSERT_EQUAL(std::string(replicaCached.server), std::string(replica.server));
    
    CPPUNIT_ASSERT_EQUAL(replicaCached.getString("filesystem"),
                         replica.getString("filesystem"));
    CPPUNIT_ASSERT_EQUAL(replicaCached.getString("pool"),
                         replica.getString("pool"));
    
    replica.fileid = s.st_ino;
    replica.server = "b.host.com";
    replica.rfn    = "http://a.host.com/replica2";
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kPermanent;
    
    replica["pool"]       = std::string("the-pool");
    replica["filesystem"] = std::string("the-fs");
    
    this->catalog->addReplica(replica);

    std::vector<dmlite::Replica> replicaVector = this->catalog->getReplicas(FILE);

    CPPUNIT_ASSERT_EQUAL(2, (int) replicaVector.size());
  }

  void testCachedModify()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;
    
    dmlite::Replica replica;
    
    replica.fileid = s.st_ino;
    replica.server = "a.host.com";
    replica.rfn    = "http://a.host.com/replica";
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kPermanent;
    
    replica["pool"]       = std::string("the-pool");
    replica["filesystem"] = std::string("the-fs");

    this->catalog->addReplica(replica);

    // value might be cached
    replica = this->catalog->getReplicas(FILE)[0];

    replica.ltime  = 12348;
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kVolatile;
    
    this->catalog->updateReplica(replica);

    replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL(12348, (int)replica.ltime);
    CPPUNIT_ASSERT_EQUAL(dmlite::Replica::kAvailable, replica.status);
    CPPUNIT_ASSERT_EQUAL(dmlite::Replica::kVolatile,  replica.type);
  }

  void testDuplicated()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;
    
    dmlite::Replica replica;
    
    replica.fileid = s.st_ino;
    replica.server = "a.host.com";
    replica.rfn    = "http://a.host.com/replica";
    replica.status = dmlite::Replica::kAvailable;
    replica.type   = dmlite::Replica::kPermanent;
    
    // Add once
    this->catalog->addReplica(replica);
    
    // Add twice
    try {
      this->catalog->addReplica(replica);
      CPPUNIT_FAIL("Added twice the same rfn");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_EXISTS, e.code());
    }
  }

  CPPUNIT_TEST_SUITE(TestReplicas);
  CPPUNIT_TEST(testAddAndRemove);
  CPPUNIT_TEST(testAddNoHost);
  CPPUNIT_TEST(testModify);
  CPPUNIT_TEST(testCachedEntries);
  CPPUNIT_TEST(testCachedModify);
  CPPUNIT_TEST(testDuplicated);
  CPPUNIT_TEST_SUITE_END();
};

const int   TestReplicas::MODE   = 0700;
const char* TestReplicas::FOLDER = "test-replica";
const char* TestReplicas::FILE   = "test-replica/file";

CPPUNIT_TEST_SUITE_REGISTRATION(TestReplicas);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
