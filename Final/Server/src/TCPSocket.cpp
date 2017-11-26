/*
 * TCPSocket.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: anon
 */

#include "TCPSocket.h"



TCPSocket::TCPSocket()
{
    enabled = true;
    isServer = false;

    sd = SD_UNDEFINED;

    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {
        remoteSD[i] = SD_UNDEFINED;
        remoteLen[i] = 0;

    }
    nextRemote = 0;
};

TCPSocket::~TCPSocket()
{
};

bool TCPSocket::connectSocket(int port)
{
    isServer = true;

    if(isFull()
        || ! openSocket()
        || ! bindSocket(port)
        || ! listenSocket()
        || ! acceptSocket())
        return false;


    return true;
};

bool TCPSocket::connectSocket(int port, const char* server)
{
    isServer = false;


    if(!openSocket())
        return false;


    struct hostent *destination = gethostbyname(server);


    bzero((char*)&localAddr, sizeof(localAddr));
    localAddr.sin_port = htons(port);
    localAddr.sin_family = AF_INET;
    bcopy((char*)destination->h_addr, (char*)&localAddr.sin_addr.s_addr, destination->h_length);


    if(connect(sd, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0)
    {
        cerr << "Failed to create TCP connection " << endl;
        return false;
    }

    return true;
};

bool TCPSocket::openSocket()
{
    if((sd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
    {
          cerr << "Cannot open a TCP socket." << endl;
          return false;
    }

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&enabled, sizeof(int));
    return true;
};

bool TCPSocket::bindSocket(int port)
{
    // Bind our local address
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(port);
    localAddr.sin_family = AF_INET;
    bzero((char*) &localAddr.sin_zero, sizeof(localAddr));

    if(bind(sd, (struct sockaddr *) &localAddr, sizeof(localAddr)) < 0)
    {
        cerr << "Cannot bind while opening TCP socket "<< endl;
        return false;
    }

    return true;
};

bool TCPSocket::listenSocket()
{
    if(listen(sd, MAX_CONNECTIONS) < 0)
    {
        cerr << "Cannot to TCP socket " << endl;
        return false;
    }

    return true;
};

bool TCPSocket::isFull()
{
    if(nextRemote >= MAX_CONNECTIONS)
    {
        cout << "Maximum conections reached." << endl;
        return false;
    }

    return true;
};

bool TCPSocket::acceptSocket()
{
    remoteLen[nextRemote] = sizeof(remoteAddr[nextRemote]);
    remoteSD[nextRemote] = accept(sd, (struct sockaddr *) &remoteAddr[nextRemote], &remoteLen[nextRemote]);

    if(remoteSD[nextRemote] < 0)
    {
        cerr << "Cannot to accept socket " << endl;
        return false;
    }

    nextRemote++;
    return true;
};

