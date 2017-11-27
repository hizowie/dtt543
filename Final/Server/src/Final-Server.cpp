//============================================================================
// Name        : Final-Server.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include "TCPSocket.h"


#include <cstdlib>



extern "C"
{
    #include <sys/types.h>    // for sockets
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

    #include <netdb.h>        // for gethostbyname( )
    #include <unistd.h>       // for close( )
    #include <string.h>       // for bzero( )

    #include <sys/poll.h>     // for poll( )
}


using namespace std;

int createSocket(int port);

int main()
{
    TCPSocket socket;
    socket.connectSocket(50029);

    //socket ->connectSocket(50029);



}




