/*
FUSE: Filesystem in Userspace
Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

This program can be distributed under the terms of the GNU GPL.
See the file COPYING.

gcc -Wall client.c `pkg-config fuse --cflags --libs` -o client
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
//We need to write these header functions (that implement network calls to server)
#include <networkfunc.h>

static const char *client_str = "Hello World!\n";
static const char *client_path = "/client";
static const char *server_ip = NULL;

static int client_getattr(const char *path, struct stat *stbuf)
{

	return network_getattr(server_ip,path,stbuf);
}

static int client_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	return network_readdir(server_ip, path, buf, filler, offset, fi);
}

static int client_open(const char *path, struct fuse_file_info *fi)
{
	return network_open(server_ip, path, fi);
}

static int client_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	return network_read(server_ip,path,buf,size,offset,fi);
}

static struct fuse_operations client_oper = {
	.getattr	= client_getattr,
	.readdir	= client_readdir,
	.open		= client_open,
	.read		= client_read,
};

int main(int argc, char *argv[])
{
	
	return fuse_main(argc, argv, &client_oper, NULL);
}
