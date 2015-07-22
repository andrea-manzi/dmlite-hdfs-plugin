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
#include "HdfsNS.h"

using namespace dmlite;

// HdfsNSFactory implementation
HdfsNSFactory::HdfsNSFactory() throw (DmException):
      nameNode("localhost"), port(8020), uname("dpmmgr"),mode("rw")

{
  // Nothing
}

  

void HdfsNSFactory::configure(const std::string& key, const std::string& value) throw (DmException)
{

  if (key == "TokenPassword") {
    this->tokenPasswd = value;
  }
  else if (key == "TokenId") {
    this->tokenUseIp = (strcasecmp(value.c_str(), "ip") == 0);
  }
  else if (key == "TokenLife") {
    this->tokenLife = (unsigned)atoi(value.c_str());
  }

  else if (key == "HdfsNameNode") {
    this->nameNode = value;
  }
  else if (key == "HdfsPort") {
    this->port = (unsigned)atoi(value.c_str());
  }
  else if (key == "HdfsUser") {
    this->uname = value;
  }
  else if (key == "HdfsMode") {
      this->mode = value;
    }
  else if (key == "HadoopHomeLib") {
	if (value == "")
                throw DmException(DMLITE_SYSERR(ENOSYS), "HadoopHomeLib is not set ");

      HDFSUtil::setClasspath(value);
      std::string libFolder = std::string(value);
      HDFSUtil::setClasspath(libFolder.append(std::string("/lib")));
  }
  else if (key == "HdfsHomeLib") {
         if (value == "")
               throw DmException(DMLITE_SYSERR(ENOSYS), "HdfsHomeLib is not set ");
 
      HDFSUtil::setClasspath(value);
      std::string libFolder = std::string(value);

      HDFSUtil::setClasspath(libFolder.append(std::string("/lib")));

  }
  else if (key == "JavaHome"){
         if (value == "")
               throw DmException(DMLITE_SYSERR(ENOSYS), "JavaHome is not set ");

        HDFSUtil::setLibraryPath(value);
 
 }
 else if (key == "MapFile")
 {
    this->mapFile = value;
 }
 else if (key == "HdfsGateway") {
    std::stringstream gatewayString(value);
    std::string gateway;
    while( std::getline( gatewayString , gateway , ',' ) ) {
	 if (std::find(this->gateways.begin(),this->gateways.end(), HDFSUtil::trim(gateway))==this->gateways.end()){
	         this->gateways.push_back( HDFSUtil::trim(gateway) );
	}
    }

  }
  else
    throw DmException(DMLITE_CFGERR(DMLITE_UNKNOWN_KEY),
                      "Unrecognised option " + key);

}

Catalog* HdfsNSFactory::createCatalog(PluginManager* pm) throw(DmException)
{

         return new HdfsNS(this->nameNode,
                         this->port,
                         this->uname,
                         this->mode,
			 this->gateways);
}



PoolManager* HdfsNSFactory::createPoolManager(PluginManager * pm) throw(DmException)
{
        return new HdfsPoolManager(
                        this->nameNode,
                        this->port,
                        this->uname,
                        this->mode,
			this->tokenPasswd,
			this->tokenUseIp,
			this->tokenLife);
}

Authn* HdfsNSFactory::createAuthn(PluginManager*) throw (DmException)
{       
  return new HdfsAuthn(this->mapFile);
}


HdfsNSFactory::~HdfsNSFactory() throw (DmException)
{

}


HdfsNS::HdfsNS(std::string nameNode,
		unsigned port,
		std::string uname,
		std::string mode,
		const std::vector<std::string>& gateways)

 throw (DmException): nameNode(nameNode),
 port(port),uname(uname),mode(mode),cwd("")
{
	fs = hdfsConnectAsUser(nameNode.c_str(),
            port,
            uname.c_str());

	if (fs == 0)
		throw DmException(DMLITE_SYSERR(errno),
				"Could not create a HdfsNS: cannot connect to Hdfs");

	this->cwd = getWorkingDir();
	this->gateways = std::vector<std::string>(gateways);
	Log(Logger::Lvl4,hdfslogmask, hdfslogname, "gateway size: " << this->gateways.size());
}


