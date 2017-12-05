/*
 * TCPSocket.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: hcatlin
 */


#include "TCPSocket.h"


TCPSocket::TCPSocket(int port)
{
	this->port = port;
	this->clientFD = -1;
	this->serverFD = -1;
}
TCPSocket::~TCPSocket()
{
	if(clientFD != -1)
		close(clientFD);

	if(serverFD != -1)
		close(serverFD);
}

int TCPSocket::connectServer()
{
	if(serverFD == -1)
	{
		sockaddr_in myAddr;

		//build the socket
		if((serverFD = socket(AF_INET, SOCK_STREAM, 0 )) < 0 )
		{
			cerr << "Could not build the socket " << endl;
			return -1;
		}

		//set reuse options
		const int enabled = 1;
		if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, (char *)&enabled, sizeof(enabled) ) < 0)
		{
			cerr << "Could not set reuse option"  << endl;
			return -1;
		}

		// Bind our local address so that the client can send to us
		bzero((char*)&myAddr, sizeof(myAddr));
		myAddr.sin_family      = AF_INET; // Address Family Internet
		myAddr.sin_addr.s_addr = htonl( INADDR_ANY );
		myAddr.sin_port        = htons( port );


		if(bind(serverFD, (sockaddr*)&myAddr, sizeof(myAddr)) < 0 )
		{
			cerr << "Could not bind socket."  << endl;
			return -1;
		}

		listen(serverFD, 5);
	}

	// Read to accept new requests
	int connectionFD = -1;
	sockaddr_in destAddr;
	socklen_t newSockAddrSize = sizeof(destAddr);

	if((connectionFD = accept( serverFD, (sockaddr*)&destAddr, &newSockAddrSize)) < 0 )
	{
		cerr << "could not accept connection." << endl;
		return -1;
	}

	return connectionFD;
}

int TCPSocket::connectClient(char name[])
{
	struct hostent* host = gethostbyname(name);

	if(host == NULL)
	{
		cerr << "could not find host IP by name. " << endl;
		return -1;
	}

	  // Fill in the structure "sendSockAddr" with the address of the server.
	  sockaddr_in myAddr;
	  bzero((char*)&myAddr, sizeof(myAddr));
	  myAddr.sin_family = AF_INET; // Address Family Internet
	  myAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	  myAddr.sin_port = htons(port);


	  if((clientFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	  {
		  cerr << "could not open a client TCP socket." << endl;
		  return -1;
	  }

	  while(connect(clientFD, (sockaddr*)&myAddr, sizeof(myAddr)) < 0);

	  // Connected
	  return clientFD;
}
