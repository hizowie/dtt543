/*
 * HTTPClient.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: anon
 */

#include "HTTPClient.h"

void HTTPClient::getPostParameters()
{
    string input = "";

    cout << "would you like to add parameters to your POST request? [y\n]";

    cin >> input;

    while(input != "n")
    {
        if(input != "y")
        {
            cout << "could not understand your response." << endl;
            cout << "would you like to add parameters to your POST request? [y\n]";
            cin >> input;
            continue;
        }



        cout << "please enter a parameter name" << endl;
        cin >> input;
        this->parameters.push_back(input);


        cout << "please enter a value for the parameter" << endl;
        cin >> input;
        this->pvalues.push_back(input);


        cout << "would you like to add another parameter to your POST request? [y\n]";
    }
}


void HTTPClient::getPutContent()
{
    string input = "";

    cout << "Please enter some information to be PUT";

    cin >> input;

    this->putContent = input;
}


void HTTPClient::buildPutContent()
{
    if(this->putContent.substr(0,1) == "<")
        return;

    this->putContent = "<p>" + this->putContent + "</p>";
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

    this->postContent = parametersString;
}


bool HTTPClient::buildRequest()
{
    const int bufSize = 512;
    char sendBuffer[bufSize];

    string command = "";
    string nextLine = "\r\n";
    string httpVersion = " HTTP/1.1";
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
            contentLength = "Content-Length:" + postContent.length() + nextLine + nextLine;
            content =  this->postContent + nextLine;
            break;
        }
    case PUT:
        {
            command = "PUT " + this->filename + httpVersion + nextLine; break;
        }
    case DELETE:
        {
            command = "DELETE " + this->filename + httpVersion + nextLine; break;
        }
    case TRACE:
        {
            command = "TRACE " + this->filename + httpVersion + nextLine; break;
        }
    case CONNECT:
        {
            command = "CONNECT " + this->filename + httpVersion + nextLine; break;
        }
    default:
        cout << "There is currently no request command selected " << endl;
        return false;


    }


    sprintf(sendBuffer, command.c_str());
    if(DEBUG) cout << getDebugIndent() << "\t3.1. sendBuffer: '" << sendBuffer << "'" << endl;

    strcat(sendBuffer, host.c_str());
    if(DEBUG) cout << getDebugIndent() << "\t3.2. sendBuffer: '" << sendBuffer << "'" << endl;

    //strcat(sendBuffer, connection.c_str());
    if(DEBUG) cout << getDebugIndent() << "\t3.3. sendBuffer: '" << sendBuffer << "'" << endl;

    strcat(sendBuffer, contentType.c_str());
    if(DEBUG) cout << getDebugIndent() << "\t3.4. sendBuffer: '" << sendBuffer << "'" << endl;

    strcat(sendBuffer, contentLength.c_str());
    if(DEBUG) cout << getDebugIndent() << "\t3.5. sendBuffer: '" << sendBuffer << "'" << endl;

    strcat(sendBuffer, content.c_str());
    if(DEBUG) cout << getDebugIndent() << "\t3.6. sendBuffer: '" << sendBuffer << "'" << endl;

    strcat(sendBuffer, nextLine.c_str());
    if(DEBUG) cout << getDebugIndent() << "\t3.7. sendBuffer: '" << sendBuffer << "'" << endl;


    this->request = sendBuffer;

    cout << "sendBuffer = '" << sendBuffer << "'" << endl;
    cout << "this->request = '" << this->request << "'" << endl;
    return true;
};


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


