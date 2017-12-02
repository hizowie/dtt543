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
#include <ctype.h>


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
        //bool connectSocket(const int port);
        bool connectSocket(const int port, const char* server);
        bool connectSocket(const int port, const string server);
        const char* createGETRequest(const string url);
        const char* createGETRequest(const char* url);
        bool sendRequest(const char * msg);
        bool sendRequest(string msg);
        bool recV();
        string readHTML();
        string receiveReply();

        char* readURL(const char* URL, long &bytesReturnedOut, char* headerOut[]);
        //char* readURL(string URL, long &bytesReturnedOut, char* headerOut[]);

    protected:
        int getSocketDescriptor();
        int* getClientSD();

    private:
        bool DEBUG;


        char buffer[1024];
        int enabled;
        int sd;
        int remoteSD;

        struct sockaddr_in server;
        //struct sockaddr_in localAddr;
        //struct sockaddr_in remoteAddr;
        //struct sockaddr srcAddr;
        socklen_t remoteLen;

        //bool isFull();
        //bool openSocket();
        //bool bindSocket(int port);
        //bool listenSocket();
        //bool acceptSocket();
        //bool sendTo(char msg[], int len);
        //bool recvFrom(char msg[], int len);
        bool pollRecvFrom();
        //bool ackTo(char msg[], int len);

        void parseURL(string URL);
        void parseURL(const char* URL);

        int getHeaderLength(string content);
        int getHeaderLength(char* content);


        long bytesReturned;
        char* header[];


        string servername;
        string filepath;
        string filename;



};

#endif /* SRC_TCPSOCKET_H_ */
