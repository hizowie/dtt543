

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace std;



const char* error_ACKTimeout = "Packet ACK Timeout.";
const char* msg_ACKSend = "Sending Ack # ";
const char* msg_ACKResend = "Resending Ack # ";
const char* msg_packetTime = "Packet sent: ";

const char* instructions = "This Client accepts the following input:\n"
                            "\n\tportNumber - the port to listen on (required)"
                            "\n\tmachineName - the name of the receiving computer (required)"
                            "\n\ttestID - the test to execute (optional)"
                            "\n\t\t1 - Stop-and-Wait"
                            "\n\t\t2 - Sliding Window"
                            "\n\ttimeoutLength - the amount of time before the window times out (optional)"
                            "\n\t\t1 - small  (200 nanseconds)"
                            "\n\t\t2 - medium (1 microsecond)"
                            "\n\t\t3 - large  (200 microseconds)"
                            "\n\twindowSize - the size of the window in the Sliding Window algorithm (optional)"
                            "\n\t\t1 - small  (100 packets)"
                            "\n\t\t2 - medium (1,000 packets)"
                            "\n\t\t3 - large  (10,000 packets)";


const char * error_invalidInput = "Invalid Input: ";
const char * error_notEnoughArguments = "Not enough arguments. ";
const char * error_portNumber = "Port number must be between 0 and 65535";
const char * error_machineName = "Could not establish connection, check machineName.";
const char * error_testID = "Invalid testID selected.";
const char * error_windowSize = "Invalid windowSize selected.";
const char * error_timeoutLength = "Invalid timeoutLength selected.";
const char * error_requiredOptions_Server = "When a testID is specified, timeoutLength is a required parameter.";
const char * error_requiredOptions_Stop = "When testID '1' (Stop-and-Wait) is specified, timeoutLength is a required parameter.";
const char * error_requiredOptions_Sliding = "When testID '2' (Sliding Window) is specified, timeoutLength and windowSize are required parameters.";




int getTestID(const char* input)
{
    int userSelection = atoi(input);
    if(userSelection < 1 || userSelection > 2)
    {
        cout << error_invalidInput << error_testID << endl;
        cout << instructions << endl;
        return -1;
    }

    return userSelection;
}

int getTimeoutLength(const char* input)
{
    int userSelection = atoi(input);
    if(userSelection < 1 || userSelection > 3)
    {
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
}

int getWindowSize(const char* input)
{
    int userSelection = atoi(input);
    if(userSelection < 1 || userSelection > 3)
    {
        cout << error_invalidInput << error_windowSize << endl;
        cout << instructions << endl;
        return -1;
    }

    return pow(10, (userSelection + 1));
}

