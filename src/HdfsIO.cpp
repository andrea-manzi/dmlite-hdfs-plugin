#include "Hdfs.h"
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <sstream>
#include <string.h>

using namespace dmlite;

HdfsIOHandler::HdfsIOHandler(HdfsIODriver* driver,
                                 const std::string& uri, 
                                 int flags) throw (DmException):
  driver(driver), path(uri),isWriting(false)
{	
  //connect to the cluster

  this->fs = hdfsConnectAsUser(driver->nameNode.c_str(), driver->port, driver->uname.c_str());

  if(!this->fs)
   throw DmException(DMLITE_SYSERR(errno),
                      "Could not open the Hdfs Filesystem");

  //remove the host info if present
  std::string uri_string = std::string(uri);
 
  size_t index = uri_string.find(':');

  if (index!=std::string::npos){
          uri_string = uri_string.substr(index+1, uri.size());
  }

  //O_RDWR is not supported in HDFS move to O_RDONLY (in the case of xrootd)
  if (flags & O_RDWR) {
	//flags = flags & ~O_RDWR | O_WRONLY | O_CREAT;
	flags =  flags & ~O_RDWR | O_RDONLY;
  }
   
  if (flags & O_WRONLY) {
       isWriting = true;
  } 
  
  // Try to open the hdfs file, map the errno to the DmException otherwise
  this->file = hdfsOpenFile(this->fs, uri_string.c_str(), flags, 0, 0, 0);
  
  if (!this->file)//workaround using ENOENT always
    throw DmException(ENOENT, "Can not open the Hdfs file '%s'", uri_string.c_str());

  syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::HdfsIOHandler: opened file %s", uri_string.c_str());
  
  this->isEof = false;

  //in case of write operation try to open the file used to buffer the call
  if (this->isWriting) {
	//initilialize rand
	int random;
	std::stringstream strs;
	srand (time(NULL));
	random =rand()/10+1;
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

	this->temp_fd = ::open(this->temp_path,O_CREAT|O_WRONLY, 0700);

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
  }

  syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::~HdfsIOHandler: closed file");

}



void HdfsIOHandler::close(void) throw (DmException)
{

  syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::close: close file %s", this->path.c_str());

  //in case of write operation write the file to hdfs
  if (this->isWriting) {
	if(this->copyToHDFS() == -1) {
		   throw DmException(errno, "Could not copy the temp file to HDFS %s", this->temp_path);
	}
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

}



size_t HdfsIOHandler::read(char* buffer, size_t count) throw (DmException)
{
	size_t bytes_read = hdfsRead(this->fs, this->file, buffer, count);
 
        //EOF flag is returned if the number of bytes read is lesser than the HDFS BUFSIZE
  	if (bytes_read < BUFF_SIZE)
  		this->isEof = true;
	
        syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::read: reading %d bytes from  file %s, requested bytes %d",bytes_read,this->path.c_str(), count);

	return bytes_read;
}



// Write a chunk of a file in a HDFS FS
size_t HdfsIOHandler::write(const char* buffer, size_t count) throw (DmException){

        syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::write: writing %d bytes to  file %s",count,this->path.c_str());

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

	syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::writeToHDFS writing %d bytes to  file %s",count,this->path.c_str());
	
        return hdfsWrite(this->fs, this->file, buffer, count);
}


int HdfsIOHandler::copyToHDFS(void) throw (DmException) {
    int fd_from;
    char buf[8096];
    ssize_t nread;
    int saved_errno;

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
	syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::copyToHDFS: Succesfully written file");
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
	} else {


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
       }

        syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::seek: seeking to offset %ld for file %s",positionToSet,this->path.c_str());


}



off_t HdfsIOHandler::tell(void) throw (DmException){
  
	syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::tell: file %s",this->path.c_str());

	return hdfsTell(this->fs, this->file);
}



void HdfsIOHandler::flush(void) throw (DmException){

	syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::flush: file %s",this->path.c_str());
     	
	hdfsFlush(this->fs, this->file);
}



bool HdfsIOHandler::eof(void) throw (DmException){
	return this->isEof;
}


size_t HdfsIOHandler::pread(void* buffer, size_t count, off_t offset) throw (DmException){


      syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::pread: read %d bytes from  file %s at offset ",count,this->path.c_str(), offset);
      
	
      return hdfsPread(this->fs, this->file,offset, (char*)buffer, count);


}

struct stat HdfsIOHandler::fstat(void) throw (DmException){

      syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::fstat: on file %s ", this->path.c_str());

      struct stat st;

      st.st_size = hdfsAvailable(this->fs, this->file);

      syslog(LOG_USER | LOG_DEBUG, "HdfsIOHandler::fstat: file %s has %d size",this->path.c_str(), st.st_size);

      return st;

}

HdfsIODriver::HdfsIODriver(const std::string& nameNode,
                               unsigned port,
                               const std::string& uname,
                               const std::string& passwd,
                               bool useIp,
			       const std::string& tmpFolder):
  nameNode(nameNode), port(port), uname(uname),tokenPasswd(passwd), tokenUseIp(useIp), tmpFolder(tmpFolder)
{
   syslog(LOG_USER | LOG_DEBUG, "HdfsIODriver::HdfsIODriver: initialized");
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

  syslog(LOG_USER | LOG_DEBUG, "HdfsIODriver::doneWriting: trying to rename replica");


  // Name the replica properly (supress .upload)
  std::string final = loc[0].url.path.substr(0, loc[0].url.path.length() - 7);

  hdfsFS fs =  hdfsConnectAsUser(this->nameNode.c_str(), this->port, this->uname.c_str());

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

  syslog(LOG_USER | LOG_DEBUG, "HdfsIODriver::doneWriting: renaming replica to %s", final.c_str());

  
}



void  HdfsIODriver::updateReplica(hdfsFS fs, std::string& final) throw (DmException) 
{
    
  //update replica
  Replica copy(this->si_->getCatalog()->getReplicaByRFN(final.c_str()));
  copy.status = Replica::kAvailable;

  this->si_->getCatalog()->updateReplica(copy);
	
  hdfsFileInfo* hInfo = hdfsGetPathInfo(fs, final.c_str());
       if (!hInfo)
             throw DmException(DMLITE_SYSERR(errno), "Could not stat %s",
                        final.c_str());

  this->si_->getCatalog()->setSize(final.c_str(),hInfo->mSize);

  hdfsFreeFileInfo(hInfo, 1);
    
}
