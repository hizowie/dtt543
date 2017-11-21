

#include <iostream>
#include "Timer.h"
#include "UdpSocket.h"
#include <math.h>

//#include "helper.h"

using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <vector>

#include <cstdlib>
#include <unistd.h>
#include <limits.h>

#include <sys/poll.h>
#include <sys/time.h>
#include <queue>

#ifndef INFO_H_
#define INFO_H_

#define DEBUG true

using namespace std;

enum FLAG { SYN, ACK, SYNACK, END, PAYLOAD };
enum STATUS { OPENED, CLOSED, SYN_SENT, SYN_RECD, LAST_ACK, CLOSE_WAIT, LISTEN, END_WAIT_1, END_WAIT_2 };


class DataSocket
{
    private:
        //connection to destination address
        int sd = 0;
        sockaddr_in destAddr;
        socklen_t destLen;

        //local host
        sockaddr_in sourceAddr;
        socklen_t sourceLen;

        //ack destination
        sockaddr ackAddr;
        socklen_t ackAddrLen = sizeof(ackAddr);


        long timeout = 500;
        uint64_t  lastTimeoutReset = 0;


        int numPayloadPacketsSent = 0;
        int numPayloadPacketsReceived = 0;
        int numAckPacketsSent = 0;
        int numAckPacketsReceived = 0;
        int numSendTimeouts = 0;

    protected:
        queue<char> recQueue;
        queue<char> sendQueue;

        bool debugMode = true;

        unsigned short int currentSeqNum = 0;

        struct dataPacket
        {
                unsigned short int packetNumber;
                FLAG flag;
                unsigned short int payloadLength;
                char* payload;
        };

       dataPacket lastPacket;

       dataPacket currentPacket;

      STATUS status = STATUS::CLOSED;


      void newDataPacket(char* data, unsigned short int payloadLength, dataPacket* newPacket)
      {
         newPacket->flag = FLAG::PAYLOAD;
         newPacket->packetNumber = currentSeqNum++;

         newPacket->payload = &data;
         newPacket->payloadLength = payloadLength;
      }


      void newStatePacket(FLAG state, dataPacket* newPacket)
      {
          newPacket->flag = state
          newPacket->packetNumber = ++currentSeqNum;

          newPacket->payload = NULL;
          newPacket->payloadLength = 1;
      }


      uint64_t getTime()
      {
          timespec t;
          clock_gettime(CLOCK_REALTIME, &t);
          uint64_t timeResult = t.tv_sec * 1000000000;
          timeResult += t.tv_nsec;

          if(debugMode) cout << "System Time : " << timeResult << endl;

          return timeResult;

      }

      int sendPacket(dataPacket *packet)
      {
          int packetSize = sizeof(dataPacket) + (sizeof(char) * (packet->payloadLength -1));

          if(debugMode) cout << "Sending data packet #: " << packet->packetNumber << endl;
          if(debugMode) cout << ", packet size = " << packetSize << endl;

          int txSuccess = sendto(sd, packet, packetSize, 0, (sockaddr *)&destAddr, sizeof(destAddr));
      }

      bool hasPacketToReceive()
      {
          struct pollfd pfd[1];
          pfd[0].fd = sd;
          pfd[0].events = POLLRDNORM;

          return (poll(pfd, 1, 0) > 0);
      }


      void receivePacket(dataPacket * packet)
      {
          if(debugMode) cout << "Getting received packet from buffer" << endl;

          int receiveSuccess = recvfrom(sd, packet, sizeof(dataPacket), 0, (sockaddr*)&ackAddr, &ackAddrLen);
      }

public:


};




#endif
