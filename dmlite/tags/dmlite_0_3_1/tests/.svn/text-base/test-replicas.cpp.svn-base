#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <sys/stat.h>
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
        struct stat s = this->catalog->extendedStat(FILE).stat;
        std::vector<FileReplica> replicas = this->catalog->getReplicas(FILE);
        for (unsigned i = 0; i < replicas.size(); ++i) {
          this->catalog->deleteReplica("", s.st_ino, replicas[i].rfn);
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

    this->catalog->addReplica(std::string(), s.st_ino, "b.host.com",
                              "http://a.host.com/replica", '-', 'P',
                              "the-pool", "the-fs");

    FileReplica replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL((unsigned)s.st_ino, (unsigned)replica.fileid);
    CPPUNIT_ASSERT_EQUAL(std::string("http://a.host.com/replica"),
                         std::string(replica.rfn));
    CPPUNIT_ASSERT_EQUAL('-', replica.status);
    CPPUNIT_ASSERT_EQUAL(std::string("the-fs"), std::string(replica.filesystem));
    CPPUNIT_ASSERT_EQUAL(std::string("the-pool"), std::string(replica.pool));
    CPPUNIT_ASSERT_EQUAL(std::string("b.host.com"), std::string(replica.server));

    this->catalog->deleteReplica(std::string(), s.st_ino, "http://a.host.com/replica");

    CPPUNIT_ASSERT_THROW(this->catalog->get(FILE), dmlite::DmException);
  }

  void testAddNoHost()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;

    this->catalog->addReplica(std::string(), s.st_ino, std::string(),
                              "http://a.host.com/replica", '-', 'P',
                              "the-pool", "the-fs");

    FileReplica replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL(std::string("http://a.host.com/replica"),
                         std::string(replica.rfn));
    CPPUNIT_ASSERT_EQUAL(std::string("a.host.com"),
                         std::string(replica.server));

    this->catalog->deleteReplica(std::string(), s.st_ino, "http://a.host.com/replica");
  }

  void testModify()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;

    this->catalog->addReplica(std::string(), s.st_ino, "a.host",
                              "https://a.host.com/replica", '-', 'P',
                              "the-pool", "the-fs");

    this->catalog->replicaSetLifeTime("https://a.host.com/replica", 12348);
    this->catalog->replicaSetStatus("https://a.host.com/replica", 'D');
    this->catalog->replicaSetType("https://a.host.com/replica", 'V');

    FileReplica replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL(12348, (int)replica.ltime);
    CPPUNIT_ASSERT_EQUAL('D', replica.status);
    CPPUNIT_ASSERT_EQUAL('V', replica.type);
  }

  void testCachedEntries()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;

    this->catalog->addReplica(std::string(), s.st_ino, "b.host.com",
                              "http://a.host.com/replica", '-', 'P',
                              "the-pool", "the-fs");

    FileReplica replica = this->catalog->getReplicas(FILE)[0];
    FileReplica replicaCached = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL((unsigned)replicaCached.fileid, (unsigned)replica.fileid);
    CPPUNIT_ASSERT_EQUAL(std::string(replicaCached.rfn),
                         std::string(replica.rfn));
    CPPUNIT_ASSERT_EQUAL(replicaCached.status, replica.status);
    CPPUNIT_ASSERT_EQUAL(std::string(replicaCached.filesystem), std::string(replica.filesystem));
    CPPUNIT_ASSERT_EQUAL(std::string(replicaCached.pool), std::string(replica.pool));
    CPPUNIT_ASSERT_EQUAL(std::string(replicaCached.server), std::string(replica.server));

    this->catalog->addReplica(std::string(), s.st_ino, "b.host.com",
                              "http://a.host.com/replica2", '-', 'P',
                              "the-pool", "the-fs");

    std::vector<FileReplica> replicaVector = this->catalog->getReplicas(FILE);

    CPPUNIT_ASSERT_EQUAL(2, (int) replicaVector.size());
  }

  void testCachedModify()
  {
    struct stat s;

    s = this->catalog->extendedStat(FILE).stat;

    this->catalog->addReplica(std::string(), s.st_ino, "a.host",
                              "https://a.host.com/replica", '-', 'P',
                              "the-pool", "the-fs");

    // value might be cached
    FileReplica replica = this->catalog->getReplicas(FILE)[0];

    this->catalog->replicaSetLifeTime("https://a.host.com/replica", 12348);
    this->catalog->replicaSetStatus("https://a.host.com/replica", 'D');
    this->catalog->replicaSetType("https://a.host.com/replica", 'V');

    replica = this->catalog->getReplicas(FILE)[0];

    CPPUNIT_ASSERT_EQUAL(12348, (int)replica.ltime);
    CPPUNIT_ASSERT_EQUAL('D', replica.status);
    CPPUNIT_ASSERT_EQUAL('V', replica.type);
  }


  CPPUNIT_TEST_SUITE(TestReplicas);
  CPPUNIT_TEST(testAddAndRemove);
  CPPUNIT_TEST(testAddNoHost);
  CPPUNIT_TEST(testModify);
  CPPUNIT_TEST(testCachedEntries);
  CPPUNIT_TEST(testCachedModify);
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

