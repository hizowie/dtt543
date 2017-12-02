/*
 * TCPSocket.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: anon
 */

#include "TCPSocket.h"


TCPSocket::TCPSocket()
{
    DEBUG = true;

    if(DEBUG)
        cout << "TCPSocket::TCPSocket()" << endl;

    this->enabled = true;
    this->isServer = false;

    this->sd = SD_UNDEFINED;

    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {
        this->remoteSD = SD_UNDEFINED;
        this->remoteLen = 0;

    }
};

TCPSocket::~TCPSocket()
{
};

bool TCPSocket::connectSocket(int port)
{
    if(DEBUG)
        cout << "TCPSocket::connectSocket(" << port << ")" << endl;

    this->isServer = true;

    if(!openSocket()
        || !bindSocket(port)
        || !listenSocket()
        || !acceptSocket())
    {
        return false;
    }


    return true;
};


bool TCPSocket::connectSocket(int port, const char* server)
{
    if(DEBUG)
        cout << "TCPSocket::connectSocket(" << port << ", " << server << ")" << endl;

    isServer = false;


    if(!openSocket())
        return false;


    struct hostent *destination = gethostbyname(server);


    bzero((char*)&(this->localAddr), sizeof(this->localAddr));
    this->localAddr.sin_port = htons(port);
    this->localAddr.sin_family = AF_INET;
    bcopy((char*)destination->h_addr, (char*)&(this->localAddr).sin_addr.s_addr, destination->h_length);


    if(connect(this->sd, (struct sockaddr*)&(this->localAddr), sizeof(this->localAddr)) < 0)
    {
        cerr << "Failed to create TCP connection " << endl;
        return false;
    }

    return true;
};

bool TCPSocket::isFull()
{
    if(DEBUG)
    {
        cout << "TCPSocket::isFull()" << endl;
        //cout << "\tnextRemote = " << nextRemote <<endl;
        cout << "\tMAX_CONNECTIONS = " << MAX_CONNECTIONS << endl;
    }

    //if(this->nextRemote < MAX_CONNECTIONS)
    //{
    //    return false;
    //}

    cout << "Maximum connections reached." << endl;
    return true;
};

