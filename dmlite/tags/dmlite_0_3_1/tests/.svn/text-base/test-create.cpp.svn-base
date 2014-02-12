#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <sys/stat.h>
#include <unistd.h>
#include "test-base.h"
#include "dmlite/cpp/utils/dm_security.h"

class TestCreate: public TestBase
{
protected:
  struct stat statBuf, iStat;
  const static int   MODE;
  const static int   MODE_ALL;
  const static char *FOLDER;
  const static char *NESTED;
  const static char *SYMLINK;
  const static char *FILE;

  const dmlite::SecurityContext* ctx;

public:

  void setUp()
  {
    TestBase::setUp();
    this->stackInstance->setSecurityCredentials(cred1);
    this->catalog->makeDir(FOLDER, MODE);
    // Reset ACL in case FOLDER inherited some (may break some tests)
    ctx = this->stackInstance->getSecurityContext();
    std::stringstream ss;
    ss << "A7" << ctx->getUser().uid << ",C0" << ctx->getGroup(0).gid << ",E70,F00";
    this->catalog->setAcl(FOLDER, dmlite::deserializeAcl(ss.str()));
  }

  void tearDown()
  {
    if (this->catalog != 0x00) {
      this->stackInstance->setSecurityContext(root);
      this->catalog->changeMode(FOLDER, 0777);
      
      try {
        struct stat s = this->catalog->extendedStat(FILE).stat;
        std::vector<FileReplica> replicas = this->catalog->getReplicas(FILE);
        for (unsigned i = 0; i < replicas.size(); i++) {
          this->catalog->deleteReplica("", s.st_ino, replicas[i].rfn);
        }
      }
      catch (...) {
        // Ignore
      }
      
      IGNORE_NOT_EXIST(this->catalog->unlink(FILE));
      IGNORE_NOT_EXIST(this->catalog->unlink(SYMLINK));
      IGNORE_NOT_EXIST(this->catalog->removeDir(NESTED));

      IGNORE_NOT_EXIST(this->catalog->removeDir(FOLDER));

      this->catalog->umask(022);
    }
    TestBase::tearDown();
  }

  void testRegular()
  {
    struct stat s;
    time_t ini_mtime;
    // Empty
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(0, (int)s.st_nlink);
    CPPUNIT_ASSERT_EQUAL(MODE | S_IFDIR, (int)s.st_mode);
    ini_mtime = s.st_mtime;
    sleep(1); // Or mtime may be the same, since the operation can be fast

    // Add a folder
    this->catalog->makeDir(NESTED, MODE);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(1, (int)s.st_nlink);
    CPPUNIT_ASSERT(s.st_mtime > ini_mtime);
    CPPUNIT_ASSERT_EQUAL(ctx->getUser().uid, s.st_uid);
    CPPUNIT_ASSERT_EQUAL(ctx->getGroup().gid, s.st_gid);

    // Add a symlink
    this->catalog->symlink(FOLDER, SYMLINK);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(2, (int)s.st_nlink);
    
    // Add a file
    this->catalog->create(FILE, MODE);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(3, (int)s.st_nlink);
    CPPUNIT_ASSERT_EQUAL(ctx->getUser().uid, s.st_uid);
    CPPUNIT_ASSERT_EQUAL(ctx->getGroup().gid, s.st_gid);
    
    s = this->catalog->extendedStat(FILE).stat;
    CPPUNIT_ASSERT_EQUAL(MODE | S_IFREG, (int)s.st_mode);
    
    // Remove the file
    this->catalog->unlink(FILE);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(2, (int)s.st_nlink);

    // Remove the folder
    this->catalog->removeDir(NESTED);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(1, (int)s.st_nlink);

    // And the symlink
    this->catalog->unlink(SYMLINK);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(0, (int)s.st_nlink);
  }

  void testSetGid()
  {
    struct stat s;

    this->catalog->changeMode(FOLDER, MODE | S_ISGID);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(MODE | S_ISGID | S_IFDIR, (int)s.st_mode);

    this->catalog->makeDir(NESTED, MODE);
    s = this->catalog->extendedStat(NESTED).stat;
    CPPUNIT_ASSERT_EQUAL(MODE | S_IFDIR | S_ISGID, (int)s.st_mode);
    this->catalog->removeDir(NESTED);
  }

