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
#include <math.h>

//#include "helper.h"

using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <vector>

#include <cstdlib>
#include <unistd.h>
#include <limits.h>

#define DEBUG true


const int SYN    = 0;
const int ACK    = 1;
const int SYNACK = 2;
const int LEN    = 3;
const int END    = 4;
const int ENDACK = 5;

const int SeqNumIndex = 0;
const int FlagIndex = 1;
const int LenIndex = 2;

const int MAX_TESTID = 4;
const int MIN_TESTID = 1;
const int MIN_PORT = 0;
const int MAX_PORT = 65535;
const int MAX_PACKETS = 10000;

const int MAX_UDP_PAYLOAD = 1472/sizeof(int);

void printMainArguments(int argc, char* argv[])
{
    cout << "main() argc = " << argc << endl;

    for(int i = 0; i < argc; i++)
        cout << "argv[" << i << "] = " << argv[i] << endl;

    cout << endl;
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

void printPacketStats(int packet[], int seqNum)
{
    cout << "\t\tseqNum = "<< seqNum << "; packet[SeqNumIndex] " << packet[SeqNumIndex] << "; packet[FlagIndex] " << packet[FlagIndex] << endl;
}

void threewayHandshake(int packet[]);
void nagelsSender();
void nagelsReceiver();
void udpDelayedAck();
void tcpGetName();

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


    int packet[MAX_UDP_PAYLOAD/sizeof(int)];

    switch(testId)
    {
    case 2:
        if(isSender)
            nagelsSender();
        else
            nagelsReceiver();
        break;
    case 3:
        udpDelayedAck();
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
    bool isConnected = false;

    bool ackTimedOut = false;

    int timeoutLength = 15000000; //timeout in microseconds
    Timer stopwatch;
    bool isASyn = false;


    int seqNum = 0;

    //init the array
    for(int i = 0; i < MAX_UDP_PAYLOAD/sizeof(int); i++)
        packet[i] = -1;



    if(isSender)
    {
        cout << "Acting as sender" << endl;


        if(!sock.setDestAddress(destAddress))
        {
            //set the destination address
            cout << "!sock.setDestAddress(" << destAddress << ")" << endl;
            return;
        }
        isConnected = true;

        seqNum = 0;

        packet[SeqNumIndex] = seqNum;
        packet[FlagIndex] = SYN;
        isASyn = true;

        cout << "\tHandshake Step 1: Sender-to-Receiver SYN - [Begin]" << endl;

        if(DEBUG)
            printPacketStats (packet, seqNum);

        sock.sendTo((char*)packet, sizeof(&packet));

    }
    else
    {
        cout << "Acting as receiver " << endl;

        if(DEBUG)
            printPacketStats (packet, seqNum);

        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
        }
    }


    while(true) //attemptCount > 0)
    {

        stopwatch.start();
        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
            if(stopwatch.lap() >= timeoutLength && isConnected)
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
                printPacketStats (packet, seqNum);

            if(isASyn)
                sock.sendTo((char*)packet, sizeof(&packet));
            else
                sock.ackTo((char*)packet, sizeof(&packet));

            ackTimedOut = false;
            continue;
        }

        sock.recvFrom((char*)packet, sizeof(&packet));
        isConnected = true;

        if(DEBUG)
            printPacketStats (packet, seqNum);

        if(packet[FlagIndex] == ENDACK)
        {
            return;
        }

        //read the data from the packet
        if(packet[SeqNumIndex] == SYN && packet[FlagIndex] == SYN)
        {
            cout << "\tHandshake Step 1: Sender-to-Receiver SYN - [End]" << endl;

            seqNum = ++packet[SeqNumIndex];
            packet[SeqNumIndex] = seqNum;
            packet[FlagIndex] = SYNACK;

            isASyn = false;

            cout << "\tHandshake Step 2) Receiver-to-Sender SYNACK - [Begin]" << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum);

            sock.ackTo((char*)packet, sizeof(&packet));
            continue;
        }






        if(packet[FlagIndex] == SYNACK)
        {
            cout << "\tHandshake Step 2) Receiver-to-Sender SYNACK - [End]" << endl;

            seqNum = ++packet[SeqNumIndex];
            packet[SeqNumIndex] = seqNum;


            packet[FlagIndex] = ACK;
            isASyn = false;

            cout << "\tHandshake Step 3) Sender-to-Receiver ACK - [Begin]" << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum);

            sock.sendTo((char*)packet, sizeof(&packet));
            continue;
        }


        if(packet[FlagIndex] == ACK && packet[SeqNumIndex] == seqNum+1)
        {
            if(isSender)
            {
                cout << "Handshake complete "<< endl;
                return;
            }

            cout << "\tHandshake Step 3) Receiver-to-Sender ACK - [End]" << endl;

            seqNum = ++packet[SeqNumIndex];
            packet[SeqNumIndex] = seqNum;
            packet[FlagIndex] = ACK;

            isASyn = false;

            cout << "Handshake complete" << endl;

            if(DEBUG)
                printPacketStats (packet, seqNum);

            sock.ackTo((char*)packet, sizeof(&packet));

            return;
        }
    }
}




