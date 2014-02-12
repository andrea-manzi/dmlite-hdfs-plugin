#include <dmlite/cpp/dmlite.h>
#include "../src/HdfsNS.h"
#include <iostream>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/poolmanager.h>
#include <dmlite/cpp/pooldriver.h>


int main (int argc, char **argv)
{

 	dmlite::PluginManager manager;

	 dmlite::HDFSDir* dir;

	
  
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
	//polmanager   
        dmlite::PoolManager* poolmanager = stack.getPoolManager();
          
        try {
        	std::vector<dmlite::Pool> pools = poolmanager->getPools();


		for( std::vector<dmlite::Pool>::iterator ii=pools.begin(); ii!=pools.end(); ++ii)
  		{
			  std::cout << "Pool -> " <<  (*ii).name << std::endl;

		}
	

        }
        catch (dmlite::DmException& e) {
        std::cout << "Could not list pools." << std::endl
              << "Reason: " << e.what() << std::endl;
        return e.code();
        }

	//test NS
	
	
  	dmlite::Catalog* catalog = stack.getCatalog();

  	try {

		std::string dir = catalog->getWorkingDir();
	    	std::cout << "current dir -> " << dir << std::endl;

  	}
  	catch (dmlite::DmException& e) {
    		std::cout << "Could not get current working dir." << std::endl
              	<< "Reason: " << e.what() << std::endl;
		    	return e.code();
  	}

	//try {

         //       catalog->changeDir("/home/dteam");
	 //	std::string dir = catalog->getWorkingDir();
         //       std::cout << "current dir -> " << dir << std::endl;


        //}
        //catch (dmlite::DmException& e) {
        //        std::cout << "Could not change dir." << std::endl
        //        << "Reason: " << e.what() << std::endl;
        //                return e.code();
        //}
	
	// try {

         //       catalog->create("/dpm/andrea",777);


        //}
        //catch (dmlite::DmException& e) {
        //        std::cout << "Could not create file." << std::endl
        //        << "Reason: " << e.what() << std::endl;
        //                return e.code();
        //}

	dmlite::Location loc;
	try {
		std::cout << "testing whereToWrite"  << std::endl;
		loc = poolmanager->whereToWrite("/dpm/andrea");

               std::cout << "where to write done" << std::endl;

	} catch (dmlite::DmException& e) {
                std::cout << "Could not write " << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }

	
	 for( std::vector<dmlite::Chunk>::iterator ii=loc.begin(); ii!=loc.end(); ++ii)
                {
			  std::cout << "scheme -> " <<  (*ii).url.scheme << std::endl;
			  std::cout << "domain -> " <<  (*ii).url.domain << std::endl;
                          std::cout << "path-> " <<  (*ii).url.path << std::endl;


                }


        dmlite::Location locRead;
        try {
                locRead = poolmanager->whereToRead("/dpm/cern.ch/home/dteam/demo/hadoop.txt");
        } catch (dmlite::DmException& e) {
                std::cout << "Could not read " << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }


         for( std::vector<dmlite::Chunk>::iterator ii=locRead.begin(); ii!=locRead.end(); ++ii)
                {
			  std::cout << "scheme -> " <<  (*ii).url.scheme << std::endl;
                          std::cout << "domain -> " <<  (*ii).url.domain << std::endl;
			  std::cout << "path-> " <<  (*ii).url.path << std::endl;
			  std::cout << "size -> " <<  (*ii).size << std::endl;


                }

	/*
  	 try {

                catalog->makeDir("/dpm",777);
		
        }



	 catch (dmlite::DmException& e) {
                std::cout << "Could not create dir." << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }*/
	
	/*std::cout << "RENAME"  << std::endl;


	 try {

                catalog->rename("/dpm","/dpm/andrea3");


        }
        catch (dmlite::DmException& e) {
                std::cout << "Could not rename" << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }

	 std::cout << "REMOVE DIR"  << std::endl;


	  try {

	 	catalog->removeDir("/dpm/andrea3");



        }
        catch (dmlite::DmException& e) {
                std::cout << "Could not removeDirs" << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }




	 try {

		std::cout << "EXTENDSTAT file"  << std::endl;

		dmlite::ExtendedStat stat =catalog->extendedStat("/dpm/cern.ch/home/dteam",false);
		
		std::cout << "name file -> "   << stat.name << std::endl;
		
		
		printf("Last Access:  %s", ctime(&stat.stat.st_atim.tv_sec));
		printf("Last Mod:     %s", ctime(&stat.stat.st_mtim.tv_sec));
		printf("permissions:  %d",stat.stat.st_mode);
		
		std::stringstream ss;
		
		ss << stat.stat.st_size;

		printf("\n");
	
		std::cout << "size file -> "  << ss.str() << std::endl;
		


        }
        catch (dmlite::DmException& e) {
                std::cout << "Could not stat " << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }
		
	try {	

	 std::cout << "************** OpenDir **********************"  << std::endl;

	dir = (dmlite::HDFSDir*) catalog->openDir("/dpm/cern.ch/home");


	std::cout << "Opened Dir:" << dir->path << std::endl;
	std::stringstream ss;
	printf("\n");
	
	ss << dir->length;


	std::cout << "Dir size: "  << ss.str() << std::endl;

	}

	 catch (dmlite::DmException& e) {
                std::cout << "Could not stat " << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }

	 try {

         std::cout << "************** ReadDir **********************"  << std::endl;

	 std::cout << "reading dir:" << dir->path << std::endl;

         dmlite::ExtendedStat * stat = NULL;
		
	do {
	 
	 stat =catalog->readDirx(dir);

         if (stat  != 0x00)
		std::cout << "file name:" << stat->name << std::endl;

	} while (stat != 0x00);


	catalog->closeDir(dir);


	//std::cout << "reading dir:" << dir->path << std::endl;

	//std::cout << "file name:" << stat->name << std::endl;


        //std::stringstream ss;
        //printf("\n");

        //ss << dir->length;


        //std::cout << "Dir size: "  << ss.str() << std::endl;

        }

         catch (dmlite::DmException& e) {
                std::cout << "Could not stat " << std::endl
                << "Reason: " << e.what() << std::endl;
                        return e.code();
        }


	*/
	
	 return 0;


}
