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
#include <pthread.h>


#include "HTTPClient.h"
using namespace std;

#define DEBUG true

const int BOX_LENGTH = 64;
vector<string> exitCommands;
vector<string>::iterator it;

const string dashLine   = "----------------------------------------------------------------";
const string doubleLine = "================================================================";

string padString(string input, int length, char padding, bool padFront);
string padString(string input, int length);

void printStartup();
void printProgramInfo();
void printGreeting();
void printInstructions();
void printCommands();
void printExit();

string getUserCommand();

bool handle_Input(string input);
void handle_HELP(string input);
void handle_GET(string input);
void handle_HEAD(string input);
void handle_OPTIONS(string input);
void handle_PUT(string input);
void handle_POST(string input);
void handle_TRACE(string input);
void handle_DELETE(string input);
void handle_CONNECT(string input);

void initExitCommands();


/*
int main(int argc, char* argv[])
{
    //WSADATA wsaData;
    //SOCKET socket;
    int sd;
    sockaddr_in SockAddr;
    int lineCount = 0;
    int rowCount = 0;
    struct hostent * host;
    locale local;
    char buffer[10000];
    int i = 0;
    int nDataLength;
    string website_HTML;

    string url = "httpbin.org";

    string get_http = string("GET /ip HTTP/1.1") + "\r\nHost: " + url + "\r\n\r\n";      //Connection: close\r\n\r\n";
    //string get_http = "GET /ip HTTP/1.1\nHost: " + url + "\n\n";      //Connection: close\r\n\r\n";
    //string get_http = "GET /ip HTTP/1.1\r\nHost: " + url + "\n\n";    //Connection: close\r\n\r\n";
    //string get_http = "GET /ip HTTP/1.1\r\nHost: " + url + "\r\n\r\n";//Connection: close\r\n\r\n";

    cout << "get_http = " << endl << "'" << get_http << "'" << endl;

    //if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    //{
    //    cout << "WSAStartup failed.\n";
    //    System("pause");
    //    //return 1;
    //}


    sd =socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    host = gethostbyname(url.c_str());

    SockAddr.sin_port=htons(80);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    if(connect(sd,(sockaddr*)(&SockAddr),sizeof(SockAddr)) != 0)
    {
        cout << "Could not connect";
        system("pause");
        //return 1;
    }

    //send GET / HTTP
    send(sd, get_http.c_str(), strlen(get_http.c_str()), 0);


    while((nDataLength = recv(sd, buffer, 10000, 0)) > 0)
    {
        int i = 0;
        while(buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
        {
            website_HTML += buffer[i];
            i += 1;
        }
        //cout << "i = " << i << endl;
    }

    close(sd);
    //WSACleanup();

    cout << website_HTML;

    cout << "\n\nPress ANY key to close.\n\n";
    cin.ignore();
    cin.get();

    return 0;
}
*/

HTTPClient client;

int main(int argc, char* argv[])
{
    initExitCommands();

    printStartup();



    string userCMD;

    userCMD  = getUserCommand();

    while(handle_Input(userCMD))
    {
        userCMD  = getUserCommand();
    }

    /*

    const int bufLen = 1024;
    long fileSize;
    char* memBuffer;
    char* headerBuffer;
    FILE *fp;

    memBuffer = NULL;
    headerBuffer = NULL;


    string destURL;
    string website_HTML;

    cin >> destURL;

    while(destURL != "q")
    {

        memBuffer = client.readHTML(destURL.c_str(), fileSize, &headerBuffer);
        printf("returned from readURL\n");
        printf("data returned:\n%s", memBuffer);
        //if(fileSize != 0)
        if(client.bytesReturned != 0)
        {
            printf("got some data\n");
            fp = fopen("download.html", "wb");
            fwrite(memBuffer, 1, client.bytesReturned, fp);
            fclose(fp);
            delete(memBuffer);
            delete(headerBuffer);
        }

        cout << endl;
        cout << "Enter a new URL or 'q' to quit" << endl;
        cin >> destURL;
    }
    */
}

void initExitCommands()
{
    exitCommands.push_back("exit");
    exitCommands.push_back("quit");
    exitCommands.push_back("q");

}



string padString(string input, int length, char padding, bool padFront)
{
    if(input.length() >= length)
        return input;

    int diff = length - input.length();
    char addition[diff];

    for(int i = 0; i < diff; i++)
        addition[i] = padding;

    if(padFront)
        return string(addition) + input;

    return input + string(addition);
}

string padString(string input, int length)
{
    return padString(input, length, ' ', false);
}

string getUserCommand()
{
    string input;

    cout << "Enter a command to continue " << endl << endl;

    cin >> input;

    return input;
}

