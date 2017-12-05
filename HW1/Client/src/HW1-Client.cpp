//============================================================================
// Name        : HW1-Client.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================




#include <sys/types.h>      //socket, bind
#include <sys/socket.h>     //socket, bind, listen, inet_ntoa
#include <netinet/in.h>     //hton, htons, inet_ntoa
#include <arpa/inet.h>      //inet_ntoa
#include <netdb.h>          //gethostbyname
#include <unistd.h>         //read, write, close
#include <string.h>           //bzero
#include <netinet/tcp.h>    //SO_REUSEADDR
#include <sys/uio.h>        //writev

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>
#include <iostream>
#include <strings.h>

#include "TCPSocket.h"


using namespace std;

string error_MainArgs = "The provided arguments failed. The expected args are:\n"
                         "(int) port -\t\tServer IP Port\n"
                         "(int) repetitions -\tThe number of repetitions of sending a set of data buffers\n"
                         "(int) nbufs -\t\tThe number of data buffers\n"
                         "(int) bufsize -\t\tthe sieze of each data buffer (in bytes)\n"
                         "(int) serverIP -\t\tThe Server IP name\n"
                         "(int) transferType - The type of data transfer, where the options are\n"
                         "\t1 : Multiple Writes\n"
                         "\t2 : Writev\n"
                         "\t3 : Single-transfer (dump)\n";





void writeMulti(int sd, char *databuf[], int nbufs, int bufsize);
void writeV(int sd, char* databuf[], int nbufs, int bufsize);
void writeSingle(int sd, char* databuf[], int nbufs, int bufsize);

long getTimespan(timeval startTime);
long getTimespan(timeval startTime, timeval endTime);


void printResults(long startTime, long endTime, int ack, int transferType);



int main(int argc, char* argv[])
{
    //test if all expected input was provided
    if(argc != 7)
    {
        cout << "argv[1] = " << argv[1] << endl;
        cout << "argv[2] = " << argv[2] << endl;
        cout << "argv[3] = " << argv[3] << endl;
        cout << "argv[4] = " << argv[4] << endl;
        cout << "argv[5] = " << argv[5] << endl;
        cout << "argv[6] = " << argv[6] << endl;
        cout << "argc = " << argc << endl << endl;;
               //input is missing; complain and exit
        //fprintf(stderr, (char*)error_MainArgs.c_str());
        exit(1);
    }

    //inputs:
    char *port =        argv[1];
    int repetitions  = *argv[2];
    int nbufs        = *argv[3];
    int bufsize      = *argv[4];
    char *serverName =  argv[5];
    int transferType = *argv[6];

    //input sanitation
    if(nbufs * bufsize != 1500)
    {
        //default to a max data size of 1500 bytes
        bufsize = 1500 / nbufs;
    }

    if(serverName == NULL)
    {
        //default to the local machine
    	serverName = new char[256];
        gethostname(serverName, 256);
    }

    if(transferType < 1 || transferType > 3)
    {
        //default to multi-write
        transferType = 1;
    }

    if(atoi(port) < 9999)
    {
        //default to student number if given 4-digit port
        port = "50029";
    }


    cout << "Port:\t\t\t" << port << endl;
    cout << "Server Name:\t" << repetitions << endl;
    cout << "Buffer count:\t" << repetitions << endl;
    cout << "Buffer size:\t" << repetitions << endl;
    cout << "Repetitions:\t" << repetitions << endl;
    cout << "Test number:\t" << transferType << endl;


    TCPSocket socket(port);
    int sd = socket.connectClient(serverName);


    //allocate databuf[nbufs][bufsize], where nbufs * bufsize = 1500
    char databuf[nbufs][bufsize];




    //start a timer by calling gettimeofday()
    struct timeval startTime, endTime, thisLap;
    long lapTime, totalTime;
    int ack;
    gettimeofday(&startTime, NULL);



    //repeat the repetition times of data transfers, each based on write type
    for(int i = 0; i < repetitions; i++)
    {
        if(transferType == 1)
        {
            writeMulti(sd, (char**)&databuf, nbufs, bufsize);
        }
        else if(transferType == 2)
        {
            writeV(sd, (char**) &databuf, nbufs, bufsize);
        }
        else
        {
            writeSingle(sd, (char**) databuf, nbufs, bufsize);
        }
    }

    //lap the timer by calling gettimeofday() where lap-start = data-sending time
    gettimeofday(&thisLap, NULL);
    lapTime = getTimespan(startTime);

    int count;

    //receive from the server an integer acknowledgement that shows how many times the server called read()
    read(sd, &count, sizeof(count));



    //stop the timer by calling gettimeofday() where stop-start = round-trip time
    totalTime = getTimespan(startTime);


    //print out the statistics, e.g.:
        //Test 1: data-sending time = xxx usec, round-trip time = yyy usec, #reads = zzz
    printResults(lapTime, totalTime, ack, transferType);


    //close the socket
    close(sd);

    return 0;
}


//void printResults(long totalTime, int * lastAck, int transferType)
void printResults(long lapTime, long totalTime, int ack, int transferType)
{
    const char * header;

    if(transferType == 1)
        header = "Test: Multi-write\t";
    else if(transferType == 2)
        header = "Test: Writev\t\t";
    else
        header = "Test: Single-write\t";


    cout << header << endl;
    cout << "\tData-sending time:\t" << lapTime << "usec "<< endl;
    cout << "Round-trip time:\t" << totalTime << "usec " << endl;
    cout << "# Reads: " << ack << endl;

}



void writeMulti(int sd, char * databuf[], int nbufs, int bufsize)
{
    for(int j = 0; j < nbufs; j++)
    {
        write(sd, databuf[j], bufsize);
    }
}


void writeV(int sd, char* databuf[], int nbufs, int bufsize)
{
    struct iovec vector[nbufs];
    for(int j = 0; j < nbufs; j++)
    {
        vector[j].iov_base = databuf[j];
        vector[j].iov_len = bufsize;
    }
    writev(sd, vector, nbufs);
}


void writeSingle(int sd, char* databuf[], int nbufs, int bufsize)
{
    write(sd, databuf, nbufs*bufsize);
}


long getTimespan(timeval startTime)
{
	timeval endTime;
	gettimeofday(&endTime, NULL);
	return getTimespan(startTime, endTime);
}


long getTimespan(timeval startTime, timeval endTime)
{
    long diff;
    diff = (endTime.tv_sec - startTime.tv_sec) * 1000000;
    diff += (endTime.tv_usec - startTime.tv_usec);
    return diff;
}
