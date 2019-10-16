/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: volodia
 *
 * Created on May 1, 2018, 10:20 AM
 */

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

uint64_t previousOne =0;
uint64_t result =0;
uint64_t actualTime =0;

void
sigint_handler(int signalId)
{
    running = 0;
}
//________________________________________________________________GOOSE subscriber
void gooseListener(GooseSubscriber subscriber, void* parameter)
{   
    cout<<*(int *)parameter<<endl;
    if (*(int *)parameter == 1) {
            cout<<"GOOSE event RPi2 DI:\n";
            cout<<"  stNum: "<<GooseSubscriber_getStNum(subscriber)<<" sqNum: "<<GooseSubscriber_getSqNum(subscriber)<<endl;
            cout<<"  timeToLive: "<<GooseSubscriber_getTimeAllowedToLive(subscriber)<<endl;

            uint64_t timestamp = GooseSubscriber_getTimestamp(subscriber);
            cout<<"  timestamp: "<<(timestamp / 1000)<<"."<<(timestamp % 1000)<<endl;


            MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
            MmsValue* value;
            
            DataAttribute * pole[4] = {IEDMODEL_RPi1_LLN0_Loc9_stVal,IEDMODEL_RPi1_LLN0_Loc10_stVal,IEDMODEL_RPi1_LLN0_Loc11_stVal,IEDMODEL_RPi1_LLN0_Loc12_stVal};
            
            for (int i = 0; i < MmsValue_getArraySize(values); i++) {
              
                    value = MmsValue_getElement (MmsValue_getElement (values, i), 0);

                    bool val = MmsValue_getBoolean(value);

                    cout<<"  DI"<<i+1<<": "<<val<<endl; 
                    IedServer_updateBooleanAttributeValue(iedServer, pole[i], val);

            }
            cout<<endl;
            actualTime= Hal_getTimeInMs();
            result = actualTime-previousOne;
            previousOne = actualTime;
            cout<< "valor da diferença: " << result << endl;
            
    } else {
                        cout<<"GOOSE event RPi3 DI:\n";
            cout<<"  stNum: "<<GooseSubscriber_getStNum(subscriber)<<" sqNum: "<<GooseSubscriber_getSqNum(subscriber)<<endl;
            cout<<"  timeToLive: "<<GooseSubscriber_getTimeAllowedToLive(subscriber)<<endl;

            uint64_t timestamp = GooseSubscriber_getTimestamp(subscriber);
            cout<<"  timestamp: "<<(timestamp / 1000)<<"."<<(timestamp % 1000)<<endl;


            MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
            MmsValue* value;     
            
            DataAttribute * pole[4] = {IEDMODEL_RPi1_LLN0_Loc17_stVal,IEDMODEL_RPi1_LLN0_Loc18_stVal,IEDMODEL_RPi1_LLN0_Loc19_stVal,IEDMODEL_RPi1_LLN0_Loc20_stVal};
            
            for (int i = 0; i < MmsValue_getArraySize(values); i++) {
              
                    value = MmsValue_getElement (MmsValue_getElement (values, i), 0);

                    bool val = MmsValue_getBoolean(value);

                    cout<<"  DI"<<i+1<<": "<<val<<endl; 
                    IedServer_updateBooleanAttributeValue(iedServer, pole[i], val);
            }
            cout<<endl;    
    }

}
//------------------------------------------GOOSE subscriber />
//__________________________________________________________________________________________________________________Write client data
static MmsDataAccessError
writeAccessHandler (DataAttribute* dataAttribute, MmsValue* value, ClientConnection connection, void* parameter)
{
    if (dataAttribute == IEDMODEL_RPi1_GGIO1_NamPlt_vendor) {
            if (dataAttribute == IEDMODEL_RPi1_GGIO1_NamPlt_vendor) {
        cout<<"Name of device:"<<MmsValue_toString(value)<<endl; 
            

        return DATA_ACCESS_ERROR_SUCCESS;
    }
 
    return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;


    }
    
    if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn11_mag_f) {

        float newValue = MmsValue_toFloat(value);
        cout<<"New value of AnIn11.f :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0.f) && (newValue <= 1000.1f))
            return DATA_ACCESS_ERROR_SUCCESS;
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
        if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn11_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn11.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc13_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
        if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn12_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn12.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc14_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
            if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn13_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn13.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc15_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
            if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn14_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn14.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc16_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
            if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn15_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn15.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc21_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
            if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn16_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn16.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc22_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
            if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn17_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn17.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc23_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
            if (dataAttribute == IEDMODEL_RPi1_GGIO1_AnIn18_mag_i) {

        int newValue = MmsValue_toInt32(value);
        cout<<"New value of AnIn18.i :"<<newValue<<endl;
        //printf("New value for OutVarSet_setMag_f = %f\n", newValue);

        /* Check if value is inside of valid range */
        if ((newValue >= 0) && (newValue <= 1)){
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc24_stVal, newValue);
            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    }
    
    

    return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
}
//-------------------------------------------------------------------------------------------------------------------//Write data control
//Select before operate??

