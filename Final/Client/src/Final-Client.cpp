//============================================================================
// Name        : Final-Client.cpp
// Author      : Howie Catlin
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>


#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <iostream>     // std::cout
#include <algorithm>    // std::remove_if
#include <ctype.h>
#include <stdio.h>
#include <functional>



#include "HTTPClient.h"
using namespace std;

#define DEBUG true

const int BOX_LENGTH = 64;
vector<string> exitCommands;
vector<string>::iterator it;


void initExitCommands();

void printProgramInfo();
void printGreeting();
void printCommands();
void printExit();
void askToSave();

string getCommand();

bool handleCommand(string input);
void handleHELP(string input);

HTTPClient * client;


int main(int argc, char* argv[])
{
    initExitCommands();

    printProgramInfo();
    printGreeting();
    printCommands();


    string userCMD;

    userCMD  = getCommand();

    client = new HTTPClient;

    while(handleCommand(userCMD))
    {
        userCMD  = getCommand();
    }

    delete(client);
}


void initExitCommands()
{
    exitCommands.push_back("exit");
    exitCommands.push_back("quit");
    exitCommands.push_back("q");

}


struct RemoveDelimiter
{
    bool operator()(char c)
    {
        return (c =='\r' || c =='\t' || c == ' ' || c == '\n');
    }
};


bool isEmpty(string input)
{

    input.erase( std::remove_if( input.begin(), input.end(), RemoveDelimiter()), input.end());

    return input.length() == 0;
}



string getCommand()
{
    string input = "";

    cout << "Enter a command to continue... " << endl << endl;

    while(isEmpty(input))
    {
        getline(cin, input);
    }

    return input;
}



void printGreeting()
{
    cout << "Hello. Welcome the my simple HTTP Client. " << endl <<"  Please select from the following commands:" << endl << endl;
}


void printProgramInfo()
{
    cout << ".===================================================================." << endl;
    cout << "|  HTTP 1.1 Client                                                  |" << endl;
    cout << "|                                                                   |" << endl;
    cout << "|         Author:    Howie Catlin                                   |" << endl;
    cout << "|         Course:    CSS 432                                        |" << endl;
    cout << "|         Date:      2017.11.30                                     |" << endl;
    cout << "'==================================================================='" << endl << endl;
}


bool handleCommand(string input)
{
    string tmpInput = input;


    for(int i = 0; i < tmpInput.length(); i++)
        tmpInput[i] = tolower(tmpInput[i]);


    it = exitCommands.begin();

    for(it = exitCommands.begin(); it != exitCommands.end(); ++it)
    {
        if(tmpInput == *it)
        {
            printExit();
            return false;
        }
    }



    if(tmpInput.substr(0, 4) == "help")
    {
        handleHELP(tmpInput);
    }
    else if(tmpInput.substr(0, 3) == "get")
    {
        client->submitRequest(GET);
    }
    else if(tmpInput.substr(0, 4) == "head")
    {
        client->submitRequest(HEAD);
    }
    else if(tmpInput.substr(0, 7) == "options")
    {
        client->submitRequest(OPTIONS);
    }
    else if(tmpInput.substr(0, 5) == "patch")
    {
        client->submitRequest(PATCH);
    }
    else if(tmpInput.substr(0, 3) == "put")
    {
        client->submitRequest(PUT);
    }
    else if(tmpInput.substr(0, 4) == "post")
    {
        client->submitRequest(POST);
    }
    else if(tmpInput.substr(0, 6) == "delete")
    {
        client->submitRequest(DELETE);
    }
    else if(tmpInput.substr(0, 5) == "trace")
    {
        client->submitRequest(TRACE);
    }
    else if(tmpInput.substr(0, 7) == "connect")
    {
        client->submitRequest(CONNECT);
    }
    else if(tmpInput.substr(0, 4) == "save")
    {
        client->saveResponse();
    }

    else
        cout << "Could not understand command. Please try again" << endl;

    return true;
}


void printExit()
{
    cout << "thanks for playing " << endl;
};


void printCommands()
{
    cout << "\tHELP       | Prints these commands" << endl;
    cout << "\tHELP [cmd] | Prints detailed help information for the command" << endl;
    cout << "\tGET        | Performs a 'GET' request against a URL of your choosing" << endl;
    cout << "\tHEAD       | Performs a 'HEAD' request against a URL of your choosing" << endl;
    cout << "\tOPTIONS    | Performs an 'OPTIONS' request against a URL of your choosing" << endl;
    cout << "\tPATCH      | Performs a 'PATCH' request against a URL of your choosing" << endl;
    cout << "\tPOST       | Performs a 'POST' request against a test server" << endl;
    cout << "\tPUT        | Performs a 'PUT' request against a test server" << endl;
    cout << "\tDELETE     | Performs a 'DELETE' request against a test server" << endl;
    cout << "\tTRACE      | Performs a 'TRACE' request against a URL of your choosing" << endl;
    cout << "\tCONNECT    | Performs a 'CONNECT' request against a URL of your choosing" << endl<< endl;

};


