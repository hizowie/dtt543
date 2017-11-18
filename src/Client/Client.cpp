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

#define TOTAL_PACKETS 20000
#define MAX_UDP_PAYLOAD 1472

#define DEBUG false


static const char* error_ACKTimeout = "Packet ACK Timeout.";
static const char* msg_ACKSend = "Sending Ack # ";
static const char* msg_ACKResend = "Resending Ack # ";
static const char* msg_packetSent = "Packet sent: ";
static const char* msg_packetSYN = "Packet SYN :";
static const char* msg_packetACKTime = " ACK Time: ";

static const char* instructions = "This Client accepts the following input:\n"
							"\n\tportNumber - the port to listen on (required)"
							"\n\tmachineName - the name of the receiving computer (required)"
							"\n\ttestID - the test to execute (optional)"
							"\n\t\t1 - Stop-and-Wait"
                            "\n\t\t2 - Sliding Window: Go-Back-N"
                            "\n\t\t2 - Sliding Window: Selective Repeat"
							"\n\ttimeoutLength - the amount of time before the window times out (optional)"
							"\n\t\t1 - small  (200 nanseconds)"
							"\n\t\t2 - medium (1 microsecond)"
							"\n\t\t3 - large  (200 microseconds)"
							"\n\twindowSize - the size of the window in the Sliding Window algorithm (optional)"
							"\n\t\t1 - small  (100 packets)"
							"\n\t\t2 - medium (1,000 packets)"
							"\n\t\t3 - large  (10,000 packets)";


static const char * error_invalidInput = "Invalid Input: ";
static const char * error_notEnoughArguments = ;
static const char * error_portNumber = "Port number must be between 0 and 65535";
static const char * error_machineName = "Could not establish connection, check machineName.";
static const char * error_testID = "Invalid testID selected.";
static const char * error_windowSize = "Invalid windowSize selected.";
static const char * error_timeoutLength = "Invalid timeoutLength selected.";
static const char * error_requiredOptions_Server = "When a testID is specified, timeoutLength is a required parameter.";
static const char * error_requiredOptions_Stop = "When testID '1' (Stop-and-Wait) is specified, timeoutLength is a required parameter.";
static const char * error_requiredOptions_Sliding = "When testID '2' (Sliding Window) is specified, timeoutLength and windowSize are required parameters.";




int getTestID(const char* input)
{
    int userSelection = atoi(input);
    if(userSelection < 1 || userSelection > 3)
    {
        if(DEBUG)
        {
            cout << "userSelection < 1 || userSelection > 3" << endl;
            cout << "userSeletion = "<< userSelection << endl;
        }

        cout << error_invalidInput << error_testID << endl;
        cout << instructions << endl;
        return -1;
    }

    return userSelection;
};

int getTimeoutLength(const char int testID, const char* input)
{
    int userSelection = atoi(input);
    if(userSelection < 1 || userSelection > 3)
    {
        if(DEBUG)
        {
            cout << "userSelection < 1 || userSelection > 3" << endl;
            cout << "userSeletion = "<< userSelection << endl;
        }

        cout << "userSelection < 1 || userSelection > 3" <<endl;
        cout << error_invalidInput << error_timeoutLength << endl;
        cout << instructions << endl;
        return -1;
    }

    switch(userSelection)
    {
    case 1: return 200;
    case 2: return 1000;
    case 3:
    default: return 200000;
    }
};

int getWindowSize(const char int testId, const char* input)
{
    int userSelection = atoi(input);
    if(userSelection < 1 || userSelection > 3)
    {
        if(DEBUG)
        {
            cout << "userSelection < 1 || userSelection > 3" << endl;
            cout << "userSeletion = "<< userSelection << endl;
        }

        cout << error_invalidInput << error_windowSize << endl;
        cout << instructions << endl;
        return -1;
    }

    return pow(10, (userSelection + 1));
};



void sendStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength );
void sendSelectiveRepeat(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength, const int windowSize);
void sendGoBackN(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength, const int windowSize);


int getPort(const char* input);

int getPort(const char* input)
{
    int output = atoi(atoi(input));
        
    //check port number
    if (output < 0 || output > 65535)
    {
        if(DEBUG)
        {
            cout << "atoi(input) < 0 || atoi(input) > 65535; atoi(input) = " << output << endl << endl;
        }

        cout << error_invalidInput << error_portNumber;
        return output;
    }
    
    return -1
}


