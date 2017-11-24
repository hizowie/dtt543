/*
#include "DataSocket.h"

DataSocket::DataSocket()//int port) : port(port), sd(NULL_SD)
{
	srcLen = sizeof(srcAddr);
	seqNum = 0;
	MSS = MSGSIZE;

/*
	// Open a UDP socket (a datagram socket )
	if( ( sd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		cerr << "Cannot open a UDP socket." << endl;
	}

	// Bind our local address
	bzero( (char*)&myAddr, sizeof( myAddr ) );    // Zero-initialize myAddr
	myAddr.sin_family      = AF_INET;             // Use address family internet
	myAddr.sin_addr.s_addr = htonl( INADDR_ANY ); // Receive from any addresses
	myAddr.sin_port        = htons( port );       // Set my socket port

	if( bind( sd, (sockaddr*)&myAddr, sizeof( myAddr ) ) < 0 )
	{
		cerr << "Cannot bind the local address to the UDP socket." << endl;
	}
	*/
/*
}

DataSocket::~DataSocket()
{
	if(sd != NULL_SD)
		close(sd);
}


bool DataSocket::setDestAddress(char ipName[])
{
	struct hostent* host = gethostbyname(ipName);
	if( host == NULL )
	{
		cerr << "Cannot find hostname." << endl;
		return false;                                // set in failure
	}

	// Fill in the structure "sendSockAddr" with this destination host entry
	bzero( (char*)&destAddr, sizeof( destAddr ) ); // zero-initialize
	destAddr.sin_family      = AF_INET;            // Use address family internet
	destAddr.sin_addr.s_addr =                     // set the destination IP addr
	inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
	destAddr.sin_port        = htons( port );      // set the destination port

	return true;                                   // set in success
}

bool DataSocket::pollRecvFrom()
{
	struct pollfd pfd[1];
	pfd[0].fd = sd;             // declare I'll check the data availability of sd
	pfd[0].events = POLLRDNORM; // declare I'm interested in only reading from sd

	// check it immediately; if poll() is > 0 it is readable
	// if not readable, it returns 0 or -1
	return poll( pfd, 1, 0 ) > 0;
}


// Send data packet  of length size through the sd socket
int DataSocket::sendTo(DataPacket* p, int packetLength)
{

  // return the number of bytes sent
  return sendto( sd, p, packetLength, 0, (sockaddr *)&destAddr,
		 sizeof( destAddr ) );
}


// Receive data through the sd socket and store it in msg[] of lenth size -----
int DataSocket::recvFrom(DataPacket* p, int packetLength)
{

  // zero-initialize the srcAddr structure so that it can be filled out with
  // the address of the source computer that has sent a packet of data

  socklen_t addrlen = sizeof( srcAddr );
  bzero( (char *)&srcAddr, sizeof( srcAddr ) );

  // return the number of bytes received
  return recvfrom( sd, p, packetLength, 0, &srcAddr, &addrlen );
}


int DataSocket::ackTo(DataPacket *p, int packetLength)
{
	// assume that srcAddress has be filled out upon the previous recvFrom( )
	// method.

	// return the number of bytes sent
	return sendto( sd, p, packetLength, 0, &srcAddr, sizeof( srcAddr ) );
}

void DataSocket::newDataPacket(char* data, unsigned short int payloadLength, DataPacket* newPacket)
{
   newPacket->flag = PAYLOAD;
   newPacket->number = seqNum++;

   newPacket->payload = data;
   newPacket->length = payloadLength;
}


void DataSocket::newStatePacket(FLAG state, DataPacket* newPacket)
{
    newPacket->flag = state;
    newPacket->number = ++seqNum;

    newPacket->payload = NULL;
    newPacket->length = 1;
}

void DataSocket::setDestAddr(sockaddr_in dest)
{
	destAddr = dest;

}

void DataSocket::setMyAddr(sockaddr_in myAdd)
{
	myAddr = myAdd;
}

void DataSocket::setSocketDescriptor(const int socketDescriptor)
{
	sd = socketDescriptor;
}

*/
