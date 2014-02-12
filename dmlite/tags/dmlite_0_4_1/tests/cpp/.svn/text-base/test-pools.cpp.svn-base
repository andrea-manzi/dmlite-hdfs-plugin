#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/poolmanager.h>
#include <iostream>
#include <vector>
#include "test-base.h"

const float GB = 1073741824;


static void printPools(dmlite::StackInstance* stack, const std::vector<dmlite::Pool>& pools)
{
  for (unsigned i = 0; i < pools.size(); ++i) {
    try {
      dmlite::PoolHandler *handler = stack->getPoolDriver(pools[i].type)->createPoolHandler(pools[i].name);

      std::cout << "Pool type:   " << handler->getPoolType()   << std::endl
                << "Pool name:   " << handler->getPoolName()   << std::endl
                << "Capacity:    " << handler->getTotalSpace() / GB << " GB" << std::endl
                << "Free:        " << handler->getFreeSpace() / GB  << " GB" << std::endl
                << "Metadata:" << std::endl
                << pools[i].serialize() << std::endl;

      delete handler;
    }
    catch (dmlite::DmException& e) {
      if (e.code() != DM_UNKNOWN_POOL_TYPE)
        throw;
      std::cout << "Pool type:   " << pools[i].type << std::endl
                << "Pool name:   " << pools[i].name << std::endl
                << "UNKNOWN POOL TYPE!!" << std::endl;
    }
    std::cout << std::endl;
  }
}


class TestPools: public TestBase {
protected:
  dmlite::PoolManager* poolManager;
  
public:
  
  void setUp()
  {
    TestBase::setUp();
    poolManager = this->stackInstance->getPoolManager();
  }
  
  void tearDown()
  {
    dmlite::Pool pool;
    
    if (poolManager) {
      try {
        pool.name = "test_hadoop";
        pool.type = "hadoop";
        poolManager->deletePool(pool);
      }
      catch (dmlite::DmException& e) {
        if (e.code() != DM_NO_SUCH_POOL) throw;
      }

      try {
        pool.name = "test_fs";
        pool.type = "filesystem";
        poolManager->deletePool(pool);
      }
      catch (dmlite::DmException& e) {
        if (e.code() != DM_NO_SUCH_POOL) throw;
      }
    }
    
    TestBase::tearDown();
  }
  
  void testBase()
  {
    std::cout << "============= ALL POOLS ============="
            << std::endl << std::endl;
    printPools(stackInstance, poolManager->getPools(dmlite::PoolManager::kAny));

    // Available for read
    std::cout << "======== AVAILABLE FOR READ ========="
              << std::endl << std::endl;
    printPools(stackInstance, poolManager->getPools(dmlite::PoolManager::kForRead));

    // Available for write
    std::cout << "======== AVAILABLE FOR WRITE ========"
              << std::endl << std::endl;
    printPools(stackInstance, poolManager->getPools(dmlite::PoolManager::kForWrite));

    // Show not available
    std::cout << "=========== NOT AVAILABLE ==========="
              << std::endl << std::endl;
    printPools(stackInstance, poolManager->getPools(dmlite::PoolManager::kNone));
  }
  
  void testUnknown()
  {
    dmlite::Pool pool;
    
    pool.name = "does_not_matter";
    pool.type = "unknown_pool_type";
    
    CPPUNIT_ASSERT_THROW(poolManager->newPool(pool), dmlite::DmException);
  }
  
  void testAddHadoop()
  {
    dmlite::Pool pool;
    
    // Add it
    pool.name = "test_hadoop";
    pool.type = "hadoop";
    pool["hostname"] = std::string("namenode.cern.ch");
    pool["port"]     = 8020;
    pool["username"] = std::string("test");
    pool["mode"]     = std::string("r");
    
    poolManager->newPool(pool);
    
    // Get it back and check it was OK
    pool.name = pool.type = "XYZ";
    pool.clear();
    
    pool = poolManager->getPool("test_hadoop");
    
    CPPUNIT_ASSERT_EQUAL(std::string("test_hadoop"),      pool.name);
    CPPUNIT_ASSERT_EQUAL(std::string("hadoop"),           pool.type);
    CPPUNIT_ASSERT_EQUAL(std::string("namenode.cern.ch"), pool.getString("hostname"));
    CPPUNIT_ASSERT_EQUAL(8020ul,                          pool.getUnsigned("port"));
    CPPUNIT_ASSERT_EQUAL(std::string("test"),             pool.getString("username"));
    CPPUNIT_ASSERT_EQUAL(std::string("r"),                pool.getString("mode"));
    
    poolManager->deletePool(pool);
    CPPUNIT_ASSERT_THROW(poolManager->getPool("test_hadoop"), dmlite::DmException);
  }
   
  CPPUNIT_TEST_SUITE(TestPools);
  CPPUNIT_TEST(testBase);
  CPPUNIT_TEST(testUnknown);
  CPPUNIT_TEST(testAddHadoop);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestPools);

int main(int argn, char **argv)
{
  return testBaseMain(argn, argv);
}
