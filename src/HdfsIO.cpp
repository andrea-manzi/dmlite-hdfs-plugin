/*
 * Copyright (c) CERN 2013
 * 
 * Copyright (c) Members of the EMI Collaboration. 2010-2013
 * See  http://www.eu-emi.eu/partners for details on the copyright
 * holders.
 *  
 * Licensed under Apache License Version 2.0        
 * 
 *
*/ 
#include "Hdfs.h"
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <string.h>

using namespace dmlite;

HdfsIOHandler::HdfsIOHandler(HdfsIODriver* driver,
                                 const std::string& uri, 
                                 int flags) throw (DmException):
  driver(driver), path(uri),isWriting(false)
{
  int err;       
  std::string filename;
  //mutex 
  if ((err=pthread_mutex_init(&this->mtx_, 0)))   throw DmException(err, "Could not create a new mutex"); 
  //connect to the cluster
  Log(Logger::Lvl4,hdfslogmask,hdfslogname," Trying to open file" << uri.c_str());
  
  this->fs = hdfsConnectAsUserNewInstance(driver->nameNode.c_str(), driver->port, driver->uname.c_str());

  if(!this->fs)
   throw DmException(DMLITE_SYSERR(errno),
                      "Could not open the Hdfs Filesystem");

  //remove the host info if present
  std::string uri_string = std::string(uri);
 
  size_t index = uri_string.find(':');

  if (index!=std::string::npos){
          uri_string = uri_string.substr(index+1, uri.size());
  }

   //getting filename
  size_t index_last = uri_string.find_last_of("/");

  if (index_last!=std::string::npos){
          filename = uri_string.substr(index_last+1, uri_string.size());
  }

  //O_RDWR is not supported in HDFS move to O_RDONLY or O_WRONLY accordingly (in the case of xrootd)
  if (flags & (O_RDWR  | O_CREAT)) {
        flags =  flags & ~O_RDWR | O_WRONLY;
        Log(Logger::Lvl4,hdfslogmask,hdfslogname,"Trying to open file for Write " << uri.c_str());
  } else if (flags & O_RDWR ) {
        flags =  flags & ~O_RDWR | O_RDONLY;
        Log(Logger::Lvl4,hdfslogmask,hdfslogname,"Trying to open file for Read " << uri.c_str());
  } 
  if (flags & O_WRONLY) {
       isWriting = true;
  } 
  
  // Try to open the hdfs file, map the errno to the DmException otherwise
  this->file = hdfsOpenFile(this->fs, uri_string.c_str(), flags, 0, this->driver->replication, 0);
  
  if (!this->file)//workaround using ENOENT always
    throw DmException(ENOENT, "Can not open the Hdfs file '%s'", uri_string.c_str());

  Log(Logger::Lvl4,hdfslogmask,hdfslogname," opened file: "<< uri_string.c_str());
  
  this->isEof = false;

  //in case of write operation try to open the file used to buffer the call
  if (this->isWriting) {
	lk l(&this->mtx_);
	//initilialize rand
	int random;
	std::stringstream strs;
	srand (time(NULL));
	random =rand();
  	strs << random;
	//create temp folder
	struct stat st = {0};

        strcpy(this->temp_path,this->driver->tmpFolder.c_str());
        strcat(this->temp_path,"/hdfs-io-temp");

	if (stat(this->temp_path, &st) == -1) {
 	   if ( HDFSUtil::mkdirs(this->temp_path) == -1)
		 throw DmException(errno, "Could not create the temp folder for writing %s", this->temp_path);
        }

	strcat(this->temp_path,"/temp");
	strcat(this->temp_path,strs.str().c_str());
        strcat(this->temp_path,filename.c_str());

	this->temp_fd = ::open(this->temp_path,O_CREAT|O_WRONLY, 0700);
	Log(Logger::Lvl4,hdfslogmask,hdfslogname," opened temp file: "<< this->temp_path);
	if (this->temp_fd == -1)
	    throw DmException(errno, "Could not create the temp file for writing %s", this->temp_path);

   }

}



HdfsIOHandler::~HdfsIOHandler()
{
  // Close the file if its still open
  if(this->file)
    hdfsCloseFile(this->fs, this->file);
  
  hdfsDisconnect(this->fs);

  //close and remove the temp file
  if(this->isWriting) {
	  if (this->temp_fd != -1)
	    ::close(this->temp_fd);
          ::remove(this->temp_path);
	  strcpy(this->temp_path, "");
	  this->temp_fd = -1;
  }
  pthread_mutex_destroy(&this->mtx_); 

  Log(Logger::Lvl4,hdfslogmask,hdfslogname,"closed file");

}