static CheckHandlerResult
checkHandler(void* parameter, MmsValue* ctlVal, bool test, bool interlockCheck, ClientConnection connection)
{
    printf("check handler called!\n");

    if (interlockCheck)
        printf("  with interlock check bit set!\n");

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO1)
        return CONTROL_ACCEPTED;

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO2)
        return CONTROL_ACCEPTED;

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO3)
        return CONTROL_ACCEPTED;

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO4)
        return CONTROL_ACCEPTED;


    return CONTROL_OBJECT_UNDEFINED;
}

//Operate (control)

static ControlHandlerResult
controlHandlerForBinaryOutput(void* parameter, MmsValue* value, bool test)
{
    if (test)
        return CONTROL_RESULT_FAILED;

    if (MmsValue_getType(value) == MMS_BOOLEAN) {
        printf("received binary control command: ");

        if (MmsValue_getBoolean(value))
            printf("on\n");
        else
            printf("off\n");
    }
    else
        return CONTROL_RESULT_FAILED;

    uint64_t timeStamp = Hal_getTimeInMs();

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO1) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO1_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO1_stVal, value);
        cout<<"SPCSO1"<<endl;
    }

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO2) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO2_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO2_stVal, value);
        cout<<"SPCSO2"<<endl;
    }

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO3) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO3_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO3_stVal, value);
        cout<<"SPCSO3"<<endl;
    }

    if (parameter == IEDMODEL_RPi1_GGIO1_SPCSO4) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO4_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO4_stVal, value);
        cout<<"SPCSO4"<<endl;
    }

    return CONTROL_RESULT_OK;
}
//-----------------------------------------------------------------------------------------------------------//Control

static void
connectionHandler (IedServer self, ClientConnection connection, bool connected, void* parameter)
{
    if (connected)
        printf("Connection opened\n");
    else
        printf("Connection closed\n");
}

