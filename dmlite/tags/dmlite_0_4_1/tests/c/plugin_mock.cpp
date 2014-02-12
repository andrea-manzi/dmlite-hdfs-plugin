/// @file  plugin_mock.cpp
/// @brief Mock plugin for the C tests.
#include <dirent.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/utils/urls.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "plugin_mock.h"

using namespace dmlite;



MockCatalog::MockCatalog(): DummyCatalog(NULL)
{
  // Nothing
}



MockCatalog::~MockCatalog()
{
  // Nothing
}



std::string MockCatalog::getImplId() const throw ()
{
  return std::string("MockCatalog");
}



void MockCatalog::changeDir(const std::string& path) throw (DmException)
{
  ::chdir(path.c_str());
}



std::string MockCatalog::getWorkingDir() throw (DmException)
{
  char buffer[512];
  ::getcwd(buffer, sizeof(buffer));
  return std::string(buffer);
}



ExtendedStat MockCatalog::extendedStat(const std::string& path, bool) throw (DmException)
{
  ExtendedStat sx;
  
  if (::stat(path.c_str(), &sx.stat) != 0)
    throw DmException(errno, "Could not stat %s", path.c_str());
  
  std::vector<std::string> components = Url::splitPath(path);
  sx.name = components.back();
  sx["easter"] = std::string("egg");
  
  return sx;
}



Directory* MockCatalog::openDir(const std::string& path) throw (DmException)
{
  MockDirectory *md;
  
  md = new MockDirectory();
  
  md->d = ::opendir(path.c_str());
  if (md->d == NULL) {
    delete md;
    throw DmException(errno, "Could not open %s", path.c_str());
  }
  memset(&md->holder.stat, 0, sizeof(struct stat));
  return (Directory*)md;
}



void MockCatalog::closeDir(Directory* d) throw (DmException)
{
  MockDirectory *md = (MockDirectory*)d;
  ::closedir(md->d);
  delete md;
}



ExtendedStat* MockCatalog::readDirx(Directory* d) throw (DmException)
{
  MockDirectory *md = (MockDirectory*)d;
  
  errno = 0;
  struct dirent* entry = ::readdir(md->d);
  if (entry == NULL) {
    if (errno != 0)
      throw DmException(errno, "Could not read: %d", errno);
    return NULL;
  }
  
  md->holder.clear();
  md->holder.name = entry->d_name;
  md->holder["easter"] = std::string("egg");
  
  return &(md->holder);
}



mode_t MockCatalog::umask(mode_t mask) throw ()
{
  return ::umask(mask);
}



void MockCatalog::setMode(const std::string& path, mode_t mode)  throw (DmException)
{
  if (::chmod(path.c_str(), mode) != 0)
    throw DmException(errno, "Could not change the mode of %s", path.c_str());
}



void MockCatalog::create(const std::string& path, mode_t mode) throw (DmException)
{
  int fd = ::open(path.c_str(), O_WRONLY | O_CREAT, mode);
  if (fd < 0)
    throw DmException(errno, "Could not create %s", path.c_str());
  close(fd);
}



void MockCatalog::makeDir(const std::string& path, mode_t mode) throw (DmException)
{
  if (::mkdir(path.c_str(), mode) != 0)
    throw DmException(errno, "Could not make dir %s", path.c_str());
}



void MockCatalog::rename(const std::string& oldp, const std::string& newp) throw (DmException)
{
  if (::rename(oldp.c_str(), newp.c_str()) != 0)
    throw DmException(errno, "Could not rename %s", oldp.c_str());
}



void MockCatalog::removeDir(const std::string& path) throw (DmException)
{
  if (::rmdir(path.c_str()) != 0)
    throw DmException(errno, "Could not remove %s", path.c_str());
}



void MockCatalog::unlink(const std::string& path) throw (DmException)
{
  if (::unlink(path.c_str()) != 0)
    throw DmException(errno, "Could not unlink %s", path.c_str());
}



void MockCatalog::addReplica(const Replica& replica) throw (DmException)
{
  InoReplicasType::iterator i = replicas.find(replica.fileid);
  
  if (i == replicas.end()) {
    RfnReplicaType rfnDict;
    rfnDict[replica.rfn] = replica;
    replicas[replica.fileid] = rfnDict;
  }
  else {
    (i->second)[replica.rfn] = replica;
  }
}



