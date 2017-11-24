//============================================================================
// Name        : HW2.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "Timer.h"
#include "UdpSocket.h"



using namespace std;
#include <stdio.h>
#include <stdlib.h>


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
void nagels(int packet[]);
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
}


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

	cout << "Acting as sender" << endl;


	if(!sock.setDestAddress(destAddress))
	{
		//set the destination address
		cout << "!sock.setDestAddress(" << destAddress << ")" << endl;
		return;
	}

	cout << "\tHandshake Step 1: Sender-to-Receiver SYN - [Begin]" << endl;

	packet[SeqNumIndex] = seqNum;

	if(DEBUG)
		printPacketStats (packet, seqNum, 1);



	sock.sendTo((char*)packet, sizeof(&packet));


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


            sock.sendTo((char*)packet, sizeof(&packet));
            ackTimedOut = false;
            continue;
        }

        sock.recvFrom((char*)packet, sizeof(&packet));

        if(DEBUG)
            printPacketStats (packet, seqNum, 3);



        if(packet[SeqNumIndex] == SYNACK)
        {
            cout << "\tHandshake Step 2) Receiver-to-Sender SYNACK - [End]" << endl;

            packet[SeqNumIndex] = ACK;


            cout << "\tHandshake Step 3) Sender-to-Receiver ACK - [Begin]" << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum, 4);

            sock.sendTo((char*)packet, sizeof(&packet));
            continue;
        }


        if(packet[SeqNumIndex] == ACK)
        {
			cout << "Handshake complete "<< endl;
			return;
        }

    }

}


void nagels(int packet[])
{
    UdpSocket sock(port);
    Timer stopwatch;
    int timeoutLength = 125000;

    bool ackTimedOut = false;
    bool sendingBuf1 = true;

    int seqNum = 0;
    int packetLength = 0;
    

    int buf1[MAX_UDP_PAYLOAD];
    int buf2[MAX_UDP_PAYLOAD];


    for(int i = 0; i < MAX_UDP_PAYLOAD; i++)
    {
        buf1[i] = -1;
        buf2[i] = -1;
    }



    cout << "Acting as sender" << endl;

    if(!sock.setDestAddress(destAddress))
    {
    	cout << "testio" << endl;
        //set the destination address
        cerr << "!sock.setDestAddress(" << destAddress << ")" << endl;
        return;
    }

    timeval startTime;
    gettimeofday(&startTime, NULL);

    //create the initial packet
    buf1[SeqNumIndex] = seqNum;
    packetLength++;
    buf1[LenIndex] = packetLength;
    buf1[LenIndex + packetLength] =  -10+rand()*(10);

    buf2[SeqNumIndex] = seqNum;


    if(DEBUG)
    {
    	cout << "buffer 1:" << endl;
    	printPacketStats (buf1, seqNum, 1);
    	cout << sizeof(buf1) <<endl;

    	cout << "buffer2:" << endl;
    	printPacketStats (buf2, seqNum, 1);
        cout << sizeof(buf2) << endl;
    }



    sendingBuf1 = true;
    sock.sendTo((char*)buf1, MAX_PAYLOAD_BITS);

    buf2[SeqNumIndex] = seqNum;
    packetLength = 0;



    while (seqNum < MAX_PACKETS)
    {
        stopwatch.start();
        while(sock.pollRecvFrom() <= 0)
        {

			if(packetLength + 1 < MAX_UDP_PAYLOAD)
			{
				if(sendingBuf1)
				{
					++packetLength;
					buf2[LenIndex + packetLength] = 808;
				}
				else
				{
					++packetLength;
					buf1[LenIndex + packetLength] = 101;
				}
			}

            usleep(1);

            if(stopwatch.lap() > timeoutLength)
            {
                ackTimedOut = true;
                break;
            }
        }

        if(DEBUG)
        {
        	cout << "buffer 1:" << endl;
        	printPacketStats (buf1, seqNum, 2);
        	cout << sizeof(buf1) <<endl;

        	cout << "buffer2:" << endl;
        	printPacketStats (buf2, seqNum, 2);
            cout << sizeof(buf2) << endl;
        }


        if(ackTimedOut)
        {
        	cout << "ack timed out " << endl;

            if(sendingBuf1)
            {
                sock.sendTo((char*)buf1, MAX_PAYLOAD_BITS);
            }
            else
            {;
                sock.sendTo((char*)buf2, MAX_PAYLOAD_BITS);
            }

            ackTimedOut = false;
            continue;
        }


		if(sendingBuf1)
		{
			buf2[LenIndex] = packetLength;
		}
		else
		{
			buf1[LenIndex] = packetLength;
		}



		if(DEBUG)
			cout << "recv from " << endl;


        sock.recvFrom((char*)packet, sizeof(&packet));


        if(DEBUG)
        {
        	cout << "\tsending buffer 1 = " << sendingBuf1 << endl;
        	cout << "packet:" << endl;
        	printPacketStats (packet, seqNum, 3);

        	cout << "buffer 1:" << endl;
        	printPacketStats (buf1, seqNum, 3);

        	cout << "buffer2:" << endl;
        	printPacketStats (buf2, seqNum, 3);
        }









        if(packet[SeqNumIndex] >= seqNum)
        {

            seqNum = packet[SeqNumIndex];

            if(DEBUG)
			{
            	cout << "packet:" << endl;
            	printPacketStats (packet, seqNum, 4);
			}


            buf1[SeqNumIndex] = seqNum;
            buf2[SeqNumIndex] = seqNum;

            if(sendingBuf1)
            {
            	sendingBuf1 = false;
            	sock.sendTo((char*)buf2, MAX_PAYLOAD_BITS);
				packetLength = 0;
            }
            else
            {
            	sendingBuf1 = true;
            	sock.sendTo((char*)buf1, MAX_PAYLOAD_BITS);
            	packetLength = 0;
            }



        }
    }

    long totalTime = getTimespan(startTime);

    cout << "Total run time:" << totalTime << endl;
    cout << "Timeout: " << timeoutLength << endl;
}


