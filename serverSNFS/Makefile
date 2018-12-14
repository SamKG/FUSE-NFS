all: serverSNFS
serverSNFS: serverSNFS.cpp
	g++ serverSNFS.cpp hashtable.c -g -o serverSNFS -pthread -std=c++11 -I ..
clean:
	rm -rf serverSNFS
