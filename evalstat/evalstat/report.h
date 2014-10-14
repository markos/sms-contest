/***************************************************************************
                          report.h  -  description
                             -------------------
    begin                : ??? ???? 23 2003
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

#ifndef REPORT_H
#define REPORT_H


/**
  *@author Alex Davelos
  */


#include "logger.h"
#include "sqliteconnection.h"
#include "sqltable.h"
#include "timestamp.h"
#include "timeperiod.h"
#include "evaldata.h"
#include "htmleditor.h"

#define DBNAME "competition.db"

/* This class is the main class of evalstat
   It is responsible for .
*/

#define FILENAME_PREFIX "evalserver_report_"
#define LOGFILE "report_log"

class Report {
public:
     /* type of report enumeration */
     enum report_type { INDEX=0, ALL_CODES, WINNING_CODES } ;
     Report();
	 ~Report();
  
     /* initialize SQLTable objects        */
     void initTables();

     /** Check if log file exists...
         If not , create it specifying the  SOC and EOC timestamps. 
         f exists check the reports for each day from SOC to EOC. */
     void initLogFile();
     /** Return the last report of the contest that has not */

     /** Append the last report (current report) date to logfile */
     void appendDate(const string& curday);
     /** Get the next date from logfile */
     bool getNextDate(string& date);
  /** The function that creates the index.html file 
which describes the table conters, for each 
date during the contest */
  void createIndexPage();
  /** Create the participants report page */
  void createReportsPage();
  /** Write Participants and Winners report */
  void writeReport(string day , timePeriod period);

     /* SQLTable objects used to retrieve data from database
        each one of them must be initialized in initTables function   */
     SQLTable<string, EvalData> Counters;
     SQLTable<string,EvalData> Codes;
     SQLTable<string,EvalData> WinCodes;

     /** The file that keeps log of each report file written in disk */
      fstream reportLogFile;
private: // Private attributes
  /** Obsolete object  */
  Logger* logger;
  /**  */
  SQLiteConnection* dbconnection;
  /**  */
  TimeStamp reportTime ;
  /** Start of contest timestamp */
  TimeStamp contestStart;
  /** End of contest timestamp */
  TimeStamp contestEnd;

};

#endif
