/*
 * Copyright (c) CERN 2013
 * 
 * Copyright (c) Members of the EMI Collaboration. 2010-2013
 * See  http://www.eu-emi.eu/partners for details on the copyright
 * holders.
 *  
 * Licensed under Apache License Version 2.0        
 * 
 */

#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/poolmanager.h>
#include "Hdfs.h"
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <syslog.h>


using namespace dmlite;

HdfsPoolDriver::HdfsPoolDriver(const std::string& passwd,
                                   bool useIp,
        	                   unsigned lifetime,
				   const std::vector<std::string>& gateways,
				   unsigned replication) throw (DmException):
 stack(0x00), tokenPasswd(passwd), tokenUseIp(useIp), tokenLife(lifetime), replication(replication)
{

  this->gateways = std::vector<std::string>(gateways);

  Log(Logger::Lvl4,hdfslogmask,hdfslogname," Pool Driver created");



}



HdfsPoolDriver::~HdfsPoolDriver()
{
  // Nothing
}



std::string HdfsPoolDriver::getImplId() const throw ()
{
  return "HdfsPoolDriver";
}



void HdfsPoolDriver::setSecurityContext(const SecurityContext* ctx) throw (DmException)
{
  if (this->tokenUseIp)
    this->userId = ctx->credentials.remoteAddress;
  else
    this->userId = ctx->credentials.clientName;
}



void HdfsPoolDriver::setStackInstance(StackInstance* si) throw (DmException)
{
  this->stack = si;
}



PoolHandler* HdfsPoolDriver::createPoolHandler(const std::string& poolName) throw (DmException)
{
  Pool meta = this->stack->getPoolManager()->getPool(poolName);
  
  std::string host      = meta.getString("hostname");
  std::string uname     = meta.getString("username");
  long        port      = meta.getLong("port");
  
  char mode;
  if (meta.getString("mode").empty())
    mode = 0;
  else if (meta.getString("mode") == "r")
    mode = O_RDONLY;
  else if (meta.getString("mode") == "w")
    mode = O_WRONLY;
  else if (meta.getString("mode") == "rw")
    mode = O_RDWR;
  else
    throw DmException(DMLITE_SYSERR(DMLITE_MALFORMED),
                      "Wrong mode '%s' configured in the database for the hdfs pool %s",
                      meta.getString("mode").c_str(), poolName.c_str());
  
  hdfsFS fs = hdfsConnectAsUserNewInstance(host.c_str(),
                                port,
                                uname.c_str());
  if (fs == 0)
    throw DmException(DMLITE_SYSERR(errno),
                      "Could not create a HdfsPoolDriver: cannot connect to Hdfs");
    
  return new HdfsPoolHandler(this, host, poolName, fs, this->stack, mode,replication);
}



void HdfsPoolDriver::toBeCreated(const Pool& pool) throw (DmException)
{
  // Nothing
}



void HdfsPoolDriver::justCreated(const Pool& pool) throw (DmException)
{
  // Nothing
}



void HdfsPoolDriver::update(const Pool& pool) throw (DmException)
{
  // Nothing
}



void HdfsPoolDriver::toBeDeleted(const Pool& pool) throw (DmException)
{
  // Nothing
}



HdfsPoolHandler::HdfsPoolHandler(HdfsPoolDriver* driver,
                                     const std::string& nameNode,
                                     const std::string& poolName,
                                     hdfsFS fs,
                                     StackInstance* si,
                                     char mode,
				     unsigned replication):
  driver(driver), nameNode(nameNode), fs(fs), poolName(poolName), stack(si),
  mode(mode), replication(replication)
{
//nothing to do 
}



HdfsPoolHandler::~HdfsPoolHandler()
{
  hdfsDisconnect(this->fs);
}



std::string HdfsPoolHandler::getPoolType(void) throw (DmException)
{
  return "hdfs";
}



std::string HdfsPoolHandler::getPoolName(void) throw (DmException)
{
  return this->poolName;
}



uint64_t HdfsPoolHandler::getTotalSpace(void) throw (DmException)
{
  tOffset total = hdfsGetCapacity(this->fs);
  if (total < 0)
    throw DmException(DMLITE_SYSERR(errno),
                      "Could not get the total capacity of %s",
                      this->poolName.c_str());
  //replication factor
  total = total/ this->replication;
  return total;
}



uint64_t HdfsPoolHandler::getUsedSpace(void) throw (DmException)
{
  tOffset used = hdfsGetUsed(this->fs);
  if (used < 0)
    throw DmException(DMLITE_SYSERR(errno),
                      "Could not get the free space of %s",
                      this->poolName.c_str());
  used = used/ this->replication;
  return used;
}



uint64_t HdfsPoolHandler::getFreeSpace(void) throw (DmException)
{
  return this->getTotalSpace() - this->getUsedSpace();
}



