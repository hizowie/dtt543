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

#define DEBUG true


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
static const char * error_notEnoughArguments = "Not enough arguments. ";
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

int getTimeoutLength(const char* input)
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

int getWindowSize(const char* input)
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



int main(int argc, char* argv[])
{
    if(DEBUG)
    {
        for(int i = 0; i < argc; i++)
           cout << "argv[" << i << "]: "<< argv[i]<< endl;

        cout << endl << endl;
    }

    //input:
        //[0]: int port; required
        //[1]: string //set the destination addressmachineNamargce; required
        //[2]: int testID; optional
        //[3]: int timeoutLength; optional
        //[4]: int windowSize; optional


    //input sanitation
    if(argc < 2)
    {
        if(DEBUG)
        {
            cout << "argc < 2; argc = " << argc << endl << endl;
        }

        cout << error_invalidInput << error_notEnoughArguments << endl;
        cout << instructions << endl;
        return 0;
    }

    //check port number
    if (atoi(argv[1]) < 0 || atoi(argv[1]) > 65535)
    {
        if(DEBUG)
        {
            cout << "atoi(argv[1]) < 0 || atoi(argv[1]) > 65535; atoi(argv[1]) = " << atoi(argv[1]) << endl << endl;
        }

        cout << error_invalidInput << error_portNumber;
        return 0;
    }

    //create an udpsocket with the given port
    UdpSocket sock(atoi(argv[1]));


    //set the destination address
    if(!sock.setDestAddress(argv[2]))
    {
        cout << "!sock.setDestAddress(argv[1])" <<endl;
        cout << error_invalidInput << error_machineName << endl;
        return 0;
    }


    //local variable creation
    int testID, timeoutLength, windowSize;
    Timer stopwatch;
    int transmission[MAX_UDP_PAYLOAD/sizeof(int)];


    //test if given commandline arguments
    if(argc > 2)
    {
        //get test ID
        testID = getTestID(argv[3]);
        if(testID == -1)
            return 0;

        //test stop-and-wait testID
        if(testID == 1)
        {
            //stop-and-wait requires at least 4 arguments
            if(argc < 4)
            {
                if(DEBUG)
                {
                    cout << "argc < 4; argc = " << argc << endl << endl;
                }

                cout << error_invalidInput << error_requiredOptions_Stop << endl;
                return 0;
            }

            //get timeoutLength
            timeoutLength = getTimeoutLength(argv[4]);
            if(timeoutLength == -1)
                return 0;
        }


        //test sliding-window testID
        if(testID == 2)
        {
            //sliding-window requires at least 5 arguments
            if(argc < 5)
            {
                if(DEBUG)
                {
                    cout << "argc < 5; argc = " << argc << endl << endl;
                }

                cout << error_invalidInput << error_requiredOptions_Sliding << endl;
                return 0;
            }

            //get timeoutLength
            timeoutLength = getTimeoutLength(argv[4]);
            if(timeoutLength == -1)
                return 0;

            //get windowSize
            windowSize = getWindowSize(argv[5]);
            if(windowSize == -1)
                return 0;
        }
    }
    else
    {
        //otherwise, use default values
        testID = 1;
        timeoutLength = getTimeoutLength("1");
    }


    //start the timer
    stopwatch.start();

    //delegate testing
    switch(testID)
    {
    case 2:
        sendGoBackN(sock, transmission, TOTAL_PACKETS, timeoutLength, windowSize);
        cout << "Total Time: " << stopwatch.lap() << endl;
        break;
    case 3:
        sendSelectiveRepeat(sock, transmission, TOTAL_PACKETS, timeoutLength, windowSize);
        cout << "Total Time: " << stopwatch.lap() << endl;
        break;
    case 1:
    default:
        sendStopAndWait(sock, transmission, TOTAL_PACKETS, timeoutLength);
        cout << "Total Time: " << stopwatch.lap() << endl;
        break;
    }
}



void sendStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength)
{
    //create local variables
    Timer stopwatch;
    bool ackTimedOut = false;

    //start sending data
    for(int i = 0; i < sendCount; i++)
    {
        transmission[0] = i;
        sock.sendTo((char*)transmission, sizeof(&transmission));
        
        stopwatch.start();
        
        //poll for ACKs
        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
            
            //check if ack timed out
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << error_ACKTimeout << msg_ACKResend << (i+1) << endl;
                ackTimedOut = true;
                break;
            }
        }

        //check flag for failure
        if(ackTimedOut)
        {
            //step back one to resend
            i--;
            
            //reset flag
            ackTimedOut = false;
            continue;
        }

        //poll succeeded, receive ack
        sock.recvFrom((char*)transmission, sizeof(&transmission));
        cout << msg_packetSYN << (i + 1) << msg_packetACKTime << stopwatch.lap() << endl;
    }
}


void sendGoBackN(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength, const int windowSize)
{
    Timer stopwatch;
    int seqNum = 0;

    int pendingACKs = 0;
    bool ackTimedOut = false;

    while(seqNum < sendCount)
    {
        //bulk-send packets
        while(pendingACKs < windowSize)
        {
            transmission[0] = seqNum;
            sock.sendTo((char*)transmission, sizeof(&transmission));
            cout << msg_packetSent << (seqNum + 1) << endl;
            seqNum++;
            pendingACKs++;
        }

        //bulk-receive ACKs
        if(sock.pollRecvFrom() > 0)
        {
            sock.recvFrom((char*)transmission, sizeof(&transmission));
            ackTimedOut = false;
            pendingACKs--;
            continue;
        }

        stopwatch.start();
        
        //poll for ACKs
        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
            
            //check if timed out
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << error_ACKTimeout << msg_ACKResend << (seqNum + 1) << endl;
                ackTimedOut = true;
                break;
            }
        }

        //check flag for failure
        if(ackTimedOut)
        {
            //step back one to resend
            seqNum--;
            
            //reset flag
            ackTimedOut = false;
            continue;
        }
    }
}


