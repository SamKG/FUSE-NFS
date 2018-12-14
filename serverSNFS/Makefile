all: server
server: serverSNFS.cpp
	g++ serverSNFS.cpp hashtable.c -g -o server -pthread -std=c++11 -I ..
clean:
	rm -rf server