HdfsNS::~HdfsNS() throw (DmException)
{
	 hdfsDisconnect(this->fs);
}

//poolmanager methods

std::string HdfsNS::getImplId() const throw ()
{
  return "HdfsNS";
}



void HdfsNS::changeDir(const std::string& path) throw (DmException)
{

	if (hdfsSetWorkingDirectory(this->fs,path.c_str())!= 0)
		 throw DmException(DMLITE_SYSERR(errno), "Could not changeDir to %s",
		                       path.c_str());
	this->cwd = path;

}

std::string HdfsNS::getWorkingDir(void) throw (DmException)
{
	
        char buffer[PATH_MAX]; 

	hdfsGetWorkingDirectory(this->fs,buffer,sizeof(buffer));

	if ( buffer == NULL)
	 throw DmException(DMLITE_SYSERR(errno), "Could not get Current Working dir");

	this->cwd = std::string(buffer);

	return this->cwd;
	
}

ExtendedStat HdfsNS::extendedStat(const std::string& path,
			bool followSym) throw (DmException)
{
	hdfsFileInfo* hInfo = hdfsGetPathInfo(this->fs, path.c_str());


	if (!hInfo)
		throw DmException(ENOENT, "HDFSNS: Cannot stat %s",path.c_str());

	ExtendedStat exStat;


	exStat.stat.st_atim.tv_sec = hInfo->mLastAccess;
        exStat.stat.st_ctim.tv_sec = hInfo->mLastMod;
        exStat.stat.st_mtim.tv_sec = hInfo->mLastMod;

        exStat.stat.st_gid   = 0;
        exStat.stat.st_uid   = 0;
 	
	if (hInfo->mKind == kObjectKindDirectory) {
		 int numFiles;
		 hdfsFileInfo* hFolder = hdfsListDirectory(this->fs, path.c_str(),&numFiles);
		 exStat.stat.st_nlink = numFiles;
		 hdfsFreeFileInfo(hFolder, numFiles);
	} else  
		exStat.stat.st_nlink =  1;

	
	exStat.stat.st_ino   = 0;
    	exStat.stat.st_mode  =  (hInfo->mKind == kObjectKindDirectory) ? (S_IFDIR | hInfo->mPermissions) :  (S_IFREG | hInfo->mPermissions);

    	exStat.stat.st_size  = (hInfo->mKind == kObjectKindDirectory) ? 4096 : hInfo->mSize;
	exStat.status  = ExtendedStat::kOnline;
	exStat["type"]  = hInfo->mKind;
	

	exStat.parent = 0;
	exStat["pool"] = std::string("hdfs_pool");
 
	
 	std::vector<std::string> components = Url::splitPath(path);
	exStat.name = components.back();

	hdfsFreeFileInfo(hInfo, 1);

	return exStat;


}

ExtendedStat HdfsNS::extendedStatByRFN(const std::string& rfn) throw (DmException)
{

  //remove the host info if present
  std::string uri_string = std::string(rfn);

  size_t index = uri_string.find(':');

  if (index!=std::string::npos){
          uri_string = uri_string.substr(index+1, rfn.size());
  }

   hdfsFileInfo* hInfo = hdfsGetPathInfo(this->fs, uri_string.c_str());

   if (!hInfo)
        throw DmException(ENOENT, "HDFSNS: Cannot stat %s",uri_string.c_str());

   ExtendedStat exStat;


   exStat.stat.st_atim.tv_sec = hInfo->mLastAccess;
   exStat.stat.st_ctim.tv_sec = hInfo->mLastMod;
   exStat.stat.st_mtim.tv_sec = hInfo->mLastMod;

   exStat.stat.st_gid   = 0;
   exStat.stat.st_uid   = 0;
  
   if (hInfo->mKind == kObjectKindDirectory) {
           int numFiles;
           hdfsFileInfo* hFolder = hdfsListDirectory(this->fs, rfn.c_str(),&numFiles);
           exStat.stat.st_nlink = numFiles;
           hdfsFreeFileInfo(hFolder, numFiles);
     } else  
           exStat.stat.st_nlink =  1;
  
   exStat.stat.st_ino   = 0;
   exStat.stat.st_mode  =  (hInfo->mKind == kObjectKindDirectory) ? (S_IFDIR | hInfo->mPermissions) :  (S_IFREG | hInfo->mPermissions);

   exStat.stat.st_size  = (hInfo->mKind == kObjectKindDirectory) ? 4096 : hInfo->mSize;
   exStat.status  = ExtendedStat::kOnline;
   exStat["type"]  = hInfo->mKind;
   exStat.parent = 0;
   exStat["pool"] = std::string("hdfs_pool");

   std::vector<std::string> components = Url::splitPath(uri_string);
   exStat.name = components.back();

   hdfsFreeFileInfo(hInfo, 1);

   return exStat;

}