  void testChmodDifferentUser()
  {
    struct stat s;

    this->catalog->create(FILE, MODE);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(MODE | S_IFREG, (int)s.st_mode);

    // Change user
    this->stackInstance->setSecurityCredentials(cred2);

    // Nested file shouldn't pass
    CPPUNIT_ASSERT_THROW(this->catalog->extendedStat(NESTED), dmlite::DmException);

    // Change user back 
    this->stackInstance->setSecurityCredentials(cred2);

    // Changing the mode of the parent
    this->catalog->changeMode(FILE, MODE_ALL);
    s = this->catalog->extendedStat(FOLDER).stat;
    CPPUNIT_ASSERT_EQUAL(MODE | S_IFREG, (int)s.st_mode);

    // Change user
    this->stackInstance->setSecurityCredentials(cred2);

    // Nested file should pass
    s = this->catalog->extendedStat(NESTED).stat;
    CPPUNIT_ASSERT_EQUAL(MODE | S_IFREG, (int)s.st_mode);
  }

  void testUmask()
  {
    struct stat s;

    this->catalog->umask(066);
    mode_t prev = this->catalog->umask(0077);
    CPPUNIT_ASSERT_EQUAL(066, (int)prev);
    
    this->catalog->makeDir(NESTED, 0777);
    s = this->catalog->extendedStat(NESTED).stat;
    CPPUNIT_ASSERT_EQUAL(0700 | S_IFDIR, (int)s.st_mode);

    this->catalog->removeDir(NESTED);
  }

  void testExistsNoReplicas()
  {
    struct stat s;

    this->catalog->create(FILE, MODE);
    // Again, should not fail
    this->catalog->create(FILE, MODE);

    s = this->catalog->extendedStat(FILE).stat;
    CPPUNIT_ASSERT_EQUAL(0l, (long)s.st_size);
  }

  void testExistsWithReplicas()
  {
    struct stat s;

    this->catalog->create(FILE, MODE);
    s = this->catalog->extendedStat(FILE).stat;

    this->catalog->addReplica("", s.st_ino,
                              "localhost", "sfn://something",
                              '-', 'P',
                              "", "");

    CPPUNIT_ASSERT_THROW(this->catalog->create(FILE, MODE), dmlite::DmException);

    this->catalog->deleteReplica("", s.st_ino, "sfn://something");
  }

  void testPathDoesNotExist()
  {
    try {
      this->catalog->create("/fake/something/else/file", MODE);
      CPPUNIT_FAIL("Exception not thrown");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_NO_SUCH_FILE, e.code());
    }
  }

  void testPermissionDenied()
  {
    this->catalog->changeMode(FOLDER, 0500);
    try {
      this->catalog->create(FILE, MODE);
      CPPUNIT_FAIL("Exception not thrown");
    }
    catch (dmlite::DmException& e) {
      CPPUNIT_ASSERT_EQUAL(DM_FORBIDDEN, e.code());
    }

    this->catalog->changeMode(FOLDER, 0600);
    try {
      this->catalog->extendedStat(FILE).stat;
      CPPUNIT_FAIL("Exception not thrown");
    }
    catch (dmlite::DmException& e) {
      // The problem should have been the S_IEXEC!!
      CPPUNIT_ASSERT_EQUAL(DM_FORBIDDEN, e.code());
    }
  }
  
  void testSetSize()
  {
    this->catalog->create(FILE, MODE);
    this->catalog->changeSize(FILE, 555);
    struct stat s = this->catalog->extendedStat(FILE).stat;
    CPPUNIT_ASSERT_EQUAL((__off_t)555, s.st_size);
  }

  CPPUNIT_TEST_SUITE(TestCreate);
  CPPUNIT_TEST(testRegular);
  CPPUNIT_TEST(testSetGid);
  CPPUNIT_TEST(testUmask);
  CPPUNIT_TEST(testExistsNoReplicas);
  CPPUNIT_TEST(testExistsWithReplicas);
  CPPUNIT_TEST(testPathDoesNotExist);
  CPPUNIT_TEST(testPermissionDenied);
  CPPUNIT_TEST(testSetSize);
  CPPUNIT_TEST_SUITE_END();
};

const int   TestCreate::MODE    = 0700;
const int   TestCreate::MODE_ALL = 0777;
const char* TestCreate::FOLDER  = "test-create";
const char* TestCreate::NESTED  = "test-create/nested////";
const char* TestCreate::SYMLINK = "test-create/test-link";
const char* TestCreate::FILE    = "test-create/test-file";

CPPUNIT_TEST_SUITE_REGISTRATION(TestCreate);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
