/***************************************************************************
                          timerhandle.cpp  -  description
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

#include "timerhandle.h"

#include "simtraffic.h"

SimTimerHandle::SimTimerHandle(){
   
   interval_index = 0;
   //SimConnectionThread::tot_counter = 0;
   //SimConnectionThread::suc_counter = 0;
   //SimConnectionThread::est_counter = 0;
   srand((unsigned)time(NULL));
   test_counter = 0;
  
}
SimTimerHandle::~SimTimerHandle(){

   //Delete the SimConnectionThread object
   //delete connThread;
       
}

int SimTimerHandle::setConnectionTimer(const ACE_Time_Value& expire,int sim_flag){
        //schedule a timer with expire time equal to [expire]
        int timer_id =  ACE_Reactor::instance()->schedule_timer( this, (const void*) sim_flag, expire);
        cout<<"TIMER_ID:"<<timer_id<<endl;
        cur_timer_id = timer_id;
        return timer_id;
}

int SimTimerHandle::handle_timeout(const ACE_Time_Value& expire_time,const void* sim_flag){

        int flag = int( sim_flag );
        if ( sim_flag )
        {
           cout<<"FULL TRAFFIC SIM:"<<endl;
 
           //EXIT OR set global flag (one of the following options must be uncommented)
           //exit(0);                 //shut down simulation
           globalStopFlag = 0;        //exit gracefully
        }
        else
        {
          
          //Create new thread object
          test_counter++;
          createThread(trafficInstance);

          //When the new thread is spawned , a new timer is being scheduled
          //The old timer (which calls handle_timeout) is cancelled automatically at timeout
          //but the timer_id increases by one each time the schedule_timer is called
          
                    
          // Schedlue the next timer by reading next value of time_intervals vector 
          // of SimTimerHandle class
          
          // Next interval - The interval is a floating number, so we extract the integer part and the decimal part
          // and pass them as arguments to the ACE_Time_Value object ( The decimal part is multiplied twice by 1000 to
          // convert from second to microsecond)
          double curin = getRndInterval(trafficInstance->getSimDuration(),trafficInstance->getParam());
          cout<<"RND NEXT INTERVAL:"<<curin<<endl;
          //Integer part of interval
          int intpart = (int)floor(curin);
          //Decimal part of interval
          double decmodul = fmod(curin,1);
          int decpart = (int)floor(decmodul*1000*1000);  //First muliply by 1000 to get the milliseconds and then again to 1000 to get microseconds
          // Schedule next timer according to the values above                                                                            
          int timer_id = ACE_Reactor::instance()->schedule_timer( this, (const void*)flag , ACE_Time_Value(intpart,decpart) );  
          
          return timer_id;
        }

        return 0;
}

int SimTimerHandle::open(){    
    return 0;           
}


int SimTimerHandle::createThread(SimTraffic* inst){

      //Create new thread object(ACE_Task) and call open() method of ACE_Task class 
      //Open() calls activate() which creates a new thread which is responsible for handling
      //the connection with the server
      connThread.open();

      //Set the SimTraffic instance pointer to the thread
      connThread.setHostname(inst->getHostname());
      //Increase the total threads counter
      trafficInstance->incCounter(0);
      return 1;
}

vector<double> SimTimerHandle::getIntervals(){
      return time_intervals;
}

double SimTimerHandle::getInterval(int index){
    return time_intervals[index];
}/** No descriptions */
void SimTimerHandle::setSteadyIntervals(double interval,int total_intervals){

  for (int i=0; i<total_intervals; i++)
  {
    time_intervals.push_back(interval);
  }
}
/** No descriptions */
void SimTimerHandle::setNextInterval(double value){

    time_intervals.push_back(value);

}
void SimTimerHandle::setInterval(double value,int index){

    time_intervals[index] = value;

}
/** No descriptions */
int SimTimerHandle::GetIntervalsSize()
{
   return time_intervals.size();
}
/** No descriptions */
double SimTimerHandle::getRndInterval(double duration,vector<double> traff_param)
{
        double interval_avg = traff_param[0];
        double interval_std;

        if ( traff_param[1] != 0 )
          interval_std = traff_param[1];
        else
          interval_std = 2.0*interval_avg;

        double cur_interval;

        cur_interval = (double)( rand()/(double)RAND_MAX ) * interval_std + ( (double)interval_avg - (double)(interval_std)/2 );        return cur_interval ;
                 
}
