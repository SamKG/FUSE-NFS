#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "SNFS.h"

using namespace std;

void connection_handler(int sock){
    char message[256];
    int m;
    recv(sock, &message, sizeof(message), 0);

    printf("Server received message: %s\n", message);

    m = stoi(message);
    if(m < 0){
        return;
    }
    sprintf(message, "%d", m + 1);

    send(sock, &message, sizeof(message), 0);

    close(sock);
}

int main(int argc, char** argv){
    // define variables
    int s_sock;                             // socket descriptor for server socket
    int c_sock = 0;                         // descriptor for client sockets
    int err;                                // error status
    struct sockaddr_in server_addr;         // address of server
    struct sockaddr_in client_addr;         // address of client
    vector<thread> threads;                 // stores created threads
    int sin_size = sizeof(sockaddr_in);     // size of socket address, used in accept()
    fd_set accept_fd;                       // for use in select
    struct timeval timeout;                 // allows setting a timeout period

    // create server socket
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(s_sock < 0){
        printf("Error creating server socket, error%d\n", errno);
    }

    // set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind server socket to address and port
    err = bind(s_sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (bind < 0){
        printf("Error binding server, error%d\n", errno);
    }

    // listen for connections
    err = listen(s_sock, 10);
    if(err < 0){
        printf("Error listening, error%d\n", errno);
    }

    // accept connections
    while(1){
        // set a 10 second timeout
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        FD_ZERO(&accept_fd);
        FD_SET(s_sock, &accept_fd);
        err = select(s_sock + 1, &accept_fd, NULL, NULL, &timeout);

        if((err == 1) && (FD_ISSET(s_sock, &accept_fd))){
            c_sock = accept(s_sock, (struct sockaddr*) &client_addr, (socklen_t*) &sin_size);
            threads.push_back(thread(&connection_handler, c_sock));
        }
        else{
            cout << "Accept timeout\n";
            break;
        }
    }
    if(c_sock < 0){
        printf("Error accepting client, error%d\n", errno);
    }
    for(auto &t: threads){
        t.join();
    }

    // close server socket
    close(s_sock);

    return 0;
}