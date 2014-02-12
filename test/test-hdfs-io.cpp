#include <dmlite/cpp/dmlite.h>
#include "../src/Hdfs.h"
#include <iostream>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/poolmanager.h>
#include <dmlite/cpp/pooldriver.h>


int main(int argc, char **argv)
{

 	dmlite::PluginManager manager;
  
  	if (argc < 2) {
    		std::cout << "Need at least one parameter." << std::endl;
    		return 1;
  	}
  
  	try {
    		manager.loadConfiguration(argv[1]);
  	}
  	catch (dmlite::DmException& e) {
    		std::cout << "Could not load the configuration file." << std::endl << "Reason: " << e.what() << std::endl;
    		return 1;
  	}
	// Create StackInstance
 	dmlite::StackInstance stack(&manager);

  	//Set security credentials
  	dmlite::SecurityCredentials creds;
  	creds.clientName = "/C=CH/O=CERN/OU=GD/CN=Test user 1";

  	creds.remoteAddress = "127.0.0.1";
  	try {
    		stack.setSecurityCredentials(creds);
  	}		
  	catch (dmlite::DmException& e) {
    	std::cout << "Could not set the credentials." << std::endl
              << "Reason: " << e.what() << std::endl;
    	return 4;
  	}

	dmlite::IODriver* iodriver;
	dmlite::Extensible      extras;
        extras["token"] = dmlite::generateToken("", "/dev/zero", "test", 1000, false);
	

	iodriver = stack.getIODriver();

	dmlite::HdfsIOHandler *handler = iodriver->createIOHandler("/dpm/myfile.txt",  O_RDONLY, args);


	std::string test;
	const char *test = "This file has been written by DMLITE\0";
	char test[1024] = {'\0'};

	memset(test, '\0', 1024);;
	handler->read(test, 10);
	std::cout << test;


	handler->seek(10, std::ios_base::cur);

	int i = 0;	
	while(handler->eof() != true){
		memset(test, '\0', 1024);;
		handler->read(test, 3);
		std::cout << test;
		i++;
	}

 	std::cout << std::endl;
	std::cout << i << std::endl;
	delete(handler);

  return 0;
}