void HdfsIOHandler::close(void) throw (DmException)
{


  Log(Logger::Lvl4,hdfslogmask,hdfslogname,"closing file "  << this->path.c_str()) ;
  int ret = 0;
  //in case of write operation write the file to hdfs
  if (this->isWriting) {
	ret = this->copyToHDFS();
  }

  if(this->file)
    hdfsCloseFile(this->fs, this->file);
  this->file = 0;

  //close the temp file for writing operataions and remove the temp file
  if(this->isWriting) {
         ::close(this->temp_fd);
         this->temp_fd = -1;
	 ::remove(this->temp_path);
  }

  if (this->isWriting && (ret==-1))
	throw DmException(EIO, "Could not copy the temp file to HDFS %s", this->temp_path);

}



size_t HdfsIOHandler::read(char* buffer, size_t count) throw (DmException)
{
	lk l(&this->mtx_);
	size_t bytes_read = hdfsRead(this->fs, this->file, buffer, count);
 
        //EOF flag is returned if the number of bytes read is lesser than the HDFS BUFSIZE
  	if (bytes_read < BUFF_SIZE)
  		this->isEof = true;
	
        Log(Logger::Lvl4,hdfslogmask,hdfslogname,"reading " << bytes_read << " bytes from file " << this->path.c_str() << ", requested bytes " << count);
	
	return bytes_read;
}



// Write a chunk of a file in a HDFS FS
size_t HdfsIOHandler::write(const char* buffer, size_t count) throw (DmException){
        Log(Logger::Lvl4,hdfslogmask,hdfslogname,"writing " << count << " bytes to  file " << this->path.c_str());
        lk l(&this->mtx_);
	ssize_t nbytes = ::write(this->temp_fd, buffer, count);
 
         if (nbytes < 0) {
	    char errbuffer[128];
	    strerror_r(errno, errbuffer, sizeof(errbuffer));
	    throw DmException(errno, "%s", errbuffer);
	  }
 
	return static_cast<size_t>(nbytes);
}

// Write a chunk of a file in a HDFS FS
size_t HdfsIOHandler::writeToHDFS(const char* buffer, size_t count) throw (DmException){
       
	Log(Logger::Lvl4,hdfslogmask,hdfslogname,"writing " << count << " bytes to  file " << this->path.c_str());
	return hdfsWrite(this->fs, this->file, buffer, count);
}


