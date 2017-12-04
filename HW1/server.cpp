/*
 * temp2.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: anon
 */
#include <sys/types.h>      //socket, bind
#include <sys/socket.h>     //socket, bind, listen, inet_ntoa
#include <netinet/in.h>     //hton, htons, inet_ntoa
#include <arpa/inet.h>      //inet_ntoa
#include <netdb.h>          //gethostbyname
#include <unistd.h>         //read, write, close
#include <strings.h>           //bzero
#include <netinet/tcp.h>    //SO_REUSEADDR
#include <sys/uio.h>        //writev

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>
#include <iostream>

#include <signal.h>
using namespace std;


#define BUFSIZE 1500




void readData(int repetitions);


int serverSd;
int repetitions;

int main(int argc, char* argv[])
{
    int port =     atoi(argv[1]);
    repetitions =  atoi(argv[2]);

    //accept a connection
    sockaddr_in acceptSockAddr;
    bzero((char*)&acceptSockAddr, sizeof(acceptSockAddr));
    acceptSockAddr.sin_family = AF_INET; //address family : internet
    acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    acceptSockAddr.sin_port = htons(port);

    //open a stream-oriented socket with internet address family
    serverSd = socket(AF_INET, SOCK_STREAM, 0);

    //set the SO_REUSEADDR option
    const int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int));

    //bind the socket to local address w/ descriptor, address, & data size
    bind(serverSd, (sockaddr*)&acceptSockAddr, sizeof(acceptSockAddr));

    //instruct OS to listen up to 5 connection requests from client
    listen(serverSd, 5);



    //receive a request from client by calling accept
    //sockaddr_in newSockAddr;
    //socklen_t newSockAddrSize = sizeof(newSockAddr);
    //int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);



    //change the socket into an asynchronous connection
    signal(SIGIO, readData);
    fcntl(serverSd, F_SETOWN, getpid());
    fcntl(serverSd, F_SETOWN, FASYNC);

    //let this server sleep forever
    while(true)
    {
        sleep(1000);
    }




}



void readData(int repetitions)
{
    //allocate databuf[bufSize] where bufsize = 1500
    char databuf[BUFSIZE];
    struct timeval startTime, endTime;

    //start a timer by calling gettimeofday()
    gettimeofday(&startTime, NULL);


    //use the read system to receive data from client (use newSd but not serverSd)

    //repeat reading data from the client into databuf[bufSize]
        //note that the read system call may return without reading the
        //entire data if the network is slow. you have to repeat calling
        //read like:
    int count = 0;
    double timediff;

    for(int i = 0; i < repetitions; i++)
    {
        for(int nRead = 0;
                (nRead += read(serverSd, databuf + nRead, BUFSIZE - nRead)) < BUFSIZE; ++count);

    }

    //use the write system call to send back a response to the client (user newSd, but not serverSd)
    write(serverSd, &count, sizeof(count));
    gettimeofday(&endTime, NULL);

    timediff = (endTime.tv_sec - startTime.tv_sec) +
                        (endTime.tv_usec - startTime.tv_usec)/1000000.0;
    fprintf(stdout, "\t\tTransfer time: %f milliseconds", timediff);


    //close the socket by calling close
    close(serverSd);
}