void handleHELP(string input)
{
    input.erase(0,4);


    if(input.length() == 0)
    {
        printCommands();
        return;
    }

    it = exitCommands.begin();

    for(it = exitCommands.begin(); it != exitCommands.end(); ++it)
    {
        if(input == *it)
        {
            printExit();
            return;
        }
    }

    //remove the space between help [command]
    input.erase(0,1);

    if(input.substr(0, 4) == "help")
    {
        cout << "\tThe HELP command provides additional information about each" << endl;
        cout << "\tcommand." << endl << endl;
        return;
    }
    else if(input.substr(0, 3) == "get")
    {
        cout << "\tThe GET command guides you through issuing a 'GET' request" << endl;
        cout << "\tto a URL of your choosing. This request returns the page " << endl;
        cout << "\tmetadata and body and may take a while depending on page size." << endl;
        return;
    }
    else if(input.substr(0, 4) == "head")
    {
        cout << "\tThe HEAD command guides you through issuing a 'HEAD' request" << endl;
        cout << "\tto the URL of your choosing. This request returns the page " << endl;
        cout << "\tmetadata only." << endl << endl;
        return;
    }
    else if(input.substr(0, 7) == "options")
    {
        cout << "\tThe 'options' command guides you through issuing a 'OPTIONS' request " << endl;
        cout << "\tto a URL of your choosing. This request returns the page " << endl;
        cout << "\tmetadata only." << endl << endl;
        return;
    }
    else if(input.substr(0, 3) == "patch")
    {
        cout << "\tThe 'patch' command guides you through issuing a 'PATCH' request," << endl;
        cout << "\tto a URL of your choosing. This request returns the page " << endl;
        cout << "\tmetadata and body." << endl << endl;
        return;
    }
    else if(input.substr(0, 3) == "put")
    {
        cout << "\tThe 'put' command guides you through issuing a 'PUT' request," << endl;
        cout << "\tincluding asking for a string to write to the webpage." << endl;
        cout << "\tDue to the need to submit information to the page, this " << endl;
        cout << "\trequest is being executed against httpbin.org for testing " << endl;
        cout << "\tpurposes." << endl << endl;
        return;
    }
    else if(input.substr(0, 4) == "post")
    {
        cout << "\tThe 'post' command guides you through issuing a 'POST' request," << endl;
        cout << "\tincluding asking for a series of parameters and values to submit." << endl;
        cout << "\tDue to the need to submit information to the page, this " << endl;
        cout << "\trequest is being executed against httpbin.org for testing " << endl;
        cout << "\tpurposes." << endl << endl;
        return;
    }
    else if(input.substr(0, 6) == "delete")
    {
        cout << "\tThe 'delete' command guides you through issuing a 'DELETE' request." << endl;
        cout << "\tDue to the need to run against a server, this request uses" << endl;
        cout << "\thttpbin.org for testing purposes." << endl << endl;
        return;
    }
    else if(input.substr(0, 5) == "trace")
    {
        cout << "\tThe 'trace' command guides you through issuing a 'TRACE' request " << endl;
        cout << "\tto a URL of your choosing. This request returns the page " << endl;
        cout << "\tmetadata only." << endl << endl;
        return;
    }
    else if(input.substr(0, 7) == "connect")
    {
        cout << "\tThe 'connect' command guides you through issuing a 'CONNECT' request " << endl;
        cout << "\tto a URL of your choosing. This request returns the page " << endl;
        cout << "\tmetadata only." << endl << endl;
        return;
    }
    else if(input.substr(0, 4) == "save")
    {
        cout << "\tThe 'save' command allows you to save a previous server response " << endl;
        cout << "\tto file. A request must be made before the response can be saved, " << endl;
        cout << "\tand the content of the response will depend on the request made. " << endl;
        cout << "\tAn MD5 hash of the response content is printed to screen to ensure" << endl;
        cout << "\tfile integrity. " << endl << endl;
        return;
    }
    else
    {
        cout << "Could not understand your HELP request. These are the available commands:" << endl << endl;
        printCommands();
        return;
    }
};



