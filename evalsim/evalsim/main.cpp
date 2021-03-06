/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Jan 27 09:50:30 EET 2003
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

using namespace std; 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>

#include "simtraffic.h"  
#include "timerhandle.h"
#include "connectionmsgblock.h"
#include <ace/Get_Opt.h>


int main(int argc, char *argv[])
{
  
  SimTraffic straf;

  ACE_Get_Opt get_opt(argc,argv,"t:d:r:s:h:");

  //Parameters vector - changed to hold double value, if we want to specify floating interval
  vector<double> parameters;
  double dur;
  float par1;
  float  par2;
  int type;
  char hostname[80];

  if (argc != 11)
  {
     cout<<" Usage, param traffic: evasim -h <HOSTNAME> -t <TYPE=3> -d <DURATION/SECS> -r <MESSAGE_INTERVAL> -s <DEVIATION>"<<endl;
     cout<<" Usage, full traffic: evasim -h <HOSTNAME> -t <TYPE=0> -d <DURATION/SECS> -r <THREAD NUMBER/INTERVAL> -s <SLEEP INTERVAL>"<<endl;
  }
  else
  {
  	//Parse ACE_Get_Opt
 	  char c;
  	while ((c=get_opt()) != EOF)
 	  {
  	 switch(c)
 	   {
        case 't':
          type=atoi(get_opt.optarg);
          break;
        case 'd':
          dur = atoi(get_opt.optarg);
          break;
        case 'r':
          par1=atof(get_opt.optarg);
          break;
       case 's':
          par2=atof(get_opt.optarg);
          break;
       case 'h':
          strcpy(hostname,get_opt.optarg);
          break;
        default:
          cout  << "Usage: evasim -h <HOSTNAME> -t <TYPE> -d <DURATION/SECS> -r <MESSAGE_INTERVAL> -s <DEVIATION>" << endl 
                << "TYPES: FULL_TRAFFIC=0\nREAL_TRAFFIC=1\nPARAM_TRAFFIC=2\nRANDOM_TRAFFIC=3\nRANDOM_GS_TRAFFIC=4\nSH_DOWN_TRAFFIC=5"
                << endl;
          ACE_OS::exit(1);
  	    }
   	 }


  	 cout<<"TYPES: FULL_TRAFFIC=0\nREAL_TRAFFIC=1\nPARAM_TRAFFIC=2\nRANDOM_TRAFFIC=3\nRANDOM_GS_TRAFFIC=4\nSH_DOWN_TRAFFIC=5"<<endl;

 		 //For RANDOM_TRAFFIC simulation the parameters[0] must be the average time between the messages
 		 //and prameters[1] the range of random intervals.
 		 parameters.push_back(par1);
 		 parameters.push_back(par2);

 	 	 straf.startSim(type, (int)(dur), parameters, hostname);
   
		 while(straf.getTimerHandle()->globalStopFlag)
   	 ACE_Reactor::instance()->handle_events();	

  	 cout<<"TOTAL THREADS:"<<straf.getCounter(0)<<endl;
  	 cout<<"SUCCESFULL THREADS:"<<straf.getCounter(1)<<endl;
  }
  return EXIT_SUCCESS; 
                                                                   
}
