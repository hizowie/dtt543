//============================================================================
// Name        : HW1-Client.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

/*
 * temp.cpp
 *
 *  Created on: Oct 14, 2017
 *      Author: anon
 */




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




int sd;
void writeMulti(int sd, char *databuf[], int nbufs, int bufsize);
void writeV(int sd, char* databuf[], int nbufs, int bufsize);
void writeSingle(int sd, char* databuf[], int nbufs, int bufsize);


void printResults(timeval startTime, timeval endTime, std::vector<timeval> laps, int * lastAck, int transferType);



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
    char *serverIP    = argv[5];
    int transferType = *argv[6];

    //input sanitation
    if(nbufs * bufsize != 1500)
    {
        //default to a max data size of 1500 bytes
        bufsize = 1500 / nbufs;
    }

    if(serverIP == NULL)
    {
        //default to the local machine
        serverIP = new char[256];
        gethostname(serverIP, 256);
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



    int sd;
    const int enable = 1;
    struct sockaddr_in myAddr;

    struct hostent *destination;
    char data[256];

    // Open a TCP socket
    if(( sd = socket( AF_INET, SOCK_STREAM, 0 )) < 0)
    {
        cerr << "Cannot open a TCP socket." << endl;
    }

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int));

    destination = gethostbyname(serverIP);

    bzero((char*)&myAddr, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    bcopy((char*)destination->h_addr, (char*)&myAddr.sin_addr.s_addr, destination->h_length);
    myAddr.sin_port = htons(atoi(port));
    if(connect(sd, (struct sockaddr*)&myAddr, sizeof(myAddr))< 0)
    {
        cerr << "Failed to create TCP connection " << endl;
    }

    //local variables
    //int port = YOUR_ID;         //the last 5 digits of your student id
    //sockaddr_in sendSockAddr;
    //bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    //sendSockAddr.sin_family = AF_INET;  //address family: internet
    //sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr)*host->h_addr_list));
    //sendSockAddr.sin_port = htons(port);



    //open socket and establish a connection to a server

    //allocate databuf[nbufs][bufsize], where nbufs * bufsize = 1500
    char databuf[nbufs][bufsize];
    //char * d = &databuf;

    //retrieve a hostent struct to this ip by calling gethostname
    //struct hostent * host;

    /*

     //get the host info

     int cliendSd;
     struct addrinfo hints, *servinfo, *p;
     int rv;

     memset(&hints, 0, sizeof hints);
     hints.ai_flags = AI_PASSIVE;
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_STREAM;

     //attempt to get machine info
     if((rv = getaddrinfo(serverIP, port, &hints, &servinfo)) != 0)
     {
         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
         exit(1);
     }


     //iterate over the matches that were found
     for(p = servinfo; p != NULL; p = p->ai_next)
     {
         //check the sockets that were found
         if((cliendSd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
         {
             perror("socket");
             continue;
         }

         //attempt to connect
         if(connect(cliendSd, p->ai_addr, p->ai_addrlen) == -1)
         {
             perror("connect");
             close(cliendSd);
             continue;
         }

         break; //connected successfully
     }

     if(p == NULL)
     {
         //never managed to connect
         fprintf(stderr, "failed to connect\n");
         exit(2);
     }

     //release the address object
     freeaddrinfo(servinfo);
     */



    //start a timer by calling gettimeofday()
    struct timeval startTime, endTime, thisLap;
    int ack;
    std::vector<timeval> laps;
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

        //lap the timer by calling gettimeofday() where lap-start = data-sending time
        gettimeofday(&thisLap, NULL);
        laps.push_back(thisLap);
    }


    //receive from the server an integer acknowledgement that shows how many times the server called read()
    read(sd, &ack, sizeof(int));


    //stop the timer by calling gettimeofday() where stop-start = round-trip time
    gettimeofday(&endTime, NULL);


    //print out the statistics, e.g.:
        //Test 1: data-sending time = xxx usec, round-trip time = yyy usec, #reads = zzz
    printResults(startTime, endTime, laps, &ack, transferType);


    //close the socket
    close(sd);

}


void printResults(timeval startTime, timeval endTime, std::vector<timeval> laps, int * lastAck, int transferType)
{
    double totalTime = (endTime.tv_sec - startTime.tv_sec) + ((endTime.tv_usec - startTime.tv_usec)/1000000.0);

    const char * header;

    if(transferType == 1)
        header = "Test: Multi-write\t\tTotal time:\t%d";
    else if(transferType == 2)
        header = "Test: Writev\t\t\tTotal time:\t%d";
    else
        header = "Test: Single-write\t\tTotal time:\t%d";

    fprintf(stdout, header, totalTime);



    double timediff;
    int counter = 1;
    for(std::vector<timeval>::iterator it = laps.begin(); ++it != laps.end(); ++it)
    {
         timediff = ((*it).tv_sec - (*(++it)).tv_sec) +
                     (((*it).tv_usec - (*(++it)).tv_usec)/1000000.0);

         fprintf(stdout, "\t\tTransfer %d: %f milliseconds\n", counter, timediff);
         counter++;
         timediff = -1.0;
    }

    fprintf(stdout, "\tAck'd sends: %d\n", *lastAck);
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


