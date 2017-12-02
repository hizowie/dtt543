/*
 * HTTPClient_1.cpp
 *
 *  Created on: Nov 29, 2017
 *      Author: anon
 */


#include "HTTPClient.h"

HTTPClient::HTTPClient()
{
        //debug stuff
        this->DEBUG = false;

        if(DEBUG)
            cout << getDebugIndent() << "HTTPClient::HTTPClient()" << endl;

        this->debugIndentLength = 0;
        this->dIndent = "";

        //socket stuff
        this->enabled = true;
        this->sd = SD_UNDEFINED;

        //request stuff
        this->currReq = NONE;
        this->servername = "";
        this->filepath = "/";
        this->filename = "";
        this->request = "";
        this->requestLen = 0x0;
        this->reqContent = "";
};


HTTPClient::~HTTPClient()
{
    if(DEBUG)
        cout << getDebugIndent() << "HTTPClient::~HTTPClient()" << endl;

    close(this->sd);
};


void HTTPClient::reset()
{
    //debug stuff

    if(DEBUG)
        cout << getDebugIndent() << "HTTPClient::reset()" << endl;

    this->debugIndentLength = 0;
    this->dIndent = "";

    //socket stuff
    this->enabled = true;
    this->sd = SD_UNDEFINED;

    //request stuff
    this->currReq = NONE;
    this->servername = "";
    this->filepath = "/";
    this->filename = "";
    this->request = "";
    this->requestLen = 0x0;
    this->reqContent = "";
}


void HTTPClient::saveResponse()
{
    if(this->responseHeader == "")
    {
        cout << "No Data to save. Please make a request first. " << endl << endl;
        return;
    }

    if(this->headerLength != 0)
    {
        std::ofstream out("downloaded.file");

        string fileData = this->responseHeader + "\r\n" + this->responseBody;

        out << fileData;
        out.close();


        cout << "File data written to 'download.file'" << endl;
        cout << "  MD5: " << md5(fileData) << endl << endl;
    }
}


bool HTTPClient::submitRequest(REQUEST req)
{
    bool result = false;
        switch(req)
        {
            case GET:
            {
                this->currReq = GET;

                result = sendRequest();

                if(result)
                    result = getResponse();

                printResponse();
                break;
            }
            case HEAD:
            {
                this->currReq = HEAD;
                sendRequest();
                getResponse();
                printHeader();
                break;
            }
            case OPTIONS:
             {
                 this->currReq = OPTIONS;
                 result = sendRequest();

                 if(result)
                     result = getResponse();

                 printHeader();
                 break;
             }
             case PATCH:
             {
                 this->currReq = PATCH;
                 sendRequest();
                 getResponse();
                 printResponse();
                 break;
             }
            case POST:
            {
                this->currReq = POST;
                cout << "For testing purposes, POST will be sent to httpbin.org" << endl;
                cout << "\tNote: this can be changed in HTTPClient::submitRequest()" << endl << endl;
                setServerName("httpbin.org");
                setFilePath("/post");

                result = sendRequest();

                if(result)
                    result = getResponse();

                printResponse();
                break;
            }
            case PUT:
            {
                this->currReq = PUT;
                cout << "For testing purposes, PUT will be sent to httpbin.org" << endl;
                cout << "\tNote: this can be changed in HTTPClient::submitRequest()" << endl << endl;
                setServerName("httpbin.org");
                setFilePath("/put");

                result = sendRequest();

                if(result)
                    result = getResponse();

                printResponse();
                break;
            }
            case DELETE:
            {
                this->currReq = DELETE;
                cout << "For testing purposes, DELETE will be sent to httpbin.org" << endl;
                cout << "\tNote: this can be changed in HTTPClient::submitRequest()" << endl << endl;
                setServerName("httpbin.org");
                setFilePath("/delete");

                result = sendRequest();

                if(result)
                    result = getResponse();

                printResponse();
                break;
            }
            case TRACE:
            {
                this->currReq = TRACE;
                result = sendRequest();

                if(result)
                    result = getResponse();

                printHeader();
                break;
            }
            case CONNECT:
            {
                this->currReq = CONNECT;
                result = sendRequest();

                if(result)
                    result = getResponse();

                printHeader();
                break;
            }
            case NONE:
                default:
            {
                cout << "Could not understand command. Please try again" << endl;
                break;
            }
        }


        return result;
}


