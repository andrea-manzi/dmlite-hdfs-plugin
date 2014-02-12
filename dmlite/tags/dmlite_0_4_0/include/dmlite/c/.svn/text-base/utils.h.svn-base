/** @file   include/dmlite/c/utils.h
 *  @brief  C wrapper for DMLite utils.
 *  @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
 */
#ifndef DMLITE_UTILS_H
#define DMLITE_UTILS_H

#include <limits.h>
#include <stdint.h>

#define ACL_ENTRIES_MAX 300
#define ACL_SIZE         13
#define CSUMTYPE_MAX      3
#define CSUMVALUE_MAX    33
#define GUID_MAX         36
#ifndef HOST_NAME_MAX
# define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif
#define QUERY_MAX      1024
#define SCHEME_MAX        7
#define URL_MAX        8192

#define ACL_USER_OBJ  1
#define ACL_USER      2
#define ACL_GROUP_OBJ 3
#define ACL_GROUP     4
#define ACL_MASK      5
#define ACL_OTHER     6
#define ACL_DEFAULT   0x20

#ifdef	__cplusplus
extern "C" {
#endif

/** @brief Handles URL */
typedef struct dmlite_url {
  char     scheme[SCHEME_MAX];
  char     domain[HOST_NAME_MAX];
  unsigned port;
  char     path  [PATH_MAX];
  char     query [QUERY_MAX];
} dmlite_url;

/** @brief Handles ACL entries */
typedef struct dmlite_aclentry {
  uint8_t  type;
  uint8_t  perm;
  uint32_t id;
} dmlite_aclentry;


/**
 * @brief        Parses a URL.
 * @param source Original URL.
 * @param dest   Parsed URL.
 */
void dmlite_parse_url(const char* source, dmlite_url* dest);

/**
 * @brief          Serializes into a string a set of ACL entries
 * @param nEntries The number of ACL entries in the array.
 * @param acl      The ACL.
 * @param buffer   Where to put the resulting string.
 * @param bsize    The buffer size.
 */
void dmlite_serialize_acls(unsigned nEntries, dmlite_aclentry* acl,
                           char* buffer, size_t bsize);

/**
 * @brief          Deserializes a string into an array of ACL entries.
 * @param buffer   The string.
 * @param nEntries The resulting number of ACL entries.
 * @param acl      The resulting ACL.
 */
void dmlite_deserialize_acl(const char* buffer, unsigned* nEntries,
                             dmlite_aclentry** acl);

/**
 * @brief          Frees an array of ACL entries as returned by dm_deserialize_acls
 * @param nEntries The number of entries in the array.
 * @param acl      The ACL.
 */
void dmlite_acl_free(unsigned nEntries, dmlite_aclentry* acl);

#ifdef	__cplusplus
}
#endif

#endif /* DMLITE_UTILS_H */
