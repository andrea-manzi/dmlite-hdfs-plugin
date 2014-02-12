/// @file   include/dmlite/cpp/base.h
/// @brief  Base interfaces.
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#ifndef DMLITE_CPP_BASE_H
#define DMLITE_CPP_BASE_H

#include <string>
#include "exceptions.h"

namespace dmlite {

  // Forward declarations.
  class StackInstance;
  class SecurityContext;

  /// Base class for interfaces.
  class BaseInterface {
   public:
    /// Virtual destructor
    virtual ~BaseInterface();

    /// String ID of the implementation.
    virtual std::string getImplId(void) const throw() = 0;

   protected:
    friend class StackInstance;

    /// Set the StackInstance.
    /// Some plugins may need to access other stacks (i.e. the pool may need the catalog)
    /// However, at construction time not all the stacks have been populated, so this will
    /// be called once all are instantiated.
    virtual void setStackInstance(StackInstance* si) throw (DmException) = 0;

    /// Set the security context.
    virtual void setSecurityContext(const SecurityContext* ctx) throw (DmException) = 0;

    /// These method allows plugins to call other plugins setStackInstance and setSecurityContext
    static void setStackInstance(BaseInterface* i,
                                 StackInstance* si) throw (DmException);

    static void setSecurityContext(BaseInterface*   i,
                                   const SecurityContext* ctx) throw (DmException);
  };


  /// Base class for factories.
  class BaseFactory {
   public:
    /// Virtual destructor
    virtual ~BaseFactory();

    /// Set a configuration parameter
    /// @param key   The configuration parameter
    /// @param value The value for the configuration parameter
    virtual void configure(const std::string& key, const std::string& value) throw (DmException) = 0;
  };

};

#endif // DMLITE_CPP_BASE_H
