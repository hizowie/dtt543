/*
 * TCPSocket.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: anon
 */

#include "TCPSocket.h"



TCPSocket::TCPSocket()
{
    DEBUG = true;

    if(DEBUG)
        cout << "TCPSocket::TCPSocket()" << endl;

    this->enabled = true;
    //this->isServer = false;

    this->sd = SD_UNDEFINED;

    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {
        this->remoteSD = SD_UNDEFINED;
        this->remoteLen = 0;

    }
    bytesReturned = 0;
};


TCPSocket::~TCPSocket()
{
    close(this->remoteSD);
    close(this->sd);
};




bool TCPSocket::connectSocket(int port, const char* servername)
{
    if(DEBUG)
        cout << "TCPSocket::connectSocket(" << port << ", (char*) " << servername << ")" << endl;


    if(DEBUG)
        cout << "\tconnect() - step 1" << endl;


    if((this->sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0)
    {
          cerr << "Cannot open a TCP socket." << endl;
          return false;
    }


    if(DEBUG)
            cout << "\tconnect() - step 2" << endl;


    setsockopt(this->sd, SOL_SOCKET, SO_REUSEADDR, (char*)&(this->enabled), sizeof(int));

    struct hostent *destination;


    if(DEBUG)
            cout << "\tconnect() - step 3" << endl;


    if(inet_addr(servername)==INADDR_NONE)
    {
        if(DEBUG)
                cout << "\tconnect() - step 4a" << endl;

        destination = gethostbyname(servername);
    }
    else
    {
        if(DEBUG)
                cout << "\tconnect() - step 4b" << endl;

        unsigned int addr = inet_addr(servername);
        destination = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }

    if(DEBUG)
            cout << "\tconnect() - step 5" << endl;

    if(destination == NULL)
    {
        if(DEBUG)
                cout << "\tconnect() - step 6" << endl;

        close(this->sd);
        return false;
    }


    if(DEBUG)
            cout << "\tconnect() - step 7" << endl;


    this->server.sin_addr.s_addr =*((unsigned long*)destination->h_addr);
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons(port);


    if(DEBUG)
            cout << "\tconnect() - step 8" << endl;


    //bzero((char*)&(this->localAddr), sizeof(this->localAddr));
    //this->localAddr.sin_family = AF_INET;
    //this->localAddr.sin_port = htons(port);
    //bcopy((char*)destination->h_addr, (char*)&(this->localAddr).sin_addr.s_addr, destination->h_length);


    if(DEBUG)
            cout << "\tconnect() - step 9" << endl;


    //if(connect(this->sd, (struct sockaddr*)&(this->localAddr), sizeof(this->localAddr)) < 0)
    if(connect(this->sd, (struct sockaddr*)&(this->server), sizeof(this->server)) < 0)
    {
        if(DEBUG)
                cout << "\tconnect() - step 10" << endl;


        cerr << "Failed to create TCP connection " << endl;
        return false;
    }

    if(DEBUG)
            cout << "\tconnect() - step 11" << endl;

    return true;
};
/*
char* TCPSocket::readURL(string URL, long &bytesReturnedOut, char **headerOut)
{
    return readURL(URL.c_str(), bytesReturnedOut, headerOut);
}
*/
char* TCPSocket::readURL(const char* URL, long &bytesReturnedOut, char **headerOut)
{
    if(DEBUG)
            cout << "\treadURL(" <<URL <<", " << bytesReturnedOut << ", " << headerOut << ")" << endl;

    if(DEBUG)
            cout << "\treadURL() - step 1" << endl;

    const int bufSize = 512;
    char readBuffer[bufSize];
    char sendBuffer[bufSize];
    char  tmpBuffer[bufSize];
    char* tmpResult = NULL;
    char* result;
    //string server, filepath, filename;
    long totalBytesRead, thisReadSize, headerLen;

    //split the URL into server name, file path, and file name
    parseURL(URL);


    if(DEBUG)
            cout << "\treadURL() - step 2" << endl;


    //connect to the server
    connectSocket(80, this->servername);


    if(DEBUG)
            cout << "\treadURL() - step 3 " << endl;



    //send the get request
    /*

    sprintf(tmpBuffer, "GET %s HTTP/1.1", this->filepath.c_str());
    strcpy(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    sprintf(tmpBuffer, "Host: %s", this->servername.c_str());
    strcat(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    strcat(sendBuffer, "\r\n");
    */
    sprintf(sendBuffer, "GET %s HTTP/1.1", this->filepath.c_str());
    strcat(sendBuffer, "\r\n");
    sprintf(tmpBuffer, "Host: %s", this->servername.c_str());
    strcat(tmpBuffer, "\r\n\r\n");
    strcat(sendBuffer, tmpBuffer);


    send(this->sd, sendBuffer, strlen(sendBuffer), 0);


    printf("Buffer being sent:\n%s", sendBuffer);
    printf("\tlength:%i", strlen(sendBuffer));


    if(DEBUG)
            cout << "\treadURL() - step 4" << endl;


    //get received bytes
    totalBytesRead = 0;
    while(true)
    {
        memset(readBuffer, 0, bufSize);
        thisReadSize = recv(this->sd, readBuffer, bufSize, 0);

        if(thisReadSize <= 0)
            break;

        tmpResult = (char*) realloc(tmpResult, thisReadSize + totalBytesRead);
        totalBytesRead += thisReadSize;
    }

    if(DEBUG)
            cout << "\treadURL() - step 5" << endl;


    headerLen = getHeaderLength(tmpResult);
    long contentLen = totalBytesRead - headerLen;
    result = new char[contentLen + 1];
    memcpy(result, tmpResult + headerLen, contentLen);
    result[contentLen] = 0x0;
    char * myTmp;

    if(DEBUG)
            cout << "\treadURL() - step 6" << endl;



    myTmp = new char[headerLen + 1];
    strncpy(myTmp, tmpResult, headerLen);
    myTmp[headerLen] = 0;
    delete(tmpResult);
    *headerOut = myTmp;

    bytesReturnedOut = contentLen;



    *this->header = myTmp;
    this->bytesReturned = contentLen;


    if(DEBUG)
    {
        cout << "\treadURL() - step 7" << endl;
        cout << "\treadURL(" <<URL <<", " << bytesReturnedOut << ", " << headerOut << ")" << endl;
    }


    close(this->sd);
    return (result);
}

/*
bool TCPSocket::sendTo(char msg[], int len)
{
    if(DEBUG)
           cout << "TCPSocket::sendTo(" << msg << ", " << len << ")" << endl;


    return (sendto(this->sd, msg, len, 0, (sockaddr *)&(this->remoteAddr), sizeof(remoteAddr))) > 0;
}
*/



const char* TCPSocket::createGETRequest(const string url)
{
    if(DEBUG)
           cout << "TCPSocket::createGETRequest(" << url << ")" << endl;


    return createGETRequest(url.c_str());
}


const char* TCPSocket::createGETRequest(const char* url)
{
    if(DEBUG)
           cout << "TCPSocket::createGETRequest(" << url << ")" << endl;

    const int bufSize = 512;
    char readbufffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
    char *tmpResult = NULL;
    char * result;
    long totalBytesRead, thisReadSize, headerLen;

    sprintf(tmpBuffer, "GET %s HTTP/1.1", this->filepath.c_str());
    strcpy(sendBuffer,tmpBuffer);
    strcat(sendBuffer, "\r\n");
    sprintf(tmpBuffer, "Host: %s", this->servername.c_str());
    strcat(sendBuffer,tmpBuffer);
    strcat(sendBuffer, "\r\n");
    strcat(sendBuffer, "\r\n");


    string output = string("GET / HTTP/1.1\r\nhost:") + url + "\r\nConnection: close\r\n\r\n";
    return output.c_str();
}


bool TCPSocket::sendRequest(string msg)
{
    if(DEBUG)
           cout << "TCPSocket::sendRequest((string) " << msg << ")" << endl;


    return sendRequest(msg.c_str());
}


bool TCPSocket::sendRequest(const char* msg)
{
    if(DEBUG)
           cout << "TCPSocket::sendRequest((char*) " << msg << ")" << endl;


    return (send(this->sd, msg, strlen(msg), 0) > 0);
}


bool TCPSocket::recV()
{
    if(DEBUG)
           cout << "TCPSocket::recv()" << endl;

    return (recv(this->sd, this->buffer, sizeof(this->buffer), 0) > 0);
}


string TCPSocket::readHTML()
{
    if(DEBUG)
           cout << "TCPSocket::readHTML()" << endl;


    string output  = "";

    int i = 0;
    while(buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
    {
        output+= buffer[i];
        i++;
        //i+=1;
    }

    return output;
}


string TCPSocket::receiveReply()
{
    if(DEBUG)
           cout << "TCPSocket::receiveReply()" << endl;


    string output = "";

    while(recV())
    {
        output += readHTML();

    }
    return output;
}


void TCPSocket::parseURL(const char* URL)
{
    parseURL(string(URL));
}


void TCPSocket::parseURL(string URL)
{
    if(URL.substr(0,7) == "http://")
        URL.erase(0,7);

    if(URL.substr(0,8) == "https://")
        URL.erase(0,8);


    string::size_type n = URL.find('/');


    if(n != string::npos)
    {
        this->servername = URL.substr(0, n);
        this->filepath = URL.substr(n);
        n = filepath.rfind('/');
        this->filename = filepath.substr(n + 1);
    }
    else
    {
        this->servername = URL;
        this->filepath = "/";
        this->filename = "";
    }
}


int TCPSocket::getHeaderLength(string content)
{
    return getHeaderLength(content.c_str());
}


int TCPSocket::getHeaderLength(char * content)
{
    const char* srchStr1 = "\r\n\r\n";
    const char* srchStr2 = "\n\r\n\r";
    char * findPos;
    int offset = -1;

    findPos = strstr(content, srchStr1);
    if(findPos != NULL)
    {
        offset = findPos - content;
        offset += strlen(srchStr1);
    }
    else
    {
        findPos = strstr(content, srchStr2);
        if(findPos != NULL)
        {
            offset = findPos - content;
            offset += strlen(srchStr2);
        }
    }
    return offset;
}



bool TCPSocket::connectSocket(const int port, const string server)
{
    if(DEBUG)
           cout << "TCPSocket::connectSocket(" << port << ", (string) " << server << ")" << endl;


    return this->connectSocket(port, server.c_str());
}





bool TCPSocket::pollRecvFrom()
{
    if(DEBUG)
           cout << "TCPSocket::pollRecvFrom()" << endl;


    struct pollfd pfd[1];
    pfd[0].fd = this->sd;       //declare I'm checking data availability of SD
    pfd[0].events = POLLRDNORM; //declare only interested in reading from SD

    // check now; poll returns 0 or -1 if no data is available
    return poll(pfd, 1, 0) > 0;
}


/*
bool TCPSocket::connectSocket(int port)
{
    if(DEBUG)
        cout << "TCPSocket::connectSocket(" << port << ")" << endl;

    //this->isServer = true;

    if(!openSocket()
        || !bindSocket(port)
        || !listenSocket()
        || !acceptSocket())
    {
        return false;
    }


    return true;
};

bool TCPSocket::bindSocket(int port)
{
    if(DEBUG)
        cout << "TCPSocket::bindSocket(" << port << ")" << endl;

    // Bind our local address
    this->localAddr.sin_addr.s_addr = INADDR_ANY;
    this->localAddr.sin_port = htons(port);
    this->localAddr.sin_family = AF_INET;
    bzero((char*) &(this->localAddr).sin_zero, sizeof(this->localAddr));

    if(bind(this->sd, (struct sockaddr *) &(this->localAddr), sizeof(this->localAddr)) < 0)
    {
        cerr << "Cannot bind while opening TCP socket "<< endl;
        return false;
    }

    return true;
};


bool TCPSocket::listenSocket()
{
    if(DEBUG)
        cout << "TCPSocket::listenSocket()" << endl;

    if(listen(this->sd, MAX_CONNECTIONS) < 0)
    {
        cerr << "Cannot listen to TCP socket " << endl;
        return false;
    }

    return true;
};


bool TCPSocket::acceptSocket()
{
    if(DEBUG)
        cout << "TCPSocket::acceptSocket()" << endl;

    this->remoteLen = sizeof(this->remoteAddr);
    this->remoteSD = accept(this->sd, (struct sockaddr *) &(this->remoteAddr), &(this->remoteLen));

    if(this->remoteSD < 0)
    {
        cerr << "Cannot to accept socket " << endl;
        return false;
    }

    return true;
};


bool TCPSocket::ackTo(char msg[], int len)
{
    if(DEBUG)
           cout << "TCPSocket::ackTo(" << msg << ", " << len << ")" << endl;


    return (sendto(this->sd, msg, len, 0, &srcAddr, sizeof(srcAddr))) > 0;
}

bool TCPSocket::recvFrom(char msg[], int len)
{
    if(DEBUG)
           cout << "TCPSocket::recvFrom(" << msg << ", " << len << ")" << endl;


    socklen_t addrlen = sizeof(this->srcAddr);
    bzero((char *)&this->srcAddr, sizeof(this->srcAddr) );

    // return the number of bytes received
    return (recvfrom(this->sd, msg, len, 0, &(this->srcAddr), &addrlen)) > 0;
}

bool TCPSocket::openSocket()
{
    if(DEBUG)
        cout << "TCPSocket::openSocket()" << endl;

    //if((this->sd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
    if((this->sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0)
    {
          cerr << "Cannot open a TCP socket." << endl;
          return false;
    }

    setsockopt(this->sd, SOL_SOCKET, SO_REUSEADDR, (char*)&(this->enabled), sizeof(int));
    return true;
};
*/