void HdfsNS::unlink(const std::string& path) throw (DmException)
{
	if (hdfsDelete(this->fs,path.c_str(),1)!= 0)
			 throw DmException(DMLITE_SYSERR(errno), "Could not unlink path %s",
			                       path.c_str());
}

void HdfsNS::create(const std::string& path,mode_t mode) throw (DmException)
{
	int ret = hdfsExists(this->fs, path.c_str());


	if(ret==0)
		throw DmException(DMLITE_SYSERR(errno),"Path %s already exists on HDFS",path.c_str());
	else {
		hdfsFile file =  hdfsOpenFile(this->fs, path.c_str(), mode, 0, 0, 0);
		 
		if (file)
		    {
			hdfsWrite(this->fs, file, 0, 0);
    			hdfsCloseFile(this->fs, file);
		    }

		else  throw DmException(DMLITE_SYSERR(errno),"Could not create path %s",path.c_str());
		
	}

}

void HdfsNS::makeDir(const std::string& path, mode_t mode) throw (DmException)
{
	//mode is ignored
	if(hdfsCreateDirectory(this->fs, path.c_str())!=0)
		throw DmException(DMLITE_SYSERR(errno),"Could not create directory %s ",path.c_str());
}


void HdfsNS::rename(const std::string& oldPath, const std::string& newPath) throw (DmException)
{
	if(hdfsRename(this->fs, oldPath.c_str(),newPath.c_str())!=0)
		throw DmException(DMLITE_SYSERR(errno),"Could not  rename  %s to %s",oldPath.c_str(),newPath.c_str());
}

void HdfsNS::removeDir(const std::string& path) throw (DmException)
{
	if (hdfsDelete(this->fs,path.c_str(),1)!= 0)
				 throw DmException(DMLITE_SYSERR(errno), "Could not delete dir %s",path.c_str());
}


void HdfsNS::setStackInstance(StackInstance* si) throw (DmException)
{
	this->si=si;
}


void HdfsNS::setSecurityContext(const SecurityContext* ctx) throw (DmException)
{
	this->secCtx = ctx;
}




/// Add a new replica for a file.
/// @param replica Stores the data that is going to be added. fileid must
//                point to the id of the logical file in the catalog.
void HdfsNS::addReplica(const Replica& replica) throw (DmException)
{
  //not avilable for hdfs
}

/// Delete a replica.
/// @param replica The replica to remove.
void HdfsNS::deleteReplica(const Replica& replica) throw (DmException)
{
//not avaialble for hdfs
}

void HdfsNS::updateReplica(const Replica& replica) throw (DmException)
{
//not avaialble for hdfs
}

/// Get replicas for a file.
/// @param path The file for which replicas will be retrieved.
std::vector<Replica> HdfsNS::getReplicas(const std::string& path) throw (DmException)
{

  if(hdfsExists(this->fs, path.c_str()) != 0){
    throw DmException(DMLITE_NO_REPLICAS, "HdfsNS: No replicas found on Hdfs for %s",
                      path.c_str());
  }

    std::vector<Replica> replicas;

    for (unsigned i = 0; i < this->gateways.size(); ++i) {

        Log(Logger::Lvl4,hdfslogmask, hdfslogname, "gateway: " << this->gateways.at(i).c_str());
    
    	Replica      replica;
  	ExtendedStat xStat = this->extendedStat(path, true);

  	replica.replicaid  = 0;
  	replica.atime      = xStat.stat.st_atime;
  	replica.fileid     = xStat.stat.st_ino;
  	replica.nbaccesses = 0;
  	replica.ptime      = 0;
  	replica.ltime      = 0;
  	replica.type       = Replica::kPermanent;
  	replica.status     = Replica::kAvailable;
  	replica.server     = this->gateways.at(i).c_str();
  	replica["pool"]    = std::string("hdfs_pool");
	replica.rfn 	   = path;

    	replicas.push_back(replica);
	}

  return replicas;

}



