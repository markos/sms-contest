/***************************************************************************
                          timerhandle.h  -  description
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

#ifndef TIMERHANDLE_H
#define TIMERHANDLE_H

#include "connectionthread.h"

#include <vector>
#include <math.h>
#include "ace/Reactor.h"



/**
  *@author Alex Davelos
  */

class SimTraffic;


/** SimTimerHandle class:
    Responsible class for setting timers
    and handling timeouts.
*/
class SimTimerHandle:public ACE_Event_Handler {
public: 

  /** 
     Constructor
  */
	SimTimerHandle();
  /**
     This is the constructor that is currently used
     in order to pass the SimTraffic object instance
     to the SimTimerHandle class
  */
  SimTimerHandle(SimTraffic* inst){ trafficInstance = inst; }

  /** 
     Destructor
  */
	~SimTimerHandle();

  /**
     Open() is called after the constructor automatically 
  */
  int open();

  /** Schedule a timer with the expire time as an argument 
      The timer is scheduled in the GLOBAL reactor
  */
  int setConnectionTimer(const ACE_Time_Value& expire,int sim_flag);

  /** Called automatically when a timer expire
      The expire time is passed as an argument
    */
  virtual int handle_timeout(const ACE_Time_Value& expire_time,const void* arg);
  
  /** Create new connection thread and call open() method
    */
  int createThread(SimTraffic* inst);

  vector<double> getIntervals();
  double getInterval(int index);

  /** Set uniform intervals according to arguments*/
  void setSteadyIntervals(double interval,int total_intervals);
  
  /** Set next interval value */
  void setNextInterval(double value);

  //Set interval element at index
  void setInterval(double value,int index);

  /*Flag checked in the expiration of the timer
    to determine whether it is the end of the simulation
  */
  int globalStopFlag;

  /** Return the number of intervals set */
  int GetIntervalsSize();
  /** No descriptions */
  double getRndInterval(double duration,vector<double> traff_param);

  int test_counter;

private:
  /* Thread object
     Used to spawn a new thread which will take care of 
     connection/data handling with the evalserver
  */
  SimConnectionThread connThread;

  /* SimTraffic Instance */
  SimTraffic* trafficInstance;

  /* Vector that holds the intervals used by the timers  - Changed to hold double values 
  */
  vector<double> time_intervals;
  int interval_index;

  /*Current timer id */
  int cur_timer_id;

 

};

#endif
