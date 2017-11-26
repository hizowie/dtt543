/*
 * TCPSocket.h
 *
 *  Created on: Nov 24, 2017
 *      Author: anon
 */

#ifndef SRC_TCPSOCKET_H_
#define SRC_TCPSOCKET_H_


#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <cstdlib>

using namespace std;

extern "C"
{
    #include <sys/types.h>    // for sockets
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>        // for gethostbyname( )
    #include <unistd.h>       // for close( )
    #include <string.h>       // for bzero( )
    #include <sys/poll.h>     // for poll( )
}

#define SD_UNDEFINED -2
#define MAX_CONNECTIONS 5


class TCPSocket
{
    public:
        TCPSocket();
        ~TCPSocket();
        bool connectSocket(const int port);
        bool connectSocket(const int port, const char* server);

    protected:
        int getSocketDescriptor();
        int* getClientSD();

    private:
        bool isServer;

        int enabled;
        int sd;
        int remoteSD[];
        int nextRemote;

        struct sockaddr_in localAddr;
        struct sockaddr_in remoteAddr[];
        socklen_t remoteLen[];

        bool isFull();
        bool openSocket();
        bool bindSocket(int port);
        bool listenSocket();
        bool acceptSocket();
};


#endif /* SRC_TCPSOCKET_H_ */
