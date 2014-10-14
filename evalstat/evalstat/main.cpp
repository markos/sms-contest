/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Τετ Ιούν 18 13:41:14 EEST 2003
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>

#include <evaldata.h>
#include <report.h>
#include <sqltable.h>
#include <htmleditor.h>
#include <timeperiod.h>
#include <timestamp.h>

using namespace std;

int main(int argc, char *argv[])
{

  //check if we are in critical time ( 00:00 - 01:00)
  time_t run_time = time(NULL);
  struct tm* tinfo;
  tinfo = localtime ( & run_time );
  int cur_hour = tinfo->tm_hour;
  
  if ( cur_hour == 0 )
     cout << "Critical time , not running " << endl ;
  else
  {
      cout << "Creating statistics " << endl;
      Report rep;
      rep.createIndexPage();
      rep.createReportsPage();
  }


 
  return EXIT_SUCCESS;
}
