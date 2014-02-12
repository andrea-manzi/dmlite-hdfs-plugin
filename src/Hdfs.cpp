/// @file    plugins/hadoop/Hdfs.cpp
/// @brief   hadoop plugin.
/// @author  Alexandre Beche <abeche@cern.ch>
/// @author  Andrea Manzi <andrea.manzi@cern.ch>
#include "Hdfs.h"

using namespace dmlite;


// HdfsFactory implementation
HdfsFactory::HdfsFactory() throw (DmException):
      nameNode("localhost"), port(8020), uname("dpmmgr"), gateway(false),
      tokenPasswd("default"), tokenUseIp(true), tokenLife(600)
{
  // Nothing
}



void HdfsFactory::configure(const std::string& key, const std::string& value) throw (DmException)
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
  else if (key == "HdfsGateway") {
    this->gateway = (strcasecmp(value.c_str(), "true") == 0);
  }

   else if (key == "HadoopHomeLib") {
        if (value == "")
                throw DmException(DMLITE_SYSERR(ENOSYS), "HadoopHomeLib is not set ");
      //setting CLASSPATH
      HDFSUtil::setClasspath(value);
      std::string libFolder = std::string(value);
      HDFSUtil::setClasspath(libFolder.append(std::string("/lib")));
  }
  else if (key == "HdfsHomeLib") {
         if (value == "")
               throw DmException(DMLITE_SYSERR(ENOSYS), "HdfsHomeLib is not set ");

      //setting CLASSPATH
      HDFSUtil::setClasspath(value);
      std::string libFolder = std::string(value);

      HDFSUtil::setClasspath(libFolder.append(std::string("/lib")));

  }

   else if (key == "JavaHome"){
         if (value == "")
               throw DmException(DMLITE_SYSERR(ENOSYS), "JavaHome is not set ");

        HDFSUtil::setLibraryPath(value);
 
 }


  else
    throw DmException(DMLITE_CFGERR(DMLITE_UNKNOWN_KEY),
                      "Unrecognised option " + key);
}



IODriver* HdfsFactory::createIODriver(PluginManager* pm) throw (DmException)
{
  return new HdfsIODriver(this->nameNode, this->port, this->uname,
                            this->tokenPasswd, this->tokenUseIp);
}



std::string HdfsFactory::implementedPool() throw ()
{
  return "hdfs";
}



PoolDriver* HdfsFactory::createPoolDriver() throw (DmException)
{
  return new HdfsPoolDriver(this->tokenPasswd,
                              this->tokenUseIp,
                              this->tokenLife,
			      this->gateway);
}


static void registerPluginHdfs(PluginManager* pm) throw (DmException)
{
  pm->registerPoolDriverFactory(new HdfsFactory());
}

static void registerIOHdfs(PluginManager* pm) throw (DmException)
{
  pm->registerIOFactory(new HdfsFactory());
}



/// This is what the PluginManager looks for
PluginIdCard plugin_hdfs_pooldriver = {
  PLUGIN_ID_HEADER,
  registerPluginHdfs
};

PluginIdCard plugin_hdfs_io = {
  PLUGIN_ID_HEADER,
  registerIOHdfs
};

