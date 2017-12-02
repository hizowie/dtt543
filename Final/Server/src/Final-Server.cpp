//============================================================================
// Name        : Final-Server.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "TCPSocket.h"





using namespace std;

#define DEBUG true

void printArgs(int argc, char* argv[]);


int main(int argc, char* argv[])
{
    if(DEBUG)
        printArgs(argc, argv);


    TCPSocket socket;
    socket.connectSocket(50029);

    //socket ->connectSocket(50029);


    return 1;
}



void printArgs(int argc, char* argv[])
{
    cout << "Main arguments count: " << argc << endl;

    for(int i = 0; i < argc; i++)
        cout << "\targv[" << i << "]: " << argv[i] << endl;

    cout << endl;
}


