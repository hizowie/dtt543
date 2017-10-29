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



int receiveStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength );
int receiveSlidingWindow(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength, const int windowSize);


const char* instructions = "This Server accepts the following input:\n"
                            "\n\tportNumber - the port to listen on (required)"
                            "\n\ttimeoutLength - the amount of time before the window times out (optional)"
                            "\n\t\t1 - small  (200 nanseconds)"
                            "\n\t\t2 - medium (1 microsecond)"
                            "\n\t\t3 - large  (200 microseconds)";
/*
                            "\n\tmachineName - the name of the receiving computer (required)"
                            "\n\ttestID - the test to execute (optional)"
                            "\n\t\t1 - Stop-and-Wait"
                            "\n\t\t2 - Sliding Window"

                            "\n\twindowSize - the size of the window in the Sliding Window algorithm (optional)"
                            "\n\t\t1 - small  (100 packets)"
                            "\n\t\t2 - medium (1,000 packets)"
                            "\n\t\t3 - large  (10,000 packets)";
*/

const char * error_invalidInput = "Invalid Input: ";
const char * error_notEnoughArguments = "Not enough arguments. ";
const char * error_portNumber = "Port number must be between 0 and 65535";
/*
const char * error_machineName = "Could not establish connection, check machineName.";
const char * error_testID = "Invalid testID selected.";
const char * error_windowSize = "Invalid windowSize selected.";
const char * error_timeoutLength = "Invalid timeoutLength selected.";
const char * error_requiredOptions_Stop = "When testID '1' (Stop-and-Wait) is specified, timeoutLength is a required parameter.";
const char * error_requiredOptions_Sliding = "When testID '2' (Sliding Window) is specified, timeoutLength and windowSize are required parameters.";
*/

#define TOTAL_PACKETS 20000
#define MAX_UDP_PAYLOAD 1472

const char* error_ACKTimeout = "Packet ACK Timeout.";
const char* msg_ACKSend = "Sending Ack # ";
const char* msg_ACKResend = "Resending Ack # ";
const char* msg_packetTime = "Packet sent: ";

int getWindowSize(int userSelection);

int main(int argc, char* argv[])
{
    //input:
        //[0]: int port; required
        //[1]: string //set the destination addressmachineNamargce; required
        //[2]: int testID; optional
        //[3]: int timeoutLength; optional
        //[4]: int windowSize; optional


    //input sanitation
    if(argc < 1)
    {
        cout << error_invalidInput << error_notEnoughArguments << endl;
        cout << instructions << endl;
        return 0;
    }

    //check port number
    if (atoi(argv[0]) < 0 || atoi(argv[0]) > 65535)
    {
        cout << error_invalidInput << error_portNumber;
        return 0;
    }

    //create an udpsocket with the given port
    UdpSocket sock(atoi(argv[0]));



    //local variable creation
    Timer stopwatch;
    int transmission[MAX_UDP_PAYLOAD/sizeof(int)];



/*

    if(argc > 1)
    {
        int tID = atoi(argv[1]);

        if(tID < 1 || tID > 2)
        {
            cout << error_invalidInput << error_testID << endl;
            return 0;
        }

        int tLen, wSize;

        if(tID == 1)
        {
            if(argc < 2)
            {
                cout << error_invalidInput << error_requiredOptions_Stop << endl;
                return 0;
            }

            tLen = atoi(argv[3]);
            if(tLen > 0 || tLen < 4)
            {
                timeoutLength = tLen;
            }
            else
            {
                cout << error_invalidInput << error_timeoutLength << endl;
                cout << instructions << endl;
                return 0;
            }

        }


        if(tID == 2)
        {
            if(argc < 3)
            {
                cout << error_invalidInput << error_requiredOptions_Sliding << endl;
                return 0;
            }

            tLen = atoi(argv[3]);
            if(tLen > 0 || tLen < 4)
            {
                timeoutLength = tLen;
            }
            else
            {
                cout << error_invalidInput << error_timeoutLength << endl;
                cout << instructions << endl;
                return 0;
            }

            wSize = atoi(argv[4]);
            if(wSize > 0 || wSize < 4)
            {
                windowSize = pow(10, (wSize + 1));
            }
            else
            {
                cout << error_invalidInput << error_windowSize << endl;
                cout << instructions << endl;
                return 0;
            }
        }




    }
    else
    {
        //otherwise, use default values
        testID = 1;
        timeoutLength = 1;
        windowSize = 1;
    }
*/





    //start the timer
    stopwatch.start();

    switch(testID)
    {
    case 2:
        receiveSlidingWindow(sock, transmission, TOTAL_PACKETS, timeoutLength, windowSize);
    case 1:
    default:
        receiveStopAndWait(sock, transmission, TOTAL_PACKETS, timeoutLength);
        cout << stopwatch.lap() << endl;
        break;
    }
}


int receiveStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength)
{
    for(int i = 0; i < sendCount; i++)
    {
        if(transmission[0] == sendCount-1)
            break;

        sock.recvFrom((char*) transmission, sizeof(&transmission));
        if(transmission[0] == i)
        {
            cout << msg_ACKSend << (i+1) << endl;
            sock.ackTo((char*) transmission, sizeof(&transmission));
            continue;
        }

        while(sock.pollRecvFrom() <= 0)
        {
            usleep(timeoutLength);
        }
        i--;
        cout << error_ACKTimeout << msg_ACKSend << msg_ACKResend << (i+1) << endl;
    }

    return transmission[0];
}
/*

    Timer internalClock;
    int ackFails = 0;
    int lastFails = ackFails;
    for(int i = 0; i < sendCount; i++)
    {
        internalClock.start();
        transmission[0] = i;
        sock.sendTo((char*)transmission, sizeof(&transmission));

        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);
            if(internalClock.lap() >= timeoutLength)
            {
                cout << error_ACKTimeout << msg_ACKResend << (i+1) << endl;
                ackFails++;
                break;
            }
        }

        //check if timed out
        if(lastFails != ackFails)
        {
            //timed out, resend packet
            i--;
            lastFails = ackFails;
            continue;
        }

        sock.recvFrom((char*) transmission, sizeof(&transmission));
        cout << msg_packetTime << internalClock.lap() << endl;
    }

    return ackFails;
}
*/

int receiveSlidingWindow(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength, const int windowSize)
{
    for(int i = 0; i < sendCount; i++)
    {
        transmission[0] = i;
        sock.sendTo((char*)transmission, MAX_UDP_PAYLOAD);
        cout <<  "sent packet: " << (i +1) << endl;

    }
}

int getWindowSize(int userSelection)
{
    switch(userSelection)
    {
    case 1: return 200;
    case 2: return 1000;
    case 3:
    default: return 200000;
    }
}
