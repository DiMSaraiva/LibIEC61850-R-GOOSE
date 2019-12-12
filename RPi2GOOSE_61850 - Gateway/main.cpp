
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Diogo Saraiva
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

void
sigint_handler(int signalId) {
    running = 0;
}



void die(char *s) {
    perror(s);
    exit(1);
}


int
main(int argc, char** argv) {

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
        cout << endl;
        printf("Waiting for data...");
        cout << endl << endl;
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
            die("recvfrom()");
        }

        //        parseGooseMessage(receiver1, recv_len);

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        cout << endl;

        //printf("Data: %s\n", buf);

        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) {
            die("sendto()");
        }



        //parseUDP
        int bufPos = 12;
        uint8_t* buffer = buf;
        int numbytes = recv_len;

        //too small
        if (numbytes < 22)
            return 0;

        int headerLength = bufPos + 2;

        cout << "bufPos: " << bufPos << endl;
        cout << "headerLength: " << headerLength << endl;


        /* check for VLAN tag */
        if ((buffer[bufPos] == 0x81) && (buffer[bufPos + 1] == 0x00)) {
            cout << "VLAN TAG 0x81 0X00" << endl;
            bufPos += 4; /* skip VLAN tag */
            headerLength += 4;
        } else
            cout << "VLAN TAG WRONG!" << endl;


        /* check for GOOSE Ethertype */
        if (buffer[bufPos++] != 0x88)
            cout << "GOOOSE Ethertype different 1 !" << endl;
        if (buffer[bufPos++] != 0xb8)
            cout << "GOOOSE Ethertype different 2 !" << endl;
        ;

        uint16_t appId;

        appId = buffer[bufPos++] * 0x100;
        appId += buffer[bufPos++];

        uint16_t length;

        length = buffer[bufPos++] * 0x100;
        length += buffer[bufPos++];

        /* skip reserved fields */
        bufPos += 4;

        int apduLength = length - 8;


        //print variables
        printf("GOOSE_SUBSCRIBER:   APPID: %u\n", appId);
        printf("GOOSE_SUBSCRIBER:   LENGTH: %u\n", length);
        printf("GOOSE_SUBSCRIBER:   APDU length: %i\n", apduLength);


    }

    close(s);

    return 0;

} /* main() */
