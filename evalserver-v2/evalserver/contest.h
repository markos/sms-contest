/***************************************************************************
                          contest.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef CONTEST_H
#define CONTEST_H

using namespace std;

/**
	*@author	Konstantinos Margaritis
	*@brief	The main class of evalserver
    The Contest class is where it all happens. It inherits the ACE_Event_Handler
    structure and it is responsible for handling timer events, signals (like INT
    or TERM), starting the thread pool and initializing the database connection.
    Regarding the database, it also sets up the tables and populates them as appropriate
    (mainly the Prizes table). The initialization happens in two phases, one in the
    constructor Contest() and in init(). init() is called when the SOC timer event is triggered
    and this is where the competition actually starts. The event handling is not activated
    in the constructor but rather when start() is called. This is the main loop of the program.
    All event handling is done in handle_events() until a signal is caught or the contest
    ends. Then ReadyToShudtown is set to true and the program goes out of the main loop and
    shutdown() is called and the program exits.
  */

#include <ace/Reactor.h>
#include <ace/Timer_Queue.h>
#include <ace/Thread.h>
#include <ace/Synch.h>
#include <ace/Auto_Ptr.h>
#include <vector>
#include <set>
#include <algorithm>

#include "Count.h"
#include "client_acceptor.h"
#include "connectionmsgblock.h"                   
#include "day.h"
#include "giftdetails.h"
#include "logger.h"
#include "processor.h"
#include "sqliteconnection.h"
#include "sqltable.h"

/// The filename of the SQLite database
#define DBNAME "competition.db"

/// The filename of the logfile
#define LOGFILENAME "evalserver.log"

class Contest: public ACE_Event_Handler {
	/// A basic Thread_Mutex class that will ensure safe access to data.
	ACE_Thread_Mutex mutex_;

  /// Reactor
  ACE_Reactor reactor;

  /// The thread pool acceptor
  Client_Acceptor *peer_acceptor;

  /// The internet address to bind to
	ACE_INET_Addr *iaddr;

   /// Day objects vector
  vector<class Day> ContestDays;

  /// The program will operate while this is false.
  bool ReadyToShutdown;

  /// We need to know the parent thread.
  pid_t basethreadpid;

  /// If we are restarting (from a crash?) there is no need to setup the prizes again.
  bool SetupPrizes;

public:
  /// The default port number on which the evalserver will listen on
  enum { PORT_NUM = 10101 };
  /** handle_timeout() receives one of ID_{SOC,EOC,EOD,EOW,EOM} to
    signify the end of the respective timer. All these are set
    as arguments in schedule_timer() in initTimers().
  */
  enum { ID_SOC = 0, ID_EOD = 1, ID_EOW = 2, ID_EOM = 4, ID_EOC = -1 };

  /** The quantity of the prizes in their respective periods.
    The minute/hour are rather specified as daily prizes with
    the quantity set accordingly.
  */
  enum {  MINUTEPRIZES = 1440, HOURLYPRIZES = 24, DAILYPRIZES = 1,
          WEEKLYPRIZES = 1, MONTHLYPRIZES = 1 };

	/// Start of Contest Timestamp
	TimeStamp					SOC;
	/// End-of-day Timer. There will be apprx. 90 of them (Oct 1, 2002 - Dec 31, 2002)
	vector<TimeStamp> EOD;
	/// End-of-week Timer. Apprx 12 of them.
	vector<TimeStamp> EOW;
	/// End-of-month Timer.
	vector<TimeStamp> EOM;
	/// End-of-contest Timer. Basically it will shutdown the program.
	TimeStamp					EOC;

  /// Index variables to keep count of where we are.
	u_int current_day, current_week, current_month;

  ///	The Constructor. it will take as arguments the hostname and the port number to bind to.
	Contest(char *hostname);

  /// The destructor. Clean up everything and write the final reports.
	~Contest();

  /// The init() method is responsible for some setup that cannot happen in the constructor
  void init();

  /// Shutdown cleans up the stuff from init()
  void shutdown();

  /// Sets the varible ReadyToShutdown to true.
  void preparetoShutdown();

  /// The codes vector. Use this to fast-check for existing codes.
  vector<string> codes_;
  
  ///	The Participants map; basically it will hold a log of every participation
	SQLTable<string, partDetails> Participants;
  
  /// Winners map is for search purposes only, 
  SQLTable<string, partDetails> Winners;

  /// Subscribers is used to hold the unique MSISDNS and a counter for each.
	SQLTable<string, Count> Subscribers;

	///	A map of the gifts of the contest
	SQLTable<string, giftDetails> Prizes;

  /// This one holds the names of the prizes.
	vector<string> PrizeNames;

  /// We also keep an SQL TABLE called COUNTERS for faster statistics.
	SQLTable<string, Day> Counters;

	///	The method to handle timeouts.
	virtual int handle_timeout( const ACE_Time_Value &tv, const void *arg);

  /// Catch and handle signals
  int handle_signal(int signum, siginfo_t*, ucontext_t*);

	///	Set up and schedule all timers, and their respective actions		
	bool initTimers();
	
	///	Set up the thread creation engine.
	bool initThreads(ACE_INET_Addr *iaddr);

  /// Set up the signal handler to catch signals
  bool initSignal();

	///	Allocate and initialize the Prizes map.
	bool setupPrizes();

  /// Initialize Database Connection
  bool initDB();

  /// Load codes into codes_ vector
  bool loadCodes();
	
  /// Initialize the logger object
  bool initLogger();

  /// Initialize the Processor object
  bool initProcessor();
  
	///	Basically a wrapper to handle_events().
	void start();

	/// Return current day
	Day *getCurrentDay();

	/// Return last day
	Day *getLastDay();

	/// Return ContestDays vector
	vector<class Day> &getContestDays();

	/// Return true if gift gid is given in period tp
	bool giftIsGiven(TimePeriod tp, u_int gid);

	/// Report class
	// class Report *report;

  /// Logger object
  Logger *logger;

  /// Processor object
  Processor *processor;

  /// Database Connection Object
  SQLiteConnection *dbconnection;

  /// Wrapper around logger object's logMsg
  void logMsg(string msg);
};
#endif
