all: clientSNFS
clientSNFS: client.c networkfunc.c 
	gcc -Wall client.c networkfunc.c `pkg-config fuse --cflags --libs` -std=c11 -g -I.. -Wfatal-errors -Wno-unused-variable -o clientSNFS
clean:
	rm -rf clientSNFS
