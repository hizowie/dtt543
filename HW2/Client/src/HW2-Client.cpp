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
    int ackNum = -1;
    int MAX_SEQ = 10000;
    int MAX_BUF = 1460;
    int window = 100;
    int timeout = 10000;

    char buf[MAX_BUF];
    char ack[MAX_BUF];



    UdpSocket sock(50029);
    sock.setDestAddress("127.0.0.1");
    Timer stopwatch;

    stopwatch.start();
    while(seqNum < MAX_SEQ)
    {

        while(seqNum - ackNum < window)
        {
            buf[0] = seqNum;
            sock.sendTo(buf, sizeof(buf));
            ++seqNum;
        }

        if(stopwatch.lap() >= timeout)
        {
            seqNum = ackNum;
            continue;
        }

        if(sock.pollRecvFrom() <= 0)
        {
            usleep(10);
            continue;
        }

        sock.recvFrom(ack, sizeof(ack));

        if(ack[0] > ackNum)
        {
            ackNum = ack[0];
            cout << "ack# " << ackNum << endl;
        }

    }


}
