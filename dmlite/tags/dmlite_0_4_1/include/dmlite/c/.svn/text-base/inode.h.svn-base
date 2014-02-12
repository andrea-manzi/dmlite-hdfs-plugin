/** @file   include/dmlite/c/inode.h
 *  @brief  C wrapper for DMLite INode API.
 *  @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
 *  @note   This is a low-level API, so security checks will NOT be done.
 */
#ifndef DMLITE_INODE_H
#define DMLITE_INODE_H

#include <stdint.h>
#include "any.h"
#include "dmlite.h"
#include "utils.h"

#ifdef	__cplusplus
extern "C" {
#endif

/** Possible replica statuses */
enum dmlite_replica_status { kAvailable      = '-',
                             kBeingPopulated = 'P',
                             kToBeDeleted    = 'D'
                           };
/** Possible replica types */
enum dmlite_replica_type   { kVolatile  = 'V',
                             kPermanent = 'P'
                           };

/** A replica of a file */
typedef struct dmlite_replica {
  int64_t    replicaid;
  int64_t    fileid;

  int64_t    nbaccesses;
  time_t     atime;
  time_t     ptime;
  time_t     ltime;

  enum dmlite_replica_status status;
  enum dmlite_replica_type   type;

  char server[HOST_NAME_MAX];
  char rfn   [URL_MAX];
  
  dmlite_any_dict* extra; /**< @brief   If != NULL, additional metadata will be
                           *            put here.
                           *   @details Caller is generally responsible for
                           *            allocating and freeing. Exception:
                           *            when an array is allocated by the called
                           *            method */
} dmlite_replica;

/** Posible file statuses */
enum dmlite_file_status { kOnline = '-',
                          kMigrated = 'm'
                        };
                        
/** File metadata */
typedef struct dmlite_xstat {
  ino_t                   parent;
  struct stat             stat;
  enum dmlite_file_status status;
  char                    name     [NAME_MAX];
  char                    guid     [GUID_MAX];
  char                    csumtype [CSUMTYPE_MAX];
  char                    csumvalue[CSUMVALUE_MAX];
  char                    acl      [ACL_ENTRIES_MAX * ACL_SIZE];
  
  dmlite_any_dict* extra; /**< @brief   If != NULL, additional metadata will be
                           *            put here.
                           *   @details Caller is responsible for allocating
                           *            and freeing*/
} dmlite_xstat;

/** Opaque directory handler */
typedef struct dmlite_idir dmlite_idir;

/** 
 * @brief         Starts a transaction.
 * @details       Depending on the plugin stack, it can be possible to nest
 *                several calls.
 * @param context The DM context.
 * @return        0 on success, error code otherwise.
 */
int dmlite_ibegin(dmlite_context* context);

/**
 * @brief         Commits the changes.
 * @details       Depending on the plugin stack, it can be possible to nest
 *                several calls, and there must be one icommit per ibegin
 *                for the changes to be permanent.
 * @param context The DM context.
 * @return        0 on success, error code otherwise.
 */
int dmlite_icommit(dmlite_context* context);

/**
 * @brief         Undo the changes.
 * @details       If several ibegin were nested, all the transactions will
 *                be probable be undone, regardless on the nesting level.
 * @param context The DM context.
 * @return        0 on success, error code otherwise.
 */
int dmlite_irollback(dmlite_context* context);

/**
 * @brief         Creates a new file.
 * @param context The DM context.
 * @param f       Only some fields from this struct will be used. That would depend
 *                on the plugin, but usually it will be: parent, name, mode, uid, gid,
 *                size, status, checksum and ACL.
 * @return        0 on success, error code otherwise.
 * @note          mode will probably be used raw (i.e. no cleanup or set of format bits)
 */
int dmlite_icreate(dmlite_context* context, const dmlite_xstat* f);

/**
 * @brief         Associates a symlink with an existing file.
 * @param context The DM context.
 * @param inode   The file that will be a symlink.
 * @param link    The destination link.
 * @return        0 on success, error code otherwise.
 */
int dmlite_isymlink(dmlite_context* context, ino_t inode, const char* link);

/**
 * @brief         Removes a file or directory from the database.
 * @param context The DM context.
 * @param inode   The id of the entry to remove.
 * @return        0 on success, error code otherwise.
 * @note          Not empty directories, or files will replicas may fail.
 */
int dmlite_iunlink(dmlite_context* context, ino_t inode);

/**
 * @brief         Moves a file to a different parent directory.
 * @param context The DM context.
 * @param inode   The file id.
 * @param dest    The destination id.
 * @return        0 on success, error code otherwise.
 */
int dmlite_imove(dmlite_context* context, ino_t inode, ino_t dest);

/**
 * @brief         Changes the name of an entry.
 * @param context The DM context.
 * @param inode   The file id.
 * @param name    The new name.
 * @return        0 on success, error code otherwise.
 */
int dmlite_irename(dmlite_context* context, ino_t inode, const char* name);

/**
 * @brief         Does a stat of an entry using the inode instead of the path.
 * @param context The DM context.
 * @param inode   The entry inode.
 * @param buf     Where to put the retrieved information.
 * @return        0 on success, error code otherwise.
 * @note          Security checks won't be done if you use this function,
 *                so keep in mind doing it yourself.
 */
int dmlite_istat(dmlite_context* context, ino_t inode, struct stat* buf);

/**
 * @brief         Does an extended stat of an entry using the inode instead of
 *                the path.
 * @param context The DM context.
 * @param inode   The entry inode.
 * @param buf     Where to put the retrieved information.
 * @return        0 on success, error code otherwise.
 * @note          Security checks won't be done if you use this function,
 *                so keep in mind doing it yourself.
 */
int dmlite_istatx(dmlite_context* context, ino_t inode, dmlite_xstat* buf);

/**
 * @brief         Does an extended stat using the parent inode and the entry name.
 * @param context The DM context.
 * @param parent  The parent id.
 * @param name    The entry name.
 * @param buf     Where to put the retrieved information.
 * @return        0 on success, error code otherwise.
 */
int dmlite_istatx_by_name(dmlite_context* context, ino_t parent, const char* name,
                          dmlite_xstat* buf);

/**
 * @brief         Reads a symbolic link.
 * @param context The DM context.
 * @param inode   The file id.
 * @param path    The link will be put here.
 * @param bufsize The size of the memory area pointed by path.
 * @return        0 on success, error code otherwise.
 */
int dmlite_ireadlink(dmlite_context* context, ino_t inode,
                     char* path, size_t bufsize);

/**
 * @brief         Adds a new replica.
 * @param context The DM context.
 * @param replica The replica to add. replica->fileid must point to a valid file.
 * @return        0 on success, error code otherwise.
 */
int dmlite_iaddreplica(dmlite_context* context, const dmlite_replica* replica);

/**
 * @brief         Deletes a replica.
 * @param context The DM context.
 * @param replica The replica to remove.
 * @return        0 on success, error code otherwise.
 */
int dmlite_ideletereplica(dmlite_context* context, const dmlite_replica* replica);

/**
 * @brief         Gets a specific replica using its replica id.
 * @param context The DM context.
 * @param rid     The replica id.
 * @param buf     Where to put the retrieved data.
 * @return        0 on success, error code otherwise.
 */
int dmlite_igetreplica(dmlite_context* context, int64_t rid, dmlite_replica* buf);

/**
 * @brief           Gets all the replicas associated to a file.
 * @param context   The DM context.
 * @param inode     The file id.
 * @param nreplicas The number of replicas will be put here.
 * @param replicas  It will be initialized to an array of nreplicas replicas.
 *                  Free it with <b>dmlite_replicas_free</b>.
 * @return          0 on success, error code otherwise.
 */
int dmlite_igetreplicas(dmlite_context* context, ino_t inode,
                        unsigned* nreplicas, dmlite_replica** replicas);

/**
 * @brief         Sets the access and modification time.
 * @param context The DM context.
 * @param inode   The file id.
 * @param buf     The timestamps.
 * @return        0 on success, error code otherwise.
 */
int dmlite_iutime(dmlite_context* context, ino_t inode,
                  const struct utimbuf* buf);

/**
 * @brief          Sets the mode and ACL of a file.
 * @param context  The DM context.
 * @param inode    The file id.
 * @param uid      The new UID.
 * @param gid      The new GID.
 * @param mode     The new mode.
 * @param nentries The number of acl entries.
 * @param acl      The new ACL.
 * @return         0 on success, error code otherwise.
 * @note           This call may not validate that uid, gid, mode and acl
 *                 are coherent.
 */
int dmlite_isetmode(dmlite_context* context, ino_t inode, uid_t uid, gid_t gid,
                    mode_t mode, unsigned nentries, dmlite_aclentry* acl);

/**
 * @brief         Sets the size of a file.
 * @param context The DM context.
 * @param inode   The file id.
 * @param size    The new size.
 * @return        0 on success, error code otherwise.
 */
int dmlite_isetsize(dmlite_context* context, ino_t inode, size_t size);

/**
 * @brief           Sets the checksum of a file.
 * @param context   The DM context.
 * @param inode     The file id.
 * @param csumtype  The new checksum type.
 * @param csumvalue The new checksum value.
 * @return          0 on success, error code otherwise.
 */
int dmlite_isetchecksum(dmlite_context* context, ino_t inode,
                        const char* csumtype, const char* csumvalue);

/**
 * @brief         Gets the comment associated with an entry.
 * @param context The DM context.
 * @param inode   The file id.
 * @param comment Where to put the comment.
 * @param bufsize The size of the memory pointed by comment.
 * @return        0 on success, error code otherwise.
 */
int dmlite_igetcomment(dmlite_context* context, ino_t inode,
                       char* comment, size_t bufsize);

/**
 * @brief         Sets the comment associated with an entry.
 * @param context The DM context.
 * @param inode   The file id.
 * @param comment The new comment.
 * @return        0 on success, error code otherwise.
 */
int dmlite_isetcomment(dmlite_context* context,  ino_t inode,
                       const char* comment);

/**
 * @brief         Deletes the comment associated with an entry.
 * @param context The DM context.
 * @param inode   The file id.
 * @return        0 on success, error code otherwise.
 */
int dmlite_ideletecomment(dmlite_context* context, ino_t inode);

/**
 * @brief         Sets the file Grid Unique Identifier.
 * @param context The DM context.
 * @param inode   The entry id.
 * @param guid    The new GUID.
 * @return        0 on success, error code otherwise.
 */
int dmlite_isetguid(dmlite_context* context, ino_t inode, const char* guid);

/**
 * @brief         Updates the file extended attributes.
 * @param context The DM context.
 * @param inode   The entry id.
 * @param xattr   The new set of extended attributes.
 * @return        0 on success, error code otherwise.
 */
int dmlite_iupdate_xattr(dmlite_context* context, ino_t inode,
                         const dmlite_any_dict* xattr);

/**
 * @brief         Opens a directory.
 * @param context The DM context.
 * @param inode   The directory ID.
 * @return        NULL on failure. A pointer to an internal struct to be used
 *                with dmlite_ireaddir*
 */
dmlite_idir* dmlite_iopendir(dmlite_context* context, ino_t inode);

/**
 * @brief         Closes a directory, freeing any internally allocated memory.
 * @param context The DM context.
 * @param dir     The directory to close, as returned by dmlite_opendir.
 * @return        0 on success, error code otherwise.
 */
int dmlite_iclosedir(dmlite_context* context, dmlite_idir* dir);

/**
 * @brief         Reads a directory. Extended data.
 * @param context The DM context.
 * @param dir     The directory to read, as returned by dmlite_opendir.
 * @return        NULL on failure, or end of dir.
 */
dmlite_xstat* dmlite_ireaddirx(dmlite_context* context, dmlite_idir* dir);

/**
 * @brief         Reads a directory.
 * @param context The DM context.
 * @param dir     The directory to read, as returned by dmlite_opendir.
 * @return        NULL on failure, or end of dir.
 */
struct dirent* dmlite_ireaddir(dmlite_context* context, dmlite_idir* dir);

#ifdef	__cplusplus
}
#endif

#endif /* DMLITE_INODE_H */
