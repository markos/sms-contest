/***************************************************************************
                          connectionthread.h  -  description
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

#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include <partdetails.h>
#include "simconnectionhandler.h"
#include "connectionmsgblock.h"
#include <ace/Task.h>
#include <ace/SOCK_Connector.h>
#include <ace/Reactor.h>
#include <ace/Svc_Handler.h>
#include <ace/Connector.h>

#define PORT_NO 10101

/**
  *@author Alex Davelos
  */

/**
  SimConnector class is the connector  that initializes the connection with evalserver.
  Then SimConnectionHandler is responsible to handle the socket that is created
*/
class SimTraffic;
class SimConnectionHandler;
typedef ACE_Connector<SimConnectionHandler,ACE_SOCK_CONNECTOR> SimConnector;

/**
  SimConnectionThread class is an ACE_Task derived class
  This means that it is an active object ( thread object)
  When we create the object, we call manually the open()
  method, which involve the activate() method.
  Activate() is crating a new thread (according ti the options
  that we support as an argument).
  Then the svc() method is called automatically.
  Svc() is the body of the new thread.
*/
class SimConnectionThread : public ACE_Task<ACE_MT_SYNCH>  {
public: 
  /**
    Constructor
  */
	SimConnectionThread();
 
  /**
    Destructor
  */
	~SimConnectionThread();

  /** 
     The body of the new thread
   */
  int svc();

  /** 
     This method we call when we want
     to create the new thread     
   */
  int open();

  int close();

  //static int tot_counter;
  //static int suc_counter;
  //static int est_counter;
  
  ACE_Thread_Mutex mutex_;

  /* Set the SimTraffic instance
  */
  void setInstance(SimTraffic* inst);
  void setHostname(char* hostname);
private:
  //Connector
  SimConnector* conn;

  //Traffic Object Instance (USED TO SET THE SIM TRAFFIC COUNTERS)
  SimTraffic* trafficInstance;
  char eval_hostname[80];  
  
};

//int SimConnectionThread::tot_counter;
//int SimConnectionThread::suc_counter;
//int  SimConnectionThread::est_counter;
   
#endif

