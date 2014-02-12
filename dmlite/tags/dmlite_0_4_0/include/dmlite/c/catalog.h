/** @file   include/dmlite/c/catalog.h
 *  @brief  C wrapper for DMLite Catalog API.
 *  @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
 */
#ifndef DMLITE_CATALOG_H
#define DMLITE_CATALOG_H

#include "dmlite.h"
#include "inode.h"
#include "utils.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct dmlite_dir dmlite_dir;
  
/**
 * @brief         Changes the working dir.
 * @param context The DM context.
 * @param path    The new working dir.
 * @return        0 on success, error code otherwise.
 */
int dmlite_chdir(dmlite_context* context, const char* path);

/**
 * @brief         Gets the current working directory.
 * @param context The DM context.
 * @param buffer  If not NULL, the path will be stored here. <b>malloc</b> will be used otherwise.
 * @param size    The buffer size.
 * @return        A pointer to a string with the current working dir.
 */
char* dmlite_getcwd(dmlite_context* context, char* buffer, size_t size);

/**
 * @brief         Sets the file mode creation mask.
 * @param context The DM context.
 * @param mask    The new mask.
 * @return        The previous mask.
 */
mode_t dmlite_umask(dmlite_context* context, mode_t mask);

/**
 * @brief         Does a stat of a file or directory.
 * @param context The DM context.
 * @param path    The path.
 * @param buf     Where to put the retrieved information.
 * @return        0 on success, error code otherwise.
 */
int dmlite_stat(dmlite_context* context, const char* path, struct stat* buf);

/**
 * @brief         Does a stat of a file, directory, or symbolic link (does not follow).
 * @param context The DM context.
 * @param path    The path.
 * @param buf     Where to put the retrieved information.
 * @return        0 on success, error code otherwise.
 */
int dmlite_statl(dmlite_context* context, const char* path, struct stat* buf);

/**
 * @brief         Does an extended stat of a file, directory or symbolic link.
 * @param context The DM context.
 * @param path    The path.
 * @param buf     Where to put the retrieved information.
 * @return        0 on success, error code otherwise.
 */
int dmlite_statx(dmlite_context* context, const char* path, dmlite_xstat* buf);

/**
 * @brief            Adds a new replica to an entry.
 * @param context    The DM context.
 * @param replica    The replica to add.
 * @return           0 on success, error code otherwise.
 */
int dmlite_addreplica(dmlite_context* context, const dmlite_replica* replica);

/**
 * @brief         Deletes a replica.
 * @param context The DM context.
 * @param replica The replica to delete.
 * @return        0 on success, error code otherwise.
 */
int dmlite_delreplica(dmlite_context* context, const dmlite_replica* replica);

/**
 * @brief              Gets the replicas of a file.
 * @param context      The DM context.
 * @param path         The logical file name.
 * @param nReplicas    The number of entries will be put here.
 * @param fileReplicas An array with nEntries elements will be stored here. <b>Use dmlite_replicas_free to free it.</b>
 * @return             0 on success, error code otherwise.
 */
int dmlite_getreplicas(dmlite_context* context, const char* path, unsigned *nReplicas,
                       dmlite_replica** fileReplicas);

/**
 * @brief              Frees a replica list.
 * @param context      The DM context.
 * @param nReplicas    The number of replicas contained in the array.
 * @param fileReplicas The array to free.
 * @return             0 on success, error code otherwise.
 */
int dmlite_replicas_free(dmlite_context* context,
                         unsigned nReplicas, dmlite_replica* fileReplicas);

/**
 * @brief         Creates a symlink
 * @param context The DM context.
 * @param oldPath The old path.
 * @param newPath The new path.
 * @return        0 on success, error code otherwise.
 */
int dmlite_symlink(dmlite_context* context,
                   const char* oldPath, const char* newPath);