bool HdfsPoolHandler::poolIsAvailable(bool write) throw (DmException)
{
  return (mode == O_RDWR) || (mode == O_RDONLY && !write) || (mode == O_WRONLY && write);
}



bool HdfsPoolHandler::replicaIsAvailable(const Replica& replica) throw (DmException)
{ 

  Log(Logger::Lvl4,hdfslogmask,hdfslogname," checking replica " << replica.rfn.c_str());

   //remove the host info if present
  std::string _rfn = std::string(replica.rfn);

  size_t index = _rfn.find(':');

  if (index!=std::string::npos){
	  _rfn = _rfn.substr(index+1, _rfn.size());
  }

  switch (replica.status) {
	
    // Need to check if finished, and set the file size in that case
    case Replica::kBeingPopulated:
      if (hdfsExists(this->fs, _rfn.c_str()) != 0) {
        return false;
      }
      // It does exist, so update status and size
      else {
        Replica copy(replica);
        copy.status = Replica::kAvailable;
	
	//moving to Catalog interface
	this->stack->getCatalog()->updateReplica(copy);
        hdfsFileInfo* hInfo = hdfsGetPathInfo(this->fs, _rfn.c_str());
        if (!hInfo)
          throw DmException(DMLITE_SYSERR(errno), "Could not stat %s",
                            _rfn.c_str());

	//moving to Catalog interface
	this->stack->getCatalog()->setSize(_rfn, hInfo->mSize);

        hdfsFreeFileInfo(hInfo, 1);
      }      
      // Done here
      return true;
    // If marked as available, if it actually exists
    case Replica::kAvailable:
      return (hdfsExists(this->fs, _rfn.c_str()) == 0);
    // Being deleted, so no
    default:
      return false;
  }
}



Location HdfsPoolHandler::whereToRead(const Replica& replica) throw (DmException)
{

  Location loc;
  
  Chunk chunk;
		
  //TO CHECK
  //remove the host info if present
  std::string _rfn = std::string(replica.rfn);

  size_t index = _rfn.find(':');

  if (index!=std::string::npos){
         _rfn = _rfn.substr(index+1, _rfn.size());
  }
    
  chunk.url.domain = HDFSUtil::getRandomGateway(this->driver->gateways).c_str();
  chunk.url.path = _rfn;
  chunk.offset = 0;
  chunk.size   = this->stack->getCatalog()->extendedStat(_rfn,true).stat.st_size;

  chunk.url.query["token"] = generateToken(this->driver->userId,
                               chunk.url.path,
                               this->driver->tokenPasswd,
                               this->driver->tokenLife,
                               false);
    
  loc.push_back(chunk);

  return loc;
}



void HdfsPoolHandler::removeReplica(const Replica& replica) throw (DmException)
{
  switch (replica.status) {
    case Replica::kBeingPopulated:
      hdfsDelete(this->fs, (replica.rfn + ".upload").c_str(),0);
    default:
      hdfsDelete(this->fs, replica.rfn.c_str(),0);
  }
  this->stack->getCatalog()->deleteReplica(replica);
}


Location HdfsPoolHandler::whereToWrite(const std::string& fn) throw (DmException)
{
  // Get the path to create (where the file will be put)
  std::string path;
  path = fn.substr(0, fn.find_last_of('/'));

  // Create the path
  hdfsCreateDirectory(this->fs, path.c_str());
  
  // Uri returned_uri;
  Chunk single;
  
  single.url.domain = HDFSUtil::getRandomGateway(this->driver->gateways).c_str();
  
  single.url.path = fn + ".upload";

  single.offset = 0;
  single.size   = 0;
  single.url.query["token"] = generateToken(this->driver->userId,
                                            single.url.path,
                                            this->driver->tokenPasswd,
                                            this->driver->tokenLife,
                                            true);

  Location loc(1, single);
  
  // Add this replica
  struct stat s;

  try { 	
   s = this->stack->getCatalog()->extendedStat(fn).stat;

  }catch (...) {  }
  
  Replica newReplica;
  
  newReplica.fileid = s.st_ino;
  newReplica.status = Replica::kBeingPopulated;
  newReplica.type   = Replica::kPermanent;
  newReplica.rfn    = fn;
  newReplica["pool"] = std::string(this->poolName.c_str());
  
  this->stack->getCatalog()->addReplica(newReplica);
  
  // No token used
  return loc;
}

void  HdfsPoolHandler::cancelWrite(const Location& loc) throw (DmException)
{
  if (loc.empty())
    throw DmException(EINVAL, "Empty location");
  //getting the replica
  Replica rep = this->stack->getCatalog()->getReplicaByRFN(loc[0].url.path);
  this->removeReplica(rep);

}


