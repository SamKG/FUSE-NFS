// This file implements network functions for usage in FUSE 

#define FUSE_USE_VERSION 26
#define h_addr h_addr_list[0]
#include <stdio.h>
#include <fuse.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "SNFS.h"
// The job of the networkInfo struct is to store relevant data about the server (ie: address and port)
typedef struct networkInfo {
	int port;
	char* address;
} networkInfo;
// Implement network functions below

// sets up socket connection, returns socket descriptor
int connection_setup(const networkInfo* info);
rpcRecv network_create(const networkInfo*, const char*, mode_t);
rpcRecv network_open(const networkInfo*, const char*, const int);
rpcRecv network_getattr(const networkInfo*, const char*, struct stat*);
rpcRecv network_flush(const networkInfo* netinfo, const char* path);
rpcRecv network_release(const networkInfo* netinfo, const char* path);
rpcRecv network_truncate(const networkInfo* netinfo, const char* path, const int size);
rpcRecv network_read(const networkInfo* netinfo, const char* path, char* buff, size_t size, off_t offset);
rpcRecv network_write(const networkInfo* netinfo, const char* path, const char* buff, size_t size, off_t offset);
rpcRecv network_opendir(const networkInfo* netinfo, const char* path);
rpcRecv network_readdir(const networkInfo* netinfo, const char* path, void* buf, off_t offset);
rpcRecv network_releasedir(const networkInfo* netinfo, const char* path);
rpcRecv network_mkdir(const networkInfo* netinfo, const char* path,mode_t mode);
rpcRecv network_ping(const networkInfo* netinfo);

