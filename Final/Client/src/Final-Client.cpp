//============================================================================
// Name        : Final-Client.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>


#include <iostream>
#include "TCPSocket.h"
using namespace std;

int main()
{

    //welcome the user

    //establish a connection

    TCPSocket socket;
    socket.connectSocket(50029, "localhost");//"uw1-320-11.uwb.edu");

}
