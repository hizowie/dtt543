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
#include "CommonClient-Server.h"

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



int sendStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength );
int sendSlidingWindow(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength, const int windowSize);


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



    //set the destination address
    if(!sock.setDestAddress(argv[1]))
    {
        cout << error_invalidInput << error_machineName << endl;
        return 0;
    }


    //local variable creation
    int testID, timeoutLength, windowSize;
    Timer stopwatch;
    int transmission[MAX_UDP_PAYLOAD/sizeof(int)];





    if(argc > 1)
    {
        testID = getTestID(argv[1]);

        if(testID == -1)
            return 0;


        if(testID == 1)
        {
            if(argc < 2)
            {
                cout << error_invalidInput << error_requiredOptions_Stop << endl;
                return 0;
            }

            timeoutLength = getTimeoutLength(argv[3]);
            if(timeoutLength == -1)
                return 0;
        }


        if(testID == 2)
        {
            if(argc < 3)
            {
                cout << error_invalidInput << error_requiredOptions_Sliding << endl;
                return 0;
            }


            timeoutLength = getTimeoutLength(argv[3]);
            if(timeoutLength == -1)
                return 0;

            windowSize = getWindowSize(argv[4]);
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

    switch(testID)
    {
    case 2:
        sendSlidingWindow(sock, transmission, TOTAL_PACKETS, timeoutLength, windowSize);
        cout << stopwatch.lap() << endl;
        break;
    case 1:
    default:
        sendStopAndWait(sock, transmission, TOTAL_PACKETS, timeoutLength);
        cout << stopwatch.lap() << endl;
        break;
    }
}



int sendStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength)
{
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


int sendSlidingWindow(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength, const int windowSize)
{
    for(int i = 0; i < sendCount; i++)
    {
        transmission[0] = i;
        sock.sendTo((char*)transmission, MAX_UDP_PAYLOAD);
        cout <<  "sent packet: " << (i +1) << endl;

    }
}