/// Sets the calling processs file mode creation mask to mask & 0777.
/// @param mask The new mask.
/// @return     The value of the previous mask.
mode_t  HdfsNS::umask(mode_t mask) throw ()
{
return 0;
}



/// Set the mode of a file.
/// @param path The file to modify.
/// @param mode The new mode as an integer (i.e. 0755)
void  HdfsNS::setMode(const std::string& path,mode_t mode) throw (DmException)
{ 
     //mode is ignored
     if(hdfsChmod(this->fs, path.c_str(),mode)!=0)
                throw DmException(DMLITE_SYSERR(errno),"Could not set mode %s , %d",path.c_str(),mode);

}

/// Set the owner of a file.
/// @param path   The file to modify.
/// @param newUid The uid of the new owneer.
// @param newGid The gid of the new group.
/// @param followSymLink If set to true, symbolic links will be followed.
void  HdfsNS::setOwner(const std::string& path, uid_t newUid, gid_t newGid, bool followSymLink) throw (DmException)
{

   struct passwd * userInfo ;
   struct group * groupInfo ;

   userInfo =(struct passwd * )wrapCall(getpwuid(newUid));

   groupInfo = (struct group * )wrapCall(getgrgid(newGid));


   if(hdfsChown(this->fs, path.c_str(),userInfo->pw_name,groupInfo->gr_name)!=0)
                throw DmException(DMLITE_SYSERR(errno),"Could not set Owner %s , %d,%d",path.c_str(),newUid,newGid);

}

void HdfsNS::setSize(const std::string& path, size_t newSize) throw (DmException)
{
//not avaialble for hdfs
}

void HdfsNS::setChecksum(const std::string& path, const std::string& csumtype, const std::string& csumvalue) throw (DmException)
{
//not avaialble for hdfs
}

/// Set access and/or modification time.
/// @param path The file path.
/// @param buf  A struct holding the new times.
void  HdfsNS::utime(const std::string& path, const struct utimbuf* buf) throw (DmException)
{


  if (hdfsUtime(this->fs, path.c_str(), buf->modtime, buf->actime)!=0)
	 throw DmException(DMLITE_SYSERR(errno),"Could not acc/mod time %s , %d,%d",path.c_str(),buf->modtime, buf->actime);


}



/// Open a directory for reading.
/// @param path The directory to open.
/// @return     A pointer to a handle that can be used for later calls.
Directory*  HdfsNS::openDir(const std::string& path) throw (DmException)
{
 	HDFSDir *dir;
	ExtendedStat stat;
	int numEntries = 0;

	stat = this->extendedStat(path);
	if (!S_ISDIR(stat.stat.st_mode))
	    throw DmException(ENOTDIR, "path %s is not a directory", path.c_str());

	dir = new HDFSDir();
        dir->path = path;


	hdfsFileInfo* fileInfos = hdfsListDirectory(this->fs, dir->path.c_str(), &numEntries);

	
        dir->stat = stat;
        dir->length = numEntries;
        dir->offset = 0;

	return dir;
}

/// Close a directory opened previously.
/// @param dir The directory handle as returned by NsInterface::openDir.
void  HdfsNS::closeDir(Directory* dir) throw (DmException)
{

        HDFSDir *_dir = dynamic_cast<HDFSDir*>(dir);
       
        if (_dir == NULL)
		throw DmException(DMLITE_SYSERR(EFAULT), "Tried to close a null directory");
	
	delete _dir;

}

