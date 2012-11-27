#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/poolmanager.h>
#include "Hdfs.h"
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace dmlite;

HdfsPoolDriver::HdfsPoolDriver(const std::string& passwd,
                                   bool useIp,
                                   unsigned lifetime) throw (DmException):
 stack(0x00), tokenPasswd(passwd), tokenUseIp(useIp), tokenLife(lifetime)
{
  // Nothing
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
  
  hdfsFS fs = hdfsConnectAsUser(host.c_str(),
                                port,
                                uname.c_str());
  if (fs == 0)
    throw DmException(DMLITE_SYSERR(errno),
                      "Could not create a HdfsPoolDriver: cannot connect to Hdfs");
    
  return new HdfsPoolHandler(this, host, poolName, fs, this->stack, mode);
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
                                     char mode):
  driver(driver), nameNode(nameNode), fs(fs), poolName(poolName), stack(si),
  mode(mode)
{
  // Nothing
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
  return total;
}



uint64_t HdfsPoolHandler::getUsedSpace(void) throw (DmException)
{
  tOffset used = hdfsGetUsed(this->fs);
  if (used < 0)
    throw DmException(DMLITE_SYSERR(errno),
                      "Could not get the free space of %s",
                      this->poolName.c_str());

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
  switch (replica.status) {
    // Need to check if finished, and set the file size in that case
    case Replica::kBeingPopulated:
      if (hdfsExists(this->fs, replica.rfn.c_str()) != 0) {
        return false;
      }
      // It does exist, so update status and size
      else {
        Replica copy(replica);
        copy.status = Replica::kAvailable;
	
	//moving to Catalog interface
        //this->stack->getINode()->updateReplica(copy);
	this->stack->getCatalog()->updateReplica(copy);
        hdfsFileInfo* hInfo = hdfsGetPathInfo(this->fs, replica.rfn.c_str());
        if (!hInfo)
          throw DmException(DMLITE_SYSERR(errno), "Could not stat %s",
                            replica.rfn.c_str());

	//moving to Catalog interface
        //this->stack->getINode()->setSize(replica.fileid, hInfo->mSize);
	this->stack->getCatalog()->setSize(replica.rfn, hInfo->mSize);

        hdfsFreeFileInfo(hInfo, 1);
      }      
      // Done here
      return true;
    // If marked as available, if it actually exists
    case Replica::kAvailable:
      return (hdfsExists(this->fs, replica.rfn.c_str()) == 0);
    // Being deleted, so no
    default:
      return false;
  }
}



Location HdfsPoolHandler::whereToRead(const Replica& replica) throw (DmException)
{
  // To be done
//  throw DmException(DM_NOT_IMPLEMENTED, "hadoop::getLocation");
  std::vector<std::string> datanodes;
  if(hdfsExists(this->fs, replica.rfn.c_str()) == 0){
    char*** hosts = hdfsGetHosts(this->fs, replica.rfn.c_str(), 0, 1);
    if(hosts){
      int i=0;
      while(hosts[i]) {
        int j = 0;
        while(hosts[i][j]) {
          datanodes.push_back(std::string(hosts[i][j]));
          ++j;
        }
        ++i;
      }
      hdfsFreeHosts(hosts);
    }
  }
  
  // Beware! If the file size is 0, no host will be returned
  // Remit to the name node (for instance)
  if (datanodes.size() == 0) {
    throw DmException(DMLITE_NO_REPLICAS, "No replicas found on Hdfs for %s",
                      replica.rfn.c_str());
  }

  Location loc;  
  for (unsigned i = 0; i < datanodes.size(); ++i) {
    Chunk chunk;
    
    chunk.host = datanodes[i].c_str();
    chunk.path = replica.rfn;
    // TODO: Figure out each chunk size
    chunk.offset = 0;

    //moving to Catalog Interface
    //chunk.size   = this->stack->getINode()->extendedStat(replica.fileid).stat.st_size;
    
    chunk.size   = this->stack->getCatalog()->extendedStat(replica.rfn,true).stat.st_size;

    chunk["token"] = generateToken(this->driver->userId,
                                   chunk.path, 
                                   this->driver->tokenPasswd,
                                   this->driver->tokenLife,
                                   false);
    
    loc.push_back(chunk);
  }
  
  
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
}

std::string HdfsPoolHandler::getDatanode(void) throw (DmException)
{

  //getting info on datanodes
  Pool meta = this->stack->getPoolManager()->getPool(poolName);

  std::string host      = meta.getString("hostname");
  long        port      = meta.getLong("port");

  int size=0;
  int i =0;
  long maxCapacity = 0;
  
  hdfsDataNodeInfo * infos =hdfsGetDataNodesFromFS(host.c_str(),
                                                   port,
                                                   "LIVE",
                                                   &size);
  
  if (size == 0) {
    throw DmException(DMLITE_NO_COMMENT, "No LIVE datanodes found on Hdfs");
  }

  std::map<long,std::string> datanodes;


  for (i = 0; i<size; i++) 
  {
      datanodes[infos[i].remaining] = infos[i].hostName;
  }

  for( std::map<long,std::string>::iterator ii=datanodes.begin(); ii!=datanodes.end(); ++ii)
  {
        if (maxCapacity < (*ii).first)
        {
            maxCapacity= (*ii).first;
        }
   }
  
   std::string datanode=datanodes[maxCapacity];

   //remove the port info if present
   size_t index = datanode.find(':');

   if (index!=std::string::npos){
        datanode = datanode.substr(0,index-1);
   }
  
  hdfsFreeDataNodeInfo(infos, size);

  return datanode;

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
  
 
  single.host = this->getDatanode();
  single.path = fn + ".upload";
  single.offset = 0;
  single.size   = 0;
  single["token"] = generateToken(this->driver->userId,
                                  single.path, 
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