void printStartup()
{
    printProgramInfo();
    printGreeting();
    printInstructions();
    printCommands();
}

void printGreeting()
{
    cout << "\tHello. Welcome the my simple HTTP Client." << endl << endl;
}

void printProgramInfo()
{
    string boxSpacer = padString("|", BOX_LENGTH - 1) + "|";

    cout << doubleLine << endl;
    cout << padString("|    HTTP 1.1 Client", BOX_LENGTH - 1) << "|" << endl;
    cout << boxSpacer << endl;
    cout << padString("| Author: Howie Catlin ", BOX_LENGTH -1) << "|" << endl;
    cout << padString("| Course: CSS 432", BOX_LENGTH -1) << "|" << endl;
    cout << padString("| Date:   2017.11.30", BOX_LENGTH -1) << "|" << endl;
    cout << doubleLine << endl << endl;
}


void printInstructions()
{
    cout << "Use the following commands to operate the client: " << endl;
}

bool handle_Input(string input)
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


    if(tmpInput.substr(0, 4) == "help")    { handle_HELP(input);    return true; }
    if(tmpInput.substr(0, 3) == "get")     { handle_GET(input);     return true; }
    if(tmpInput.substr(0, 4) == "head")    { handle_HEAD(input);    return true; }
    if(tmpInput.substr(0, 7) == "options") { handle_OPTIONS(input); return true; }
    if(tmpInput.substr(0, 4) == "post")    { handle_POST(input);    return true; }
    if(tmpInput.substr(0, 3) == "put")     { handle_PUT(input);     return true; }
    if(tmpInput.substr(0, 6) == "delete")  { handle_DELETE(input);  return true; }
    if(tmpInput.substr(0, 5) == "trace")   { handle_TRACE(input);   return true; }
    if(tmpInput.substr(0, 7) == "connect") { handle_TRACE(input);   return true; }



    cout << "Could not understand command. Please try again" << endl;
    return true;
}

void printExit()
{
    cout << "thanks for playing " << endl;
};


void printCommands()
{
    cout << "\tHELP - prints these commands" << endl;
    cout << "\tHELP [command] - prints detailed help information for the command" << endl << endl;
    cout << "\tGET [address] - performs a 'GET' command against the provided address" << endl;
    cout << "\tHEAD [address] - retrieves the metadata from the provided address" << endl;
    cout << "\tOPTIONS - " << endl;
    cout << "\tPOST - " << endl;
    cout << "\tPUT - " << endl;
    cout << "\tDELETE - " << endl;
    cout << "\tTRACE [] - " << endl;
    cout << "\tCONNECT [] - " << endl;
};

void handle_HELP(string input)
{
};
void handle_GET(string input)
{
    client.currReq = GET;

    client.getAddress();
    client.readHTML();

    client.printHTML();

    client.currReq = NONE;


};
void handle_HEAD(string input)
{
    client.currReq = HEAD;

    client.getAddress();
    client.readHTML();
    client.printHeader();

    client.currReq = NONE;
};
void handle_OPTIONS(string input)
{
    client.currReq = OPTIONS;

    client.getAddress();
    client.readHTML();
    client.printHeader();

    client.currReq = NONE;
};
void handle_POST(string input)
{
    client.currReq = POST;

    cout << "For testing purposes, POST will be sent to httpbin.org" << endl;
    client.setServerName("httpbin.org");
    client.setFilePath("/post");

    client.readHTML();
    client.printHTML();

    client.currReq = NONE;
};
void handle_PUT(string input)
{
    client.currReq = PUT;

    cout << "For testing purposes, PUT will be sent to httpbin.org" << endl;
    client.setServerName("httpbin.org");
    client.setFilePath("/put");

    client.readHTML();
    client.printHTML();

    client.currReq = NONE;
};
void handle_DELETE(string input)
{
    client.currReq = DELETE;

    cout << "For testing purposes, DELETE will be sent to httpbin.org" << endl;
    client.setServerName("httpbin.org");
    client.setFilePath("/delete");

    client.readHTML();
    client.printHTML();

    client.currReq = NONE;
};
void handle_TRACE(string input)
{
    client.currReq = TRACE;

    client.getAddress();
    client.readHTML();
    client.printHeader();

    client.currReq = NONE;
};
void handle_CONNECT(string input)
{
    client.currReq = TRACE;

    client.getAddress();
    client.readHTML();
    client.printHeader();

    client.currReq = NONE;
};

