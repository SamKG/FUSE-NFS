all: client
client: client.c #clientSNFS.cpp
	gcc -Wall client.c `pkg-config fuse --cflags --libs` -I.. -o client
	#g++ clientSNFS.cpp -o client -pthread -std=c++11 -I..
clean:
	rm -rf client
