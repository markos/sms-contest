/***************************************************************************
                          simbatchtimerhandle.h  -  description
                             -------------------
    begin                : Fri Feb 7 2003
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

#ifndef SIMBATCHTIMERHANDLE_H
#define SIMBATCHTIMERHANDLE_H

#include <timerhandle.h>

/**
  *@author Alex Davelos
  */


/** We subclass the SimTimerHandle in order to define 
    a new handle_timeout function which set a batch of timers
    each time
    A new function is implemented also: setBatchConnectionTimer for that purpose.
    We reuse the inherited  createThread(), setConnectionTimer(),get/setInterval functions
*/
class SimBatchTimerHandle : public SimTimerHandle  {
public: 

  // Class constructor ( use the base class constructor )
	SimBatchTimerHandle();
  // Class destructor ( use base class destructor )
	~SimBatchTimerHandle();

  /**
  * Overload ACE_Event_Handler's handle_input :
  * If the input is OK_RESPONSE from the server then the client exits
  * else it keeps waiting for OK_RESPONSE
  */
  int handle_timeout(const ACE_Time_Value& expire_time,const void* sim_flag);

  // Set batch of timers. The argument is the index
  void setBatchConnectionTimer( int t_index );

  // Set the batch size
  void setBatchSize(int bsize){ batch_size = bsize; }

  // Get the batch size
  int getBatchSize(){ return batch_size; }
                                                                                          

private:
 
  //Batch size
  int batch_size;                        

};

#endif