void udpDelayedAck(int packet[])
{
    UdpSocket sock(port);
    Timer stopwatch;
    int timeoutLength = 50000;
    int windowSize = 10000;

    bool ackTimedOut = false;


    int seqNum = 0;
    int ackNumber = 0;


    cout << "Acting as sender" << endl;

    if(DEBUG)
    	cout << "size of bits " << MAX_PAYLOAD_BITS << endl;



    if(!sock.setDestAddress(destAddress))
    {

        //set the destination address
        cerr << "!sock.setDestAddress(" << destAddress << ")" << endl;
        return;
    }

    timeval startTime;
    gettimeofday(&startTime, NULL);

    //create the initial packet
    packet[SeqNumIndex] = seqNum;

    if(DEBUG)
    	printPacketStats (packet, seqNum, 1);


    //send the first packet to establish a connection
    sock.sendTo((char*)packet, MAX_PAYLOAD_BITS);


    while (ackNumber < MAX_PACKETS)
    {
        stopwatch.start();
        while(sock.pollRecvFrom() <= 0)
        {
        	usleep(1);
        	if(seqNum - ackNumber < windowSize)
        	{
        		packet[SeqNumIndex] = seqNum;
        		sock.sendTo((char*)packet, MAX_PAYLOAD_BITS);
        		seqNum++;
        	}


        	if(stopwatch.lap() >= timeoutLength)
        	{
        		ackTimedOut = true;
        		break;
        	}
        }


        if(ackTimedOut)
        {
        	cout << "Ack timed out, re-sending" << ackNumber << endl;
        	seqNum = ackNumber;
        	ackTimedOut = false;
        	continue;
        }

        sock.recvFrom((char*)packet, MAX_UDP_PAYLOAD * sizeof(int));

        if(DEBUG)
        	cout << "recvFrom " << packet[SeqNumIndex] << endl;

        if(packet[SeqNumIndex] > ackNumber)
        {

        	 cout << "setting new ackNumber : " << ackNumber << " -> " << packet[SeqNumIndex] << endl;

        	ackNumber = packet[SeqNumIndex];
        }
    }



    long totalTime = getTimespan(startTime);

    cout << "Total run time:" << totalTime << endl;
    cout << "\tWindow: " << windowSize << endl;
    cout << "Timeout: " << timeoutLength << endl;

}


void tcpGetName()
{
	int sd;
	const int enable = 1;
	struct sockaddr_in myAddr;

	struct hostent *destination;
	char data[256];

    // Open a TCP socket
    if( ( sd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
      cerr << "Cannot open a TCP socket." << endl;
    }

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int));

    destination = gethostbyname(destAddress);

    bzero((char*)&myAddr, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    bcopy((char*)destination->h_addr, (char*)&myAddr.sin_addr.s_addr, destination->h_length);
    myAddr.sin_port = htons(port);
    if(connect(sd, (struct sockaddr*)&myAddr, sizeof(myAddr))< 0)
    {
    	cerr << "Failed to create TCP connection " << endl;
    }


    if(read(sd, data, 255) > 0)
    	cout << "connection terminated by command from other side" << endl;

    sleep(5);
}



long getTimespan(timeval startTime)
{
	timeval endTime;
    gettimeofday(&endTime, NULL);
    long secs, micros;
    secs = (endTime.tv_sec - startTime.tv_sec)* 1000000;
    micros = endTime.tv_usec - startTime.tv_usec;

    return secs + micros;

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


