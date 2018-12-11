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
#include "SNFS.h"
// The job of the networkInfo struct is to store relevant data about the server (ie: address and port)
typedef struct networkInfo {
	int port;
	char* address;
} networkInfo;
// Implement network functions below

// sets up socket connection, returns socket descriptor
int connection_setup(networkInfo* info);
rpcRecv network_open(const networkInfo*, const char*, const int);
rpcRecv network_getattr(const networkInfo*, const char*);
rpcRecv network_read(const networkInfo*, const char*, char*, size_t, off_t, struct fuse_file_info*);
rpcRecv network_readdir(const networkInfo*, const char*, void*, fuse_fill_dir_t, off_t, struct fuse_file_info*);
