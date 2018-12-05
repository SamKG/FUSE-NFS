#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "SNFS.h"

using namespace std;

void socket_test(int id){
    // define variables
    int sock;                         // socket descriptor for network socket
    int err;                          // error status
    struct sockaddr_in server_addr;   // address of SNFS server
    
    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        printf("Error creating client socket, error%d\n", errno);
    }

    // specify server address for socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // connect to server
    err = connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(connect < 0){
        printf("Error connecting to server, error%d\n", errno);
    }
    
    char message[256];
    sprintf(message, "%d", id);
    send(sock, &message, sizeof(message), 0);
    recv(sock, &message, sizeof(message), 0);
    printf("%s\n", message);

    // close socket
    close(sock);
}

int main(int argc, char** argv){
    vector<thread> threads;
    for(int i = 5; i >= 0; i--){
        threads.push_back(thread(socket_test, i));
    }

    for(auto &t: threads){
        t.join();
    }

    return 0;
}