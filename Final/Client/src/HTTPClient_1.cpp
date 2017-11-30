/*
 * HTTPClient_1.cpp
 *
 *  Created on: Nov 29, 2017
 *      Author: anon
 */


#include "HTTPClient.h"


HTTPClient::HTTPClient()
{
    DEBUG = false;
    this->debugIndentLength = 0;

    if(DEBUG)
        cout << getDebugIndent() << "HTTPClient::HTTPClient()" << endl;

    this->enabled = true;
    this->sd = SD_UNDEFINED;
    this->serverLen = 0;
    //this->bytesReturned = 0;

    this->currReq = NONE;
};


HTTPClient::~HTTPClient()
{
    if(DEBUG)
        cout << getDebugIndent() << "HTTPClient::~HTTPClient()" << endl;

    close(this->sd);
};


string HTTPClient::getDebugIndent()
{
    if(dIndent.length() == debugIndentLength)
        return dIndent;


    if(dIndent.length() < debugIndentLength)
    {
        for(int i = debugIndentLength - dIndent.length(); i > 0; --i)
            dIndent += "\t";
    }
    else
    {
        dIndent = "";

        for(int i = debugIndentLength; i > 0; --i)
            dIndent += "\t";
    }

    return dIndent;
}


bool HTTPClient::connectSocket(const char* server)//, const int port)
{
    if(DEBUG)
    {
        cout << getDebugIndent() << "HTTPClient::connectSocket((char*) " << server << ")" << endl;
        this->debugIndentLength += 1;
    }


    if(DEBUG)
        cout << getDebugIndent() << "connect() - step 1" << endl;


    if((this->sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0)
    {
          cerr << getDebugIndent() << "Cannot open a TCP socket." << endl;
          return false;
    }


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 2" << endl;


    setsockopt(this->sd, SOL_SOCKET, SO_REUSEADDR, (char*)&(this->enabled), sizeof(int));

    struct hostent *dest;


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 3" << endl;


    if(inet_addr(server)==INADDR_NONE)
    {
        if(DEBUG)
                cout << getDebugIndent() << "connect() - step 4a" << endl;

        dest = gethostbyname(server);
    }
    else
    {
        if(DEBUG)
                cout << getDebugIndent() << "connect() - step 4b" << endl;

        unsigned int addr = inet_addr(server);
        dest = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }

    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 5" << endl;

    if(dest == NULL)
    {
        if(DEBUG)
                cout << getDebugIndent() << "connect() - step 6" << endl;

        close(this->sd);
        return false;
    }


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 7" << endl;


    //this->serverAddr.sin_addr.s_addr =*((char*)dest->h_addr);
    //this->serverAddr.sin_family = AF_INET;
    //this->serverAddr.sin_port = htons(80);//port);


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 8" << endl;


    bzero((char*)&(this->serverAddr), sizeof(this->serverAddr));
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(80);//port);
    bcopy((char*)dest->h_addr, (char*)&(this->serverAddr).sin_addr.s_addr, dest->h_length);


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 9" << endl;

    cout << "Attempting to connect TCP Socket to " << this->servername << endl;

    this->serverLen = sizeof(this->serverAddr);
    //if(connect(this->sd, (struct sockaddr*)&(this->localAddr), sizeof(this->localAddr)) < 0)

    if(connect(this->sd, (struct sockaddr*)&(this->serverAddr), sizeof(this->serverAddr)) < 0)
    {
        if(DEBUG)
        {
            cout << getDebugIndent() << "connect() - step 10 [BAD!]" << endl;
            this->debugIndentLength -= 1;
        }

        cerr << "Failed to create TCP connection " << endl;
        return false;
    }


    if(DEBUG)
    {
        cout << getDebugIndent() << "connect() - step 11 [OK]" << endl;
        this->debugIndentLength -= 1;
    }
    return true;
};


bool HTTPClient::connectSocket(string server)//, const int port)
{
    return connectSocket(server.c_str());
};


void HTTPClient::parseURL(const char* URL)
{
    parseURL(string(URL));
}


void HTTPClient::parseURL(string URL)
{
    if(DEBUG)
    {
        cout << getDebugIndent() << "parseURL(" << URL << ")" << endl;
        this->debugIndentLength += 1;
    }

    if(DEBUG)
        cout << getDebugIndent() << "parseURL() - step 1" << endl;

    if(URL.substr(0,7) == "http://")
        URL.erase(0,7);

    if(DEBUG)
        cout << getDebugIndent() << "parseURL() - step 2; URL = " << URL <<endl;


    if(URL.substr(0,8) == "https://")
        URL.erase(0,8);

    if(DEBUG)
        cout << getDebugIndent() << "parseURL() - step 3; URL = " << URL <<endl;


    string::size_type n = URL.find('/');


    if(n != string::npos)
    {
        this->servername = URL.substr(0, n);
        this->filepath = URL.substr(n);
        n = filepath.rfind('/');
        this->filename = filepath.substr(n + 1);


        if(DEBUG)
        {
            cout << getDebugIndent() << "parseURL() - step 4a" << endl;
            cout << getDebugIndent() << "\tthis->servername = " << this->servername << endl;
            cout << getDebugIndent() << "\tthis->filepath = " << this->filepath << endl;
            cout << getDebugIndent() << "\tthis->filename = " << this->filename << endl;
        }
    }
    else
    {
        this->servername = URL;
        this->filepath = "/";
        this->filename = "";

        if(DEBUG)
        {
            cout << getDebugIndent() << "parseURL() - step 4b" << endl;
            cout << getDebugIndent() << "\tthis->servername = " << this->servername << endl;
            cout << getDebugIndent() << "\tthis->filepath = " << this->filepath << endl;
            cout << getDebugIndent() << "\tthis->filename = " << this->filename << endl;
        }
    }

    if(DEBUG)
        this->debugIndentLength -=1;
}


int HTTPClient::getHeaderLength(string content)
{
    return getHeaderLength(content.c_str());
}


int HTTPClient::getHeaderLength(char * content)
{
    if(DEBUG)
    {
        cout << getDebugIndent() <<"getHeaderLength(" << content << ")" << endl;
        this->debugIndentLength += 1;
    }


    const char* srchStr1 = "\r\n\r\n";
    const char* srchStr2 = "\n\r\n\r";
    const char* srchStr3 = "\n\n";
    char * findPos;
    int offset = -1;

    findPos = strstr(content, srchStr1);

    //if(DEBUG) cout << getDebugIndent() << "getHeaderLength() - step 1; findPos =  " << findPos << endl;


    if(findPos != NULL)
    {

        offset = findPos - content;
        offset += strlen(srchStr1);

        if(DEBUG) cout << getDebugIndent() << "getHeaderLength() - step 2a;  offset = " << offset << endl;
    }
    else
    {
        if(DEBUG) cout << getDebugIndent() << "getHeaderLength() - step 2b; offset = " << offset << endl;

        findPos = strstr(content, srchStr2);
        if(findPos != NULL)
        {
            offset = findPos - content;
            offset += strlen(srchStr2);

            if(DEBUG) cout << getDebugIndent() << "\tgetHeaderLength() - step 2b.1; offset = " << offset << endl;
        }
        else
        {
            findPos = strstr(content, srchStr3);
            if(findPos != NULL)
            {
                offset = findPos - content;
                offset += strlen(srchStr3);


                if(DEBUG) cout << getDebugIndent() << "\tgetHeaderLength() - step 2b.2;  offset = " << offset << endl;
            }
        }
    }

    if(DEBUG)
    {
        this->debugIndentLength -= 1;
    }

    return offset;
}


bool HTTPClient::pollRecvFrom()
{

    //if(DEBUG)
    //       cout << getDebugIndent() << "HTTPClient::pollRecvFrom()" << endl;


    struct pollfd pfd[1];
    pfd[0].fd = this->sd;       //declare I'm checking data availability of SD
    pfd[0].events = POLLRDNORM; //declare only interested in reading from SD

    // check now; poll returns 0 or -1 if no data is available
    return poll(pfd, 1, 0) > 0;
}
