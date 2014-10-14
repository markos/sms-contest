/***************************************************************************
                          connectionhandler.h  -  description
                             -------------------
    begin                : Mon Sep 2 2002
    copyright            : (C) 2002 by Alex Davelos
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

#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

using namespace std;

#include <ace/Svc_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/Reactor.h>
#include <ace/Synch.h>
#include <ace/Thread.h>

#include "MimeCodes.h"

/**
  *@author Alex Davelos
  */

/**
*   <The handler of the connection>
*   Its method open is called automatically when connection 
*   is established and handle_input is called when input is
*   available through the socket.
*/
class connectionHandler: public ACE_Svc_Handler<ACE_SOCK_STREAM,ACE_MT_SYNCH>
{
public:
  bool readyToShutdown;
  
	connectionHandler();

  /**
  * By calling the open() buffer data is sent to the peer
  */
  int open(void*);

  /**
  * Overload ACE_Event_Handler's handle_input :
  * If the input is OK_RESPONSE from the erver then the client exits
  * else it keeps waiting for OK_RESPONSE
  */
  int handle_input(ACE_HANDLE);

  virtual int handle_timeout( const ACE_Time_Value &tv, const void *arg);

  /**
  * Sets the member variables ( MSISDN, phone, timestamp )
  * for the client standard output 
  */
  void setPart(long long code,char * msdn, int tm)
  {
    _code = code;
    _msdn = msdn;
    _time = tm;
  }
  
  /**
  * Sets the buffer member (memcpy) to buf
  */
  void setBuffer(unsigned char* buf);

  /**
  * Sends current buffer value to the underlying peer stream
  */
  void sendBuffer();

  /**
  * Process the server's response
  * Called in the handle_input function
  * and returns the response as an integer
  */
  int process_input();

  
	~connectionHandler();
private:

  /**
  * Buffer used to send data
  */
  char* buffer;


  /**
  * Variable to store the server's response
  */
  int response;

  /**
  * Counter for send retries
  */
  int counter;
  
  /**
  * Current sending status
  */
  int status;

  /**
  * Participant elements
  */
  int _time;
  long long _code;
  char* _msdn;
  

};

#endif
