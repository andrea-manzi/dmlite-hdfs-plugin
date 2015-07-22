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

/// @file    plugins/hadoop/Hdfs.h
/// @brief   plugin to store data in a hdfs backend.
/// @author  Alexandre Beche <abeche@cern.ch>
/// @author  Andrea Manzi <abeche@cern.ch>
#ifndef HDFS_H
#define	HDFS_H

#include <dmlite/cpp/dmlite.h>
#include <dmlite/cpp/io.h>
#include <dmlite/cpp/pooldriver.h>
#include <dmlite/cpp/dummy/DummyPool.h>
#include <dmlite/cpp/dummy/DummyCatalog.h>
#include <dmlite/cpp/poolmanager.h>
#include <dmlite/cpp/utils/urls.h>
#include <dmlite/cpp/catalog.h>
#include <dmlite/cpp/utils/logger.h>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <hdfs.h>
#include <pthread.h>
#define PATH_MAX 4096
#define BUFF_SIZE 65536


namespace dmlite {

class HdfsPoolDriver;

   extern Logger::bitmask hdfslogmask;
   extern Logger::component hdfslogname;

/// PoolHandler
class HdfsPoolHandler: public PoolHandler {
public:
	HdfsPoolHandler(HdfsPoolDriver*, const std::string& nameNode,
			const std::string& poolName, hdfsFS fs,
			StackInstance* si, char mode,unsigned replication);
	~HdfsPoolHandler();

	std::string getPoolType (void) throw (DmException);
	std::string getPoolName (void) throw (DmException);
	uint64_t getTotalSpace  (void) throw (DmException);
	uint64_t getUsedSpace   (void) throw (DmException);
	uint64_t getFreeSpace   (void) throw (DmException);
	bool     poolIsAvailable(bool) throw (DmException);

	bool     replicaIsAvailable(const Replica&) throw (DmException);
	Location whereToRead       (const Replica&) throw (DmException);
	void     removeReplica     (const Replica&) throw (DmException);

	Location whereToWrite(const std::string&) throw (DmException);
	void cancelWrite(const Location& ) throw (DmException);

private:
	HdfsPoolDriver* driver;

	std::string    nameNode;
	hdfsFS         fs;
	std::string    poolName;
	StackInstance* stack;
	char           mode;
	unsigned replication;
};



/// PoolDriver
class HdfsPoolDriver: public PoolDriver {
public:
	HdfsPoolDriver(const std::string&, bool, unsigned, const std::vector<std::string>&,  unsigned replication) throw (DmException);
	~HdfsPoolDriver();

	std::string getImplId() const throw();

	void setStackInstance(StackInstance*) throw (DmException);
	void setSecurityContext(const SecurityContext*) throw (DmException);

	PoolHandler* createPoolHandler(const std::string& poolName) throw (DmException);

	void toBeCreated(const Pool& pool) throw (DmException);
	void justCreated(const Pool& pool) throw (DmException);
	void update(const Pool& pool) throw (DmException);
	void toBeDeleted(const Pool& pool) throw (DmException);

private:
	friend class HdfsPoolHandler;
	StackInstance* stack;
	
	std::string tokenPasswd;
	bool        tokenUseIp;
	unsigned    tokenLife;
	std::string userId;
	std::vector<std::string> gateways;
        unsigned replication;
};



// IO Handler
class HdfsIODriver;

class HdfsIOHandler: public IOHandler{
public:

	HdfsIOHandler(HdfsIODriver* driver, const std::string& pfn,
			int flags) throw (DmException);
	~HdfsIOHandler();

	void   close(void) throw (DmException);
	size_t read (char* buffer, size_t count) throw (DmException);
	size_t write(const char* buffer, size_t count) throw (DmException);
	void   seek (off_t offset, Whence whence) throw (DmException);
	off_t  tell (void) throw (DmException);
	void   flush(void) throw (DmException);
	bool   eof  (void) throw (DmException);
        size_t pread(void* buffer, size_t count, off_t offset) throw (DmException);
        struct stat fstat(void) throw (DmException);
        size_t writeToHDFS(const char* buffer, size_t count) throw (DmException);
        int    copyToHDFS(void) throw (DmException);
protected:
        pthread_mutex_t mtx_;
        class lk {
              public:
                lk(pthread_mutex_t *mp): mp(mp)
                 { int err; if (mp && (err=pthread_mutex_lock(mp)))
                     throw DmException(err, "Could not lock a mutex"); }
                ~lk()
                  { int err; if (mp && (err=pthread_mutex_unlock(mp)))
                    throw DmException(err, "Could not unlock a mutex"); }
              private:
                pthread_mutex_t *mp;
            };

private:
	HdfsIODriver* driver;
        hdfsFS fs;
	hdfsFile file;  // Hdfs file descriptor
	bool     isEof; // Set to true if end of the file is reached
	std::string path;
	std::string tmpFolder;// temporary folder
	bool isWriting; //set for writing operations;
        int  temp_fd; //file descriptor of the tmp file used to buffer write requests
        char temp_path[PATH_MAX];
	

};



/// IO Driver
class HdfsIODriver: public IODriver {
public:
	HdfsIODriver(const std::string&, unsigned, const std::string&,
			const std::string&, bool, const std::string&,  unsigned replication);
	~HdfsIODriver();

	std::string getImplId() const throw();

	void setSecurityContext(const SecurityContext*) throw (DmException);

        void setStackInstance(StackInstance* si) throw (DmException);

	IOHandler *createIOHandler(const std::string& pfn,
			int flags,
			const Extensible& extras, mode_t mode) throw (DmException);

	void doneWriting(const Location& loc) throw (DmException);
private:
	friend class HdfsIOHandler;

        StackInstance* si_;

        std::string nameNode;
	unsigned    port;
        std::string uname;
	std::string tokenPasswd;
	bool        tokenUseIp;
	std::string userId;
	std::string tmpFolder;
        unsigned replication;
	void updateReplica(hdfsFS fs, std::string& final) throw (DmException);

};



/// IO Factory
class HdfsFactory: public IODriverFactory, public PoolDriverFactory{
public:
	HdfsFactory() throw (DmException);

	void configure(const std::string& key,
			const std::string& value) throw (DmException);

	std::string implementedPool() throw();
	PoolDriver* createPoolDriver() throw (DmException);

	IODriver* createIODriver(PluginManager*) throw (DmException);

private:
	std::string nameNode;
	unsigned    port;
	std::string uname;
	std::vector<std::string> gateways;
	std::string tmpFolder;
	std::string tokenPasswd;
	bool        tokenUseIp;
	unsigned    tokenLife;
        unsigned    replication;
	
};

void ThrowExceptionFromErrno(int err, const char* extra = 0x00) throw(DmException);
int   wrapCall(int   ret) throw (DmException);
void* wrapCall(void* ret) throw (DmException);

class HDFSUtil {

public:
	HDFSUtil();
	~HDFSUtil();
	static void setClasspath(std::string basefolder) throw ();
        static void setLibraryPath(std::string baseFolder) throw ();
	static std::string getRandomGateway(const std::vector<std::string>& gateways) throw();
	static int mkdirs(const char *dir) throw ();
        static std::string trim(std::string& str) throw ();

};


};

#endif	// HDFS_H
