/// @file    include/dmlite/cpp/dummy/DummyPool.h
/// @brief   A dummy plugin that just delegates calls to a decorated one.
/// @details It makes sense as a base for other decorator plug-ins.
/// @author  Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#ifndef DMLITE_CPP_DUMMY_POOL_H
#define DMLITE_CPP_DUMMY_POOL_H

#include "../poolmanager.h"

namespace dmlite {

  class DummyPoolManager: public PoolManager {
   public:
    DummyPoolManager(PoolManager* decorated) throw (DmException);
    virtual ~DummyPoolManager();

    virtual void setStackInstance(StackInstance*) throw (DmException);
    virtual void setSecurityContext(const SecurityContext*) throw (DmException);

    virtual std::vector<Pool> getPools(PoolAvailability availability) throw (DmException);

    virtual Pool getPool(const std::string& poolname) throw (DmException);

    virtual Location whereToRead(const std::string& path) throw (DmException);

    virtual Location whereToWrite(const std::string& path) throw (DmException);

   protected:
    PoolManager* decorated_;
  };

};

#endif // DMLITE_DUMMYPOOL_H