void MockCatalog::deleteReplica(const Replica& replica) throw (DmException)
{
  InoReplicasType::iterator i = replicas.find(replica.fileid);
  
  if (i == replicas.end())
    throw DmException(ENOENT, "Could not find %d", replica.fileid);
  else {
    RfnReplicaType::iterator j = i->second.find(replica.rfn);
    if (j == i->second.end())
      throw DmException(DM_NO_SUCH_REPLICA, "Could not find %s", replica.rfn.c_str());
    i->second.erase(j);
  }
}



std::vector<Replica> MockCatalog::getReplicas(const std::string& path) throw (DmException)
{
  std::vector<Replica> rl;
  
  ExtendedStat sx = this->extendedStat(path, true);
  
  InoReplicasType::iterator i = replicas.find(sx.stat.st_ino);  
  if (i != replicas.end()) {
    RfnReplicaType::const_iterator j;
    for (j = i->second.begin(); j != i->second.end(); ++j) {
      rl.push_back(j->second);
    }
  }
  return rl;
}



Replica MockCatalog::getReplica(const std::string& rfn) throw (DmException)
{
  InoReplicasType::iterator i;
  
  for (i = replicas.begin(); i != replicas.end(); ++i) {
    RfnReplicaType::const_iterator j = i->second.find(rfn);
    if (j != i->second.end())
      return j->second;
  }
  
  throw DmException(DM_NO_SUCH_REPLICA, "Not found %s", rfn.c_str());
}



void MockCatalog::updateReplica(const Replica& replica) throw (DmException)
{
  InoReplicasType::iterator i = replicas.find(replica.fileid);
  
  if (i == replicas.end())
    throw DmException(ENOENT, "Could not find %d", replica.fileid);
  else {
    RfnReplicaType::iterator j = i->second.find(replica.rfn);
    if (j == i->second.end())
      throw DmException(DM_NO_SUCH_REPLICA, "Could not find %s", replica.rfn.c_str());
    j->second = replica;
  }
}



MockPoolManager::MockPoolManager(): DummyPoolManager(NULL)
{
  // Nothing
}



MockPoolManager::~MockPoolManager()
{
  // Nothing
}



std::string MockPoolManager::getImplId() const throw()
{
  return std::string("MockPoolManager");
}



std::vector<Pool> MockPoolManager::getPools(PoolAvailability availability) throw (DmException)
{
  Pool p;
  
  p.name = "hardcoded";
  p.type = "mock";
  
  p["extra"] = std::string("something");
  
  return std::vector<Pool>(1, p);
}



void MockPoolManager::newPool(const Pool&) throw (DmException)
{
  throw DmException(DM_NOT_IMPLEMENTED,
                    "MockPoolManager::addPool not implemented");
}



void MockPoolManager::updatePool(const Pool&) throw (DmException)
{
  throw DmException(DM_NOT_IMPLEMENTED, "MockPoolManager::updatePool not implemented");
}



void MockPoolManager::deletePool(const Pool&) throw (DmException)
{
  throw DmException(DM_NOT_IMPLEMENTED,
                    "MockPoolManager::deletePool not implemented");
}



Location MockPoolManager::whereToRead(const std::string& path) throw (DmException)
{
  Location loc;
  Chunk    chunk;
  
  chunk.host   = "host1.cern.ch";
  chunk.path   = "/storage/chunk01";
  chunk.offset = 0;
  chunk.size   = 100;
  
  loc.push_back(chunk);
  
  chunk.host   = "host2.cern.ch";
  chunk.path   = "/storage/chunk02";
  chunk.offset = 101;
  chunk.size   =  50;
  chunk["token"] = std::string("123456789");
  
  loc.push_back(chunk);
  
  return loc;
}



Location MockPoolManager::whereToRead(ino_t) throw (DmException)
{
  return this->whereToRead("/");
}



Location MockPoolManager::whereToWrite(const std::string& path) throw (DmException)
{
  Chunk chunk;
  
  chunk.host = "host1.cern.ch";
  chunk.path   = "/storage/chunk01";
  chunk.offset = 0;
  chunk.size   = 0;
  
  chunk["token"] = std::string("987654321");
  
  return Location(1, chunk);
}



MockIOHandler::MockIOHandler(): p(0), closed(false)
{
  char c = 'a';
  for (unsigned i = 0; i < sizeof(content); ++i) {
    content[i] = c++;
    if (c > 'z') c = 'a';
  }
}