int HdfsIOHandler::copyToHDFS(void) throw (DmException) {
    int fd_from;
    char buf[8096];
    ssize_t nread;
    int saved_errno;
    lk l(&this->mtx_); 
    fd_from = ::open(this->temp_path, O_RDONLY);
    if (fd_from < 0)
        return -1;

    while (nread = ::read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = this->writeToHDFS(out_ptr,nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        ::close(fd_from);
        /* Success! */
        Log(Logger::Lvl4,hdfslogmask,hdfslogname,"Succesfully written file");
	return 0;
    }

  out_error:
    saved_errno = errno;
    ::close(fd_from);
    errno = saved_errno;
    return -1;
}

// Position the reader pointer to the desired offset
void HdfsIOHandler::seek(off_t offset, Whence whence) throw (DmException){

        long positionToSet = 0;

	if (this->isWriting) {
		if (::lseek64(this->temp_fd, offset, whence) == ((off_t) - 1))
		    throw DmException(errno, "Could not seek");
                Log(Logger::Lvl4,hdfslogmask,hdfslogname,"seeking to offset " << offset << " for  file " << this->path.c_str());
	} else {

	    lk l(&this->mtx_);
	    // Whence described from where the offset has to be set (begin, current or end)
	    switch(whence)
		{
	    case SEEK_SET:
			positionToSet = offset;
			break;	
	    case SEEK_CUR:
    			positionToSet = (this->tell() + offset);
    			break;
	    case SEEK_END :
			positionToSet = (hdfsAvailable(this->fs, this->file) - offset);
			break;
		default:
                  break;
		}

   	    hdfsSeek(this->fs, this->file, positionToSet);
            Log(Logger::Lvl4,hdfslogmask,hdfslogname,"seeking to offset " << positionToSet << " for  file " << this->path.c_str());
	
       }
}



off_t HdfsIOHandler::tell(void) throw (DmException){

        Log(Logger::Lvl4,hdfslogmask,hdfslogname,"file " << this->path.c_str());
        lk l(&this->mtx_);
	return hdfsTell(this->fs, this->file);
}



void HdfsIOHandler::flush(void) throw (DmException){

 	Log(Logger::Lvl4,hdfslogmask,hdfslogname,"file " << this->path.c_str());
	hdfsFlush(this->fs, this->file);
}



bool HdfsIOHandler::eof(void) throw (DmException){
	return this->isEof;
}

size_t HdfsIOHandler::pread(void* buffer, size_t count, off_t offset) throw (DmException){
	
      lk l(&this->mtx_);
      Log(Logger::Lvl4,hdfslogmask,hdfslogname,"read " << count << " bytes from file " << this->path.c_str() << " at offset " << offset);
      size_t n;
      n = hdfsPread(this->fs, this->file,offset, (char*)buffer, count);
      return n;

}

struct stat HdfsIOHandler::fstat(void) throw (DmException){

      struct stat st;
      st.st_size = hdfsAvailable(this->fs, this->file);
      Log(Logger::Lvl4,hdfslogmask,hdfslogname, "File " << this->path.c_str() << " has size" <<  st.st_size);
      return st;

}

HdfsIODriver::HdfsIODriver(const std::string& nameNode,
                               unsigned port,
                               const std::string& uname,
                               const std::string& passwd,
                               bool useIp,
			       const std::string& tmpFolder,
			       unsigned replication):
  nameNode(nameNode), port(port), uname(uname),tokenPasswd(passwd), tokenUseIp(useIp), tmpFolder(tmpFolder), replication(replication)
{
//nothing
}



HdfsIODriver::~HdfsIODriver()
{
  //nothing to do
}



std::string HdfsIODriver::getImplId() const throw ()
{
  return "HdfsIODriver";
}



void HdfsIODriver::setSecurityContext(const SecurityContext* ctx) throw (DmException)
{
  if (this->tokenUseIp)
    this->userId = ctx->credentials.remoteAddress;
  else
    this->userId = ctx->credentials.clientName;
}

void HdfsIODriver::setStackInstance(StackInstance* si) throw (DmException)
{
  this->si_ = si;
}



IOHandler *HdfsIODriver::createIOHandler(const std::string& pfn,
                                           int flags,
                                           const Extensible& extras,
                                           mode_t mode) throw (DmException)
{

    if ( !(flags & IODriver::kInsecure) ) {

	if (!extras.hasField("token"))
    	throw DmException(EACCES, "Missing token");

    	if (dmlite::validateToken(extras.getString("token"),
                            this->userId,
                            pfn, this->tokenPasswd,
                            flags != O_RDONLY) != kTokenOK)
    	throw DmException(EACCES, "Token does not validate (ID %s)",
                      this->userId.c_str());
    }		
  
    return new HdfsIOHandler(this, pfn, flags);
}



void HdfsIODriver::doneWriting(const Location& loc) throw (DmException)
{

  if (loc.empty())
    throw DmException(EINVAL, "Location is empty");

  Log(Logger::Lvl4,hdfslogmask,hdfslogname," trying to rename replica"); 

  // Name the replica properly (supress .upload)
  std::string final = loc[0].url.path.substr(0, loc[0].url.path.length() - 7);

  hdfsFS fs =  hdfsConnectAsUserNewInstance(this->nameNode.c_str(), this->port, this->uname.c_str());

  if (hdfsRename(fs, loc[0].url.path.c_str(), final.c_str()) != 0) {

    hdfsDisconnect(fs);

    throw DmException(errno, "Could not rename %s to %s",
                     loc[0].url.path.c_str(), final.c_str());

  }

  //set status and size
  try {
   
     this->updateReplica(fs, final);

  } catch (DmException& e) { 
	hdfsDisconnect(fs);
	throw e;
  }

  hdfsDisconnect(fs);

  Log(Logger::Lvl4,hdfslogmask,hdfslogname," renaming replica to " << final.c_str());

  
}



void  HdfsIODriver::updateReplica(hdfsFS fs, std::string& final) throw (DmException) 
{
  
  //remove the host info if present
  std::string uri_string = std::string(final);

  size_t index = uri_string.find(':');

  if (index!=std::string::npos){
          uri_string = uri_string.substr(index+1,final.size());
  }
  
  //update replica
  Replica copy(this->si_->getCatalog()->getReplicaByRFN(uri_string.c_str()));
  copy.status = Replica::kAvailable;

  this->si_->getCatalog()->updateReplica(copy);
	
  hdfsFileInfo* hInfo = hdfsGetPathInfo(fs, uri_string.c_str());
       if (!hInfo)
             throw DmException(DMLITE_SYSERR(errno), "Could not stat %s",
                        final.c_str());

  this->si_->getCatalog()->setSize(uri_string.c_str(),hInfo->mSize);

  hdfsFreeFileInfo(hInfo, 1);
    
}