int
main(int argc, char** argv)
{
    
    printf("Using libIEC61850 version %s\n", LibIEC61850_getVersionString());

    iedServer = IedServer_create(&iedModel);
    IedServer_setGooseInterfaceId(iedServer, "eth0");
    
//    /* Set the base path for the MMS file services */
//    MmsServer mmsServer = IedServer_getMmsServer(iedServer);
//    MmsServer_setFilestoreBasepath(mmsServer, "./vmd-filestore/");
    


    /* Install handler for operate command */
    IedServer_setControlHandler(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO1,
            (ControlHandler) controlHandlerForBinaryOutput,
            IEDMODEL_RPi1_GGIO1_SPCSO1);

    IedServer_setControlHandler(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO2,
            (ControlHandler) controlHandlerForBinaryOutput,
            IEDMODEL_RPi1_GGIO1_SPCSO2);

    IedServer_setControlHandler(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO3,
            (ControlHandler) controlHandlerForBinaryOutput,
            IEDMODEL_RPi1_GGIO1_SPCSO3);

    IedServer_setControlHandler(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO4,
            (ControlHandler) controlHandlerForBinaryOutput,
            IEDMODEL_RPi1_GGIO1_SPCSO4);

    IedServer_setConnectionIndicationHandler(iedServer, (IedConnectionIndicationHandler) connectionHandler, NULL);

    /* MMS server will be instructed to start listening to client connections. */
    IedServer_start(iedServer, 102);
    /* Don't allow access to SP variables by default */
	IedServer_setWriteAccessPolicy(iedServer, IEC61850_FC_ALL, ACCESS_POLICY_ALLOW);

	/* Instruct the server that we will be informed if a clients writes to a
	 * certain variables we are interested in.
	 */
		IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_NamPlt_vendor, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn11_mag_f, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn11_mag_i, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn12_mag_i, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn13_mag_i, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn14_mag_i, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn15_mag_i, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn16_mag_i, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn17_mag_i, writeAccessHandler, NULL);
        IedServer_handleWriteAccess(iedServer, IEDMODEL_RPi1_GGIO1_AnIn18_mag_i, writeAccessHandler, NULL);

        
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


    GooseSubscriber subscriber = GooseSubscriber_create("IEDPiRPi1/LLN0$GO$DIname", NULL);
    GooseSubscriber subscriber1 = GooseSubscriber_create("IEDPiRPi1/LLN0$GO$DIname", NULL);

    GooseSubscriber_setAppId(subscriber, 0x1001);
    GooseSubscriber_setAppId(subscriber1, 0x1002);
    int sub = 1;
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
    //___________________________________________________________________________Loop
    //
    while (running) { 

        uint64_t timestamp = Hal_getTimeInMs();
        
        //cout<<"Teste msecs : "<< timestamp << endl;

        
        //
        //_______________________________________________________________________Write Analogs
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


        
        
        
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal) != digitalRead(26)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind1_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind1_stVal, digitalRead(26));
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc1_stVal, digitalRead(26));
            cout<<"Change pin 12 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal)<<endl;
        }
        

        
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind2_stVal) != digitalRead(27)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind2_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind2_stVal, digitalRead(27));
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc2_stVal, digitalRead(27));            
            cout<<"Change pin 16 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind2_stVal)<<endl;
        }
        
        
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal) != digitalRead(28)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind3_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind3_stVal, digitalRead(28));
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc3_stVal, digitalRead(28));            
            cout<<"Change pin 20 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal)<<endl;
        }

       
        if (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind4_stVal) != digitalRead(29)) {
            IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind4_t, &iecTimestamp);
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_Ind4_stVal, digitalRead(29));
            IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_RPi1_LLN0_Loc4_stVal, digitalRead(29));
            cout<<"Change pin 21 :"<<IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind4_stVal)<<endl;
        }

        IedServer_unlockDataModel(iedServer);

        if ((IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc12_stVal) ==1) || 
            (IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_LLN0_Loc20_stVal) ==1)
                ) {
            MmsValue * value = MmsValue_newBoolean(0);
            IedServer_updateAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO1_stVal, value);
            IedServer_updateAttributeValue(iedServer, IEDMODEL_RPi1_GGIO1_SPCSO2_stVal, value);
        }

        digitalWrite(2,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_SPCSO1_stVal)));

        digitalWrite(3,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_SPCSO2_stVal)));

        digitalWrite(12,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_SPCSO3_stVal)));
  
        digitalWrite(13,!(IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_SPCSO4_stVal)));

        
        //______________________________________________________________________________________________________//Write//
        //
        //_______________________________________________________________________Vypis hodnot vstupu
        //        
//        float value;
//        
//        value = IedServer_getFloatAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_AnIn1_mag_f);        
//        cout<<"AnIn1: "<<value<<endl;
//        value = NULL;
//
//        value = IedServer_getFloatAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_AnIn2_mag_f);        
//        cout<<"AnIn2: "<<value<<endl;
//        value = NULL;
//
//        value = IedServer_getFloatAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_AnIn3_mag_f);        
//        cout<<"AnIn3: "<<value<<endl;
//        value = NULL;
//
//        value = IedServer_getFloatAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_AnIn4_mag_f);        
//        cout<<"AnIn4: "<<value<<endl;
//        value = NULL;        
                
//        cout<<endl;
//        
//        value = IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind1_stVal);      
//        cout<<"Ind1: "<<value<<endl;
//        value = NULL;
//
//        value = IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind2_stVal);  
//        cout<<"Ind2: "<<value<<endl;
//        value = NULL;
//
//        value = IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind3_stVal);     
//        cout<<"Ind3: "<<value<<endl;
//        value = NULL;
//
//        value = IedServer_getBooleanAttributeValue(iedServer,IEDMODEL_RPi1_GGIO1_Ind4_stVal);        
//        cout<<"Ind4: "<<value<<endl;
//        value = NULL;        
        
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