void HTTPClient::printResponse()
{
    printHeader();
    printBody();
}


void HTTPClient::printBody()
{
    cout << "----[RESPSONSES BODY]------------------------------------------------" << endl;
    cout << this->responseBody <<endl;
    cout << "---------------------------------------------------------------------" << endl << endl;
}


void HTTPClient::printHeader()
{
    cout << "----[RESPONSE HEADER]------------------------------------------------" << endl;
    cout << this->responseHeader << endl;
    cout << "---------------------------------------------------------------------" << endl << endl;
}


bool HTTPClient::sendRequest()
{
    if(this->servername == "")
        getAddress();


    if(!this->connectSocket())
    {
        cout << "Cannot continue without a valid TCP connection " << endl;
        return false;
    }


    if(!this->buildRequest())
    {
        cout << "Cannot continue without a valid TCP connection " << endl;
        return false;
    }


    send(this->sd, this->request.c_str(), this->requestLen, 0);

    return true;
}


void HTTPClient::getAddress()
{
    string input;

    cout << "Please enter a URL to test against" << endl << endl;

    cin >> input;


    this->parseURL(input);
}


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


bool HTTPClient::connectSocket()
{
    if(DEBUG)
    {
        cout << getDebugIndent() << "HTTPClient::connectSocket((char*) " << this->servername << ")" << endl;
        this->debugIndentLength += 1;
    }

    if(this->servername == "")
        getAddress();

    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 1" << endl;



    struct hostent *dest;
    unsigned int addr;
    struct sockaddr_in tmpServer;
    int tempSD;


    tempSD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(tempSD <= 0)
    {
          cerr << getDebugIndent() << "Cannot open a TCP socket." << endl;
          return false;
    }


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 2" << endl;


    //setsockopt(this->sd, SOL_SOCKET, SO_REUSEADDR, (char*)&(this->enabled), sizeof(int));


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 3" << endl;


    if(inet_addr(this->servername.c_str()) == INADDR_NONE)
    {
        if(DEBUG)
            cout << getDebugIndent() << "connect() - step 4a" << endl;

        dest = gethostbyname(this->servername.c_str());
    }
    else
    {
        if(DEBUG)
            cout << getDebugIndent() << "connect() - step 4b" << endl;

        addr = inet_addr(this->servername.c_str());
        dest = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }

    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 5" << endl;

    if(dest == NULL)
    {
        if(DEBUG)
            cout << getDebugIndent() << "connect() - step 6" << endl;

        close(tempSD);
        this->sd = -1;
        return false;
    }


    if(DEBUG) cout << getDebugIndent() << "connect() - step 7" << endl;

     tmpServer.sin_addr.s_addr=*((unsigned long*)dest->h_addr);
     tmpServer.sin_family=AF_INET;
     tmpServer.sin_port=htons(80);


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 8" << endl;


    if(DEBUG)
            cout << getDebugIndent() << "connect() - step 9" << endl;

    cout << "Attempting to connect TCP Socket to " << this->servername << endl;


    if(connect(tempSD, (struct sockaddr *)&tmpServer, sizeof(tmpServer)) < 0)
    {
        cerr << "Failed to create TCP connection " << endl;

        if(DEBUG)
        {
            cout << getDebugIndent() << "connect() - step 10 [BAD!]" << endl;
            this->debugIndentLength -= 1;
        }

        close(tempSD);
        this->sd = -1;
        return false;
    }

    if(DEBUG)
    {
        cout << getDebugIndent() << "connect() - step 11 [OK]" << endl;
        this->debugIndentLength -= 1;
    }

     this->serverAddr = tmpServer;
     this->sd = tempSD;
     return true;
};


