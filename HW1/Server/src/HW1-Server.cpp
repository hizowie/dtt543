//============================================================================
// Name        : HW1-Server.cpp
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





void readData(int repetitions);

#define BUFSIZE 1500



int repetitions;
int newSD;

int main(int argc, char* argv[])
{

    if(argc < 3)
    {
        cout << "Must provide at least 2 arguments: [port] and [repetitions]" << endl;
        return 0;
    }

    int port =     atoi(argv[1]);
    repetitions =  atoi(argv[2]);


    int sd;
    socklen_t destSockLen;
    const int enable = 1;

    struct sockaddr_in myAddr;
    struct sockaddr_in destAddr;



    if(( sd = socket( AF_INET, SOCK_STREAM, 0)) < 0 )
    {
          cerr << "Cannot open a TCP socket." << endl;
    }

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int));


    // Bind our local address
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons(port);
    myAddr.sin_family = AF_INET;
    bzero((char*) &myAddr.sin_zero, sizeof(myAddr));



    if(bind(sd, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
    {
        cerr << "Cannot bind while opening TCP socket "<< endl;
    }


    listen(sd, 5);
    destSockLen = sizeof(destAddr);
    newSD = accept(sd, (struct sockaddr *) &destAddr, &destSockLen);



    //change the socket into an asynchronous connection
    signal(SIGIO, readData);
    fcntl(newSD, F_SETOWN, getpid());
    fcntl(newSD, F_SETOWN, FASYNC);


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
                (nRead += read(newSD, databuf + nRead, BUFSIZE - nRead)) < BUFSIZE; ++count);

    }

    //use the write system call to send back a response to the client (user newSd, but not serverSd)
    write(newSD, &count, sizeof(count));
    gettimeofday(&endTime, NULL);

    timediff = (endTime.tv_sec - startTime.tv_sec) +
                        (endTime.tv_usec - startTime.tv_usec)/1000000.0;

    fprintf(stdout, "\t\tTransfer time: %f milliseconds", timediff);


    //close the socket by calling close
    //close(newSd);
}


