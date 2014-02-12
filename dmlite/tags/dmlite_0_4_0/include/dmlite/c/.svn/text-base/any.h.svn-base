/** @file   include/dmlite/c/any.h
 *  @brief  Opaque handler to pass different types of values to the API.
 *  @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
 *  @note   Basically it wraps boost::any and dmlite::Extensible.
 */
#ifndef DMLITE_ANY_H
#define DMLITE_ANY_H

#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief Used to pass configuration values.
 */
typedef struct dmlite_any dmlite_any;

/**
 * @brief Handles key->value pairs.
 */
typedef struct dmlite_any_dict dmlite_any_dict;

/**
 * @brief      Creates a new dmlite_any.
 * @param  str The string that will be wrapped. It is safe to free afterwards.
 * @return     A newly allocated dmlite_any.
 */
dmlite_any* dmlite_any_new_string(const char* str);

/**
 * @brief       Creates a new dmlite_any.
 * @param  n    The number of elements.
 * @param  strv The strings that will be wrapped. It is safe to free afterwards.
 * @return      A newly allocated dmlite_any.
 */
dmlite_any* dmlite_any_new_string_array(unsigned n, const char** strv);

/**
 * @brief    Creates a new dmlite_any.
 * @param  l The long that will be wrapped.
 * @return   A newly allocated dmlite_any.
 */
dmlite_any* dmlite_any_new_long(long l);

/**
 * @brief     Creates a new dmlite_any.
 * @param  n  The number of elements.
 * @param  lv The longs that will be wrapped.
 * @return    A newly allocated dmlite_any.
 */
dmlite_any* dmlite_any_new_long_array(unsigned n, long* lv);

/**
 * @brief     Frees a dmlite_any.
 * @param any The dmlite_any to destroy.
 */
void dmlite_any_free(dmlite_any* any);

/**
 * @brief        Gets the string interpretation of the dmlite_any.
 * @details      Defaults to "".
 * @param any    The dmlite_any to convert.
 * @param buffer Where to put the string.
 * @param bsize  The size of the buffer.
 */
void dmlite_any_to_string(const dmlite_any* any, char* buffer, size_t bsize);

/**
 * @brief     Returns the long interpretation of they dmlite_any.
 * @details   Defaults to 0.
 * @param any The dmlite_any to convert.
 */
long dmlite_any_to_long(const dmlite_any* any);


/**
 * @brief  Created a new generic dictionary.
 * @return A newly allocated dmlite_any_dict.
 */
dmlite_any_dict* dmlite_any_dict_new();

/**
 * @brief      Make a copy of the dictionary.
 * @param dict The original
 * @return A newly allocated copy of dict.
 */
dmlite_any_dict* dmlite_any_dict_copy(const dmlite_any_dict* dict);

/**
 * @brief Frees a dmlite_any_dict
 */
void dmlite_any_dict_free(dmlite_any_dict* d);

/**
 * @brief Clears the dictionary.
 */
void dmlite_any_dict_clear(dmlite_any_dict* d);

/**
 * @brief   Insert a new dmlite_any value into the dictionary.
 * @details Replaces if already present.
 * @param d The dictionary.
 * @param k The key.
 * @param v The value.
 */
void dmlite_any_dict_insert(dmlite_any_dict* d, const char* k, const dmlite_any* v);

/**
 * @brief Returns how many elements there are in a specific dictionary.
 */
unsigned long dmlite_any_dict_count(const dmlite_any_dict* d);

/**
 * @brief  Returns the value associated with the key k.
 * @return NULL if not found.
 */
dmlite_any* dmlite_any_dict_get(const dmlite_any_dict* d, const char* k);

/**
 * @brief   Removes a key-value from the dictionary.
 * @param d The dictionary.
 * @param k The key to be removed.
 */
void dmlite_any_dict_erase(dmlite_any_dict* d, const char* k);

/**
 * @brief  Generates a JSON serialization of the dictionary.
 * @return The same pointer as buffer.
 */
char* dmlite_any_dict_to_json(const dmlite_any_dict* d, char* buffer, size_t bsize);

/**
 * @brief Populates a dmlite_any_dict from a JSON string.
 */
dmlite_any_dict* dmlite_any_dict_from_json(const char* json);

/**
 * @brief       Puts in keys a pointer to an array of strings with all the available
 *              keys in d.
 * @details     Use dmlite_any_dict_keys_free to free.
 * @param d     The Dictionary.
 * @param nkeys Will be set to the number of stored keys.
 */
void dmlite_any_dict_keys(const dmlite_any_dict* d, unsigned* nkeys, char*** keys);

/**
 * @brief Frees an array of strings allocated by dmlite_any_dict_keys.
 * @param n    The number of keys in **keys
 * @param keys The array of keys.
 */
void dmlite_any_dict_keys_free(unsigned n, char** keys);

#ifdef	__cplusplus
}
#endif

#endif /* DMLITE_ANY_H */