bool TCPSocket::openSocket()
{
    if(DEBUG)
        cout << "TCPSocket::openSocket()" << endl;

    if((this->sd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
    {
          cerr << "Cannot open a TCP socket." << endl;
          return false;
    }

    setsockopt(this->sd, SOL_SOCKET, SO_REUSEADDR, (char*)&(this->enabled), sizeof(int));
    return true;
};

bool TCPSocket::bindSocket(int port)
{
    if(DEBUG)
        cout << "TCPSocket::bindSocket(" << port << ")" << endl;

    // Bind our local address
    this->localAddr.sin_addr.s_addr = INADDR_ANY;
    this->localAddr.sin_port = htons(port);
    this->localAddr.sin_family = AF_INET;
    bzero((char*) &(this->localAddr).sin_zero, sizeof(this->localAddr));

    if(bind(this->sd, (struct sockaddr *) &(this->localAddr), sizeof(this->localAddr)) < 0)
    {
        cerr << "Cannot bind while opening TCP socket "<< endl;
        return false;
    }

    return true;
};

bool TCPSocket::listenSocket()
{
    if(DEBUG)
        cout << "TCPSocket::listenSocket()" << endl;

    if(listen(this->sd, MAX_CONNECTIONS) < 0)
    {
        cerr << "Cannot listen to TCP socket " << endl;
        return false;
    }

    return true;
};

bool TCPSocket::acceptSocket()
{
    if(DEBUG)
        cout << "TCPSocket::acceptSocket()" << endl;

    this->remoteLen = sizeof(this->remoteAddr);
    this->remoteSD = accept(this->sd, (struct sockaddr *) &(this->remoteAddr), &(this->remoteLen));

    if(this->remoteSD < 0)
    {
        cerr << "Cannot to accept socket " << endl;
        return false;
    }

    return true;
};


bool TCPSocket::sendTo(char msg[], int len)
{
    return (sendto(this->sd, msg, len, 0, (sockaddr *)&(this->remoteAddr), sizeof(remoteAddr))) > 0;
}



bool TCPSocket::recvFrom(char msg[], int len)
{
    socklen_t addrlen = sizeof(this->srcAddr);
    bzero((char *)&this->srcAddr, sizeof(this->srcAddr) );

    // return the number of bytes received
    return (recvfrom(this->sd, msg, len, 0, &(this->srcAddr), &addrlen)) > 0;
}

bool TCPSocket::pollRecvFrom()
{
    struct pollfd pfd[1];
    pfd[0].fd = this->sd;       //declare I'm checking data availability of SD
    pfd[0].events = POLLRDNORM; //declare only interested in reading from SD

    // check now; poll returns 0 or -1 if no data is available
    return poll(pfd, 1, 0) > 0;
}

bool TCPSocket::ackTo(char msg[], int len)
{
    return (sendto(this->sd, msg, len, 0, &srcAddr, sizeof(srcAddr))) > 0;
}




/*

TCPSocket::TCPSocket()
{
    DEBUG = true;

    if(DEBUG)
        cout << "TCPSocket::TCPSocket()" << endl;

    this->enabled = true;
    this->isServer = false;

    this->sd = SD_UNDEFINED;

    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {
        this->remoteSD[i] = SD_UNDEFINED;
        this->remoteLen[i] = 0;

    }
    this->nextRemote = 0;
};

TCPSocket::~TCPSocket()
{
};

bool TCPSocket::connectSocket(int port)
{
    if(DEBUG)
        cout << "TCPSocket::connectSocket(" << port << ")" << endl;

    this->isServer = true;

    if(isFull()
        || !openSocket()
        || !bindSocket(port)
        || !listenSocket()
        || !acceptSocket())
    {
        return false;
    }


    return true;
};


bool TCPSocket::connectSocket(int port, const char* server)
{
    if(DEBUG)
        cout << "TCPSocket::connectSocket(" << port << ", " << server << ")" << endl;

    isServer = false;


    if(!openSocket())
        return false;


    struct hostent *destination = gethostbyname(server);


    bzero((char*)&(this->localAddr), sizeof(this->localAddr));
    this->localAddr.sin_port = htons(port);
    this->localAddr.sin_family = AF_INET;
    bcopy((char*)destination->h_addr, (char*)&(this->localAddr).sin_addr.s_addr, destination->h_length);


    if(connect(this->sd, (struct sockaddr*)&(this->localAddr), sizeof(this->localAddr)) < 0)
    {
        cerr << "Failed to create TCP connection " << endl;
        return false;
    }

    return true;
};

bool TCPSocket::isFull()
{
    if(DEBUG)
    {
        cout << "TCPSocket::isFull()" << endl;
        cout << "\tnextRemote = " << nextRemote <<endl;
        cout << "\tMAX_CONNECTIONS = " << MAX_CONNECTIONS << endl;
    }

    if(this->nextRemote < MAX_CONNECTIONS)
    {
        return false;
    }

    cout << "Maximum connections reached." << endl;
    return true;
};

bool TCPSocket::openSocket()
{
    if(DEBUG)
        cout << "TCPSocket::openSocket()" << endl;

    if((this->sd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
    {
          cerr << "Cannot open a TCP socket." << endl;
          return false;
    }

    setsockopt(this->sd, SOL_SOCKET, SO_REUSEADDR, (char*)&(this->enabled), sizeof(int));
    return true;
};

bool TCPSocket::bindSocket(int port)
{
    if(DEBUG)
        cout << "TCPSocket::bindSocket(" << port << ")" << endl;

    // Bind our local address
    this->localAddr.sin_addr.s_addr = INADDR_ANY;
    this->localAddr.sin_port = htons(port);
    this->localAddr.sin_family = AF_INET;
    bzero((char*) &(this->localAddr.sin_zero), sizeof(this->localAddr));

    if(bind(this->sd, (struct sockaddr *) &(this->localAddr), sizeof(this->localAddr)) < 0)
    {
        cerr << "Cannot bind while opening TCP socket "<< endl;
        return false;
    }

    return true;
};

bool TCPSocket::listenSocket()
{
    if(DEBUG)
        cout << "TCPSocket::listenSocket()" << endl;

    if(listen(this->sd, MAX_CONNECTIONS) < 0)
    {
        cerr << "Cannot listen to TCP socket " << endl;
        return false;
    }

    return true;
};



bool TCPSocket::acceptSocket()
{
    if(DEBUG)
        cout << "TCPSocket::acceptSocket()" << endl;

    this->remoteLen[nextRemote] = sizeof(this->remoteAddr[this->nextRemote]);
    this->remoteSD[nextRemote] = accept(this->sd, (struct sockaddr *) &(this->remoteAddr[this->nextRemote]), &(this->remoteLen[this->nextRemote]));

    if(this->remoteSD[nextRemote] < 0)
    {
        cerr << "Cannot to accept socket " << endl;
        return false;
    }

    this->nextRemote++;
    return true;
};
*/
