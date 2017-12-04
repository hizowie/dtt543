/*
 * TCPSocket.h
 *
 *  Created on: Nov 24, 2017
 *      Author: anon
 */

#ifndef SRC_HTTPCLIENT_H_
#define SRC_HTTPCLIENT_H_


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include <cstdlib>
#include <ctype.h>
#include <sys/time.h>
#include <vector>
#include "md5.h"


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

enum REQUEST { GET, HEAD, OPTIONS,POST, PUT, DELETE, TRACE, CONNECT, PATCH, SAVE, NONE };

class HTTPClient
{
    public:
        HTTPClient();
        ~HTTPClient();


        bool submitRequest(REQUEST req);

        void printResponse();
        void printBody();
        void printHeader();

        void saveResponse();

        void setServerName(string name);
        void setFilePath(string path);



    private:
        //debug stuff
        bool DEBUG;
        int debugIndentLength;
        string getDebugIndent();
        string dIndent;

        //socket stuff
        int enabled;
        int sd;
        struct sockaddr_in serverAddr;
        socklen_t serverLen;

        //request stuff
        REQUEST currReq;
        string servername;
        string filepath;
        string filename;
        string request;
        size_t requestLen;
        vector<string> parameters;
        vector<string> pvalues;
        string reqContent;


        //response stuff
        long headerLength;
        long bodyLength;
        string responseHeader;
        string responseBody;



        //request methods
        bool sendRequest();
        void getAddress();

        void parseURL(string URL);
        void parseURL(const char* URL);

        bool connectSocket();

        bool buildRequest();


        void getPutParameters();
        void buildPutContent();


        void getPostParameters();
        void buildPostContent();


        long getHeaderLength(string content);
        long getHeaderLength(char* content);


        bool getResponse();


        void reset();

        string intToString(int number);





};

#endif /* SRC_HTTPCLIENT_H_ */


