#include <dmlite/cpp/dmlite.h>
#include <iostream>
#include <vector>

const float GB = 1073741824;

int main(int argn, char **argv)
{
  std::vector<Pool> pools;

  // Need a plugin to load
  if (argn < 2) {
    std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
    return -1;
  }

  // Load plugin
  dmlite::PluginManager   manager;
  dmlite::StackInstance*  stack;
  dmlite::PoolManager*    poolManager;
  dmlite::SecurityContext secCtx;

  try {
    manager.loadConfiguration(argv[1]);
    stack = new dmlite::StackInstance(&manager);
    
    stack->setSecurityContext(secCtx);
    
    poolManager = stack->getPoolManager();
    // Ask for the pools
    pools = poolManager->getPools();
  }
  catch (dmlite::DmException& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  // Print info
  for (unsigned i = 0; i < pools.size(); ++i) {
    try {
      dmlite::PoolHandler *handler = stack->getPoolDriver(pools[i].pool_type)->createPoolHandler(pools[i].pool_name);

      std::cout << "Pool type:   " << handler->getPoolType()   << std::endl
                << "Pool name:   " << handler->getPoolName()   << std::endl
                << "Capacity:    " << handler->getTotalSpace() / GB << " GB" << std::endl
                << "Free:        " << handler->getFreeSpace() / GB  << " GB" << std::endl;
      
      delete handler;
    }
    catch (dmlite::DmException& e) {
      if (e.code() != DM_UNKNOWN_POOL_TYPE)
        throw;
      std::cout << "Pool type:   " << pools[i].pool_type << std::endl
                << "Pool name:   " << pools[i].pool_name << std::endl
                << "UNKNOWN POOL TYPE!!" << std::endl;
    }
    std::cout << std::endl;
  }


  // Free
  delete stack;

  // Done
  return 0;
}