/// Read next entry from a directory (simple read).
/// @param dir The directory handle as returned by NsInterface::openDir.
/// @return    0x00 on failure or end of directory.
struct dirent* HdfsNS::readDir(Directory* dir) throw (DmException)
{

	int numEntries = 0;
  	HDFSDir* _dir = dynamic_cast<HDFSDir*>(dir);
	
	if (_dir->offset == _dir->length)
                return 0x00;


	hdfsFileInfo* fileInfos = hdfsListDirectory(this->fs, _dir->path.c_str(), &numEntries);

	//TO DO update access time
	struct dirent* d;
	
	strcpy (d->d_name,fileInfos[_dir->offset].mName);
	d->d_fileno = 0;

	if (fileInfos[_dir->offset].mKind == kObjectKindFile)
		d->d_type = DT_REG;
	else d->d_type = DT_DIR;
	
	_dir->offset++;
	
  	return d;
}

/// Read next entry from a directory (stat information added).
/// @param dir The directory handle as returned by NsInterface::openDir.
/// @return    0x00 on failure (and errno is set) or end of directory.
ExtendedStat*  HdfsNS::readDirx(Directory* dir) throw (DmException)
{
	int numEntries = 0;

	ExtendedStat * stat;


	HDFSDir* _dir = (HDFSDir*)dir;

	//printf("reading dir %s with length %d and offset %d\n",_dir->path.c_str(),_dir->length,_dir->offset);

	if (_dir->offset == _dir->length)
		return 0x00;

	hdfsFileInfo* fileInfos = hdfsListDirectory(this->fs, _dir->path.c_str(), &numEntries);

	//TO DO update access time


        if (numEntries == 0)
                throw DmException(DMLITE_SYSERR(errno),"Could not list directory %s",_dir->path.c_str());

	std::string fileName = std::string(fileInfos[_dir->offset].mName);

	 //remove the port info if present
  	size_t index = fileName.find_last_of('/');

   	if (index!=std::string::npos){
        	fileName = fileName.substr(index+1,fileName.size());
   	}

	std::string filePath;

	if (_dir->path != ("/"))
		filePath =  filePath.append(_dir->path).append(std::string("/")).append(fileName);
	else filePath =  filePath.append(_dir->path).append(fileName);

	//printf("Reading file: %s\n", filePath.c_str());


	_dir->stat  = this->extendedStat(filePath,false);
	_dir->offset++;
	
	return &_dir->stat;

}


/// @param rfn The replica file name.
Replica  HdfsNS::getReplicaByRFN(const std::string& rfn) throw (DmException)
{

  if(hdfsExists(this->fs, rfn.c_str()) != 0){
    throw DmException(DMLITE_NO_REPLICAS, "No replicas found on Hdfs for %s",
                      rfn.c_str());
  }
	//for now i take the first returned
        Replica      replica;
        ExtendedStat xStat = this->extendedStat(rfn, true);

        replica.replicaid  = 0;
        replica.atime      = xStat.stat.st_atime;
        replica.fileid     = xStat.stat.st_ino;
        replica.nbaccesses = 0;
        replica.ptime      = 0;
        replica.ltime      = 0;
        replica.type       = Replica::kPermanent;
        replica.status     = Replica::kAvailable;
        replica.server     = HDFSUtil::getRandomGateway(this->gateways).c_str();//random
        replica["pool"]    = std::string("hdfs_pool");
        replica.rfn        = rfn;

	return replica;
}


HdfsPoolManager::HdfsPoolManager(
                std::string nameNode,
                unsigned port,
                std::string uname,
                std::string mode,
		const std::string& passwd,
                bool useIp,
		unsigned    life) throw (DmException): nameNode(nameNode),port(port),uname(uname),mode(mode),tokenPasswd(passwd), tokenUseIp(useIp),tokenLife(life), userId("")

{
        //nothing
}

HdfsPoolManager::~HdfsPoolManager() throw (DmException)
{
//nothing
}

std::string HdfsPoolManager::getImplId() const throw ()
{
  return "HdfsPoolManager";
}




std::vector<Pool> HdfsPoolManager::getPools(PoolAvailability availability) throw (DmException)
{
        std::vector<Pool> pools;
        
	Pool pool;
	pool = this->getPool("hdfs_pool");
	pools.push_back(pool);
   	
	return pools;

}

