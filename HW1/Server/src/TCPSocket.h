/*
 * TCPSocket.h
 *
 *  Created on: Dec 4, 2017
 *      Author: hcatlin
 */

#ifndef SRC_TCPSOCKET_H_
#define SRC_TCPSOCKET_H_

#include <iostream>      // cerr

using namespace std;

extern "C"
{
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // TCP_NODELAY
#include <stdlib.h>
#include <stdio.h>
}


class TCPSocket
{
public:
	TCPSocket(int port);
	~TCPSocket();
	int connectServer();
	int connectClient(char name[]);

private:
	int port;
	int clientFD;
	int serverFD;

};



#endif /* SRC_TCPSOCKET_H_ */
