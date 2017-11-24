//============================================================================
// Name        : HW2.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "Timer.h"
#include "UdpSocket.h"





#include <stdio.h>
#include <stdlib.h>


#include <cstdlib>



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



#define DEBUG true

using namespace std;

const int SYN    = 0;
const int ACK    = 1;
const int SYNACK = 2;


const int SeqNumIndex = 0;
const int FlagIndex = 2;
const int LenIndex = 1;


const int MAX_TESTID = 4;
const int MIN_TESTID = 1;
const int MIN_PORT = 0;
const int MAX_PORT = 65535;
const int MAX_PACKETS = 10000;

const int MAX_PAYLOAD_BITS = 1472;
const int MAX_UDP_PAYLOAD = MAX_PAYLOAD_BITS/sizeof(int);



void threewayHandshake(int packet[]);
void nagels(int packet[]);;
void udpDelayedAck(int packet[]);
void tcpGetName();


long getTimespan(timeval startTime);
void printMainArguments(int argc, char* argv[]);
bool isNumber(const char* input);
bool validateInput(int argc, char* argv[]);
void printPacketStats(int packet[], int seqNum, int iteration);



int port;
char* destAddress;
bool isSender;


int main(int argc, char* argv[])
{

    if(DEBUG)
        printMainArguments(argc, argv);

    if(!validateInput(argc, argv))
        return 0;

    int testId = atoi(argv[1]);
    port = atoi(argv[2]);

    if(argc > 3)
    {
        isSender = true;
        destAddress = argv[3];
    }
    else
    {
        isSender = false;
    }


    int packet[MAX_UDP_PAYLOAD];

    switch(testId)
    {
    case 2:
        nagels(packet);
        break;
    case 3:
        udpDelayedAck(packet);
        break;
    case 4:
        tcpGetName();
        break;
    case 1: //intentional fall-through
    default:
        threewayHandshake(packet);
        break;
    }
};


void threewayHandshake(int packet[])
{
    //create the socket
	UdpSocket sock(port);
    bool ackTimedOut = false;
    int timeoutLength = 15000000; //timeout in microseconds
    Timer stopwatch;
    int seqNum = 0;


    //init the array
    for(int i = 0; i < MAX_UDP_PAYLOAD; i++)
        packet[i] = -1;


	cout << "Acting as receiver " << endl;


	if(DEBUG)
		printPacketStats (packet, seqNum, 1);


    while(true)
    {
        stopwatch.start();
        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << "failed to receive packet within timeout " << endl;
                ackTimedOut = true;
                break;
            }
        }

        if(ackTimedOut)
        {
            cout << "resending previous packet " << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum, 2);

            sock.ackTo((char*)packet, sizeof(&packet));

            ackTimedOut = false;
            continue;
        }

        sock.recvFrom((char*)packet, sizeof(&packet));


        if(DEBUG)
            printPacketStats (packet, seqNum, 3);



        //read the data from the packet
        if(packet[SeqNumIndex] == SYN)
        {
            cout << "\tHandshake Step 1: Sender-to-Receiver SYN - [End]" << endl;

            packet[SeqNumIndex] = SYNACK;

            cout << "\tHandshake Step 2) Receiver-to-Sender SYNACK - [Begin]" << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum, 4);

            sock.ackTo((char*)packet, sizeof(&packet));
            continue;
        }


       if(packet[SeqNumIndex] == ACK)
       {
            cout << "\tHandshake Step 3) Receiver-to-Sender ACK - [End]" << endl;

            packet[SeqNumIndex] = ACK;

            cout << "Handshake complete" << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum, 5);

            sock.ackTo((char*)packet, sizeof(&packet));
            sock.ackTo((char*)packet, sizeof(&packet));

            return;
        }
    }
}





void nagels(int packet[])
{
    UdpSocket sock(port);
    Timer stopwatch;
    int timeoutLength = 50000; //timeout in microseconds
    bool ackTimedOut = false;
    int seqNum = 0;


    //init everything to negative 1
    for(int i = 0; i < MAX_UDP_PAYLOAD; i++)
        packet[i] = -1;



    cout << "Acting as receiver " << endl;

    //wait for the first packet
    while(sock.pollRecvFrom())
    {
        usleep(1);
    }



    //loop until we hit max packet sent
    while(packet[SeqNumIndex] - packet[LenIndex] < MAX_PACKETS)
    {
        sock.recvFrom((char*)packet, MAX_PAYLOAD_BITS);

        if(DEBUG)
        	cout << "\t\tseqNum = " << seqNum << endl;

        if(packet[SeqNumIndex] > seqNum || (packet[SeqNumIndex] == seqNum && packet[LenIndex] != 0))
        {
        	if(DEBUG)
        		printPacketStats (packet, seqNum, 1);


        	seqNum += packet[LenIndex];
        	packet[SeqNumIndex] = seqNum;


        	if(DEBUG)
        		printPacketStats (packet, seqNum, 2);


        	//packet[SeqNumIndex] = seqNum;
        	sock.ackTo((char*)packet, MAX_PAYLOAD_BITS);
			ackTimedOut = false;
		}
        else
        {
        	cout << "packet[SeqNumIndex] != seqNum; re-acking " << endl;

        	packet[SeqNumIndex] = seqNum;
        	sock.ackTo((char*)packet, MAX_PAYLOAD_BITS);
        }




    	//poll for packets
        stopwatch.start();
        while(sock.pollRecvFrom() > 0)
        {
            usleep(1);
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << "failed to receive packet within timeout " << endl;
                ackTimedOut = true;
                break;
            }
        }

        if(ackTimedOut)
        {
            cout << "resending previous packet " << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum, 3);


            sock.ackTo((char*)packet, MAX_PAYLOAD_BITS);

            ackTimedOut = false;
            continue;
        }
    }
}









