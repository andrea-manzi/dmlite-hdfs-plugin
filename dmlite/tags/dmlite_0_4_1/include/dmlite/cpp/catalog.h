/// @file   include/dmlite/cpp/catalog.h
/// @brief  Catalog API.
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#ifndef DMLITE_CPP_CATALOG_H
#define DMLITE_CPP_CATALOG_H

#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <utime.h>
#include "base.h"
#include "exceptions.h"
#include "inode.h"
#include "utils/extensible.h"

namespace dmlite {

  // Forward declarations.
  class StackInstance;
  class PluginManager;
  
  /// Typedef for directories.
  struct Directory { virtual ~Directory() = 0; };
  
  /// Interface for Catalog (Namespaces).
  class Catalog: public virtual BaseInterface {
   public:    
    /// Destructor.
    virtual ~Catalog();

    /// Change the working dir. Future not-absolute paths will use this as root.
    /// @param path The new working dir.
    virtual void changeDir(const std::string& path) throw (DmException) = 0;
    
    /// Get the current working dir.
    /// @return The current working dir.
    virtual std::string getWorkingDir(void) throw (DmException) = 0;

    /// Do an extended stat of a file or directory.
    /// @param path      The path of the file or directory.
    /// @param followSym If true, symlinks will be followed.
    /// @return          The extended status of the file.
    virtual ExtendedStat extendedStat(const std::string& path,
                                      bool followSym = true) throw (DmException) = 0;

    /// Add a new replica for a file.
    /// @param replica Stores the data that is going to be added. fileid must
    ///                point to the id of the logical file in the catalog.
    virtual void addReplica(const Replica& replica) throw (DmException) = 0;

    /// Delete a replica.
    /// @param replica The replica to remove.
    virtual void deleteReplica(const Replica& replica) throw (DmException) = 0;

    /// Get replicas for a file.
    /// @param path The file for which replicas will be retrieved.
    virtual std::vector<Replica> getReplicas(const std::string& path) throw (DmException) = 0;

    /// Creates a new symlink.
    /// @param path    The existing path.
    /// @param symlink The new access path.
    virtual void symlink(const std::string& path,
                         const std::string& symlink) throw (DmException) = 0;
    
    /// Returns the path pointed by the symlink path
    /// @param path The symlink file.
    /// @return     The symlink target.
    virtual std::string readLink(const std::string& path) throw (DmException) = 0;

    /// Remove a file.
    /// @param path The path to remove.
    virtual void unlink(const std::string& path) throw (DmException) = 0;

    /// Creates an entry in the catalog.
    /// @param path The new file.
    /// @param mode The creation mode.
    virtual void create(const std::string& path,
                        mode_t mode) throw (DmException) = 0;

    /// Sets the calling process’s file mode creation mask to mask & 0777.
    /// @param mask The new mask.
    /// @return     The value of the previous mask.
    virtual mode_t umask(mode_t mask) throw () = 0;

    /// Set the mode of a file.
    /// @param path The file to modify.
    /// @param mode The new mode as an integer (i.e. 0755)
    virtual void setMode(const std::string& path,
                         mode_t mode) throw (DmException) = 0;

    /// Set the owner of a file.
    /// @param path   The file to modify.
    /// @param newUid The uid of the new owneer.
    /// @param newGid The gid of the new group.
    /// @param followSymLink If set to true, symbolic links will be followed.
    virtual void setOwner(const std::string& path, uid_t newUid, gid_t newGid,
                          bool followSymLink = true) throw (DmException) = 0;

    /// Set the size of a file.
    /// @param path    The file to modify.
    /// @param newSize The new file size.
    virtual void setSize(const std::string& path,
                         size_t newSize) throw (DmException) = 0;

    /// Set the checksum of a file.
    /// @param path      The file to modify.
    /// @param csumtype  The checksum type (CS, AD or MD).
    /// @param csumvalue The checksum value.
    virtual void setChecksum(const std::string& path,
                             const std::string& csumtype,
                             const std::string& csumvalue) throw (DmException) = 0;

