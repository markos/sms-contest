/***************************************************************************
                          connectionthread.cpp  -  description
                             -------------------
    begin                : Tue Jan 28 2003
    copyright            : (C) 2003 by Alex Davelos
    email                : dav@bullet.gr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "connectionthread.h"

#include "simtraffic.h"

SimConnectionThread::SimConnectionThread()
{                          
  conn = new SimConnector;
}

SimConnectionThread::~SimConnectionThread()
{
//    cout<<"Thread Destructor..."<<endl;            
    delete conn; 
}


int SimConnectionThread::open(){

  // Activate a new thread as a lightweight process
  // and detached ( the kernel  doesn't keep information of it)
  activate(THR_NEW_LWP|THR_DETACHED);
  
	return 0;

}
/** No descriptions */
int SimConnectionThread::svc(){

    
    //THIS CODE WILL BE CHANGED
    //START
    //ACE_DEBUG((LM_DEBUG,"Thread id: %t\n"));    
    
    //Generate random 12-digit numbers
    long long code;  
    long long code_min = 100000000000LL;    //minimum 12-digit number
    long long code_max = 999999999999LL;    //maximum 12-digit number
    
    //code = ((long long int)(rand()) / RAND_MAX)*(code_max - code_min) + code_min;
    code = (long long int)((double)rand()/RAND_MAX * (code_max - code_min) + code_min);
    
    //Generate random msdn 
    char msdn[12];             //variable which holds the mobile number
    int msdn_random_part;   //the random part of msdn (6-digit integer)
    int msdn_min = 100000;  //minimum 6-digit number
    int msdn_max = 999999;  //maximum 6-digit number
    msdn_random_part = (int)(((double)(rand()) / RAND_MAX)*(msdn_max - msdn_min) + msdn_min);
    sprintf(msdn,"306946%d",msdn_random_part);    

    int p_time = time(NULL);

    //Get evalserver hostname from the SimTraffic instance
//    cout<<"Before monkey"<<endl;
//    cout<<eval_hostname<<endl;
    //char hostname[80];
    //strcpy(hostname, trafficInstance->getHostname());
//    cout<<"After monkey"<<endl;


    /* old code */
    cout<<"MSDN:"<<msdn<<endl;
    cout<<"CODE:"<<code<<endl;
    cout<<"TIME:"<<p_time<<endl;
    /**/
    
    //Declare variables to store the buffer
    partDetails messageDetails(msdn,p_time,0);
    connectionMsgBlock dataBlock(code,messageDetails);
    //cout<<"Checksum:"<<hex<<dataBlock.retrieveCRC()<<endl;
   

    //Address to call connect()
    ACE_INET_Addr addr(PORT_NO,eval_hostname);


    //Create the connection handler object
    SimConnectionHandler* connHandler=new SimConnectionHandler();
    //Set the SimTraffic instance in  SimConnectionHandler object
    connHandler->setInstance(trafficInstance);
    
    //Set the buffer member data in the handler object
    //in order to pass the buffer to the underlying stream
    //using peer() function
    connHandler->setBuffer(dataBlock.getData());
    connHandler->setPart(code,msdn,p_time);

    //connHandler->showDebugInfo();

    //call the connect() function:
    //the open() function of the handler is going 
    //to be called
    if (conn->connect(connHandler,addr) == -1) 
    {
        ACE_DEBUG((LM_DEBUG,"Connection Failed\n"));
        return -1;
    }      

    //Send data to evalserver
    connHandler->sendBuffer();
    
    return 0;
}

int SimConnectionThread::close()
{
    //cout<<"Closing thread..."<<endl;
    return 0;
}

void SimConnectionThread::setInstance(SimTraffic* inst)
{
   trafficInstance = inst;
}

void SimConnectionThread::setHostname(char* hostname)
{
   strcpy(eval_hostname,hostname);
}
