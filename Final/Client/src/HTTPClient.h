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

#include <cstdlib>
#include <ctype.h>
#include <sys/time.h>
#include <vector>


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

enum REQUEST { GET, HEAD, OPTIONS,POST, PUT, DELETE, TRACE, CONNECT, NONE };

class HTTPClient
{
    public:
        HTTPClient();
        ~HTTPClient();

        bool connectSocket(const char* server);//, const int port, );
        bool connectSocket(const string server);//, const int port, );
        const char* createGETRequest(const string url);
        const char* createGETRequest(const char* url);
        bool sendRequest(const char * msg);
        bool sendRequest(string msg);
        bool recV();

        //string readHTML();
        string receiveReply();

        void readHTML();//const char* URL, long &bytesReturnedOut, char* headerOut[]);
        //char* readHTML(string URL, long &bytesReturnedOut, char* headerOut[]);

        //long bytesReturned;
        char* replyHeader[];
        char* replyBody[];


        void printHTML();
        void printHeader();
        void getAddress();
        void setServerName(string name);
        void setFilePath(string path);
        REQUEST currReq;

    private:
        bool DEBUG;
        int debugIndentLength;
        string getDebugIndent();
        string dIndent;

        int enabled;
        int sd;
        //char buffer[1024];



        string servername;
        string filepath;
        string filename;

        struct sockaddr_in serverAddr;
        socklen_t serverLen;


        void parseURL(string URL);
        void parseURL(const char* URL);

        int getHeaderLength(string content);
        int getHeaderLength(char* content);

        bool pollRecvFrom();


        bool buildRequest();

        char* request;

        vector<string> parameters;
        vector<string> pvalues;

        void getPostParameters();
        void buildPostContent();
        string postContent;

        void getPutContent();
        void buildPutContent();
        string putContent;




};

#endif /* SRC_HTTPCLIENT_H_ */











/*




    totalBytesRead = 0;
    while(1)
    {
        memset(readBuffer, 0, bufSize);
        thisReadSize = recv (conn, readBuffer, bufSize, 0);

        if ( thisReadSize <= 0 )
            break;

        tmpResult = (char*)realloc(tmpResult, thisReadSize+totalBytesRead);

        memcpy(tmpResult+totalBytesRead, readBuffer, thisReadSize);
        totalBytesRead += thisReadSize;
    }

    headerLen = getHeaderLength(tmpResult);
    long contenLen = totalBytesRead-headerLen;
    result = new char[contenLen+1];
    memcpy(result, tmpResult+headerLen, contenLen);
    result[contenLen] = 0x0;
    char *myTmp;

    myTmp = new char[headerLen+1];
    strncpy(myTmp, tmpResult, headerLen);
    myTmp[headerLen] = NULL;
    delete(tmpResult);
    *headerOut = myTmp;

    bytesReturnedOut = contenLen;
    closesocket(conn);
    return(result);






























*/

