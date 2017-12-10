//============================================================================
// Name        : HW2-Server.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include "UdpSocket.h"
#include "Timer.h"


using namespace std;





int main()
{
    int seqNum = -1;
    int MAX_BUF = 1460;
    int MAX_SEQ = 10000;
    int timeout = 5000;
    char ack[MAX_BUF];


    vector<int> buf;
    vector<int>::iterator it;
    Timer stopwatch;

    UdpSocket sock(50029);

    while(seqNum < MAX_SEQ)
    {
        stopwatch.start();
        bool inserted = false;

        while(stopwatch.lap() < timeout)
        {
            while(sock.pollRecvFrom() <= 0)
            {
                usleep(1);
            }

            sock.recvFrom((char*)&ack, sizeof(ack));
            int newSeqNum = ack[0] + ack[1];

            for(it = buf.begin(); it < buf.end(); ++it)
            {
                if(*it == newSeqNum)
                {
                    inserted = true;
                    break;
                }

                if(*it > (ack[0] + ack[1]))
                {
                    buf.insert(it-1, newSeqNum);
                    inserted = true;
                    break;
                }
            }

            if(!inserted)
            {
                buf.push_back(newSeqNum);
            }
        }

        bool reachedEnd = true;
        for(it = buf.begin() + 1; it < buf.end(); ++it)
        {
            if(*it != *(it - 1))
            {
                ack[0] = *(it-1);
                buf.erase(buf.begin(), it - 1);
                reachedEnd = false;
                break;
            }
        }

        if(reachedEnd)
        {
            it = buf.end();
            ack[0] = *(it-1);
            buf.clear();
        }

        sock.ackTo((char*) &ack, sizeof(ack));
    }

}
