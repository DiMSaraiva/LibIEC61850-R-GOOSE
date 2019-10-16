
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: volodia
 *
 * Created on March 16, 2018, 11:29 PM
 */
#include <goose_receiver.h>
#include <goose_subscriber.h>
#include <iostream>
#include <cstdlib>
#include <iec61850_server.h>
//#include "../../libiec61850-master/.install/include/iec61850_server.h"
#include <hal_thread.h>
//#include "../../libiec61850-master/.install/include/hal_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <wiringPi.h>

#include "static_model.h"

//RGoose
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>


#define BUFLEN 1024  //Max length of buffer
#define PORT 8888   //The port on which to send data


using std::cout;
using std::endl;
using std::cin;

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;

static int running = 0;
static IedServer iedServer = NULL;

//void gooseListener(GooseSubscriber subscriber, void* parameter) {
//    cout << *(int *) parameter << endl;
//    if (*(int *) parameter == 0) {
//        cout << "GOOSE event RPi1 LOCAL-> start DO:\n";
//        cout << "  stNum: " << GooseSubscriber_getStNum(subscriber) << " sqNum: " << GooseSubscriber_getSqNum(subscriber) << endl;
//        cout << "  timeToLive: " << GooseSubscriber_getTimeAllowedToLive(subscriber) << endl;
//
//        uint64_t timestamp = GooseSubscriber_getTimestamp(subscriber);
//        cout << "  timestamp: " << (timestamp / 1000) << "." << (timestamp % 1000) << endl;
//
//
//        MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
//        MmsValue* value;
//
//        DataAttribute * pole[4] = {IEDMODEL_RPi1_LLN0_Loc5_stVal, IEDMODEL_RPi1_LLN0_Loc6_stVal, IEDMODEL_RPi1_LLN0_Loc7_stVal, IEDMODEL_RPi1_LLN0_Loc8_stVal};
//
//        for (int i = 12; i < MmsValue_getArraySize(values) - 8; i++) {
//
//            value = MmsValue_getElement(MmsValue_getElement(values, i), 0);
//
//            bool val = MmsValue_getBoolean(value);
//
//            cout << "  LOCAL" << i + 1 << ": " << val << endl;
//            IedServer_updateBooleanAttributeValue(iedServer, pole[i - 12], val);
//
//        }
//        cout << endl;
//
//    } else {
//        cout << "GOOSE event RPi3 DI:\n";
//        cout << "  stNum: " << GooseSubscriber_getStNum(subscriber) << " sqNum: " << GooseSubscriber_getSqNum(subscriber) << endl;
//        cout << "  timeToLive: " << GooseSubscriber_getTimeAllowedToLive(subscriber) << endl;
//
//        uint64_t timestamp = GooseSubscriber_getTimestamp(subscriber);
//        cout << "  timestamp: " << (timestamp / 1000) << "." << (timestamp % 1000) << endl;
//
//
//        MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
//        MmsValue* value;
//
//        DataAttribute * pole[4] = {IEDMODEL_RPi1_LLN0_Loc1_stVal, IEDMODEL_RPi1_LLN0_Loc2_stVal, IEDMODEL_RPi1_LLN0_Loc3_stVal, IEDMODEL_RPi1_LLN0_Loc4_stVal};
//
//        for (int i = 0; i < MmsValue_getArraySize(values); i++) {
//
//            value = MmsValue_getElement(MmsValue_getElement(values, i), 0);
//
//            bool val = MmsValue_getBoolean(value);
//
//            cout << "  DI" << i + 1 << ": " << val << endl;
//            IedServer_updateBooleanAttributeValue(iedServer, pole[i], val);
//        }
//        cout << endl;
//    }
//
//}

void
sigint_handler(int signalId) {
    running = 0;
}

//static void
//connectionHandler (IedServer self, ClientConnection connection, bool connected, void* parameter)
//{
//    if (connected)
//        printf("Connection opened\n");
//    else
//        printf("Connection closed\n");
//}

void die(char *s) {
    perror(s);
    exit(1);
}

/* skip reserved fields */

/*
bufPos += 4;

int apduLength = length - 8;

if (numbytes < length + headerLength) {
    if (DEBUG_GOOSE_SUBSCRIBER)
        printf("GOOSE_SUBSCRIBER: Invalid PDU size\n");
    return;
}

if (DEBUG_GOOSE_SUBSCRIBER) {
    printf("GOOSE_SUBSCRIBER: GOOSE message:\nGOOSE_SUBSCRIBER: ----------------\n");
    printf("GOOSE_SUBSCRIBER:   APPID: %u\n", appId);
    printf("GOOSE_SUBSCRIBER:   LENGTH: %u\n", length);
    printf("GOOSE_SUBSCRIBER:   APDU length: %i\n", apduLength);
}

// check if there is an interested subscriber
LinkedList element = LinkedList_getNext(self->subscriberList);

while (element != NULL) {
    GooseSubscriber subscriber = (GooseSubscriber) LinkedList_getData(element);

    if (subscriber->appId == appId) {
        subscriberFound = true;
        break;
    }

    element = LinkedList_getNext(element);
}

if (subscriberFound)
    parseGoosePayload(self, buffer + bufPos, apduLength);
else {
    if (DEBUG_GOOSE_SUBSCRIBER)
        printf("GOOSE_SUBSCRIBER: GOOSE message ignored due to unknown APPID value\n");
}
 * */