void sendSelectiveRepeat(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength,  int windowSize)
{
    Timer stopwatch;
    bool ackTimedOut = false;


    int lastFrameAckd = 0;
    int lastFrameSent = 0;
    int seqNum = 0;
    //int pendingAcks = 0;

    windowSize = windowSize /100;

    //init window
    vector<int> window(windowSize);
    for(int i = 0; i < windowSize; i++)
    {
        window[i] = -1;
    }

    while(seqNum < sendCount)
    {

        //bulk-send packets
        while(lastFrameSent - lastFrameAckd < windowSize)
        {

            if(window[lastFrameSent % windowSize] > -1)
            {
                lastFrameSent++;
                seqNum = lastFrameSent;
                continue;
            }
            else
            {
                cout << "\tnot skipping " << lastFrameSent << endl;
            }



            transmission[0] = lastFrameSent;
            sock.sendTo((char*)transmission, sizeof(&transmission));
            cout << msg_packetSent << (seqNum + 1) << endl;
            lastFrameSent++;
            seqNum = lastFrameSent;
            //pendingAcks++;

            //usleep(100);

        }

        cout << "\tend window send" <<endl;

        //bulk-receive ACKs
        while(sock.pollRecvFrom() > 0)// && pendingAcks > 0 )
        {
            sock.recvFrom((char*)transmission, sizeof(&transmission));


            int index = *transmission % windowSize;

            cout << "\t\tPackage rec'd: " << *transmission << endl;

            if(index == lastFrameAckd)
            {

                //accept the ack
                window[lastFrameAckd] = index;

                //move the marker
                lastFrameAckd++;
                cout << "\t\t\tNext package Acked: " << *transmission << endl;
                while(window[lastFrameAckd] > -1)
                {
                    window[lastFrameAckd] = -1;
                    lastFrameAckd++;
                    lastFrameAckd = lastFrameAckd % windowSize;
                }
            }
            else
            {
                cout << "\t\t\t\tOther package acked: " << *transmission << endl;
                window[index] = *transmission;
            }

            //pendingAcks--;
            continue;
        }

        cout << "\tstart stopwatch" << endl;
        stopwatch.start();

        //poll for ACKs
        if(sock.pollRecvFrom() <= 0)
        {
            usleep(1);

            //check if timed out
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << error_ACKTimeout << msg_ACKResend << (seqNum + 1) << endl;
                ackTimedOut = true;
                break;
            }
        }


        cout << "\tcheck timeout" <<endl;
        if(ackTimedOut)
        {
            //reset flag
            ackTimedOut = false;
            cout << "\t\t\tack timed out." << endl;

            //lastFrameSent = lastFrameAckd;
            /*
            int counter = pendingAcks;

            for(int i = lastFrameAckd; i < lastFrameSent; i++)
            {
                if(window[lastFrameSent % windowSize] == -1)
                {
                    transmission[0] = i;
                    sock.sendTo((char*)transmission, sizeof(&transmission));
                    cout << msg_packetSent << (i + 1) << endl;
                    counter--;
                }

                if(counter == 0)
                {
                    i = lastFrameSent;
                }
            }

            /*
            //check the space in the windows frames
            for(int i = lastFrameAckd; i < seqNum; i++)
            {
                if(window[i] != -1)
                {
                    seqNum = i;
                    cout << endl << "Setting Sequence Number to " << i << endl << endl;
                    //sleep(1);
                    break;
                }
            }
            */
        }

        if(lastFrameAckd+1 == sendCount)
        {
            cout << "lastFrameAckd +1 == sendCount" <<endl;
            break;
        }
/*
        //check flag for failure
        if(ackTimedOut)
        {
            //reset flag
            ackTimedOut = false;

            //check the space in the windows frames
            for(int i = lastFrameAckd; i < seqNum; i++)
            {
                if(window[i] != -1)
                {
                    seqNum = i;
                    cout << endl << "Setting Sequence Number to " << i << endl << endl;
                    sleep(1);
                    break;
                }
            }
        }

        if(seqNum + 1 == sendCount)
        {
            break;
        }
        */

        /*

        //get position within the window
        int seqNum = lastFrameRec % windowSize;



        //check index is before last ACK or outside window
        if(seqNum < lastAckFrame || seqNum >= windowSize)
        {
            //previous ack failed, go-back-n
            sock.ackTo((char*) &lastSeq, sizeof(int));
            continue;
        }

        //check if index is next packet
        if(seqNum == lastAckFrame)
        {
            //accept packet list
            window[seqNum] = lastFrameRec;

            //update the sequence number
            lastSeq = lastFrameRec;

            //iterate over the list, looking for the next non-negative-one value
            while(window[lastAckFrame] > -1)
            {
                //update the sequence index
                lastSeq = window[lastAckFrame];

                //flag window as un-ACK'ed
                window[lastAckFrame] = -1;

                //move ack index
                lastAckFrame++;

                //get ack index within window
                lastAckFrame = lastAckFrame % windowSize;

                //check if window has been ack'ed
                if(window[lastAckFrame] == -1)
                {
                    //window hasn't been ack'ed, notify sender
                    sock.ackTo((char*) &lastSeq, sizeof(int));
                }
            }
        }
        else
        {
            //packet is in window but not next, save for later
            window[seqNum] = lastFrameRec;
            sock.ackTo((char*) &lastSeq, sizeof(int));
        }
        */

    }
}


