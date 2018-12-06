// This file implements network functions for usage in FUSE 
//

// The job of the networkInfo struct is to store relevant data about the server (ie: address and port)
typedef struct networkInfo {
	int port,
	char* address
} networkInfo;



// Implement network functions below
//
