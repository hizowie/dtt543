/*


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
#include <vector>

#define TOTAL_PACKETS 20000
#define MAX_UDP_PAYLOAD 1472

#define DEBUG false


void sendSelectiveRepeat(UdpSocket &sock, int transmission[], const int sendCount, const int timeoutLength,  int windowSize)
{
    Timer stopwatch;
    bool ackTimedOut = false;


    int lastFrameAckd = 0;
    int seqNum = 0;

    windowSize = windowSize / 100;

    //init window
    vector<int> window(windowSize);
    for(int i = 0; i < windowSize; i++)
    {
        window[i] = -1;
    }




    while(sock.pollRecvFrom() > 0)
        {
            sock.recvFrom((char*)transmission, sizeof(&transmission));



            int temp = *transmission;

            cout << "\t\tPackage rec'd: " << temp << endl;

            if(temp == lastFrameAckd)
            {
                //put the ack in the vector
                window[lastFrameAckd] = *transmission;

                //move the marker
                lastFrameAckd++;
                cout << "\t\t\tNext package Acked: " << temp << endl;
            }
            else if(window[temp % windowSize] != -1)
            {
                //drop the packet, or resend the next?
                    //currently drop the packet

                //send the next packet
                //transmission[0] = seqNum;
                //sock.sendTo((char*)transmission, sizeof(&transmission));
                //cout << msg_packetSent << (seqNum + 1) << endl;
                //store the packet in the frame
            }
            else
            {
                cout << "\t\t\t\tOther package acked: " << temp << endl;
                window[temp % windowSize] = temp;
            }

            continue;
        }

        stopwatch.start();

        //poll for ACKs
        while(sock.pollRecvFrom() <= 0)
        {
            usleep(1);

            //check if timed out
            if(stopwatch.lap() >= timeoutLength)
            {
                //notify of failure and flag it
                cout << error_ACKTimeout << msg_ACKResend << (seqNum + 1) << endl;
                ackTimedOut = true;
                break;
            }
        }

        if(ackTimedOut)
        {
            //reset flag
            ackTimedOut = false;
            cout << "\t\t\tack timed out." << endl;


            seqNum = lastFrameAckd;


            /*
            //check the space in the windows frames
            for(int i = lastFrameAckd; i < seqNum; i++)
            {
                if(window[i] != -1)
                {
                    seqNum = i;
                    cout << endl << "Setting Sequence Number to " << i << endl << endl;
                    //sleep(1);
                    break;
                }
            }

        }


        //check flag for failure
        if(ackTimedOut)
        {
            //reset flag
            ackTimedOut = false;

            //check the space in the windows frames
            for(int i = lastFrameAckd; i < seqNum; i++)
            {
                if(window[i] != -1)
                {
                    seqNum = i;
                    cout << endl << "Setting Sequence Number to " << i << endl << endl;
                    sleep(1);
                    break;
                }
            }
        }

        if(seqNum + 1 == sendCount)
        {
            break;
        }


        //get position within the window
        int seqNum = lastFrameRec % windowSize;



        //check index is before last ACK or outside window
        if(seqNum < lastAckFrame || seqNum >= windowSize)
        {
            //previous ack failed, go-back-n
            sock.ackTo((char*) &lastSeq, sizeof(int));
            continue;
        }

        //check if index is next packet
        if(seqNum == lastAckFrame)
        {
            //accept packet list
            window[seqNum] = lastFrameRec;

            //update the sequence number
            lastSeq = lastFrameRec;

            //iterate over the list, looking for the next non-negative-one value
            while(window[lastAckFrame] > -1)
            {
                //update the sequence index
                lastSeq = window[lastAckFrame];

                //flag window as un-ACK'ed
                window[lastAckFrame] = -1;

                //move ack index
                lastAckFrame++;

                //get ack index within window
                lastAckFrame = lastAckFrame % windowSize;

                //check if window has been ack'ed
                if(window[lastAckFrame] == -1)
                {
                    //window hasn't been ack'ed, notify sender
                    sock.ackTo((char*) &lastSeq, sizeof(int));
                }
            }
        }
        else
        {
            //packet is in window but not next, save for later
            window[seqNum] = lastFrameRec;
            sock.ackTo((char*) &lastSeq, sizeof(int));
        }


    }
}
*/
