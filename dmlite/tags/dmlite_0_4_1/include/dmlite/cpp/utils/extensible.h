/// @file   include/dmlite/cpp/utils/extensible.h
/// @brief  Extensible types (hold metadata).
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#ifndef DMLITE_CPP_UTILS_EXTENSIBLE_H
#define DMLITE_CPP_UTILS_EXTENSIBLE_H

#include <boost/any.hpp>
#include <boost/property_tree/ptree.hpp>
#include <dmlite/common/errno.h>
#include <dmlite/cpp/exceptions.h>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace dmlite {
  
  /// Helpful typedef for KeyValue containers
  struct Extensible {
   private:
     typedef std::map<std::string, boost::any> DictType_;
     DictType_ dictionary_;
     
     void populate(const boost::property_tree::ptree& root);
     
   public:
    /// Converts an any to a boolean, casting if needed.
    static bool        anyToBoolean (const boost::any& any);
    /// Converts an any to an unsigned, casting if needed.
    static unsigned    anyToUnsigned(const boost::any& any);
    /// Converts an any to a long, casting if needed.
    static long        anyToLong    (const boost::any& any);
    /// Converts an any to a double, casting if needed.
    static double      anyToDouble  (const boost::any& any);
    /// Converts an any to a string, casting if needed.
    static std::string anyToString  (const boost::any& any);    
     
     /// Returns true if there is a field name "key".
     bool hasField(const std::string& key) const;
     
     /// Returns a reference to the value associated with "key".
     /// Will throw DmException(DM_INVALID_VALUE,...) when not found.
     const boost::any& operator [] (const std::string& key) const throw (DmException);
     
     /// Returns a modifiable reference to the value associated with "key".
     /// Will create the entry if it does not exist.
     boost::any& operator [] (const std::string& key);
     
     // Comparison operators. Containers may need them.
     bool operator == (const Extensible&) const;
     bool operator != (const Extensible&) const;
     bool operator >  (const Extensible&) const;
     bool operator <  (const Extensible&) const;
     
     /// Number of elements inside this Extensible.
     unsigned long size() const;
     
     /// Removes all the content.
     void clear();
     
     /// Copies the content from another Extensible
     void copy(const Extensible& s);
     
     /// Removes an entry.
     void erase(const std::string&);
     
     /// Serializes to JSON. In principle, it only supports POD.
     std::string serialize(void) const;
     
     /// Deserializes from a JSON string.
     void deserialize(const std::string& serial) throw (DmException);
     
     /// Get all the keys available
     std::vector<std::string> getKeys(void) const throw (DmException);
     
     /// Gets a boolean. May be able to perform some conversions.
     bool getBool(const std::string& key) const throw (DmException);
     
     /// Gets an integer. May be able to perform some conversions.
     long getLong(const std::string& key) const throw (DmException);
     
     /// Gets an unsigned integer. May be able to perform some conversions.
     unsigned long getUnsigned(const std::string& key) const throw (DmException);
     
     /// Gets a float. May be able to perform some conversions.
     double getDouble(const std::string& key)  const throw (DmException);
     
     /// Gets a string. May perform some conversions.
     std::string getString(const std::string& key) const throw (DmException);
     
     /// Gets a nested dictionary.
     Extensible getExtensible(const std::string& key) const throw (DmException);
     
     /// Gets an array.
     std::vector<boost::any> getVector(const std::string& key) const throw (DmException);
  };

};

#endif // DMLITE_CPP_UTILS_TYPES_H
