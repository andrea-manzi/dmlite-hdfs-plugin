/// @file   HdfsAuthn.cpp
/// @author <amanzi@cern.ch>
#include <dmlite/cpp/utils/security.h>
#include "HdfsAuthn.h"

using namespace dmlite;

HdfsAuthn::HdfsAuthn(const std::string& mapfile ) throw(DmException):
  mapFile(mapfile)
{
  // Nothing
}



HdfsAuthn::~HdfsAuthn()
{
  // Nothing
}



std::string HdfsAuthn::getImplId(void) const throw()
{
  return std::string("HdfsAuthn");
}



SecurityContext* HdfsAuthn::createSecurityContext(const SecurityCredentials& cred) throw (DmException)
{
  
  UserInfo user;
  std::vector<GroupInfo> groups;

  this->getIdMap(cred.clientName, cred.fqans, &user, &groups);
  
  SecurityContext* sec = new SecurityContext(cred, user, groups);
  
  return sec;
}



SecurityContext* HdfsAuthn::createSecurityContext(void) throw (DmException)
{
  
  UserInfo user;
  std::vector<GroupInfo> groups;
  GroupInfo group;

  user.name    = "root";
  user["uid"]  = 0;
  group.name   = "root";
  group["gid"] = 0;
  groups.push_back(group);

  SecurityContext* sec = new SecurityContext(SecurityCredentials(), user, groups);
  
  return sec;
}

UserInfo HdfsAuthn::getUser(const std::string& userName) throw (DmException)
{
  UserInfo   user;
  uid_t      uid;
  int        banned;
  char       ca[1024], username[256], meta[1024];
	 
  Log(Logger::Lvl4,hdfslogmask, hdfslogname, "usr:" << userName);
 
  //fixed id
  user.name      = userName.c_str();
  user["uid"]    = 500;
  user["banned"] = 0;
  return user;
}

GroupInfo HdfsAuthn::getGroup(const std::string& groupName) throw (DmException)
{
  GroupInfo group;
  gid_t     gid;
  int       banned;
  char      groupname[256], meta[1024];
  
  //fixed group
  group.name      = groupName.c_str();
  group["gid"]    = 500;
  group["banned"] = 0;
  
  return group;
}

void HdfsAuthn::getIdMap(const std::string& userName,
                          const std::vector<std::string>& groupNames,
                          UserInfo* user,
                          std::vector<GroupInfo>* groups) throw (DmException)
{
  std::string vo;
  GroupInfo   group;
  
  Log(Logger::Lvl4, hdfslogmask, hdfslogname, "usr: " << userName);
  
  *user = this->getUser(userName);

  try {
    vo = voFromDn(this->mapFile, userName);
    //if it's not in teh gridmap file is not authenticated
    Log(Logger::Lvl4, hdfslogmask, hdfslogname, "vo: " << vo);
    group = this->getGroup(vo);
    } 
    catch (DmException& e) {
        throw;
    }
    groups->push_back(group);
}