void udpDelayedAck(int packet[])
{
    UdpSocket sock(port);
    Timer stopwatch;
    int timeoutLength = 35000; //timeout in microseconds
    bool ackTimedOut = false;
    int seqNum = 0;


    //init everything to negative 1
    for(int i = 0; i < MAX_UDP_PAYLOAD; i++)
        packet[i] = -1;



    cout << "Acting as receiver " << endl;

    //wait for the first packet
    while(sock.pollRecvFrom() <= 0)
    {
        usleep(1);
    }


    //loop until we hit max packet sent
    while(seqNum < MAX_PACKETS)
    {
    	sock.recvFrom((char*)packet, MAX_PAYLOAD_BITS);

    	if(DEBUG)
    		printPacketStats (packet, seqNum, 1);


		if(packet[SeqNumIndex] > seqNum)
			seqNum = packet[SeqNumIndex];

        stopwatch.start();

        while(stopwatch.lap() < timeoutLength)
        {
        	if(sock.pollRecvFrom() <= 0)
        	{
        		sleep(1);
        		continue;
        	}

        	sock.recvFrom((char*)packet, MAX_PAYLOAD_BITS);

    		if(packet[SeqNumIndex] > seqNum)
    		{
            	if(DEBUG)
            		printPacketStats (packet, seqNum, 2);


    			seqNum = packet[SeqNumIndex];
    		}
        }


        cout << "Acking to " << packet[SeqNumIndex] << endl;
        sock.ackTo((char*)packet, MAX_PAYLOAD_BITS);
    }
}


void tcpGetName()
{
    int sd, newSD;
    socklen_t destSockLen;
    const int enable = 1;

    struct sockaddr_in myAddr;
    struct sockaddr_in destAddr;



    if( ( sd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
          cerr << "Cannot open a TCP socket." << endl;
        }
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int));


	// Bind our local address
	myAddr.sin_addr.s_addr = INADDR_ANY;
	myAddr.sin_port = htons(port);
	myAddr.sin_family = AF_INET;
	bzero((char*) &myAddr.sin_zero, sizeof(myAddr));



    if(bind(sd, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
	{
    	cerr << "Cannot bind while opening TCP socket "<< endl;
	}


	listen(sd, 5);
	destSockLen = sizeof(destAddr);
	newSD = accept(sd, (struct sockaddr *) &destAddr, &destSockLen);


	socklen_t peerLen;


	peerLen = sizeof(destAddr);

	getpeername(newSD, (struct sockaddr*)&destAddr, &destSockLen);//&peerLen);


	cout << "IP address:\t" << inet_ntoa(destAddr.sin_addr)<< endl;

	cout << "Connected on Port:\t" << ntohs(destAddr.sin_port) << endl;
}





bool isNumber(const char* input)
{
    if((input != NULL && input[0] == '\0') || ((!isdigit(input[0]))&& (input[0] != '-') && (input[0] != '+')))
        return false ;

    char * p ;
    strtol(input, &p, 10) ;

    return (*p == 0);
}

bool validateInput(int argc, char* argv[])
{
    if(argc < 2)
    {
        cout << "main() needs at least 2 arguments: the testId and the portNumber" << endl;
        return false;
    }

    char* p;

    if(!(isNumber(argv[1])))
    {
        cout << "the first argument to main must be a valid testId" << endl;
        cout <<  "\tprovided argument: " << argv[1] <<endl;
        return false;
    }

    if(atoi(argv[1]) < MIN_TESTID)
    {
        cout << "the provided testId must be greater than " << (MIN_TESTID - 1) << endl;
        return false;
    }

    if(atoi(argv[1]) > MAX_TESTID)
    {
        cout << "the provided testId must be less than " << (MAX_TESTID + 1) << endl;
        return false;
    }


    if(!(isNumber(argv[2])))
    {
        cout << "the first argument to main must be a valid portNumber" << endl;
        cout <<  "\tprovided argument: " << argv[2] <<endl;
        return false;
    }

    if(atoi(argv[2]) < MIN_PORT)
    {
        cout << "the provided port must be greater than " << (MIN_PORT - 1) << endl;
        return false;
    }

    if(atoi(argv[2]) > MAX_PORT)
    {
        cout << "the provided port must be less than " << (MAX_PORT + 1) << endl;
        return false;
    }

    return true;
}

void printMainArguments(int argc, char* argv[])
{
    cout << "main() argc = " << argc << endl;

    for(int i = 0; i < argc; i++)
        cout << "argv[" << i << "] = " << argv[i] << endl;

    cout << endl;
}

void printPacketStats(int packet[], int seqNum, int iteration)
{
    cout << "\t\t[" << iteration << "]:: seqNum = "<< seqNum << "; p[SeqNumIndex] " << packet[SeqNumIndex] << "; p[LenIndex] " << packet[LenIndex] << endl;
}
