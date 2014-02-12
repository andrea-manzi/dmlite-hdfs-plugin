/// @file   core/StackInstance.cpp
/// @brief  Implementation of dm::StackInstance
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#include <dmlite/cpp/authn.h>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/inode.h>
#include <dmlite/cpp/io.h>
#include <dmlite/cpp/poolmanager.h>

using namespace dmlite;



#define INSTANTIATE(var, createFunc)\
try {\
  var = createFunc;\
}\
catch (DmException& e) {\
  if (e.code() != DM_NO_FACTORY)\
    throw;\
  var = 0;\
}


#define VALIDATE_SECURITY_CONTEXT \
  if (this->secCtx_ == 0)\
    throw DmException(DM_NO_SECURITY_CONTEXT,\
      "setSecurityContext or setSecurityCredentials must be called before accessing the instances");


StackInstance::StackInstance(PluginManager* pm) throw (DmException):
    pluginManager_(pm), secCtx_(0)
{
  // Instantiate each interface
  INSTANTIATE(this->authn_,       pm->getAuthnFactory()->createAuthn(pm));
  INSTANTIATE(this->inode_,       pm->getINodeFactory()->createINode(pm));
  INSTANTIATE(this->catalog_,     pm->getCatalogFactory()->createCatalog(pm));
  INSTANTIATE(this->poolManager_, pm->getPoolManagerFactory()->createPoolManager(pm));
  INSTANTIATE(this->ioDriver_,    pm->getIOFactory()->createIODriver(pm));
  
  
  // Everything is up, so pass this to the stacks
  // Do it here since they may need other stacks to be already up
  // (i.e. catalog may need inode)
  if (this->inode_)       this->inode_->setStackInstance(this);
  if (this->catalog_)     this->catalog_->setStackInstance(this);
  if (this->poolManager_) this->poolManager_->setStackInstance(this);
  if (this->ioDriver_)    this->ioDriver_->setStackInstance(this);
}



StackInstance::~StackInstance()
{
  if (this->authn_)       delete this->authn_;
  if (this->inode_)       delete this->inode_;
  if (this->catalog_)     delete this->catalog_;
  if (this->poolManager_) delete this->poolManager_;
  if (this->ioDriver_)    delete this->ioDriver_;
  if (this->secCtx_)      delete this->secCtx_;
  
  // Clean pool handlers
  std::map<std::string, PoolDriver*>::iterator i;
  for (i = this->poolDrivers_.begin();
       i != this->poolDrivers_.end();
       ++i) {
    delete i->second;
  }
  this->poolDrivers_.clear();
}



void StackInstance::set(const std::string& key, const boost::any& value) throw (DmException)
{
  this->stackMsg_[key] = value;
}



boost::any StackInstance::get(const std::string& key) const throw (DmException)
{
  std::map<std::string, boost::any>::const_iterator i;
  i = this->stackMsg_.find(key);
  if (i == this->stackMsg_.end())
    throw DmException(DM_UNKNOWN_KEY, "Key " + key + " not found in the Stack configuration");
  return i->second;
}



void StackInstance::erase(const std::string& key) throw (DmException)
{
  this->stackMsg_.erase(key);
}



PluginManager* StackInstance::getPluginManager() throw (DmException)
{
  return this->pluginManager_;
}



Authn* StackInstance::getAuthn() throw (DmException)
{
  if (this->authn_ == 0)
    throw DmException(DM_NO_AUTHN, "No plugin provides Authn");
  return this->authn_;
}



INode* StackInstance::getINode() throw (DmException)
{
  VALIDATE_SECURITY_CONTEXT;
  
  if (this->inode_ == 0)
    throw DmException(DM_NO_INODE, "No plugin provides INode");
  return this->inode_;
}



Catalog* StackInstance::getCatalog() throw (DmException)
{
  VALIDATE_SECURITY_CONTEXT;
  
  if (this->catalog_ == 0)
    throw DmException(DM_NO_CATALOG, "No plugin provides Catalog");
  return this->catalog_;
}



bool StackInstance::isTherePoolManager() throw ()
{
  return this->poolManager_ != NULL;
}



PoolManager* StackInstance::getPoolManager() throw (DmException)
{
  VALIDATE_SECURITY_CONTEXT;
  
  if (this->poolManager_ == 0)
    throw DmException(DM_NO_POOL_MANAGER, "No plugin provides PoolManager");
  return this->poolManager_;
}



PoolDriver* StackInstance::getPoolDriver(const std::string& poolType) throw (DmException)
{
  VALIDATE_SECURITY_CONTEXT;
  
  // Try from dictionary first
  std::map<std::string, PoolDriver*>::iterator i;
  i = this->poolDrivers_.find(poolType);
  if (i != this->poolDrivers_.end())
    return i->second;
  
  // Instantiate
  PoolDriverFactory* phf = this->pluginManager_->getPoolDriverFactory(poolType);
  PoolDriver* ph = phf->createPoolDriver();
  ph->setStackInstance(this);
  ph->setSecurityContext(this->secCtx_);
  
  this->poolDrivers_[poolType] = ph;
  
  return ph;
}



IODriver* StackInstance::getIODriver() throw (DmException)
{
  VALIDATE_SECURITY_CONTEXT;
  
  if (this->ioDriver_ == 0)
    throw DmException(DM_NO_IO, "No plugin provides the IO interface");
  return this->ioDriver_;
}



void StackInstance::setSecurityCredentials(const SecurityCredentials& cred) throw (DmException)
{
  if (this->authn_ == 0)
    throw DmException(DM_NO_AUTHN, "There is no plugin that provides createSecurityContext");
  
  if (this->secCtx_) {
    delete this->secCtx_;
    this->secCtx_ = NULL;
  }
  this->secCtx_ = this->authn_->createSecurityContext(cred);
  
  if (this->inode_ != 0)
    this->inode_->setSecurityContext(this->secCtx_);
  if (this->catalog_ != 0)
    this->catalog_->setSecurityContext(this->secCtx_);
  if (this->poolManager_ != 0)
    this->poolManager_->setSecurityContext(this->secCtx_);
  if (this->ioDriver_ != 0)
    this->ioDriver_->setSecurityContext(this->secCtx_);
}



void StackInstance::setSecurityContext(const SecurityContext& ctx) throw (DmException)
{
  if (this->secCtx_) delete this->secCtx_;
  this->secCtx_ = new SecurityContext(ctx);
  
  if (this->inode_ != 0)
    this->inode_->setSecurityContext(this->secCtx_);
  if (this->catalog_ != 0)
    this->catalog_->setSecurityContext(this->secCtx_);
  if (this->poolManager_ != 0)
    this->poolManager_->setSecurityContext(this->secCtx_);
  if (this->ioDriver_ != 0)
    this->ioDriver_->setSecurityContext(this->secCtx_);
}



const SecurityContext* StackInstance::getSecurityContext() const throw ()
{
  if (this->secCtx_ == 0)
    throw DmException(DM_NO_SECURITY_CONTEXT,
                      "The security context has not been initialized");
  return this->secCtx_;
}