bool HTTPClient::buildRequest()
{
    const int bufSize = 512;
    char sendBuffer[bufSize];
    char tmpBuffer[bufSize];


    string nextLine = "\r\n";
    string httpVersion = " HTTP/1.1";


    string command = "";


    string host = "Host: " + string(this->servername.c_str()) + nextLine;
    string connection = "Connection: close" + nextLine;
    string contentType = "";
    string contentLength = "";
    string content = "";



    switch(this->currReq)
    {
    case GET:
        {
            command = "GET " + this->filepath + httpVersion + nextLine; break;
        }
    case HEAD:
        {
            command = "HEAD " + this->filepath + httpVersion + nextLine; break;
        }
    case OPTIONS:
        {
            command = "OPTIONS *" + httpVersion + nextLine; break;
        }
    case POST:
        {
            getPostParameters();
            buildPostContent();

            command = "POST " + this->filepath + httpVersion + nextLine;
            contentType = "Content-Type: application/x-www-form-urlencoded" + nextLine;
            contentLength = "Content-Length: " + IntToString(this->reqContent.length()) + nextLine + nextLine;
            content =  this->reqContent + nextLine;
            break;
        }
    case PUT:
        {
            getPutParameters();
            buildPutContent();

            command = "PUT " + this->filepath + httpVersion + nextLine;
            contentType = "Content-Type: application/x-www-form-urlencoded" + nextLine;
            contentLength = "Content-Length: " + IntToString(this->reqContent.length()) + nextLine + nextLine;
            content =  this->reqContent + nextLine;
            break;
        }
    case DELETE:
        {
            command = "DELETE " + this->filepath + httpVersion + nextLine; break;
        }
    case TRACE:
        {
            command = "TRACE " + this->filepath + httpVersion + nextLine; break;
        }
    case PATCH:
        {
            command = "PATCH " + this->filepath + httpVersion + nextLine; break;
        }
    case CONNECT:
        {
            command = "CONNECT " + this->filepath + httpVersion + nextLine; break;
        }
    default:
        cout << "There is currently no request command selected " << endl;
        return false;

    }


    strcpy(tmpBuffer, command.c_str());
    strcpy(sendBuffer, tmpBuffer);
    if(DEBUG) cout << getDebugIndent() << "\t3.1. sendBuffer: '" << sendBuffer << "'" << endl;

    strcpy(tmpBuffer, host.c_str());
    strcat(sendBuffer, tmpBuffer);
    if(DEBUG) cout << getDebugIndent() << "\t3.2. sendBuffer: '" << sendBuffer << "'" << endl;


    strcpy(tmpBuffer, connection.c_str());
    strcat(sendBuffer, tmpBuffer);
    if(DEBUG) cout << getDebugIndent() << "\t3.3. sendBuffer: '" << sendBuffer << "'" << endl;

    strcpy(tmpBuffer, contentType.c_str());
    strcat(sendBuffer, tmpBuffer);
    if(DEBUG) cout << getDebugIndent() << "\t3.4. sendBuffer: '" << sendBuffer << "'" << endl;

    strcpy(tmpBuffer, contentLength.c_str());
    strcat(sendBuffer, tmpBuffer);
    if(DEBUG) cout << getDebugIndent() << "\t3.5. sendBuffer: '" << sendBuffer << "'" << endl;

    strcpy(tmpBuffer, content.c_str());
    strcat(sendBuffer, tmpBuffer);
    if(DEBUG) cout << getDebugIndent() << "\t3.6. sendBuffer: '" << sendBuffer << "'" << endl;

    strcpy(tmpBuffer, nextLine.c_str());
    strcat(sendBuffer, tmpBuffer);
    if(DEBUG) cout << getDebugIndent() << "\t3.7. sendBuffer: '" << sendBuffer << "'" << endl;


    this->request = sendBuffer;
    this->requestLen = this->request.length();

    return true;
};


void HTTPClient::getPostParameters()
{
    string input = "";

    cout << "would you like to add parameters to your POST request? [y\\n]" << endl;

    getline(cin, input);

    input[0] = tolower(input[0]);

    while(input[0] != 'n')
    {
        if(input[0] != 'y')
        {
            cout << "could not understand your response." << endl;
            cout << "would you like to add parameters to your POST request? [y\\n]" << endl;
            cin >> input;
            continue;
        }



        cout << "please enter a parameter name" << endl;
        cin >> input;
        this->parameters.push_back(input);


        cout << "please enter a value for the parameter" << endl;
        cin >> input;
        this->pvalues.push_back(input);


        cout << "would you like to add another parameter to your POST request? [y\\n]" << endl;
        cin >> input;
    }
}


