#include <dmlite/cpp/dmlite.h>
#include "../src/Hdfs.h"
#include <iostream>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/poolmanager.h>
#include <dmlite/cpp/pooldriver.h>
#include <stdio.h>

int main(int argc, char **argv)
{

 	dmlite::PluginManager manager;
  
  	if (argc < 3) {
    		std::cout << "Need at least two parameters." << std::endl;
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
  	creds.clientName = "/DC=ch/DC=cern/OU=Organic Units/OU=Users/CN=amanzi/CN=683749/CN=Andrea Manzi";

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
        extras["token"] = dmlite::generateToken("127.0.0.1", argv[2], "kwpoMyvcusgdbyyws6gfcxhntkLoh8jilwivnivel", 1000, false);//change third parameter to your value of TokenPassword
	

	iodriver = stack.getIODriver();

	dmlite::IOHandler *handler = iodriver->createIOHandler(argv[2],0, extras,  O_RDONLY );

	//const char *test = "This file has been written by DMLITE\0";
	char test[1024] = {'\0'};

	memset(test, '\0', 1024);;
	handler->read(test, 10);
	std::cout << test;
 	std::cout << std::endl;
	delete(handler);

  return 0;
}
