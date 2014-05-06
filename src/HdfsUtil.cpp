#include "HdfsNS.h"
#include <boost/algorithm/string/predicate.hpp>

using namespace dmlite;


HDFSUtil::HDFSUtil(){}

HDFSUtil::~HDFSUtil(){}

void HDFSUtil::setClasspath(std::string basefolder) throw ()
{

  //getting the current classpath
  char * classpath;
  classpath = getenv ("CLASSPATH");
  
  if (classpath == NULL)
  	classpath = "";
  
  //creating the string
  std::string classpathString = std::string(classpath);
 
  //configure the CLASSPATH
  DIR * dir = opendir(basefolder.c_str());
  struct dirent * d;


  while ( d = readdir(dir))
 {
	 std::string baseFolder = std::string(basefolder);

 	 baseFolder.append(std::string("/"));

        if (d->d_type == 0x8 && (boost::algorithm::ends_with(std::string(d->d_name),".jar")))
                classpathString.append(std::string(":").append(baseFolder.append(std::string(d->d_name)).c_str()));
 }

 
 closedir(dir);
 //overriding
 setenv("CLASSPATH", classpathString.c_str(),true);

}

std::string  HDFSUtil::getRandomGateway(const std::vector<std::string>& gateways) throw ()
{
	if (gateways.size() == 1)
			return gateways.at(0);
	else {
	        //initilialize rand
        	int random;
	        std::stringstream strs;
	        srand (time(NULL));
	        random =rand() % gateways.size();
		return gateways.at(random);
	}

}

void  HDFSUtil::setLibraryPath(std::string java_home) throw ()
{
 //getting the current LD_LIBRARY_PATH
  char * path;
  path = getenv ("LD_LIBRARY_PATH");

  if (path == NULL)
	  path ="";

  //creating the string
  std::string pathString =  std::string(path);
  
  std::string java_string64  = std::string(java_home).append(std::string("/jre/lib/amd64/server/"));
  std::string java_stringi386 =  std::string(java_home).append(std::string("/jre/lib/i386/server/"));

  pathString.append(std::string(":")).append(java_string64).append(std::string(":")).append(java_stringi386);

  setenv("LD_LIBRARY_PATH", pathString.c_str(),true);
}


int HDFSUtil::mkdirs(const char *dir) throw ()
{
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        return mkdir(tmp, S_IRWXU);
}

std::string HDFSUtil::trim(std::string& str) throw ()
{
    std::string::size_type begin=0;
    std::string::size_type end=str.size()-1;
    while(begin<=end && (str[begin]<=0x20 || str[begin]==0x7f))
        ++begin;
    while(end>begin && (str[end]<=0x20 || str[end]==0x7f))
      --end;
    str = str.substr(begin, end - begin + 1);
    return str;
}


