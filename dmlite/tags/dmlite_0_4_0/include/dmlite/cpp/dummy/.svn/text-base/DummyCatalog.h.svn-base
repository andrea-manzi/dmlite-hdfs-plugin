/// @file    include/dmlite/cpp/dummy/DummyCatalog.h
/// @brief   A dummy plugin that just delegates calls to a decorated one.
/// @details It makes sense as a base for other decorator plug-ins.
/// @author  Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#ifndef DMLITE_CPP_DUMMY_CATALOG_H
#define DMLITE_CPP_DUMMY_CATALOG_H

#include "../catalog.h"

namespace dmlite {

  // Dummmy catalog implementation
  class DummyCatalog: public Catalog
  {
   public:

    /// Constructor
    /// @param decorated The underlying decorated catalog.
    DummyCatalog(Catalog* decorated) throw (DmException);

    /// Destructor
    virtual ~DummyCatalog();

    // Overloading
    virtual void setStackInstance(StackInstance*) throw (DmException);
    virtual void setSecurityContext(const SecurityContext*) throw (DmException);

    virtual void        changeDir     (const std::string&) throw (DmException);
    virtual std::string getWorkingDir (void)               throw (DmException);

    virtual ExtendedStat extendedStat(const std::string&, bool) throw (DmException);

    virtual void addReplica   (const Replica&) throw (DmException);
    virtual void deleteReplica(const Replica&) throw (DmException);
    virtual std::vector<Replica> getReplicas(const std::string&) throw (DmException);

    virtual void symlink (const std::string&, const std::string&) throw (DmException);
    std::string  readLink(const std::string& path) throw (DmException);
    
    virtual void unlink(const std::string&)                     throw (DmException);

    virtual void create(const std::string&, mode_t) throw (DmException);

    virtual mode_t umask       (mode_t)                           throw ();
    virtual void   setMode     (const std::string&, mode_t)       throw (DmException);
    virtual void   setOwner    (const std::string&, uid_t, gid_t, bool) throw (DmException);

    virtual void setSize    (const std::string&, size_t) throw (DmException);
    virtual void setChecksum(const std::string&, const std::string&, const std::string&) throw (DmException);

    virtual void setAcl(const std::string&, const Acl&) throw (DmException);

    virtual void utime(const std::string&, const struct utimbuf*) throw (DmException);

    virtual std::string getComment(const std::string&)                     throw (DmException);
    virtual void        setComment(const std::string&,
                                   const std::string&) throw (DmException);

    virtual void setGuid(const std::string&,
                         const std::string&) throw (DmException);
    
    virtual void updateExtendedAttributes(const std::string&,
                                          const Extensible&) throw (DmException);


    virtual Directory* openDir (const std::string&) throw (DmException);
    virtual void       closeDir(Directory*)         throw (DmException);

    virtual struct dirent* readDir (Directory*) throw (DmException);
    virtual ExtendedStat*  readDirx(Directory*) throw (DmException);

    virtual void makeDir(const std::string&, mode_t) throw (DmException);

    virtual void rename     (const std::string&, const std::string&) throw (DmException);
    virtual void removeDir  (const std::string&)                     throw (DmException);

    virtual Replica getReplica   (const std::string& rfn) throw (DmException);
    virtual void    updateReplica(const Replica& replica) throw (DmException);

   protected:
    Catalog* decorated_;
  };

};

#endif // DMLITE_DUMMY_CATALOG_H
