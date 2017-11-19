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


#define DEBUG true




const int MAX_TESTID = 4;
const int MIN_TESTID = 1;
const int MIN_PORT = 0;
const int MAX_PORT = 65535;

const int MAX_UDP_PAYLOAD = 1472;

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

void threewayHandshake(int packet[]);
void nagelsAlgorithm();
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
        nagelsAlgorithm();
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

const int SYN    = 0;
const int ACK    = 1;
const int SYNACK = 2;
const int END    = 3;
const int ENDACK = 4;

const int SeqNumIndex = 0;
const int FlagIndex = 1;
const int MAX_PACKETS = 10000;

//const int MAX_ATTEMPTS = 100;

void threewayHandshake(int packet[])
{
    //create the socket
    UdpSocket sock(port);


    bool ackTimedOut = false;
    //bool pendingInput = true;
    //int attemptCount = MAX_ATTEMPTS;
    int timeoutLength = 250; //timeout in microseconds
    Timer stopwatch;
    struct timeval tv;
    tv.tv_usec = 1;
    int quitRequest;
    bool quitting = false;


    int seqNum = 0;

    //init the array
    for(int i = 0; i < MAX_UDP_PAYLOAD/sizeof(int); i++)
        packet[i] = -1;


    if(isSender)
    {
        cout << "acting as sender, sending SYN" << endl;


        if(!sock.setDestAddress(destAddress))
        {
            //set the destination address
            cout << "!sock.setDestAddress(" << destAddress << ")" << endl;
            return;
        }

        seqNum = 0;

        packet[SeqNumIndex] = seqNum;
        packet[FlagIndex] = SYN;


        cout << "\tseqNum " << seqNum << "; SYN " << SYN << endl;
        cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;
        sock.sendTo((char*)packet, sizeof(&packet));
    }
    else
    {
        cout << "acting as receiver " << endl;
        cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;

        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
        }
    }


    while(true) //attemptCount > 0)
    {
        sock.recvFrom((char*)packet, sizeof(&packet));

        //attemptCount = MAX_ATTEMPTS;


        //read the data from the packet
        if(packet[SeqNumIndex] == SYN && packet[FlagIndex] == SYN)
        {
            cout << "Replying to handshake, sending SYNACK" << endl;
            //start of the handshake
            seqNum = ++packet[SeqNumIndex];
            packet[SeqNumIndex] = seqNum;
            packet[FlagIndex] = SYNACK;

            //receiver reply to handshake
            cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;
            sock.sendTo((char*)packet, sizeof(&packet));
            continue;
        }


        if(packet[SeqNumIndex] == seqNum +1)
        {
            seqNum = ++packet[SeqNumIndex];
            packet[SeqNumIndex] = seqNum;


            if(packet[FlagIndex] == SYNACK)
            {
                cout << "Finishing handshake, sending ACK " << endl;
                cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;
                packet[FlagIndex] = ACK;
            }
            else if(packet[FlagIndex] == ACK)
            {
                if(seqNum == MAX_PACKETS)
                {
                    cout << "Start ending handshake " << endl;
                    cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;
                    packet[FlagIndex] = END;
                }
                else
                {
                    cout << "regular traffic " << endl;
                    cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;
                    packet[FlagIndex] = SYN;
                }
            }
            else if(packet[FlagIndex] == END)
            {
                cout << "Finish ending handshake " <<endl;
                cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;
                packet[FlagIndex] = ENDACK;

                sock.sendTo((char*)packet, sizeof(&packet));
                sock.sendTo((char*)packet, sizeof(&packet));
                return;
            }

            else if(packet[FlagIndex] == ENDACK)
            {
                return;
            }


            sock.sendTo((char*)packet, sizeof(&packet));
        }


        stopwatch.start();
        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << "\tfailed to receive packet within timeout " << endl;
                ackTimedOut = true;
                break;
            }
        }

        if(ackTimedOut)
        {
            cout << "\tresending previous packet " << endl;
            cout << "\tpacket[SeqNumIndex]" << packet[SeqNumIndex] << "; packet[FlagIndex]" << packet[FlagIndex] << endl;
            sock.sendTo((char*)packet, sizeof(&packet));

            //attemptCount--;
            ackTimedOut = false;
            continue;
        }
    }
}



void nagelsAlgorithm()
{
    fd_set rfds;
        struct timeval tv;
        int retval;

       /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

       /* Wait up to five seconds. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;

       retval = select(1, &rfds, NULL, NULL, &tv);
        /* Don't rely on the value of tv now! */

       if (retval == -1)
            perror("select()");
        else if (retval)
            printf("Data is available now.\n");
            /* FD_ISSET(0, &rfds) will be true. */
        else
            printf("No data within five seconds.\n");

       exit(EXIT_SUCCESS);
}
void udpDelayedAck()
{

}
void tcpGetName()
{

}








/*
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


void printDebug()
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
    //Semaphore sendWindowNotFull;
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

*/




