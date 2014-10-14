/***************************************************************************
                          report.cpp  -  description
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

#include "report.h"

Report::Report():reportTime(time(NULL)){
   initTables();
   initLogFile();
   
}
Report::~Report(){
  delete dbconnection;
  if ( reportLogFile )
       reportLogFile.close();
  //delete editor;
}

void Report::initTables(){

  /* used to initialize the SQLTable objects and the db connection*/
  dbconnection = new SQLiteConnection(DBNAME,logger,true);

  /*initialize tables and field types */

     vector<string> fieldnames, fieldtypes;
     string iname("date");
     string tsname("periodbegin");

     /* ------------------------------- */
     /*     COUNTERS TABLE     */
     /* ------------------------------- */
    string  tablename = "COUNTERS";
    fieldnames.clear();
    fieldtypes.clear();

    fieldnames.push_back(iname);
    fieldtypes.push_back("TEXT");
    fieldnames.push_back("codes");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("estcodes");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("msisdns");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("prizes");
    fieldtypes.push_back("INTEGER");
    Counters =  SQLTable<string,EvalData>(dbconnection, logger, tablename,
                                                            iname, tsname,fieldnames, fieldtypes, true);

     /* ------------------------------------------ */
     /*     CODES/WINCODES TABLE     */
     /* ------------------------------------------ */
 
    iname = "code" ;
    tsname = "ts" ;

    fieldnames.clear();
    fieldtypes.clear();
    fieldnames.push_back(iname);
    fieldtypes.push_back("TEXT");
    fieldnames.push_back("gid");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("msisdn");
    fieldtypes.push_back("TEXT");
    fieldnames.push_back(tsname);
    fieldtypes.push_back("INTEGER");

    tablename = "CODES";       
    Codes = SQLTable<string, EvalData>(dbconnection, logger, tablename,   
                                                     iname, tsname, fieldnames,fieldtypes, true);

    tablename = "WINNINGCODES";
    WinCodes = SQLTable<string, EvalData>(dbconnection, logger, tablename,   
                                                     iname, tsname, fieldnames,fieldtypes, true);
    

}
  
/** Check if log file exists...
If not , create it specifying the  SOC and EOC timestamps. 
If exists check the reports for each day from SOC to EOC. */
void Report::initLogFile()
{
        reportLogFile.open( LOGFILE , ios::in );

        int start_day,start_month,start_year;
        int end_day,end_month,end_year;
        reportLogFile >> start_day >> start_month >> start_year;
        reportLogFile >> end_day >> end_month >> end_year;

        contestStart =  TimeStamp( start_year , start_month , start_day , 0, 0 , 0 );
        contestEnd =  TimeStamp( end_year , end_month , end_day , 0 , 0 , 0 );

        #ifdef DEBUG
        cout<<"Contest start:"<<contestStart.toString()<<"Contest end:"<<contestEnd.toString()<<endl;
        #endif
}

void Report::appendDate(const string& curday)
{
        ofstream reportLogFileOut;
        reportLogFileOut.open( LOGFILE , ios::app );
        reportLogFileOut << curday  << endl;
        reportLogFileOut.close();           
 }
  
