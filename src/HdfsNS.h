/// @file    plugins/hadoop/HdfsNS.h
/// @brief   plugin to store data in a hdfs backend.
/// @author  Andrea Manzi <abeche@cern.ch>
#ifndef HDFSNS_H
#define	HDFSNS_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdlib>
#include <pwd.h>
#include <grp.h>
#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/io.h>
#include <dmlite/cpp/poolmanager.h>
#include <dmlite/cpp/utils/urls.h>
#include <dmlite/cpp/catalog.h>
#include <vector>
#include <dirent.h>

#include <hdfs.h>
#include "Hdfs.h"


#define PATH_MAX 4096

namespace dmlite {


class HdfsNS: public  Catalog {
public:
	HdfsNS(	std::string nameNode,
			unsigned port,
			std::string uname,
			std::string mode) throw (DmException);

	~HdfsNS() throw (DmException);

	void setStackInstance(StackInstance* si) throw (DmException);
	
	SecurityContext* createSecurityContext(const SecurityCredentials&) throw (DmException);
    
	void setSecurityContext(const SecurityContext*) throw (DmException);

	std::string getImplId() const throw();

	void changeDir(const std::string& path) throw (DmException);

	std::string getWorkingDir(void) throw (DmException);

	ExtendedStat extendedStat(const std::string& path,
			bool followSym = true) throw (DmException);



	void addReplica(const Replica& replica) throw (DmException);

	void deleteReplica(const Replica& replica) throw (DmException);

	std::vector<Replica> getReplicas(const std::string& path) throw (DmException);

	void unlink(const std::string& path) throw (DmException);

	void create(const std::string& path,mode_t mode) throw (DmException);

	mode_t umask(mode_t mask) throw ();

	void setMode(const std::string& path,mode_t mode) throw (DmException);

	void setOwner(const std::string& path, uid_t newUid, gid_t newGid,bool followSymLink = true) throw (DmException);


	void utime(const std::string& path, const struct utimbuf* buf) throw (DmException);

	Directory* openDir(const std::string& path) throw (DmException);

	void closeDir(Directory* dir) throw (DmException);

	struct dirent* readDir(Directory* dir) throw (DmException);

	ExtendedStat* readDirx(Directory* dir) throw (DmException);

	void makeDir(const std::string& path, mode_t mode) throw (DmException);

	void rename(const std::string& oldPath, 	const std::string& newPath) throw (DmException);

	void removeDir(const std::string& path) throw (DmException);

	Replica getReplicaByRFN(const std::string& rfn) throw (DmException);

private:
	StackInstance* si;

  	std::string cwd;

	const SecurityContext* secCtx;


	hdfsFS fs;
	std::string nameNode;
	unsigned    port;
	std::string uname;
	std::string mode;

};

class HdfsPoolManager :public PoolManager {

//poolmanager methods
public:
	HdfsPoolManager(std::string nameNode,
					unsigned port,
					std::string uname,
					std::string mode,
			              	const std::string& passwd,
                			bool useIp,
                			unsigned    life) throw (DmException);

	~HdfsPoolManager() throw (DmException);

	std::string getImplId() const throw();

	void setStackInstance(StackInstance* si) throw (DmException);

        SecurityContext* createSecurityContext(const SecurityCredentials&) throw (DmException);

        void setSecurityContext(const SecurityContext*) throw (DmException);


	std::vector<Pool> getPools(PoolAvailability availability = kAny) throw (DmException);
	
	Pool getPool(const std::string& poolname) throw (DmException);

	
	Location whereToRead(const std::string& path) throw (DmException);
   
        Location whereToRead(std::vector<Replica> ) throw (DmException);
	
	Location whereToWrite(const std::string& path) throw (DmException);


private:

	bool canWrite();
	bool canRead();

	StackInstance* si;
	

	std::string nameNode;
	unsigned    port;
	std::string uname;
	std::string mode;

	std::string tokenPasswd;
        bool        tokenUseIp;
        unsigned    tokenLife;
        std::string userId;

};


class HdfsNSFactory: public CatalogFactory, public PoolManagerFactory {
public:
	HdfsNSFactory() throw (DmException);
	~HdfsNSFactory() throw (DmException);

	void configure(const std::string& key, const std::string& value) throw (DmException);
	Catalog* createCatalog(PluginManager*) throw (DmException);
    	PoolManager* createPoolManager(PluginManager* pm) throw (DmException);

private:
	hdfsFS fs;
	std::string nameNode;
	unsigned    port;
	std::string uname;
	std::string mode;

        std::string tokenPasswd;
        bool        tokenUseIp;
        unsigned    tokenLife;

};

class HDFSDir: public Directory {
public:
    virtual ~HDFSDir(){};

    std::string  path; 
    ExtendedStat  stat; 
    unsigned int offset;
    unsigned int length;
};


};

#endif	// HDFSNS_H
