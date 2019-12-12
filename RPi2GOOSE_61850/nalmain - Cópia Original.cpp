
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
using std::cout;
using std::endl;
using std::cin;

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;

static int running = 0;
static IedServer iedServer = NULL;

void gooseListener(GooseSubscriber subscriber, void* parameter)
{   
    cout<<*(int *)parameter<<endl;
    if (*(int *)parameter == 0) {
            cout<<"GOOSE event RPi1 LOCAL-> start DO:\n";
            cout<<"  stNum: "<<GooseSubscriber_getStNum(subscriber)<<" sqNum: "<<GooseSubscriber_getSqNum(subscriber)<<endl;
            cout<<"  timeToLive: "<<GooseSubscriber_getTimeAllowedToLive(subscriber)<<endl;

            uint64_t timestamp = GooseSubscriber_getTimestamp(subscriber);
            cout<<"  timestamp: "<<(timestamp / 1000)<<"."<<(timestamp % 1000)<<endl;


            MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
            MmsValue* value;
            
            DataAttribute * pole[4] = {IEDMODEL_RPi1_LLN0_Loc5_stVal,IEDMODEL_RPi1_LLN0_Loc6_stVal,IEDMODEL_RPi1_LLN0_Loc7_stVal,IEDMODEL_RPi1_LLN0_Loc8_stVal};
            
            for (int i = 12; i < MmsValue_getArraySize(values)-8; i++) {
              
                    value = MmsValue_getElement (MmsValue_getElement (values, i), 0);

                    bool val = MmsValue_getBoolean(value);

                    cout<<"  LOCAL"<<i + 1<<": "<<val<<endl; 
                    IedServer_updateBooleanAttributeValue(iedServer, pole[i-12], val);

            }
            cout<<endl;            
            
    } else {
                        cout<<"GOOSE event RPi3 DI:\n";
            cout<<"  stNum: "<<GooseSubscriber_getStNum(subscriber)<<" sqNum: "<<GooseSubscriber_getSqNum(subscriber)<<endl;
            cout<<"  timeToLive: "<<GooseSubscriber_getTimeAllowedToLive(subscriber)<<endl;

            uint64_t timestamp = GooseSubscriber_getTimestamp(subscriber);
            cout<<"  timestamp: "<<(timestamp / 1000)<<"."<<(timestamp % 1000)<<endl;


            MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
            MmsValue* value;     
            
            DataAttribute * pole[4] = {IEDMODEL_RPi1_LLN0_Loc1_stVal,IEDMODEL_RPi1_LLN0_Loc2_stVal,IEDMODEL_RPi1_LLN0_Loc3_stVal,IEDMODEL_RPi1_LLN0_Loc4_stVal};
            
            for (int i = 0; i < MmsValue_getArraySize(values); i++) {
              
                    value = MmsValue_getElement (MmsValue_getElement (values, i), 0);

                    bool val = MmsValue_getBoolean(value);

                    cout<<"  DI"<<i+1<<": "<<val<<endl; 
                    IedServer_updateBooleanAttributeValue(iedServer, pole[i], val);
            }
            cout<<endl;    
    }

}