/** Get the next date from logfile */
bool Report::getNextDate(string& date)
{
    if ( reportLogFile >> date )
      return true;
   else
     return false;
}
/** The function that creates the index.html file which has
the table counters, for each date during the contest */
  void Report::createIndexPage()
{
      TimeStamp cur_timestamp;
      string cur_date_string;
      timePeriod tp;

      cur_timestamp = contestStart; 
      cur_date_string =  contestStart.toString(true);
      tp = timePeriod ( cur_timestamp , cur_timestamp.nextDay() );
      TimeStamp cur_time(time(NULL));

      EvalData data;
      HTMLEditor editor ( "Reports/index.html" );
      editor.writeHeader();
      editor.setTitle ( "Vodafone competition" + cur_date_string );
      vector<string> title;
      title.push_back("Period");
      title.push_back("Codes(Real)");
      title.push_back("Codes(Estimated)");
      title.push_back("MSISDN");
      title.push_back("Winners");
      editor.writeTableTitle(title);

      vector<string> lnk;
      while  ( ! tp.dateInPeriod( cur_time.nextDay() ) && ( cur_timestamp < contestEnd.nextDay() ) )
      {
         data = Counters[ cur_timestamp.toString(true) ];
         data.setTimestamp( cur_timestamp.toString(true) );
         data.tokenizeData();
         lnk.clear();
         lnk.push_back("<A HREF=\"./Winners_" + cur_timestamp.toString(true) + ".html\">Winners</A>");
         lnk.push_back("<A HREF=\"./Participants_" + cur_timestamp.toString(true) + ".html\">Participants</A>");
         if ( data.getVectorData().size() > 3 )
               data.addLinks( lnk );
         data.setHTMLRow(0);
         editor.writeRow(data.getVectorData());
         cur_timestamp++;

          tp = timePeriod ( cur_timestamp , cur_timestamp.nextDay() );
      }
     
        editor.writeFooter();                


}
/** Create the participants report page */
void Report::createReportsPage()
{
    TimeStamp today(time(NULL));
    timePeriod tp;
    timePeriod tp1;
    string ndate;
    // Find the correct timeperiod by browsing the log file

    //find last date in logfile
    while ( getNextDate(ndate) );

    int counter=0;

    if ( ndate != "" )
    {
       //parse ndate
       int year = atoi ( (ndate.substr(0,4)).c_str() );
       int month = atoi ( (ndate.substr(4,2)).c_str() );
       int day = atoi ( (ndate.substr(6,2)).c_str() );
       TimeStamp last_day ( year,month,day);
       tp = timePeriod ( last_day , last_day.nextDay() );

       string editor_name; // argument of HTMLEditor ( filename )

       //check if today is equal to last day in log file
       if ( tp.dateInPeriod ( today ) )
       {
             editor_name = ndate;
             writeReport ( editor_name , tp );
       }
       else
       {
             editor_name = ndate;
             writeReport ( editor_name , tp );
             last_day++;
             tp1 = timePeriod ( last_day, last_day.nextDay() );
             while ( (! tp1.dateInPeriod( today )) && counter < 10 )
             {
                 editor_name = last_day.toString( true );
                 writeReport ( editor_name , tp1 );
		 appendDate( last_day.toString( true ));
                 last_day++;
                 tp1 = timePeriod ( last_day, last_day.nextDay() );
                 counter++;
             }
             editor_name = today.toString( true  );
             writeReport ( editor_name , tp1 );
             appendDate( today.toString( true ) );
       }
       
        
     
    }
    else
    {
       TimeStamp cur_day;
       tp = timePeriod ( contestStart , contestStart.nextDay() );
       cur_day = contestStart ;
       while ( ! tp.dateInPeriod( today ) )
       {
            appendDate( cur_day.toString( true ) );
            writeReport ( cur_day.toString ( true  ) , tp );
            cur_day ++;
            tp = timePeriod ( cur_day   , cur_day.nextDay() );
       }
       appendDate  ( today.toString ( true ) );
       writeReport ( today.toString ( true ) , tp );
    }
    //now ndate is the last date of report
    //is ndate equal to curday?

    
    
    // Create the page for the timeperiod found

    // Append that date in the log file if it is the first time running at the day
    
}
/** Write Participants and Winners report page */
void Report::writeReport(string day , timePeriod period)
{
      EvalData data;
      string wn,pn;

      //title is common in the reports pages
      vector<string> title;
      title.push_back("Index");
      title.push_back("MSISDN");
      title.push_back("Number of codes");
      title.push_back("Prize");
      title.push_back("Code");
      title.push_back("Submission date/time");
    
      pn = "./Reports/Participants_" + day + ".html";                     
      HTMLEditor peditor( pn );
      peditor.writeHeader();
      peditor.setTitle ( "Vodafone competition" + day );
      peditor.writeTableTitle(title);
      Codes.selectDistinctObjectsMap( peditor.out , period , "msisdn");
      cout << "PARTICIPANTS: vector of data..." << day <<endl;
      peditor.writeRow(data.getVectorData());
      peditor.writeFooter();
      
      wn = "./Reports/Winners_" + day + ".html";
      HTMLEditor weditor  ( wn );
      weditor.writeHeader();
      weditor.setTitle ( "Vodafone competition" + day );
      weditor.writeTableTitle(title);
      WinCodes.selectDistinctObjectsMap( weditor.out , period , "msisdn");
      weditor.writeRow(data.getVectorData());
      weditor.writeFooter();

}