Pool  HdfsPoolManager::getPool(const std::string& poolname) throw (DmException)
{
	if (poolname != "hdfs_pool") 
	    throw DmException(DMLITE_NO_SUCH_POOL, "Pool " + poolname + " not found");
	
        Pool pool;

        pool.clear();
        pool.name = "hdfs_pool";
        pool.type = "hdfs";

	
	pool["hostname"]=this->nameNode;
	pool["port"]=this->port;
	pool["username"] = this->uname;
	pool["mode"] = this->mode;

        return pool;

}


void HdfsPoolManager::setStackInstance(StackInstance* si) throw (DmException)
{
        this->si = si;
}

void HdfsPoolManager::setSecurityContext(const SecurityContext* ctx) throw (DmException)
{
  if (this->tokenUseIp)
    this->userId = ctx->credentials.remoteAddress;
  else
    this->userId = ctx->credentials.clientName;
}

Location HdfsPoolManager::whereToRead(const std::string& path) throw (DmException)
{
  return this->whereToRead(this->si->getCatalog()->getReplicas(path));
}


/// Get a location for a logical name.
/// @param path     The path to get.
Location HdfsPoolManager::whereToRead(std::vector<Replica> replicas ) throw (DmException)
{

    unsigned i;

    if (this->canRead()) {

	std::vector<Location> available;

	for (i = 0; i < replicas.size(); ++i) {

 	     try {
	      PoolHandler* handler = this->si->getPoolDriver("hdfs")->createPoolHandler("hdfs_pool");
	      available.push_back(handler->whereToRead(replicas[i]));
	        delete handler;
	      }
	      catch (DmException& e) {
	        if (e.code() != DMLITE_NO_SUCH_POOL) throw;
	      }
	  }
	  //  random one from the available
	  if (available.size() > 0) {
	    i = rand() % available.size();
	    return available[i];
	  }
	  else 
	    throw DmException(DMLITE_NO_REPLICAS,
                      "None of the replicas is available for reading");
		
    }
    else 
       throw DmException(DMLITE_SYSERR(ENOSYS), "HdfsPoolManager: the pool is in write only mode");
 
	
}


/// Start the PUT of a file. 
/// @param path  The path of the file to create.
// @return      The physical location where to write.
Location HdfsPoolManager::whereToWrite(const std::string& path) throw (DmException)
{

    if (this->canWrite()){


        std::vector<Pool> pools = this->getPools(PoolManager::kForWrite);
	if (pools.size() == 0)
	    throw DmException(ENOSPC, "There are no pools available for writing");

	ExtendedStat eStat;
    	bool fileExists = true;

    	try {
		eStat = this->si->getCatalog()->extendedStat(path);
    	}
    	catch (...) {
		fileExists = false;
    	}	

    	if (fileExists)
	 throw DmException(DMLITE_SYSERR(ENOSYS), "HdfsPoolManager: the file already exists");

	
	PoolHandler* handler = this->si->getPoolDriver("hdfs")->createPoolHandler("hdfs_pool");

	Location loc = handler->whereToWrite(path);

	delete handler;

    	return loc;
  
	} 
   else
	 throw DmException(DMLITE_SYSERR(ENOSYS), "HdfsPoolManager: the pool is in read only mode");

}


bool HdfsPoolManager::canWrite(){
	
	return (this->mode == "w" || this->mode == "rw" || this->mode == "wr");

}

bool HdfsPoolManager::canRead(){
       
       return (this->mode == "r" || this->mode == "rw" || this->mode == "wr");

}


static void registerNSHdfs(PluginManager* pm) throw (DmException)
{
  HdfsNSFactory* hdfsNSFactory = new HdfsNSFactory();

  pm->registerAuthnFactory(hdfsNSFactory);
  pm->registerCatalogFactory(hdfsNSFactory);
  pm->registerPoolManagerFactory(hdfsNSFactory);
}


PluginIdCard plugin_hdfs_ns = {
  PLUGIN_ID_HEADER,
  registerNSHdfs
};