void
sigint_handler(int signalId)
{
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

int
main(int argc, char** argv)
{
    
    printf("Using libIEC61850 version %s\n", LibIEC61850_getVersionString());

    iedServer = IedServer_create(&iedModel);
    IedServer_setGooseInterfaceId(iedServer, "eth0");
    


    //IedServer_setConnectionIndicationHandler(iedServer, (IedConnectionIndicationHandler) connectionHandler, NULL);

    /* MMS server will be instructed to start listening to client connections. */
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

    GooseReceiver_start(receiver);
    GooseReceiver_start(receiver1);
//----------------------------------------------------------------------------------------------    
  
    running = 1;

    signal(SIGINT, sigint_handler);
        float t = 0.f;


        
        wiringPiSetup () ;
        pinMode (2, OUTPUT) ;
        pinMode (3, OUTPUT) ;
        pinMode (12, OUTPUT) ;
        pinMode (13, OUTPUT) ;
        pinMode (26, INPUT) ;
        pinMode (27, INPUT) ;
        pinMode (28, INPUT) ;
        pinMode (29, INPUT) ;

    //
    //___________________________________________________________________________Smycka
    //
    while (running) { 

        uint64_t timestamp = Hal_getTimeInMs();

        
        //
        //_______________________________________________________________________Zapis Analogovych a Bitovych vstupu  
        //
        IedServer_lockDataModel(iedServer);

        Timestamp iecTimestamp;

        Timestamp_clearFlags(&iecTimestamp);
        Timestamp_setTimeInMilliseconds(&iecTimestamp, timestamp);
        Timestamp_setLeapSecondKnown(&iecTimestamp, true);

        /* toggle clock-not-synchronized flag in timestamp */
        if (((int) t % 2) == 0)
            Timestamp_setClockNotSynchronized(&iecTimestamp, true);

//        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn1_t, &iecTimestamp);
//        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn1_mag_f, an1);
//
//        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn2_t, &iecTimestamp);
//        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn2_mag_f, an2);
//
//        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn3_t, &iecTimestamp);
//        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn3_mag_f, an3);
//
//        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn4_t, &iecTimestamp);
//        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_AnIn4_mag_f, an4);


        
        
        //---------START---------input
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal) != digitalRead(26)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind1_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind1_stVal, digitalRead(26));
            //IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc1_stVal, digitalRead(26));
            cout<<"Zmena pinu 12 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal)<<endl;
        }
        

        //---------OVER VOLTAGE--------input
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind2_stVal) != digitalRead(27)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind2_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind2_stVal, digitalRead(27));
            //IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc2_stVal, digitalRead(27));            
            cout<<"Zmena pinu 16 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind2_stVal)<<endl;
        }
        
        //---------LOCAL STOP------input
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal) != digitalRead(28)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind3_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind3_stVal, digitalRead(28));
            //IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc3_stVal, digitalRead(28));            
            cout<<"Zmena pinu 20 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal)<<endl;
        }

       //----------CENTRAL STOP----input
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind4_stVal) != digitalRead(29)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind4_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind4_stVal, digitalRead(29));
            //IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc4_stVal, digitalRead(29));
            cout<<"Zmena pinu 21 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind4_stVal)<<endl;
        }

        IedServer_unlockDataModel(iedServer);
        
        

        
        //---------OUTPUT1-----------
        if(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal) &&              //input start
           !(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal)) ){             //RPi3 DI central stop
                digitalWrite(2,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc5_stVal)));
                cout<<" DO1: "<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc5_stVal)<<endl;
        }else{
                digitalWrite(2,1); //DO1 = DOWN
        };
        
        //---------OUTPUT2-----------
        if(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal) &&              //input start
           !(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal)) ){             //RPi3 DI central stop
                digitalWrite(3,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc6_stVal)));
                cout<<" DO2: "<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc6_stVal)<<endl;                
        }else{
                digitalWrite(3,1); //DO2 = DOWN
        };    
                
        //---------OUTPUT3-----------
        if(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal) &&              //input start
           !(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal)) ){             //RPi3 DI central stop
                digitalWrite(12,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc7_stVal)));
                cout<<" DO3: "<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc7_stVal)<<endl;                
        }else{
                digitalWrite(12,1); //DO3 = DOWN
        };           

        //---------OUTPUT4-----------
        if(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal) &&              //input start
           !(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal)) ){             //RPi3 DI central stop
                digitalWrite(13,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc8_stVal)));
                cout<<" DO4: "<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc8_stVal)<<endl;
        }else{
                digitalWrite(13,1); //DO4 = DOWN
        };   


        

        Thread_sleep(100);
    
    }
        
    GooseReceiver_stop(receiver);
    GooseReceiver_destroy(receiver);  
    GooseReceiver_stop(receiver1);
    GooseReceiver_destroy(receiver1);     

    /* stop MMS server - close TCP server socket and all client sockets */
    IedServer_stop(iedServer);

    /* Cleanup - free all resources */
    IedServer_destroy(iedServer);

} /* main() */
