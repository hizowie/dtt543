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

#include "CommonClient-Server.h"


int receiveStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength);
int receiveSlidingWindow(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength);


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
    int testID, timeoutLength, windowSize;
    Timer stopwatch;
    int transmission[MAX_UDP_PAYLOAD/sizeof(int)];





    if(argc > 1)
    {
        testID = getTestID(argv[1]);
        if(testID == -1)
            return 0;

        if(testID == 1 || 2)
        {
            if(argc < 2)
            {
                cout << error_invalidInput << error_requiredOptions_Server << endl;
                return 0;
            }

            timeoutLength = getTimeoutLength(argv[3]);
            if(timeoutLength == -1)
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
        receiveSlidingWindow(sock, transmission, TOTAL_PACKETS, timeoutLength);
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


int receiveSlidingWindow(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength)
{
    for(int i = 0; i < sendCount; i++)
    {
        transmission[0] = i;
        sock.sendTo((char*)transmission, MAX_UDP_PAYLOAD);
        cout <<  "sent packet: " << (i +1) << endl;

    }
}