void nagelsReceiver()
{
    UdpSocket sock(port);
    Timer stopwatch;
    int timeoutLength = 10000; //timeout in microseconds
    bool ackTimedOut = false;
    bool isConnected = false;
    int seqNum = 0;


    vector<int> buffer;
    buffer.push_back(0);
    buffer.push_back(0);
    buffer.push_back(0);

    int * packet = &buffer[0];

    //int packet[MAX_UDP_PAYLOAD];

    cout << "Acting as receiver " << endl;


    while(seqNum < MAX_PACKETS)
    {

        stopwatch.start();
        while(sock.pollRecvFrom() <= 0 && isConnected)
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

            //if(DEBUG)
                //printPacketStats (packet, seqNum);


            sock.ackTo((char*)packet, sizeof(&packet));

            ackTimedOut = false;
            continue;
        }

        sock.recvFrom((char*)packet, sizeof(&packet));
        isConnected = true;

        cout << "\t\tp[SeqNumIndex] = " << packet[SeqNumIndex] << "; p[FlagIndex] = " << packet[FlagIndex] <<"; p[LenIndex] = " << packet[LenIndex] << endl;
        cout << "\t\tseqNum = " << seqNum << endl << endl;

        seqNum += packet[LenIndex];
        packet[SeqNumIndex] = seqNum;
        cout << "\t\tseqNum = " << seqNum << endl;
        cout << "\t\tp[SeqNumIndex] = " << packet[SeqNumIndex] << "; p[FlagIndex] = " << packet[FlagIndex] <<"; p[LenIndex] = " << packet[LenIndex] << endl;

        return;




        if(packet[SeqNumIndex] == seqNum && packet[LenIndex] == SYN)
        {
            cout << "\tSeqNum = " << seqNum << "; Length = " << packet[LenIndex] << endl;

            seqNum += packet[LenIndex];
            packet[SeqNumIndex] = seqNum;
            packet[FlagIndex] = ACK;

            sock.ackTo((char*)packet, sizeof(&packet));
        }
    }
}



void nagelsSender()
{
    UdpSocket sock(port);
    Timer stopwatch;
    int sendTimeout = 10000; //timeout in microseconds
    int recTimeout = 12000;

    bool ackTimedOut = false;
    bool isConnected = false;
    int seqNum = 0;
    int len = 1;

    vector<int> packet1;
    vector<int> packet2;
    int* sender;
    vector<int> *buffer;


    bool packet1Sending;

    cout << "Acting as sender" << endl;

    if(!sock.setDestAddress(destAddress))
    {
        //set the destination address
        cout << "!sock.setDestAddress(" << destAddress << ")" << endl;
        return;
    }


    //create the initial packet
    packet1.push_back(seqNum);
    packet1.push_back(SYN);
    packet1.push_back(len);
    packet1.push_back(1);


    sender = &packet1[0];
    buffer = &packet1;

    cout << "\t\ts[SeqNumIndex] = " << sender[SeqNumIndex] << "; s[FlagIndex] = " << sender[FlagIndex] <<"; s[LenIndex] = " << sender[LenIndex] << endl;
    cout << "\t\tb(SeqNumIndex) = " << buffer->at(SeqNumIndex) << "; b(FlagIndex) = " << buffer->at(FlagIndex) <<"; b(LenIndex) = " << buffer->at(LenIndex) << endl;


    packet1Sending = true;
    //sock.sendTo((char*)sender, sizeof(&sender));
    sock.sendTo((char*)buffer, sizeof(&buffer));


    return;

    buffer = &packet2;

    buffer->push_back(seqNum + len);
    buffer->push_back(SYN);
    buffer->push_back(len);

    int number;

    while (seqNum < MAX_PACKETS)
    {
        stopwatch.start();
        while(sock.pollRecvFrom() <= 0)
        {
            usleep(100);
            if(stopwatch.lap() < sendTimeout && len < MAX_UDP_PAYLOAD && len + seqNum < MAX_PACKETS)
            {

                buffer->push_back(-10+rand()*(10));
                len++;

            }
            else
            {
                buffer->at(LenIndex) = len;
                cout << "len = " << len << endl;
            }

            if(stopwatch.lap() > recTimeout)
            {
                ackTimedOut = true;
                break;
            }
        }

        cout << "\t\ts[SeqNumIndex] = " << sender[SeqNumIndex] << "; s[FlagIndex] = " << sender[FlagIndex] <<"; s[LenIndex] = " << sender[LenIndex] << endl;
        cout << "\t\tp1[SeqNumIndex] = " << packet1[SeqNumIndex] << "; p1[FlagIndex] = " << packet1[FlagIndex] <<"; p1[LenIndex] = " << packet1[LenIndex] << endl;
        cout << "\t\tp2[SeqNumIndex] = " << packet2[SeqNumIndex] << "; p2[FlagIndex] = " << packet2[FlagIndex] <<"; p2[LenIndex] = " << packet2[LenIndex] << endl;

        if(ackTimedOut)
        {
            sock.sendTo((char*)sender, sizeof(&sender));
            continue;
        }


        sock.recvFrom((char*)sender, sizeof(&sender));

        if(sender[SeqNumIndex] > seqNum)
        {
            seqNum = sender[seqNum];

            cout << "\t\ts[SeqNumIndex] = " << sender[SeqNumIndex] << "; s[FlagIndex] = " << sender[FlagIndex] <<"; s[LenIndex] = " << sender[LenIndex] << endl;

            cout << "\tnew seqNum = " << seqNum << endl;


            if(packet1Sending)
            {
                sender = &packet2[0];
                buffer = &packet1;
                packet1Sending = false;
            }
            else
            {
                sender = &packet1[0];
                buffer = &packet2;
                packet1Sending = true;
            }


            sock.sendTo((char*)sender, sizeof(&sender));
        }
    }
}



void udpDelayedAck()
{

}
void tcpGetName()
{

}