void HTTPClient::readHTML()//const char* URL, long &bytesReturnedOut, char **headerOut)
{
    if(DEBUG)
    {
        //cout << getDebugIndent() << "readHTML(" <<URL <<", " << bytesReturnedOut << ", " << headerOut << ")" << endl;
        //cout << getDebugIndent() << "readHTML(" << this->filepath <<", " << this->bytesReturned << ", " << this->replyHeader << ")" << endl;
        cout << getDebugIndent() << "readHTML(" << this->filepath <<")" << endl;
        this->debugIndentLength += 1;
    }

    if(DEBUG)
            cout << getDebugIndent() << "readHTML() - step 1" << endl;

    const int bufSize = 512;
    //char sendBuffer[bufSize];
    //char  tmpBuffer[bufSize];
    char readBuffer[bufSize];
    char* tmpResult = NULL;
    //char* result;

    long totalBytesRead;
    long thisReadSize;
    long headerLen;




    char * tmpHeader;
    char * tmpBody;

    //string server, filepath, filename;
    //long totalBytesRead, thisReadSize, headerLen;

    //split the URL into server name, file path, and file name
    //parseURL(URL);


    if(DEBUG)
            cout << getDebugIndent() << "readHTML() - step 2" << endl;


    //connect to the server
    if(!this->connectSocket(this->servername))
    {
        cout << "Cannot proceed with request without valid connection" << endl;
        return;
    }



    if(DEBUG)
        cout << getDebugIndent() << "readHTML() - step 3 " << endl;


    buildRequest();

    //send the request
    send(this->sd, this->request, strlen(this->request), MSG_DONTWAIT); //0);



    //cout << "Buffer being sent:" << endl << this->request << endl;
    //cout << "\tlength: " << strlen(this->request) << endl;


    if(DEBUG)
            cout << getDebugIndent() << "readHTML() - step 4" << endl;


    //get received bytes
    totalBytesRead = 0;


    cout << "Pending response from " + this->servername << endl;

    while(true)
    {
        memset(readBuffer, 0, bufSize);

            //if(DEBUG) cout << getDebugIndent() << "\treadBuffer = " << readBuffer << endl;


        thisReadSize = recv(this->sd, readBuffer, bufSize, 0);


        if(thisReadSize <= 0)
            break;

        tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

            //if(DEBUG) cout << "tmpResult = " << tmpResult << endl;


        memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
        totalBytesRead += thisReadSize;

            //cout << "totalBytesRead = " << totalBytesRead << endl;
    }

    if(DEBUG)
    {
        cout << getDebugIndent() << "readHTML() - step 5" << endl;
        //cout << getDebugIndent() << "\ttmpResult" << tmpResult << endl;
    }



    headerLen = getHeaderLength(tmpResult);
    long contentLen = totalBytesRead - headerLen;
    char * result = new char[contentLen + 1];
    memcpy(result, tmpResult + headerLen, contentLen);
    result[contentLen] = 0x0;
    char *myTmp;

    if(DEBUG)
            cout << getDebugIndent() << "readHTML() - step 6" << endl;


    myTmp = new char[headerLen + 1];
    strncpy(myTmp, tmpResult, headerLen);
    myTmp[headerLen] = NULL;
    delete(tmpResult);

    *this->replyHeader = myTmp;
    *this->replyBody = result;






    if(DEBUG)
    {
        cout << getDebugIndent() << "readHTML() - step 7" << endl;
        this->debugIndentLength += 1;

        cout << getDebugIndent() << "contentLen = " << contentLen << endl;
        //cout << getDebugIndent() << "bytesReturnedOut = " << bytesReturnedOut << endl;
        //cout << getDebugIndent() << "this->bytesReturned = " << this->bytesReturned << endl;

        //cout << getDebugIndent() << "myTmp = " << myTmp << endl;
        //cout << getDebugIndent() << "headerOut = " << headerOut << endl;
        cout << getDebugIndent() << "*this->replyHeader = " << *this->replyHeader << endl;
        cout << getDebugIndent() << "*this->replyBody = " << *this->replyBody << endl;
        //cout << getDebugIndent() << "result = " << result << endl;
        this->debugIndentLength -= 2;
    }


    close(this->sd);
    //return (result);
};


void HTTPClient::getAddress()
{
    string input;

    cout << "Please enter a URL to test against" << endl;

    cin >> input;

    this->parseURL(input);
}


void HTTPClient::printHTML()
{
    printHeader();
    cout << "----[ENTITY BODY]------------------------------------------------" << endl;
    cout << *this->replyBody <<endl;
    cout << "----------------------------------------------------------------" << endl;

}

void HTTPClient::printHeader()
{

    cout << "----[RESPONSE HEADER]------------------------------------------" << endl;
    cout << *this->replyHeader << endl;
    cout << "----------------------------------------------------------------" << endl;
}
