all: client
client: client.c networkfunc.c 
	gcc -Wall client.c networkfunc.c `pkg-config fuse --cflags --libs` -std=c11 -I.. -Wfatal-errors -o client
	#g++ clientSNFS.cpp -o client -pthread -std=c++11 -I..
clean:
	rm -rf client
