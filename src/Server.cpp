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

#define TOTAL_PACKETS 20000
#define MAX_UDP_PAYLOAD 1472


static const char* error_ACKTimeout = "Packet ACK Timeout.";
static const char* msg_ACKSend = "Sending Ack # ";
static const char* msg_ACKResend = "Resending Ack # ";
static const char* msg_packetTime = "Packet sent: ";

static const char* instructions = "This Client accepts the following input:\n"
							"\n\tportNumber - the port to listen on (required)"
							//"\n\tmachineName - the name of the receiving computer (required)"
							"\n\ttestID - the test to execute (optional)"
							"\n\t\t1 - Stop-and-Wait"
							"\n\t\t2 - Sliding Window"
							"\n\ttimeoutLength - the amount of time before the window times out (optional)"
							"\n\t\t1 - small  (200 nanseconds)"
							"\n\t\t2 - medium (1 microsecond)"
							"\n\t\t3 - large  (200 microseconds)";
							//"\n\twindowSize - the size of the window in the Sliding Window algorithm (optional)"
							//"\n\t\t1 - small  (100 packets)"
							//"\n\t\t2 - medium (1,000 packets)"
							//"\n\t\t3 - large  (10,000 packets)";


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
	if(userSelection < 1 || userSelection > 2)
	{
	    cout << "userSelection < 1 || userSelection > 2" <<endl;
		cout << "userSelection = " << userSelection << endl;

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
	    cout << "userSelection < 1 || userSelection > 3" <<endl;
	    cout << "userSelection = " << input << endl;

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
	    cout << "userSelection < 1 || userSelection > 3" <<endl;
	    cout << "userSelection = " << input << endl;

		cout << error_invalidInput << error_windowSize << endl;
		cout << instructions << endl;
		return -1;
	}

	return pow(10, (userSelection + 1));
};



int receiveStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength);
int receiveSlidingWindow(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength);


int main(int argc, char* argv[])
{
	for(int i = 0; i < argc; i++)
	{
		cout << "[" << i << "] : " << argv[i] << endl;
	}

	cout << endl << endl;


    //input:
        //[0]: int port; required
        //[1]: string //set the destination addressmachineNamargce; required
        //[2]: int testID; optional
        //[3]: int timeoutLength; optional
        //[4]: int windowSize; optional


    //input sanitation
    if(argc < 2)
    {
    	cout <<"if argc < 2" << endl;

        cout << error_invalidInput << error_notEnoughArguments << endl;
        cout << instructions << endl;
        return 0;
    }

    //check port number
    if (atoi(argv[1]) < 0 || atoi(argv[1]) > 65535)
    {
    	cout << "atoi(argv[0]) < 0 || atoi(argv[0]) > 65535" <<endl;
    	cout << "argv[1] = " << argv[1] << endl;

        cout << error_invalidInput << error_portNumber;
        return 0;
    }

    //create an udpsocket with the given port
    UdpSocket sock(atoi(argv[1]));



    //local variable creation
    int testID, timeoutLength, windowSize;
    Timer stopwatch;
    int transmission[MAX_UDP_PAYLOAD/sizeof(int)];





    if(argc > 1)
    {
        testID = getTestID(argv[2]);
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
    case 2: //intentionally letting this fall through, for now
        //receiveSlidingWindow(sock, transmission, TOTAL_PACKETS, timeoutLength);
    	//break;
    case 1:
    default:
        receiveStopAndWait(sock, transmission, TOTAL_PACKETS, timeoutLength);
        cout << stopwatch.lap() << endl;
        break;
    }
}



int receiveStopAndWait(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength)
{
	cout << "receiveStopAndWait (" << sendCount << ", " << timeoutLength << ")" << endl;



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
            usleep(1); //timeoutLength);
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


