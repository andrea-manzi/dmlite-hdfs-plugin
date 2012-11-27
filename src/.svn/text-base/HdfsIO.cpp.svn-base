#include "Hdfs.h"

using namespace dmlite;

HdfsIOHandler::HdfsIOHandler(HdfsIODriver* driver,
                                 const std::string& uri, 
                                 int flags) throw (DmException):
  driver(driver), path(uri)
{
  // Try to open the hdfs file, map the errno to the DmException otherwise
  this->file = hdfsOpenFile(driver->fs, uri.c_str(), flags, 0, 0, 0);
  if (!this->file)
    throw DmException(errno, "Can not open the Hdfs file '%s'", uri.c_str());

  this->isEof = false;
}



HdfsIOHandler::~HdfsIOHandler()
{
  // Close the file if its still open
  if(this->file)
    hdfsCloseFile(this->driver->fs, this->file);
}



void HdfsIOHandler::close(void) throw (DmException)
{
 // Close the file if its open
  if(this->file)
    hdfsCloseFile(this->driver->fs, this->file);
  this->file = 0;
}



size_t HdfsIOHandler::read(char* buffer, size_t count) throw (DmException)
{
	size_t bytes_read = hdfsRead(this->driver->fs, this->file, buffer, count);
 
  // EOF flag is returned if the number of bytes read is lesser than the requested
	if (bytes_read < count)
		this->isEof = true;

	return bytes_read;
}



// Write a chunk of a file in a HDFS FS
size_t HdfsIOHandler::write(const char* buffer, size_t count) throw (DmException){
	return hdfsWrite(this->driver->fs, this->file, buffer, count);
}



// Position the reader pointer to the desired offset
void HdfsIOHandler::seek(off_t offset, Whence whence) throw (DmException){

	long positionToSet = 0;	

	// Whence described from where the offset has to be set (begin, current or end)
	switch(whence)
	{
    case SEEK_SET:
			positionToSet = offset;
			break;	
    case SEEK_CUR:
			positionToSet = (this->tell() + offset);
			break;
    case SEEK_END:
			positionToSet = (hdfsAvailable(this->driver->fs, this->file) - offset);
			break;
		default:
                  break;
	}

	hdfsSeek(this->driver->fs, this->file, positionToSet);
}



off_t HdfsIOHandler::tell(void) throw (DmException){
	return hdfsTell(this->driver->fs, this->file);
}



void HdfsIOHandler::flush(void) throw (DmException){
	hdfsFlush(this->driver->fs, this->file);
}



bool HdfsIOHandler::eof(void) throw (DmException){
	return this->isEof;
}



HdfsIODriver::HdfsIODriver(const std::string& nameNode,
                               unsigned port,
                               const std::string& uname,
                               const std::string& passwd,
                               bool useIp):
  tokenPasswd(passwd), tokenUseIp(useIp)
{
  this->fs = hdfsConnectAsUser(nameNode.c_str(), port, uname.c_str());
  if(!this->fs)
    throw DmException(DMLITE_SYSERR(errno),
                      "Could not open the Hdfs Filesystem");
}



HdfsIODriver::~HdfsIODriver()
{
  hdfsDisconnect(this->fs);
}



std::string HdfsIODriver::getImplId() const throw ()
{
  return "HdfsIODriver";
}



void HdfsIODriver::setStackInstance(StackInstance*) throw (DmException)
{
  // Nothing
}



void HdfsIODriver::setSecurityContext(const SecurityContext* ctx) throw (DmException)
{
  if (this->tokenUseIp)
    this->userId = ctx->credentials.remoteAddress;
  else
    this->userId = ctx->credentials.clientName;
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



void HdfsIODriver::doneWriting(const std::string& pfn,
                                 const Extensible& params) throw (DmException)
{
  // Name the replica properly (supress .upload)
  std::string final = pfn.substr(0, pfn.length() - 7);
  if (hdfsRename(this->fs, pfn.c_str(), final.c_str()) != 0)
    throw DmException(errno, "Could not rename %s to %s",
                      pfn.c_str(), final.c_str());
}