void HTTPClient::buildPostContent()
{
    vector<string>::iterator it1 = parameters.begin();
    vector<string>::iterator it2 = pvalues.begin();

    string parametersString = "";

    if(it1 != parameters.end())
    {
        parametersString = parametersString + string(*it1 + "=" + *it2);

        ++it1;
        ++it2;
    }



    for(; it1 != parameters.end(); ++it1, ++it2)
    {
        parametersString = parametersString + string("&" + *it1 + "=" + *it2);
    }

    this->reqContent = parametersString;
}


void HTTPClient::getPutParameters()
{
    string input = "";

    cout << "Please enter some information to be PUT" << endl;

    getline(cin, input);

    this->reqContent = input;
}


void HTTPClient::buildPutContent()
{
    if(this->reqContent.substr(0,1) == "<")
        return;

    this->reqContent = "<p>" + this->reqContent + "</p>";
}


bool HTTPClient::pollRecvFrom()
{
    struct pollfd pfd[1];
    pfd[0].fd = this->sd;             // declare I'll check the data availability of sd
    pfd[0].events = POLLRDNORM; // declare I'm interested in only reading from sd

    // check it immediately and return a positive number if sd is readable,
    // otherwise return 0 or a negative number
    return poll( pfd, 1, 0 );
}


bool HTTPClient::getResponse()
{
    const int bufSize = 2048;
    char readBuffer[bufSize];
    char *tmpResponse = NULL;
    char *tmpHeader = NULL;
    char *tmpBody;


    long totalBytesRead = 0;
    long thisReadSize;

    while(true)
    {
        memset(readBuffer, 0, bufSize);

        //while(!this->pollRecvFrom())
        //{
        //    usleep(10);
        //}

        thisReadSize = recv (this->sd, readBuffer, bufSize, 0);

        if ( thisReadSize <= 0 )
            break;

        tmpResponse = (char*)realloc(tmpResponse, thisReadSize + totalBytesRead);

        memcpy(tmpResponse + totalBytesRead, readBuffer, thisReadSize);
        totalBytesRead += thisReadSize;
    }

    this->headerLength = getHeaderLength(tmpResponse);

    if(this->headerLength == -1)
    {
        cerr << "Response was not received" << endl;
        return false;
    }

    this->bodyLength = totalBytesRead - this->headerLength;

    tmpBody = new char[this->bodyLength + 1];
    memcpy(tmpBody, (tmpResponse + this->headerLength), this->bodyLength);
    tmpBody[this->bodyLength] = 0x0;


    tmpHeader = new char[this->headerLength + 1];
    strncpy(tmpHeader, tmpResponse, this->headerLength);
    tmpHeader[this->headerLength] = 0x0;
    delete(tmpResponse);

    this->responseHeader = string(tmpHeader);
    this->responseBody = string(tmpBody);
    close(this->sd);

    return true;

}


long HTTPClient::getHeaderLength(string content)
{
    return getHeaderLength(content.c_str());
}


long HTTPClient::getHeaderLength(char * content)
{
    if(DEBUG)
    {
        //cout << getDebugIndent() <<"getHeaderLength(" << content << ")" << endl;
        this->debugIndentLength += 1;
    }

    if(content == 0x0)
        return -1;

    const char* srchStr1 = "\r\n\r\n";
    const char* srchStr2 = "\n\r\n\r";
    const char* srchStr3 = "\n\n";
    char * findPos;
    long offset = -1;

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


void HTTPClient::setServerName(string name)
{
    if(name.substr(0,7) == "http://")
        name.erase(0,7);

    if(name.substr(0,8) == "https://")
        name.erase(0,8);

    string::size_type n = name.find('/');


    if(n != string::npos)
        this->servername = name.substr(0, n);
    else
       this->servername = name;
}


void HTTPClient::setFilePath(string path)
{
    string::size_type n = path.rfind('/');

    if(n != string::npos)
        this->filepath = path.substr(n);
    else
        this->filepath = "/" + path;
}

string HTTPClient::IntToString(int number)
{
    char cNumber[16];
    sprintf(cNumber, "%d", number);

    return string(cNumber);
}
