/*
 * tmpSvr.cpp
 *
 *  Created on: Dec 3, 2017
 *      Author: anon
 */

/*
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // TCP_NODELAY
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "Socket.h"
#define BUFSIZE 1500

int fd;
int repetition;


void my_function(int repetitions){
    char databuf[BUFSIZE];
    struct timeval startTime, stopTime;

    gettimeofday(&startTime, NULL);
    int count;
    for (int i = 0; i < repetition; i++){
        for (int nRead = 0; (nRead += read(fd, databuf + nRead,
                                    BUFSIZE - nRead)) < BUFSIZE; ++count);
    }
    gettimeofday(&stopTime, NULL);

    long diffTime;
    diffTime = (stopTime.tv_sec - startTime.tv_sec) * 1000000;
    diffTime += (stopTime.tv_usec - startTime.tv_usec);

    cout << "Time Received: " << diffTime << " usec" << endl;
    cerr << diffTime << endl;
    write(fd, &count, sizeof(count));
    exit(0);

}


int main(int argc, char * argv[]){

    int port = atoi(argv[1]);   //port#
    repetition = atoi(argv[2]); //#of reps on read

    Socket socket(port);

     fd = socket.getServerSocket();

    //Change the socket into an asynchronous connection
    signal(SIGIO, my_function);
    fcntl(fd, F_SETOWN, getpid());
    fcntl(fd, F_SETFL, FASYNC);

    while (true) {
        sleep(1000);
    }

}//end main
*/

    //  Client.cpp
//  HMW1
//
//  Created by Terence Schumacher on 10/2/14.
//  Copyright (c) 2014 Terence Schumacher. All rights reserved.
//============================================================================

/*
#include <stdio.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // TCP_NODELAY
#include <stdlib.h>
//#include "Socket.h"

#define DATABUFSIZE 1500

using namespace std;


void printStats(long usec, long rtt, int readNum, int type)
{
    cout << "Test "<< type <<": data-sending time = " << usec
         << " usec, round-trip time = " << rtt << " usec, #reads = "
         << readNum << endl;
}

long elapseTime(timeval start, timeval stop)
{
    long result;
    result = (stop.tv_sec - start.tv_sec) * 1000000;
    result += (stop.tv_usec - start.tv_usec);
    return result;
}



int main(int argc, char* argv[])
{
    if (argc < 7|| atoi(argv[6]) > 3 || atoi(argv[6]) < 1)
    {
        perror("Too Few Arguments passed in, 6 needed");
        exit(1);
    }

    int port = atoi(argv[1]);
    int repetition = atoi(argv[2]);
    int nbufs = atoi(argv[3]);
    int bufsize = atoi(argv[4]);
    char * serverIp = argv[5];
    int type = atoi(argv[6]);

    //check data buffer allocation
    if (nbufs* bufsize != DATABUFSIZE)
    {
        cout << "Data buffer Size must equal 1500; nbuf * bufsize = 1500" << endl;
        exit(1);
    }


    cout << "Port: " << port << "Repetition: " << repetition
         << "Number of Data Buffers: " << nbufs << "Buffer Size: "
         << bufsize << "Server IP Name: " << serverIp << "Scenario Type: "
         << type << endl;

    //create socket
    Socket socket(port);

    // socket descriptor
    int sd = socket.getClientSocket(serverIp);
    char databuf[nbufs][bufsize];
    int readNum;
    struct timeval start, lap, stop;

    gettimeofday(&start, NULL);

    for (int i = 0; i < repetition; i++)
    {
        if (type == 1)
        {            // nbufs buffers of bufsize
            for (int j = 0; j < nbufs; j++)
                write(sd, databuf[j], bufsize);
        }
        else if (type == 2)
        {     // writev of a vector of nbufs
            struct iovec vector[nbufs];
            for (int j = 0; j < nbufs; j++)
            {
                vector[j].iov_base = databuf[j];
                vector[j].iov_len = bufsize;
            }
            writev(sd, vector, nbufs);
        }
        else
        {        // write nbufs*bufsize all at once
            write( sd, databuf, nbufs * bufsize );
        }
    }

    gettimeofday(&lap, NULL);   //lap time

    int count;
    read(sd, &count, sizeof(count));
    gettimeofday(&stop, NULL);  // stop time
    long lapTime, elapsedTime;
    lapTime = elapseTime(start,lap);    //gen lap time
    elapsedTime = elapseTime(start, stop);

    printStats( lapTime, elapsedTime, readNum, type);

    cerr << lapTime << " " << elapsedTime << endl;
    close(sd);

    return 0;


}//end main



*/