/*
HINSTANCE hInst;
WSADATA wsaData;

void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename);
SOCKET connectToServer(char *szServerName, WORD portNum);
int getHeaderLength(char *content);
char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut);


int main()
{
    const int bufLen = 1024;
    char *szUrl = "http://stackoverflow.com";
    long fileSize;
    char *memBuffer, *headerBuffer;
    FILE *fp;

    memBuffer = headerBuffer = NULL;

    if ( WSAStartup(0x101, &wsaData) != 0)
        return -1;


    memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
    printf("returned from readUrl\n");
    printf("data returned:\n%s", memBuffer);
    if (fileSize != 0)
    {
        printf("Got some data\n");
        fp = fopen("downloaded.file", "wb");
        fwrite(memBuffer, 1, fileSize, fp);
        fclose(fp);
         delete(memBuffer);
        delete(headerBuffer);
    }

    WSACleanup();
    return 0;
}


void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename)
{
    string::size_type n;
    string url = mUrl;

    if (url.substr(0,7) == "http://")
        url.erase(0,7);

    if (url.substr(0,8) == "https://")
        url.erase(0,8);

    n = url.find('/');
    if (n != string::npos)
    {
        serverName = url.substr(0,n);
        filepath = url.substr(n);
        n = filepath.rfind('/');
        filename = filepath.substr(n+1);
    }

    else
    {
        serverName = url;
        filepath = "/";
        filename = "";
    }
}



int getHeaderLength(char *content)
{
    const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
    char *findPos;
    int ofset = -1;

    findPos = strstr(content, srchStr1);
    if (findPos != NULL)
    {
        ofset = findPos - content;
        ofset += strlen(srchStr1);
    }

    else
    {
        findPos = strstr(content, srchStr2);
        if (findPos != NULL)
        {
            ofset = findPos - content;
            ofset += strlen(srchStr2);
        }
    }
    return ofset;
}

char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut)
{
    const int bufSize = 512;
    char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
    char *tmpResult=NULL, *result;
    SOCKET conn;
    string server, filepath, filename;
    long totalBytesRead, thisReadSize, headerLen;

    mParseUrl(szUrl, server, filepath, filename);

    ///////////// step 1, connect //////////////////////
    conn = connectToServer((char*)server.c_str(), 80);

    ///////////// step 2, send GET request /////////////
    sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
    strcpy(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    sprintf(tmpBuffer, "Host: %s", server.c_str());
    strcat(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    strcat(sendBuffer, "\r\n");
    send(conn, sendBuffer, strlen(sendBuffer), 0);

//    SetWindowText(edit3Hwnd, sendBuffer);
    printf("Buffer being sent:\n%s", sendBuffer);

    ///////////// step 3 - get received bytes ////////////////
    // Receive until the peer closes the connection
    totalBytesRead = 0;
    while(1)
    {
        memset(readBuffer, 0, bufSize);
        thisReadSize = recv (conn, readBuffer, bufSize, 0);

        if ( thisReadSize <= 0 )
            break;

        tmpResult = (char*)realloc(tmpResult, thisReadSize+totalBytesRead);

        memcpy(tmpResult+totalBytesRead, readBuffer, thisReadSize);
        totalBytesRead += thisReadSize;
    }

    headerLen = getHeaderLength(tmpResult);
    long contenLen = totalBytesRead-headerLen;
    result = new char[contenLen+1];
    memcpy(result, tmpResult+headerLen, contenLen);
    result[contenLen] = 0x0;
    char *myTmp;

    myTmp = new char[headerLen+1];
    strncpy(myTmp, tmpResult, headerLen);
    myTmp[headerLen] = NULL;
    delete(tmpResult);
    *headerOut = myTmp;

    bytesReturnedOut = contenLen;
    closesocket(conn);
    return(result);
}



/*
    WSADATA wsaData;
    SOCKET socket;
    SOCKADDR_IN SockAddr;
    int lineCount = 0;
    int rowCount = 0;
    struct hostent * host;
    locale local;
    char buffer[10000];
    int i = 0;
    int nDataLength;
    string website_HTML;

    string url = "www.google.com";

    string get_http = " GET / HTTP/1.1\r\n\Host: " + url + "\r\nConnection: close\r\n\r\n";

    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        cout << "WSAStartup failed.\n";
        System("pause");
        //return 1;
    }

    Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    host = gethostbyname(url.c_str());

    SockAddr.sin_port=htons(80);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        cout << "Could not connect";
        system("pause");
        //return 1;
    }

    //send GET / HTTP
    send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);

    while((nDataLength = recv(Socket, buffer, 10000, 0)) > 0)
    {
        int i = 0;
        while(buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
        {
            website_HTML += buffer[i];
            i += 1;
        }
    }

    closesocket(Socket);
    WSACleanup();

    cout << website_HTML;

    cout << "\n\nPress ANY key to close.\n\n";
    cin.ignore();
    cin.get();

    return 0;
 */

