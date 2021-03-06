/***************************************************************************
                          simtraffic.h  -  description
                             -------------------
    begin                : Wed Jan 29 2003
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

#ifndef SIMTRAFFIC_H
#define SIMTRAFFIC_H

#include "timerhandle.h"



#define MAX_TIMERS 1

/**
  *@author Alex Davelos
  */

/* Enumeration for traffic simulation types
  */
enum { FULL_TRAFFIC=0, REAL_TRAFFIC, PARAM_TRAFFIC, RANDOM_TRAFFIC, RANDOM_GS_TRAFFIC, SH_DOWN_TRAFFIC};   
  
/* SimTraffic class
   This class is responsible to run the appropriate algorithm which 
   fills the time_intervals vector of SimTimerHandle class
   */
class SimTraffic {
public: 

  /* Constructor
     Set sim_flag member variable to 0 (default behaviour: Not full traffic)
     Create new SimTimerHandle object
  */ 
	SimTraffic();

  /* Destructor
     Deleted SimTimerHandle object
  */
	~SimTraffic();

  /* Operator = */
  SimTraffic& operator=(SimTraffic& so) 
  {
      // we only need to copy hostname in order to compile 
      cout<<"Equality operator"<<endl;
      strcpy(eval_host,so.eval_host);
      return *this;
  } 


  /** Sets sim_type member variable */
  void setSimType(int stype);

  /** Retrieves sim_type value */
  int getSimType();

  /** Set sim_duration variable */
  void setSimDuration(int duration);

  /** Run the appropriate algorithm and then set the initial timer */
  int startSim(int stype,int duration,vector<double> traff_param, char* hostname);

  /** Retrieve sim_duration */
  double getSimDuration();

  /** Set sim_flag to zero */
  void algoFullTraffic(int duration);

  /** Fill timerHandle time_intervals member variable with appropriate values according to traff_param */
  void algoParamTraffic(int duration, vector<double> traff_param);

  /** Fill timerHandle time_intervals member variable with fixed values according to traff_param */
  void algoRandomTraffic(int duration, vector<double> traff_param);

  /** Fill timerHandle time_intervals member variable with realistic values according to traff_param */
  void algoRandomGsTraffic(int duration, vector<double> traff_param);

  /** Fill timerHandle time_intervals member variable with appropriate values according to traff_param 
      plus flags that cause the server to shut down  */
  void algoShDownTraffic(int duration, vector<double> traff_param);

  /** Get the SimTimerHandle object associated with the traffic object */
  SimTimerHandle* getTimerHandle() { return timerHandle; }

  /** Fill time_intervals - The intervals are set according to the burst/delay options in the parameters vector */
  void algoRealTraffic(int duration, vector<double> parameters);
  /** No descriptions */
  void SetHourlyIntervals(int hour, int msg_number);
  /** Increase specified counter */
  void incCounter(int which);
  /** Get Specfied counter's value */
  int getCounter(int i);
  /** No descriptions */
  void setHostname(char* hostname);
  /** No descriptions */
  char* getHostname();
  /** No descriptions */
  void setParam(vector<double> param);

  vector<double> getParam(){ return sim_param; }



private:
  
  /* The simulation duration */
  double sim_duration;
 
  /* Flag: if 1 Full traffic simulation (without timers) , if not 1 use timers */
  int sim_flag;

  /* Current simulation run type (according to the above type enumeration) */
  int sim_type;
                              
  /* Traffic parameters vector*/
  vector<double> sim_param;

  /* SimTimerHandle object that handles timeout of timers
     This object has member data the time_intervals which 
     determine the exact time behaviour
  */
  SimTimerHandle* timerHandle;


  /* Total threads counter - increased in SimTimerHandle class,
     each time a thread is spawned
  */
  int tot_counter;
  /* Succesfull threads couter - increased in SimConnectionHandler
     class, each time a CRC OK answer is received
  */
  int suc_counter;

  /** Evalserver hostname */
  char eval_host[80];

  /* SimTraffic MUTEX */
  ACE_Thread_Mutex mutex_;

};

#endif
