/// @file   include/dmlite/cpp/authn.h
/// @brief  Authentication API. Any sort of security check is plugin-specific.
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#ifndef DMLITE_CPP_AUTHN_H
#define DMLITE_CPP_AUTHN_H

#include <string>
#include <vector>
#include "base.h"
#include "exceptions.h"
#include "utils/extensible.h"

namespace dmlite {
  
  // Forward declarations.
  class PluginManager;
  class StackInstance;
  
  /// Security credentials. To be filled by the front-end.
  struct SecurityCredentials: public Extensible {
    std::string mech;
    std::string clientName;
    std::string remoteAddress;
    std::string sessionId;
    
    std::vector<std::string> fqans;
    
    bool operator == (const SecurityCredentials&) const;
    bool operator != (const SecurityCredentials&) const;
    bool operator <  (const SecurityCredentials&) const;
    bool operator >  (const SecurityCredentials&) const;
  };
  
  /// User information.
  /// To be filled by the Authn plugin with whichever data
  /// it is needed. (i.e. uid for LCGDM Adapter)
  /// To be used by other plugins whenever they need it.
  /// IMPORTANT: This means plugins must be compatible with the Authn
  ///            put in charge of security.
  struct UserInfo: public Extensible {
    std::string name;
    
    bool operator == (const UserInfo&) const;
    bool operator != (const UserInfo&) const;
    bool operator <  (const UserInfo&) const;
    bool operator >  (const UserInfo&) const;
  };
  
  /// Group information
  /// See UserInfo
  struct GroupInfo: public Extensible {
    std::string name;
    
    bool operator == (const GroupInfo&) const;
    bool operator != (const GroupInfo&) const;
    bool operator <  (const GroupInfo&) const;
    bool operator >  (const GroupInfo&) const;
  };
  

  /// Security context. To be created by the Authn.
  struct SecurityContext {
    SecurityContext() {}
    
    SecurityContext(const SecurityCredentials& c,
                    const UserInfo& u,
                    std::vector<GroupInfo>& g):
                      credentials(c), user(u), groups(g) {}
    
    SecurityCredentials    credentials;
    
    UserInfo               user;
    std::vector<GroupInfo> groups;
    
    bool operator == (const SecurityContext&) const;
    bool operator != (const SecurityContext&) const;
    bool operator <  (const SecurityContext&) const;
    bool operator >  (const SecurityContext&) const;
  };
  
  

  /// User and group handling.
  ///@note This is the only interface not inheriting from BaseInterface.
  class Authn {
   public:
    /// Destructor
    virtual ~Authn();

    /// String ID of the user DB implementation.
    virtual std::string getImplId(void) const throw() = 0;

    /// Create a security context from the credentials.
    /// @param cred The security credentials.
    /// @return     A newly created SecurityContext.
    virtual SecurityContext* createSecurityContext(const SecurityCredentials& cred) throw (DmException) = 0;

    /// Create a new group.
    /// @param groupName The group name.
    /// @return          The new group.
    virtual GroupInfo newGroup(const std::string& groupName) throw (DmException) = 0;

    /// Get a specific group.
    /// @param groupName The group name.
    /// @return          The group.
    virtual GroupInfo getGroup(const std::string& groupName) throw (DmException) = 0;
    
    /// Get a specific group using an alternative key.
    /// @param key   The key name.
    /// @param value They value to search for.
    /// @return      The group.
    /// @note        The implementation will throw an exception if the field
    ///              can not be used as key.
    virtual GroupInfo getGroup(const std::string& key,
                               const boost::any& value) throw (DmException) = 0;
    
    /// Get the group list.
    virtual std::vector<GroupInfo> getGroups(void) throw (DmException) = 0;
    
    /// Update group info. 'name' identify uniquely the group.
    /// @param group The group metadata to update.
    virtual void updateGroup(const GroupInfo& group) throw (DmException) = 0;
    
    /// Delete a group.
    virtual void deleteGroup(const std::string& groupName) throw (DmException) = 0;

    /// Create a new user.
    /// @param userName The user name.
    /// @return         The new user.
    virtual UserInfo newUser(const std::string& userName) throw (DmException) = 0;

    /// Get a specific user.
    /// @param userName The user name.
    /// @return         The user.
    virtual UserInfo getUser(const std::string& userName) throw (DmException) = 0;
    
    /// Get a specific user using an alternative key.
    /// @param key   The key name.
    /// @param value They value to search for.
    /// @return      The user.
    /// @note        The implementation will throw an exception if the field
    ///              can not be used as key.
    virtual UserInfo getUser(const std::string& key,
                             const boost::any& value) throw (DmException) = 0;
    
    /// Get the user list.
    virtual std::vector<UserInfo> getUsers(void) throw (DmException) = 0;
    
    /// Update user info. 'name' identify uniquely the user.
    /// @param user The user metadata to update.
    virtual void updateUser(const UserInfo& user) throw (DmException) = 0;
    
    /// Delete a user.
    virtual void deleteUser(const std::string& userName) throw (DmException) = 0;

    /// Get the mapping of a user/group. Additionaly, new users and groups MAY
    /// be created by the implementation.
    /// @param userName   The user name.
    /// @param groupNames The different groups. Can be empty.
    /// @param user       Pointer to an UserInfo struct where to put the data.
    /// @param groups     Pointer to a vector where the group mapping will be put.
    /// @note If groupNames is empty, grid mapfile will be used to retrieve the default group.
    virtual void getIdMap(const std::string& userName,
                          const std::vector<std::string>& groupNames,
                          UserInfo* user,
                          std::vector<GroupInfo>* groups) throw (DmException) = 0;
  };


  /// AuthnFactory
  class AuthnFactory: public virtual BaseFactory {
   public:
    /// Destructor
    virtual ~AuthnFactory();

   protected:
    // Stack instance is allowed to instantiate Authn
    friend class StackInstance;

    /// Children of AuthnFactory are allowed to instantiate too (decorator)
    static Authn* createAuthn(AuthnFactory* factory,
                              PluginManager* pm) throw (DmException);

    /// Instantiate a implementation of Authn
    virtual Authn* createAuthn(PluginManager* pm) throw (DmException) = 0;
  };

};

#endif // DMLITE_CPP_AUTH_H
