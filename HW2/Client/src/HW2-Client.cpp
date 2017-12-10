//============================================================================
// Name        : HW2-Client.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "UdpSocket.h"
#include "Timer.h"

int main() {


    int seqNum = 0;
    int ackNum = 0;
    int MAX_SEQ = 10000;
    int MAX_BUF = 1460;
    int window = 100;
    int timeout = 100000;
    bool timedout = false;

    char buf[MAX_BUF];
    char ack[MAX_BUF];



    UdpSocket sock(50029);
    sock.setDestAddress("uw1-320-01.uwb.edu");
    Timer stopwatch;

    buf[0] = seqNum;
    sock.sendTo((char*) &buf, sizeof(buf));


    while(seqNum < MAX_SEQ)
    {
        stopwatch.start();
        while(sock.pollRecvFrom() <= 0)
        {
            while(seqNum - ackNum < window)
            {
                ++seqNum;
                buf[0] = seqNum;
                sock.sendTo((char*) &buf, sizeof(buf));
            }

            if(stopwatch.lap() >= timeout)
            {
                cout << "timed out " << endl;
                timedout = true;
                break;
            }

        }

        if(timedout)
        {
            seqNum = ackNum;
            timedout = false;
            continue;
        }

        sock.recvFrom(ack, sizeof(ack));

        cout << "recv'd" << endl;


        if(ack[0] > ackNum)
        {
            ackNum = ack[0];
            cout << "ack# " << ackNum << endl;
        }

    }


}