int
main(int argc, char** argv) {
    /*
    
      printf("Using libIEC61850 version %s\n", LibIEC61850_getVersionString());

      iedServer = IedServer_create(&iedModel);
      IedServer_setGooseInterfaceId(iedServer, "eth0");
    


      IedServer_setConnectionIndicationHandler(iedServer, (IedConnectionIndicationHandler) connectionHandler, NULL);

      /*MMS server will be instructed to start listening to client connections. */
    /*
    IedServer_start(iedServer, 102);

    if (!IedServer_isRunning(iedServer)) {
        printf("Starting server failed! Exit.\n");
        IedServer_destroy(iedServer);
        exit(-1);
    }
     
      
     
    IedServer_enableGoosePublishing(iedServer);
    
    //_______________________________________________________________________________Goose receiver, subscriber
    GooseReceiver receiver = GooseReceiver_create();
    GooseReceiver receiver1 = GooseReceiver_create();

    cout<<"GOOSE interface eth0\n";
    GooseReceiver_setInterfaceId(receiver, "eth0");
    GooseReceiver_setInterfaceId(receiver1, "eth0");


    GooseSubscriber subscriber = GooseSubscriber_create("IEDPiRPi1/LLN0$GO$LOCALname", NULL);
    GooseSubscriber subscriber1 = GooseSubscriber_create("IEDPiRPi1/LLN0$GO$DIname", NULL);

    GooseSubscriber_setAppId(subscriber, 0x1000);
    GooseSubscriber_setAppId(subscriber1, 0x1002);
    int sub = 0;
    int sub1 = 2;
    GooseSubscriber_setListener(subscriber, gooseListener, &sub);
    GooseSubscriber_setListener(subscriber1, gooseListener, &sub1);

    GooseReceiver_addSubscriber(receiver, subscriber);
    GooseReceiver_addSubscriber(receiver1, subscriber1);
    /*
    GooseReceiver_start(receiver);
    GooseReceiver_start(receiver1);
     * */
    //----------------------------------------------------------------------------------------------    


    struct sockaddr_in si_me, si_other;

    int s, i, recv_len = 0;
    socklen_t slen = sizeof (si_other);
    uint8_t* buf = (uint8_t*) malloc(BUFLEN);

    //create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket");
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof (si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if (bind(s, (struct sockaddr*) &si_me, sizeof (si_me)) == -1) {
        die("bind");
    }

    //keep listening for data
    while (1) {
        printf("Waiting for data...");
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
            die("recvfrom()");
        }

        //        parseGooseMessage(receiver1, recv_len);

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n", buf);

        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) {
            die("sendto()");
        }



        //parseUDP
        
        int bufPos = 0;
        uint8_t* buffer = buf;
        int numbytes = recv_len;
        
        
//        cout << "IP1 : " << buffer[27] + "\n" << endl;
//        cout << "Ip2 : " << buffer[31] + "\n" << endl;
        
        //for udp
        if (numbytes < 22)
            return 0;

        //print ethernet addressses
        //        printf("Ethernet receiver: " + buffer[bufPos] + "\n\n");
        
//       cout << "Ethernet receiver: " << (buffer[bufPos]*0x100) + "\n" << endl;

        bufPos += 6;
        //print ethernet addressses
        //        printf("Ethernet sender: " + buffer[bufPos] + "\n\n");
//        cout << "Ethernet sender: " << buffer[bufPos] + "\n" << endl;

        bufPos += 6;
        
        
        int headerLength = bufPos + 2;
        cout << "bufPos: " << bufPos << endl ;
        cout << "headerLength: " << headerLength << endl ;
//       cout << "conteudo buff: " << int(buffer[bufPos]) << endl ;
//       cout << "conteudo buff +1 : " << int(buffer[bufPos+1]) << endl ;

        /* check for VLAN tag */
        if ((buffer[bufPos] == 0x81) && (buffer[bufPos + 1] == 0x00)) {
            //            printf("VLAN TAG: " + buffer[bufPos] + "\n\n");
            cout << "VLAN TAG: " << buffer[bufPos] + "\n" << endl;
            bufPos += 4; /* skip VLAN tag */
            headerLength += 4;
        }

        /* check for GOOSE Ethertype */
        if (buffer[bufPos++] != 0x88)
            //            printf("Goose Ethernet type 1: " + buffer[bufPos] + "\n\n");
            cout << "GOOSE Ethernet type 1: " << buffer[bufPos] + "\n" << endl;
        if (buffer[bufPos++] != 0xb8)
            //            printf("Goose Ethernet type 1: " + buffer[bufPos] + "\n\n");
            cout << "GOOSE Ethernet type 1: " << buffer[bufPos] + "\n" << endl;


        uint16_t appId;

        appId = buffer[bufPos++] * 0x100;
        appId += buffer[bufPos++];
        //        printf("appID final: " + buffer[bufPos] + "\n\n");
        cout << "appID final: " << buffer[bufPos] + "\n" << endl;

        uint16_t length;

        length = buffer[bufPos++] * 0x100;
        length += buffer[bufPos++];
        //        printf("length final: " + buffer[bufPos] + "\n\n");
        cout << "length final: " << buffer[bufPos] + "\n" << endl;


    }

    close(s);

    return 0;




    /*
      running = 1;

      signal(SIGINT, sigint_handler);
          float t = 0.f;



     */
    /*
            Thread_sleep(100);
    
  
        
        GooseReceiver_stop(receiver);
        GooseReceiver_destroy(receiver);  
        GooseReceiver_stop(receiver1);
        GooseReceiver_destroy(receiver1);     

       /* stop MMS server - close TCP server socket and all client sockets */
    // IedServer_stop(iedServer);

    /* Cleanup - free all resources */
    // IedServer_destroy(iedServer);

} /* main() */
