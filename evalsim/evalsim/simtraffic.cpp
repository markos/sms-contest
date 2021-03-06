/***************************************************************************
                          simtraffic.cpp  -  description
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

#include "simtraffic.h"

SimTraffic::SimTraffic(){

  //Use timers (NOT FULL TRAFFIC)
  sim_flag=0;

  /*SimTimerHandle object creation - pass the instance of the class
  as an argument to the constructor in order for the SimTimerHandle
  to gain access to the counters of SimTraffic
  */
  timerHandle = new SimTimerHandle(this);

  srand((unsigned)time(NULL)); 
  //Global Stop Flag  - initialize to 1 so as the loop in ACE_Reactor:handle_Events continue
  //until the flag is zero.
  timerHandle->globalStopFlag = 1;

  //Initialize counters to 0
  suc_counter = 0;
  tot_counter = 0;

}
SimTraffic::~SimTraffic(){

  //Delete SimTimerHandle object
  delete timerHandle;
}
/** No descriptions */

//Start the simulation
int SimTraffic::startSim(int stype,int duration,vector<double> traff_param,char* hostname){

  //Set the simulation type
  setSimType(stype);
 
  //Set the simulation duration
  setSimDuration(duration);

  strcpy(eval_host, "");
  cout << hostname << endl;
  //Set the evalserver hostname
  setHostname(hostname);
  cout << eval_host << endl;

  //Set the parameters vector
  setParam(traff_param);
  
  //Run the appropriate algorithm
  switch (stype)
  {
    case FULL_TRAFFIC:
      algoFullTraffic(duration);
      break;
    case PARAM_TRAFFIC:
      algoParamTraffic(duration, traff_param);
      break;
    case RANDOM_TRAFFIC:
      //algoRandomTraffic(duration, traff_param);
      break;
    case RANDOM_GS_TRAFFIC:
      algoRandomGsTraffic(duration, traff_param);
      break;
    case REAL_TRAFFIC:
      algoRealTraffic(duration, traff_param);
      break;
    case SH_DOWN_TRAFFIC:
      algoShDownTraffic(duration, traff_param);
      break;
    default:
      ACE_ERROR_RETURN((LM_ERROR,"Error traffic simulation type\n"),-1);
      break;
  }       
  
  //First case: Full Traffic simulation
  if ( sim_flag )
  {
    //Set a timer that expires one second after the end of the simulation
    //This is done in order to exit from the process at handle_timeout method of SimTimerHandle
    //The purpose is to find a way to set a GLOBAL flag that is checked in ACE_Reactor's 
    //handle_events loop. When this flag is set, the reactor's loop is ended (TODO)
    //(ACE_Reactor's loop is essential for handling the connection with the server)
    timerHandle->setConnectionTimer(ACE_Time_Value(duration + 1),1);

    //Start time
    int start_time = time(0);

    //Loop from start_time -> start_time + duration
    while( time(0) < start_time + duration )
    {
      //Create the connection thread via timerHandle object
      int i=0;
      while(i<sim_param[0])
      {
          i++;
      	  timerHandle->test_counter++;
          ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
          {
            timerHandle->createThread(this);
          }
       	  cout<<i<<endl;
       }
       ACE_OS::sleep(sim_param[1]); 
     }
  }
  else
  {
      // NOT FULL TRAFFIC CASE
      // TODO: set timer according to the intervals specified from the algo* functions
      // TODO: at handle_timeout set new timers --> transfer the interval vector into 
      // TODO: timerHandle class.
 
      //Set the initial timer. Thix timer will expuire according to time_interval[0] value
      //subsequent timers will be scheduled into the handle_timeout body, according to the 
      //next values of time_interval vector                                                    <<
      //The setConnectionTimer function passes as argument to handle_timeout the sim_flag
      //Then  the handle_timeout  function will check it's value and will setup the right timer
      cout<<"Setting first counter..."<<endl;
      timerHandle->setConnectionTimer(ACE_Time_Value(1,0),0);

      //set expiration timer
      timerHandle->setConnectionTimer(ACE_Time_Value( duration + 1 ), 1);

      /*
      //DEBUGGING: View intervals
     cout<<"Flushing intervals:"<<endl;
      vector<double> coutInt;
      coutInt=timerHandle->getIntervals();
      double sum=0 ;
      for(int i=0;i<coutInt.size();i++)
      {
        cout<<coutInt[i]<<endl;
        sum+=coutInt[i];
      }
      cout<<"Total time:"<<sum<<endl;
      cout<<"End of flashing intervals"<<endl;
      */
  }

  cout << "got here" << endl;
  return 0;
 
}

void SimTraffic::setSimType(int stype){
  sim_type = stype;
}

void SimTraffic::setSimDuration(int duration){
  sim_duration = duration;
}

int SimTraffic::getSimType(){
  return sim_type;
}

double SimTraffic::getSimDuration(){
  return sim_duration;
}

//Full traffic algorithm
//It just sets the sim_flag to 1
//The code will be run according to that flag
void SimTraffic::algoFullTraffic(int duration){

   sim_flag = 1;

}    

//Steady traffic algorithm
//It computes the number of intervals between start  and end of simulation ,
//according to the interval value.
//Then it runs the setSteadyIntervals member function of SimTimerHandle
//which fills the time_interval vector with the steady interval values 
void SimTraffic::algoParamTraffic(int duration, vector<double> traff_param){

        double interval = traff_param[0];
        int total_intervals = int (duration / interval );
        timerHandle->setSteadyIntervals(interval,total_intervals);
}