/**
 * @brief         Reads a symlink.
 * @param context The DM context.
 * @param path    The symlink file.
 * @param buf     Where to put the symlink target.
 * @param bufsize The size of the memory pointed by buf.
  * @return       0 on success, error code otherwise.
 */
int dmlite_readlink(dmlite_context* context, const char* path,
                    char* buf, size_t bufsize);

/**
 * @brief         Removes a file.
 * @param context The DM context.
 * @param path    The logical file name.
 * @return        0 on success, error code otherwise.
 */
int dmlite_unlink(dmlite_context* context, const char* path);


/**
 * @brief         Creates a file in the catalog (no replicas).
 * @param context The DM context.
 * @param path    The logical file name.
 * @param mode    The creation mode.
 * @return        0 on success, error code otherwise.
 */
int dmlite_create(dmlite_context* context, const char* path, mode_t mode);

/**
 * @brief         Changes the mode of a file or directory.
 * @param context The DM context.
 * @param path    The logical path.
 * @param mode    The new mode.
 * @return        0 on success, error code otherwise.
 */
int dmlite_chmod(dmlite_context* context, const char* path, mode_t mode);

/**
 * @brief         Changes the owner of a file or directory.
 * @param context The DM context.
 * @param path    The logical path.
 * @param newUid  The new owner.
 * @param newGid  The new group.
 * @return        0 on success, error code otherwise.
 */
int dmlite_chown(dmlite_context* context, const char* path, uid_t newUid, gid_t newGid);

/**
 * @brief         Changes the owner of a file, directory or symlink (does not follow).
 * @param context The DM context.
 * @param path    The logical path.
 * @param newUid  The new owner.
 * @param newGid  The new group.
 * @return        0 on success, error code otherwise.
 */
int dmlite_lchown(dmlite_context* context, const char* path, uid_t newUid, gid_t newGid);

/**
 * @brief          Changes the size of a file in the catalog.
 * @param context  The DM context.
 * @param path     The logical path.
 * @param filesize The new file size.
 * @return         0 on success, error code otherwise.
 */
int dmlite_setfsize(dmlite_context* context, const char* path, uint64_t filesize);

/**
 * @brief           Changes the size and checksum of a file in the catalog.
 * @param context   The DM context.
 * @param path      The logical path.
 * @param filesize  The new file size.
 * @param csumtype  The new checksum type (CS, AD or MD).
 * @param csumvalue The new checksum value.
 * @return          0 on success, error code otherwise.
 */
int dmlite_setfsizec(dmlite_context* context, const char* path, uint64_t filesize,
                 const char* csumtype, const char* csumvalue);

/**
 * @brief          Changes the ACL of a file.
 * @param context  The DM context.
 * @param path     The logical path.
 * @param nEntries The number of entries in the acl array.
 * @param acl      An ACL array.
 * @return         0 on success, error code otherwise.
 */
int dmlite_setacl(dmlite_context* context, const char* path, unsigned nEntries, dmlite_aclentry* acl);

/**
 * @brief         Changes access and/or modification time
 * @param context The DM context.
 * @param path    The file path.
 * @param buf     A struct holding the new times.
 * @return        0 on success, error code otherwise.
 */
int dmlite_utime(dmlite_context* context, const char* path, const struct utimbuf* buf);

/**
 * @brief         Gets the comment associated with a file.
 * @param context The DM context.
 * @param path    The logical path.
 * @param comment Where to put the retrieved comment. It must be at least of size COMMENT_MAX.
 * @param bufsize Size of the memory zone pointed by comment.
 * @return        0 on success, error code otherwise.
 */
int dmlite_getcomment(dmlite_context* context, const char* path,
                      char* comment, size_t bufsize);

/**
 * @brief         Sets the comment associated with a file.
 * @param context The DM context.
 * @param path    The logical path.
 * @param comment The comment to associate. '\\0' terminated string.
 * @return        0 on success, error code otherwise.
 */
int dmlite_setcomment(dmlite_context* context, const char* path, const char* comment);

