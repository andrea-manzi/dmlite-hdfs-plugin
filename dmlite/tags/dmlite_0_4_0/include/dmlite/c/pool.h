/** @file   include/dmlite/c/pool.h
 *  @brief  C wrapper for DMLite Pool API.
 *  @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
 */
#ifndef DMLITE_POOL_H
#define DMLITE_POOL_H

#include "any.h"
#include "inode.h"
#include "dmlite.h"
#include "utils.h"

#define POOL_TYPE_MAX 16
#define POOL_MAX      16

#ifdef	__cplusplus
extern "C" {
#endif
  
/** @brief Pool data */
typedef struct dmlite_pool {
  char pool_type[POOL_TYPE_MAX];
  char pool_name[POOL_MAX];
  
  dmlite_any_dict* extra;
} dmlite_pool;

/** @brief Chunk of data */
typedef struct dmlite_chunk {
  char host[HOST_NAME_MAX];
  char path[PATH_MAX];
  
  off_t  offset;
  size_t size;
  
  dmlite_any_dict* extra;
} dmlite_chunk;

/** @brief Collection of chunks that form a replica
 * @details There may be duplicated chunks.
 */
typedef struct dmlite_location {
  dmlite_chunk* chunks;
  unsigned      nchunks;
} dmlite_location;

/**
 * @brief         Gets the list of pools.
 * @param context The DM context.
 * @param nPools  The number of pools.
 * @param pools   An array with the pools. <b>Use dmlite_freepools to free</b>.
 * @return        0 on succes, -1 on failure.
 */
int dmlite_getpools(dmlite_context* context, unsigned* nPools, dmlite_pool** pools);

/**
 * @brief         Frees an array of pools.
 * @param context The DM context.
 * @param nPools  The number of pools in the array.
 * @param pools   The array to free.
 * @return        0 on succes, -1 on failure.
 */
int dmlite_pools_free(dmlite_context* context, unsigned nPools, dmlite_pool* pools);

/**
 * @brief         Gets a single replica (synchronous).
 * @param context The DM context.
 * @param path    The logical file name.
 * @return        A pointer to a dmlite_location struct, or NULL on error.
 */
dmlite_location* dmlite_get(dmlite_context* context, const char* path);

/**
 * @brief         Gets a single replica (synchronous).
 * @param context The DM context.
 * @param inode   The file inode.
 * @return        A pointer to a dmlite_location struct, or NULL on error.
 */
dmlite_location* dmlite_iget(dmlite_context* context, ino_t inode);

/**
 * @brief         Gets the location of a replica.
 * @param context The DM context.
 * @param replica The replica to translate.
 * @return        A pointer to a dmlite_location struct, or NULL on error.
 */
dmlite_location* dmlite_getlocation(dmlite_context* context, const dmlite_replica* replica);

/**
 * @brief         Puts a file (synchronous).
 * @param context The DM context.
 * @param path    The logical file name to put.
 * @return        A pointer to a dmlite_location struct, or NULL on error.
 */
dmlite_location* dmlite_put(dmlite_context* context, const char* path);

/**
 * @brief         Frees a location struct.
 * @param context The DM context.
 * @param loc     The struct to free.
 * @return        0 on success, error code otherwise.
 */
int dmlite_location_free(dmlite_context* context, dmlite_location* loc);

#ifdef	__cplusplus
}
#endif

#endif /* DMLITE_POOL_H */
