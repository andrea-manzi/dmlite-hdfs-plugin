/// @file  plugin_mock.h
/// @brief Mock plugin for the C tests.
/// @note  We rely on the BuiltInAuth for the authentication mechanism!
#ifndef PLUGIN_MOCK_H
#define	PLUGIN_MOCK_H

#include <dmlite/cpp/io.h>
#include <dmlite/cpp/poolmanager.h>
#include <dmlite/cpp/dummy/DummyCatalog.h>
#include <dmlite/cpp/dummy/DummyPool.h>
#include <map>

namespace dmlite {
  
  struct MockDirectory {
    DIR*         d;
    ExtendedStat holder;
  };
  
  /// Catalog mock plugin (uses FS calls)
  class MockCatalog: public DummyCatalog {
   private:
    typedef std::map<std::string, Replica> RfnReplicaType;
    typedef std::map<ino_t, RfnReplicaType> InoReplicasType;
    InoReplicasType replicas;
    
   public:
    MockCatalog();
    ~MockCatalog();
    
    std::string getImplId() const throw ();
    
    void        changeDir     (const std::string&) throw (DmException);
    std::string getWorkingDir (void)               throw (DmException);
    
    ExtendedStat extendedStat(const std::string&, bool) throw (DmException);
    
    Directory*    openDir (const std::string&) throw (DmException);
    void          closeDir(Directory*)         throw (DmException);
    ExtendedStat* readDirx(Directory*)         throw (DmException);
    
    mode_t umask    (mode_t)                           throw ();
    void   setMode  (const std::string&, mode_t)       throw (DmException);
    void   makeDir  (const std::string&, mode_t)       throw (DmException);
    void   create   (const std::string&, mode_t)       throw (DmException);
    void   rename   (const std::string&, const std::string&) throw (DmException);
    void   removeDir(const std::string&)                     throw (DmException);
    void   unlink   (const std::string&)                     throw (DmException);
    
    void addReplica   (const Replica&) throw (DmException);
    void deleteReplica(const Replica&) throw (DmException);
    std::vector<Replica> getReplicas(const std::string&) throw (DmException);
    
    Replica getReplica   (const std::string& rfn) throw (DmException);
    void    updateReplica(const Replica& replica) throw (DmException);
  };

  /// Pool mock plugin
  class MockPoolManager: public DummyPoolManager {
   public:
     MockPoolManager();
     ~MockPoolManager();
     
     std::string getImplId() const throw();
     
     std::vector<Pool> getPools(PoolAvailability availability) throw (DmException);
     void newPool(const Pool&) throw (DmException);
     void updatePool(const Pool&) throw (DmException);
     void deletePool(const Pool&) throw (DmException);
     
     Location whereToRead (const std::string& path) throw (DmException);
     Location whereToRead (ino_t)                   throw (DmException);
     Location whereToWrite(const std::string& path) throw (DmException);
  };
  
  /// Mock IO handler
  class MockIOHandler: public IOHandler {
    private:
     char     content[1024];
     off_t    p;
     bool     closed;
    public:
     MockIOHandler();
     ~MockIOHandler();

     void   close(void) throw (DmException);
     size_t read (char* buffer, size_t count) throw (DmException);
     size_t write(const char* buffer, size_t count) throw (DmException);
     void   seek (off_t offset, Whence whence) throw (DmException);
     off_t  tell (void) throw (DmException);
     void   flush(void) throw (DmException);
     bool   eof  (void) throw (DmException);
  };
  
  /// Mock IO Driver
  class MockIODriver: public IODriver {
   public:
     ~MockIODriver();
     
     std::string getImplId() const throw();
     void setStackInstance(StackInstance*)     throw (DmException);
     void setSecurityContext(const SecurityContext*) throw (DmException);
     
     IOHandler* createIOHandler(const std::string& pfn,
                                int flags,
                                const Extensible& extras) throw (DmException);
     
     void doneWriting(const std::string& pfn,
                      const Extensible& params) throw (DmException);
  };

  /// Factory for mock implementation
  class MockFactory: public CatalogFactory,
                     public IOFactory,
                     public PoolManagerFactory {
   public:
    ~MockFactory();
        
    std::string implementedPool() throw();
    
    void configure(const std::string&, const std::string&) throw (DmException);
    
    Catalog*     createCatalog(PluginManager*)     throw (DmException);
    IODriver*    createIODriver(PluginManager*)    throw (DmException);
    PoolManager* createPoolManager(PluginManager*) throw (DmException);
  };
  
};

#endif	// PLUGIN_MOCK_H
