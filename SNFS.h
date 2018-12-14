#ifndef _SNFS_h
#define _SNFS_h

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#define PORT 16555

// define enums
#define CREATE      1
#define OPEN        2
#define FLUSH       3
#define RELEASE     4
#define TRUNCATE    5
#define GETATTR     6
#define READ        7
#define WRITE       8
#define OPENDIR     9
#define READDIR     10
#define RELEASEDIR  11
#define MKDIR       12
#define RMDIR	    13
#define PING        14
typedef struct rpcCall{
	int procedure;
	char path[1024];
	int flags;
	int fd;
	size_t size;
	off_t offset;
	mode_t mode;
} rpcCall;
typedef struct rpcRecv{
	int retval;
	int err;
	int dataLen;
	int dataSize;
	void* dataArray;
} rpcRecv;
#endif
