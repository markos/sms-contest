/***************************************************************************
                          simconnectionhandler.h  -  description
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

#ifndef SIMCONNECTIONHANDLER_H
#define SIMCONNECTIONHANDLER_H

#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>

#include "connectionmsgblock.h"
//#include "connectionthread.h"


class SimTraffic;
#include "MimeCodes.h"

/**
  *@author Alex Davelos
  */

/**
  SimConnectionHandler class, is bound to the SimConnector
  and is responsible for handle the communication with
  evalserver. The underlying stream (socket stream), is
  embedded in this class.
  Open() registers the handler to the GLOBAL reactor. ACE_Reactor's instance
  is calling back the handle_input function to handle socket input data.
  Handle_input() is automatically called each time 
  input is received from the socket stream peer.
  SendBuffer() is called to send the data to evalserver. This is done initially
  before the we call the ACE_Reactor::handle_events() is called .
 */

class SimConnectionThread;
class SimConnectionHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM,ACE_MT_SYNCH>
{
public: 
  /**
    Constructor
  */
	SimConnectionHandler();

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



  
	~SimConnectionHandler();
  /** No descriptions */
  void showDebugInfo();


  /* SimTraffic instance
     Used to increase the succesfull threads counter
  */
  void setInstance(SimTraffic* inst){ trafficInstance = inst; }

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

  ACE_Thread_Mutex mutex_;

  /*
    SimTraffic instance
  */
  SimTraffic* trafficInstance;
};





#endif