    /// Set the ACLs
    /// @param path The file to modify.
    /// @param acl  The Access Control List.
    virtual void setAcl(const std::string& path,
                        const Acl& acl) throw (DmException) = 0;

    /// Set access and/or modification time.
    /// @param path The file path.
    /// @param buf  A struct holding the new times.
    virtual void utime(const std::string& path,
                       const struct utimbuf* buf) throw (DmException) = 0;

    /// Get the comment associated with a file.
    /// @param path The file or directory.
    /// @return     The associated comment.
    virtual std::string getComment(const std::string& path) throw (DmException) = 0;

    /// Set the comment associated with a file.
    /// @param path    The file or directory.
    /// @param comment The new comment.
    virtual void setComment(const std::string& path,
                            const std::string& comment) throw (DmException) = 0;

    /// Set GUID of a file.
    /// @param path The file.
    /// @param guid The new GUID.
    virtual void setGuid(const std::string& path,
                         const std::string &guid) throw (DmException) = 0;
    
    /// Update extended metadata on the catalog.
    /// @param path The file to update.
    /// @param attr The extended attributes struct.
    virtual void updateExtendedAttributes(const std::string& path,
                                          const Extensible& attr) throw (DmException) = 0;

    /// Open a directory for reading.
    /// @param path The directory to open.
    /// @return     A pointer to a handle that can be used for later calls.
    virtual Directory* openDir(const std::string& path) throw (DmException) = 0;

    /// Close a directory opened previously.
    /// @param dir The directory handle as returned by NsInterface::openDir.
    virtual void closeDir(Directory* dir) throw (DmException) = 0;

    /// Read next entry from a directory (simple read).
    /// @param dir The directory handle as returned by NsInterface::openDir.
    /// @return    0x00 on failure or end of directory.
    virtual struct dirent* readDir(Directory* dir) throw (DmException) = 0;

    /// Read next entry from a directory (stat information added).
    /// @param dir The directory handle as returned by NsInterface::openDir.
    /// @return    0x00 on failure (and errno is set) or end of directory.
    virtual ExtendedStat* readDirx(Directory* dir) throw (DmException) = 0;

    /// Create a new empty directory.
    /// @param path The path of the new directory.
    /// @param mode The creation mode.
    virtual void makeDir(const std::string& path,
                         mode_t mode) throw (DmException) = 0;

    /// Rename a file or directory.
    /// @param oldPath The old name.
    /// @param newPath The new name.
    virtual void rename(const std::string& oldPath,
                        const std::string& newPath) throw (DmException) = 0;

    /// Remove a directory.
    /// @param path The path of the directory to remove.
    virtual void removeDir(const std::string& path) throw (DmException) = 0;
    
    /// Get a replica.
    /// @param rfn The replica file name.
    virtual Replica getReplica(const std::string& rfn) throw (DmException) = 0;

    /// Update a replica.
    /// @param replica The replica to modify.
    /// @return        0 on success, error code otherwise.
    virtual void updateReplica(const Replica& replica) throw (DmException) = 0;
  };

  /// Plug-ins must implement a concrete factory to be instantiated.
  class CatalogFactory: public virtual BaseFactory {
   public:
    /// Virtual destructor
    virtual ~CatalogFactory();

   protected:
    // Stack instance is allowed to instantiate catalogs
    friend class StackInstance;  

    /// Children of CatalogFactory are allowed to instantiate too (decorator)
    static Catalog* createCatalog(CatalogFactory* factory,
                                  PluginManager* pm) throw (DmException);

    /// Instantiate a implementation of Catalog
    virtual Catalog* createCatalog(PluginManager* pm) throw (DmException) = 0;
  };

};

#endif // DMLITE_CPP_CATALOG_H
