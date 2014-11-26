/// @file   HdfsAuthn.h
/// @author  <amanzi@cern.ch>
#ifndef HDFSAUTHN_H
#define HDFSAUTHN_H

#include <dmlite/cpp/authn.h>
#include <dmlite/cpp/utils/poolcontainer.h>
#include "Hdfs.h"

namespace dmlite {
  
  class HdfsAuthn: public Authn {
   public:
    /// Constructor.
    HdfsAuthn(const std::string& mapfile) throw(DmException);

    /// Destructor.
    ~HdfsAuthn();

    // Override

    std::string getImplId(void) const throw();

    SecurityContext* createSecurityContext(const SecurityCredentials& cred) throw (DmException);
    SecurityContext* createSecurityContext(void) throw (DmException);
    
    UserInfo getUser(const std::string& userName) throw (DmException);
    GroupInfo getGroup   (const std::string& groupName) throw (DmException);

    void getIdMap(const std::string& userName,
                  const std::vector<std::string>& groupNames,
                  UserInfo* user,
                  std::vector<GroupInfo>* groups) throw (DmException);
   private:
    std::string mapFile;
    
  };
  
};

#endif 
