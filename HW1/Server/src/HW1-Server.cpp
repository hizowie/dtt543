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

extern "C"
{
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // TCP_NODELAY
#include <stdlib.h>
#include <stdio.h>
}
using namespace std;

#include "TCPSocket.h"
#include "Socket.h"




void readData(int repetitions);

#define BUFSIZE 1500



int newSD;
int rep;



void readData(int repetitions)
{
    //allocate databuf[bufSize] where bufsize = 1500
    char databuf[BUFSIZE];
    struct timeval startTime, endTime;


    //start a timer by calling gettimeofday()
    gettimeofday(&startTime, NULL);


    int count;
    double timediff;

    for(int i = 0; i < rep; i++)
    {
        for(int nRead = 0; (nRead += read(newSD, databuf + nRead, BUFSIZE - nRead)) < BUFSIZE; ++count);

    }

    //use the write system call to send back a response to the client (user newSd, but not serverSd)
    write(newSD, &count, sizeof(count));

    gettimeofday(&endTime, NULL);

    timediff = (endTime.tv_sec - startTime.tv_sec) +
                        (endTime.tv_usec - startTime.tv_usec)/1000000.0;


    cout << "\t\tTransfer time: " << timediff  << " milliseconds" << endl;


    exit(0);

}



int main(int argc, char * argv[])
{
	if(argc < 3)
	{
		cout << "Must provide at least 2 arguments: [port] and [repetitions]" << endl;
		return 0;
	}


    int port = atoi(argv[1]);
    rep = atoi(argv[2]);


	TCPSocket socket(port);

	newSD = socket.connectServer();


    //Change the socket into an asynchronous connection
    signal(SIGIO, readData);
    fcntl(newSD, F_SETOWN, getpid());
    fcntl(newSD, F_SETFL, FASYNC);

    while (true)
    {
        sleep(1000);
    }

}//end main