int getAddress(const char* input);

int getAddress(const char* input)
{
    
}


void printDebug
{
    for(int i = 0; i < argc; i++)
    {
        cout << "argv[" << i << "]:\t"<< argv[i]<< endl;
    }
    
    cout << endl << endl;
}


bool checkArguments(int argc, char* argv[])
{
    //input sanitation
    if(argc < 2)
    {
        if(DEBUG)
        {
            cout << "argc < 2; argc = " << argc << endl << endl;
        }

        cout << error_invalidInput << "Not enough arguments. " << endl;
        cout << instructions << endl;
        return false;
    }

    
    //test if given commandline arguments
    if(argc > 2)
    {
        //get test ID
        testID = getTestID(argv[3]);
        if(testID == -1)
            return false;

        //test stop-and-wait testID
        


        //test sliding-window testID
        if(testID == 2 || testID == 3)
        {
            //sliding-window requires at least 5 arguments
            if(argc < 5)
            {
                if(DEBUG)
                {
                    cout << "argc < 5; argc = " << argc << endl << endl;
                }

                cout << error_invalidInput << error_requiredOptions_Sliding << endl;
                return false;
            }

            //get timeoutLength
            timeoutLength = getTimeoutLength(argv[4]);
            if(timeoutLength == -1)
                return false;

            //get windowSize
            windowSize = getWindowSize(argv[5]);
            if(windowSize == -1)
                return false;
        }
    }
    else
    {
        //otherwise, use default values
        testID = 1;
        timeoutLength = getTimeoutLength("1");
    }
    
    return true;
}

typedef struct 
{
    u_char SeqNum; 
    u_char AckNum;
    u_char Flags;
} infoheader;

typedef struct 
{
    struct sendQ
    {
        Event timeout; 
        Msg msg;
    }sendQ[SMS];
    
    struct recQ
    {
        int received;
        Msg msg;
    }reqS[RWS];
    
    u_char lastAckRecd;
    u_char lastFrameSent;
    u_char nextFrameExpected;
    Semaphone sendWindowNotFull;
    infoheader header;
    
} infopacket;

int main(int argc, char* argv[])
{
    //input:
        //[0]: int port; required
        //[1]: string //set the destination addressmachineNamargce; required
        //[2]: int testID; optional
        //[3]: int timeoutLength; optional
        //[4]: int windowSize; optional

    if(DEBUG)
        printDebug();
    

    if(!checkArguments(argc, &argv))
        return 0;
    

    //check port number
    int port = getPort(argv[1]);
    if(port == -1) 
        return 0;
  
    
    //create an udpsocket with the given port
    UdpSocket sock(port);


    //set the destination address
    if(!sock.setDestAddress(argv[2]))
    {
        cout << "!sock.setDestAddress(argv[1])" <<endl;
        cout << error_invalidInput << error_machineName << endl;
        return 0;
    }

    
    //handshake();
    sock.recvFrom((char*) transmission, sizeof(&transmission));
        if(transmission[0] == i)
        {
            cout << msg_ACKSend << (i+1) << endl;
            sock.ackTo((char*) transmission, sizeof(&transmission));
            ackTimedOut = false;
            continue;
        }

        stopwatch.start();

        //poll for ACKs
        while(sock.pollRecvFrom() <= 0)
        {
        }
}

void sendHandshake(const int timeoutLength, )
{
    bool ackTimedOut = false;
    
    infopacket p;
    u_char seqNum = 0x0;
    
    p.header.SeqNum = seqNum; 
    p.header.Flags = seqNum;

    int attemptCount = 10;
    
    
    while(attemptCount > 0)
    {
        sock.sendTo((infopacket*)p, sizeof(&p1));
        
        Timer stopwatch; 
        stopwatch.start();
        while(sock.pollRecvFrom() <= 0 && )
        {
            usleep(1);
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << error_ACKTimeout << msg_ACKResend << (i+1) << endl;
                ackTimedOut = true;
                break; 
            }
        }
        
        if(ackTimedOut)
        {
            attemptCount--;
            ackTimedOut = false;
            continue;
        }
        
        sock.recvFrom((infopacket*)p, sizeof(&p));
        if(p.header.SeqNum == seqNum + 1 && p.header.Flags == seqNum + 2)
        {
            seq++;
            p.header.SeqNum = seq;
            p.header.Flags = seq;
        }
    }
    
}






