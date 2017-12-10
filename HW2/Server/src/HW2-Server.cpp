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



            for(it = buf.begin(); it < buf.end(); ++it)
            {
                if(*it == newSeqNum)
                {
                   cout << newSeqNum << " : already exists in vector" << endl;
                   inserted = true;
                   break;
               }

                if(*it > newSeqNum)
                {
                    cout << newSeqNum << " : inserting before "  << *it << endl;
                    buf.insert(it-1, newSeqNum);
                    inserted = true;
                    break;
               }
            }


            if(!inserted)
            {
                cout << "inserting at end" << endl;
                buf.push_back(newSeqNum);
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