/** No descriptions */
void SimTraffic::algoRandomTraffic(int duration, vector<double> traff_param){

    /* UNUSED DUE TO MEMORY ALLOCATION PROBLEMS
    
        double interval_avg = traff_param[0];
        double interval_std;

        if ( traff_param[1] != 0 )
          interval_std = traff_param[1];
        else
          interval_std = 2.0*interval_avg;
        
        double total_time=0;
        double cur_interval;
        srand((unsigned)time(NULL));
        
        
        while (total_time <= duration + 2 )
        {
           //cur_interval = (double)( rand()/(double)RAND_MAX ) * interval_std + ( (double)interval_avg - (double)(interval_std)/2 );
           //timerHandle->setNextInterval( cur_interval );
           //total_time += cur_interval;
        }  */
                

}
/** No descriptions */
void SimTraffic::algoRandomGsTraffic(int duration, vector<double> traff_param){


   // Timestamp must be sorted first !!!!
   // S.O.S
   const char* t_filename = "_int_dec_10";
   fstream ftime;

   //Vector that holds the relative timestamps 
   //Initially is used to read the absolute  timestamps
   vector<int> t_vector;

   //Open the timestamps file
   //This file muse be sorted, and ach line contain one timestamp
   ftime.open(t_filename, fstream::in);

   int temp_time;
    
   //Loop in the file to read the timestamps
   while ( !ftime.eof() )
   {
      ftime>>temp_time;
      t_vector.push_back(temp_time);
   } 

   // Convert t_vector from absolute timestamp container
   // to relative to the first timestamp container -> time_intervals
   // The first element is zero, and the rest are: t_vector[i] - t_vector[0]  
   // where t_Vector[0] is the initial value of t_vector ( initial timestamp )

   // Now we can set t_vector[0] to zero
   //timerHandle->setInterval(0,0);

   for (unsigned int i=1;i<t_vector.size();i++)
   {
        //timerHandle->setNextInterval( t_vector[i] - t_vector[0] );
        //cout<<timerHandle->getInterval(i)<<endl;
   }
   


   // We set the first batch of timers
   // The arguments passed to the function, are
   // the size of batch, and a flag indicating that these
   // are the initial timers
              
   ftime.close();
  

}
/** No descriptions */
void SimTraffic::algoShDownTraffic(int duration, vector<double> traff_param){
}
/** No descriptions */
void SimTraffic::algoRealTraffic(int duration,vector<double> parameters)
{
  //Duration must be given in days

  //Read entries from file
  const char* filename1 = "_entries_1" ; 
  const char* filename2 = "_entries_2" ;
  fstream  file1( filename1, ios::in );
  fstream  file2( filename2, ios::in );
  //Variables to hold file data
  int hour;
  int msg_number;

  if ( (!file1.is_open()) || (!file2.is_open()) )
  {
     ACE_DEBUG((LM_DEBUG,"Data files are not open"));
     exit(1);
  } 

  //Read first day file
  while ( !file1.eof() )
  {
    //Read data
    file1>>hour>>msg_number;
    //Set the time intervals according to the data read
    SetHourlyIntervals( hour, msg_number );
  }
 
  //Read second day file
  while ( !file2.eof() )
  {
    //Read data
    file2>>hour>>msg_number;
    //Set the time intervals according to the data read
    SetHourlyIntervals( hour, msg_number );
  }       
  
  //Close data files
  file1.close();
  file2.close();

  //DEBUG: Size of time_intervals
  cout<<"SIZE OF INTERVALS:"<<timerHandle->GetIntervalsSize()<<endl;

  

}
/** No descriptions */
void SimTraffic::SetHourlyIntervals(int hour, int msg_number)
{
      //First calculate the average
      if ( msg_number != 0)
      {
        double interval_avg = ( 3600 / (double) msg_number );
        double interval_std = interval_avg / 2 ;
        
        srand((unsigned)time(NULL));

        double total_time = 0;
        double cur_interval;
        
        while ( total_time < 3600 )
        {
           cur_interval = (double)( rand()/(double)RAND_MAX ) * interval_std + ( (double)interval_avg - (double)(interval_std)/3 );
           timerHandle->setNextInterval( cur_interval );
           total_time += cur_interval;

           //DEBUG: cur_interval
           //cout<<" "<<hour<<" "<<cur_interval<<endl;
                     
        }
      }
      else
      {
          //Set next interval to be one hour
          timerHandle->setNextInterval(3600);
      }

}

/** Increase a counter(0=total,1=succesful) */
void SimTraffic::incCounter(int which)
{
   switch(which)
   {
     case 0:
         tot_counter++;
         break;
     case 1:
         suc_counter++;
         break;
   }

}
/** Return a counter (0=total,1=succesful) */
int SimTraffic::getCounter(int i)
{
  switch(i)
  {
    case 0:
      return tot_counter;
			break;
		case 1:
			return suc_counter;
			break;
	}
  return 0;
}
/** No descriptions */
void SimTraffic::setHostname(char* hostname)
{
   strcpy(eval_host, hostname);
}
/** No descriptions */
char* SimTraffic::getHostname()
{
   return eval_host;
}
/** No descriptions */
void SimTraffic::setParam(vector<double> param)
{
  sim_param = param;
}