/**
 * @brief         Sets the file Grid Unique Identifier.
 * @param context The DM context.
 * @param path    The logical path.
 * @param guid    The new GUID.
 * @return        0 on success, error code otherwise.
 */
int dmlite_setguid(dmlite_context* context, const char* path, const char* guid);

/**
 * @brief         Updates the file extended attributes.
 * @param context The DM context.
 * @param path    The logical path.
 * @param xattr   The new set of extended attributes.
 * @return        0 on success, error code otherwise.
 */
int dmlite_update_xattr(dmlite_context* context, const char* path,
                        const dmlite_any_dict* xattr);

/**
 * @brief           Gets the id of a group.
 * @param context   The DM context.
 * @param groupName The group name.
 * @param gid       Where to put the group ID.
 * @return          0 on success, error code otherwise.
 */
int dmlite_getgrpbynam(dmlite_context* context, const char* groupName, gid_t* gid);

/**
 * @brief          Get the user id.
 * @param context  The DM context.
 * @param userName The user name.
 * @param uid      Where to put the user ID.
 * @return         0 on success, error code otherwise.
 */
int dmlite_getusrbynam(dmlite_context* context, const char* userName, uid_t* uid);

/**
 * @brief         Opens a directory to read it later.
 * @param context The DM context.
 * @param path    The directory to open.
 * @return        A pointer to an internal structure, or NULL on failure.
 */
dmlite_dir* dmlite_opendir(dmlite_context* context, const char* path);

/**
 * @brief         Closes a directory and free the internal structures.
 * @param context The DM context.
 * @param dir     The pointer returned by dmlite_opendir.
 * @return        0 on success, error code otherwise.
 */
int dmlite_closedir(dmlite_context* context, dmlite_dir* dir);

/**
 * @brief         Reads an entry from a directory.
 * @param context The DM context.
 * @param dir     The pointer returned by dmlite_opendir.
 * @return        A pointer to a struct with the recovered data, or NULL on failure or end of directory. Do NOT free it.
  */
struct dirent *dmlite_readdir(dmlite_context* context, dmlite_dir* dir);

/**
 * @brief         Reads an entry from a directory (extended data).
 * @param context The DM context.
 * @param dir     The pointer returned by dmlite_opendir.
 * @return        A pointer to a struct with the recovered data, or NULL on failure or end of directory. Do NOT free it.
 */
dmlite_xstat *dmlite_readdirx(dmlite_context* context, dmlite_dir* dir);

/**
 * @brief         Creates a new directory.
 * @param context The DM context.
 * @param path    The directory for the new path. All the precedent folders must exist.
 * @param mode    Permissions to use for the creation.
 * @return        0 on success, error code otherwise.
 */
int dmlite_mkdir(dmlite_context* context, const char* path, mode_t mode);

/**
 * @brief         Renames a file, directory or symlink.
 * @param context The DM context.
 * @param oldPath The old name.
 * @param newPath The new name.
 * @return        0 on success, error code otherwise.
 */
int dmlite_rename(dmlite_context* context, const char* oldPath, const char* newPath);

/**
 * @brief         Deletes a directory. It must be empty.
 * @param context The DM context.
 * @param path    The directory to remove.
 * @return        0 on success, error code otherwise.
 */
int dmlite_rmdir(dmlite_context* context, const char* path);

/**
 * @brief         Gets a specific replica.
 * @param context The DM context.
 * @param rfn     The replica file name.
 * @param replica A buffer where the retrieved data will be put.
 * @return        0 on success, error code otherwise.
 */
int dmlite_getreplica(dmlite_context* context, const char* rfn, dmlite_replica* replica);

/**
 * @brief         Updates a replica.
 * @param context The DM context.
 * @param replica The replica to modify.
 * @return        0 on success, error code otherwise.
 */
int dmlite_updatereplica(dmlite_context* context, const dmlite_replica* replica);

#ifdef	__cplusplus
}
#endif

#endif /* DMLITE_CATALOG_H */
