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
    int seqNum = 0;
    int MAX_BUF = 1460;
    int MAX_SEQ = 10000;
    int timeout = 5000;
    char ack[MAX_BUF];


    vector<int> buf;
    vector<int>::iterator it;
    vector<int>::iterator begin;
    vector<int>::iterator end;


    Timer stopwatch;

    bool timedOut = false;

    buf.push_back(seqNum);

    UdpSocket sock(50029);


    while(seqNum < MAX_SEQ)
    {
        stopwatch.start();
        bool inserted = false;

        it = buf.begin();

        while(stopwatch.lap() < timeout)
        {
            while(sock.pollRecvFrom() <= 0)
            {
                usleep(1);
            }

            sock.recvFrom((char*)ack, sizeof(&ack));
            int newSeqNum = ack[0] + ack[1];

            if(newSeqNum <= seqNum)
                continue;

            cout << "newSeqNum = " << newSeqNum << endl;

            bool movingRight = true;
            begin = buf.begin();
            end = buf.end();


            while(true)
            {
                if(it == begin - 1)
                {
                    buf.insert(buf.begin(), newSeqNum);
                    it = buf.begin();
                    break;
                }

                if(it == end)
                {
                    buf.push_back(newSeqNum);
                    it = buf.begin();
                    break;
                }

                if(*it == newSeqNum)
                {
                    cout << newSeqNum << " : already exists in vector" << endl;
                    break;
                }

                if(*it < newSeqNum && movingRight)
                {
                    ++it;
                    continue;
                }

                if(*it > newSeqNum && movingRight)
                {
                    --it;
                    movingRight = false;
                    continue;
                }

                if(*it < newSeqNum && !movingRight)
                {
                    --it;
                    continue;
                }

                if(*it > newSeqNum && !movingRight)
                {
                    buf.insert(it+1, newSeqNum);
                    inserted = true;
                    break;
                }
            }
        }


        bool reachedEnd = true;

        for(it = buf.begin() + 1; it < buf.end(); ++it)
        {
            cout << "*it = " << *it << ", *(it-1) = " << *(it -1) << endl;

            if(*it != *(it - 1) + 1)
            {
                cout << "truncating vector at " << *(it -1) << endl;
                seqNum = *(it-1);
                buf.erase(buf.begin(), it - 1);
                reachedEnd = false;
                break;
            }
        }

        if(reachedEnd)
        {
            it = buf.end();
            cout  << "clearing vector; last value was " << *(it -1) << endl;
            cout << "buf.end() = " << *it << endl;


            seqNum = *(it-1);
            buf.clear();
            buf.push_back(seqNum);
        }

        cout << "seqNum = " << seqNum << endl;
        ack[0] = seqNum;

        cout << "ack[0] = " << ack[0] << ack[0] << endl;
        sock.ackTo((char*) ack, sizeof(&ack));
    }

}