MockIOHandler::~MockIOHandler()
{
  // Nothing
}



void MockIOHandler::close(void) throw (DmException)
{
  closed = true;
}


size_t MockIOHandler::read(char* buffer, size_t count) throw (DmException)
{
  if (closed) throw DmException(DM_INTERNAL_ERROR, "Can not read after closing");
  
  size_t i;
  for (i = 0; i < count && p < (off_t)sizeof(content); ++i, ++p) {
    buffer[i] = content[p];
  }
  
  return i;
}



size_t MockIOHandler::write(const char* buffer, size_t count) throw (DmException)
{
  if (closed) throw DmException(DM_INTERNAL_ERROR, "Can not write after closing");
  
  size_t i;
  for (i = 0; i < count && p < (off_t)sizeof(content); ++i, ++p) {
    content[p] = buffer[i];
  }
  
  return i;
}



void MockIOHandler::seek(off_t offset, Whence whence) throw (DmException)
{
  if (closed) throw DmException(DM_INTERNAL_ERROR, "Can not seek after closing");
  switch (whence) {
    case kSet:
      p = offset;
      break;
    case kEnd:
      p = p  + offset;
      break;
    default:
      p += offset;
  }
  
  if (p < 0) p = 0;
}



off_t MockIOHandler::tell (void) throw (DmException)
{
  if (closed) throw DmException(DM_INTERNAL_ERROR, "Can not tell after closing");
  return p;
}



void MockIOHandler::flush(void) throw (DmException)
{
  if (closed) throw DmException(DM_INTERNAL_ERROR, "Can not flush after closing");
}



bool MockIOHandler::eof(void) throw (DmException)
{
  return closed || p > (off_t)sizeof(content);
}



MockIODriver::~MockIODriver()
{
  // Nothing
}



std::string MockIODriver::getImplId() const throw()
{
  return std::string("MockIODriver");
}



void MockIODriver::setStackInstance(StackInstance*) throw (DmException)
{
  // Ignore
}



void MockIODriver::setSecurityContext(const SecurityContext*) throw (DmException)
{
  // Ignore
}



IOHandler* MockIODriver::createIOHandler(const std::string& pfn, int flags,
                                         const Extensible& extras) throw (DmException)
{
  // Only one recognised
  if (pfn != "/file")
    throw DmException(DM_NO_SUCH_FILE, "File  %s not found", pfn.c_str());
  
  // Check token
  switch (flags & ~O_CREAT) {
    case O_RDONLY:
      if (extras.getString("token") != "123456789")
        throw DmException(DM_FORBIDDEN, "Invalid token for reading");
      break;
    case O_WRONLY: case O_RDWR:
      if (extras.getString("token") != "987654321")
        throw DmException(DM_FORBIDDEN, "Invalid token for writing");
      break;
    default:
      throw DmException(DM_INVALID_VALUE, "Invalid value for the flags");
  }
  
  return new MockIOHandler();
}



void MockIODriver::doneWriting(const std::string& pfn, const Extensible& params) throw (DmException)
{
  if (params.getString("token") != "987654321")
    throw DmException(DM_FORBIDDEN, "Invalid token");
  if (pfn != "/storage/chunk01")
    throw DmException(DM_INVALID_VALUE, "Invalid rfn");  
}



MockFactory::~MockFactory()
{
  // Nothing
}



std::string MockFactory::implementedPool() throw ()
{
  return std::string("mock");
}



void MockFactory::configure(const std::string& key, const std::string&) throw (DmException)
{
  throw DmException(DM_UNKNOWN_OPTION, "Unknown parameter %s", key.c_str());
}



Catalog* MockFactory::createCatalog(PluginManager*) throw (DmException)
{
  return new MockCatalog();
}



IODriver* MockFactory::createIODriver(PluginManager*) throw (DmException)
{
  return new MockIODriver();
}



PoolManager* MockFactory::createPoolManager(PluginManager*) throw (DmException)
{
  return new MockPoolManager();
}



void registerMock(PluginManager* pm)
{
  MockFactory* factory = new MockFactory();
  pm->registerCatalogFactory(factory);
  pm->registerIOFactory(factory);
  pm->registerPoolManagerFactory(factory);
}



PluginIdCard plugin_mock = {
  PLUGIN_ID_HEADER,
  registerMock
};
