/***************************************************************************
                          contest.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "contest.h"

/** The constructor. It sets up the internet address to listen to,
    Initializes the random number generator with a seed,
    and sets up the timers of the Contest. The rest of the initialization
    is done in init().
*/

Contest::Contest(char *hostname) {
  cout << "Initialize Logger...";
  initLogger();
  cout << "done" << endl;

  logMsg("Initialize Processor object...");
  initProcessor();
  logMsg("done\n");

	logMsg("New Contest started\n");
	iaddr = new ACE_INET_Addr(PORT_NUM);

	logMsg("Initialize Random Number Generator...");
	srand(time(0));
	logMsg("done\n");

	logMsg("Initialize Timers...");
	initTimers();
	logMsg("done\n");

  ReadyToShutdown = false;
  basethreadpid = ACE_OS::getpid();
  SetupPrizes = true;
}

/** The destructor. Just a dummy, all the important stuff is done in shutdown()
*/

Contest::~Contest()
{ }

/** Contest initialization method. Some of the setup cannot happen
    in the constructor, or actually should not happen.
    The constructor initializes the object and the necessary parts
    of the Contest object, but does *not* actually start the Contest.
    This happens when the SOC (Start Of Contest) timer is triggered.
    Then the sockets are created, the thread engine is initialized,
    the database connection is setup, and the prize pool is created.
    Also, the counters current_day, current_week, current_month
    are set to zero.
*/

void Contest::init() {
	logMsg("Initialize Database...");
	initDB();
	logMsg("done\n");

/*  logMsg("Load codes into RAM...");
  loadCodes();
  logMsg("done\n");*/

	logMsg("Initialize Prizes...");
  if (SetupPrizes == true) {
  	setupPrizes();
    logMsg("done\n");
  } else
    logMsg("Already initialized.\n");

  logMsg("Started signal handler...");
	initSignal();
	logMsg("done\n");

 	logMsg("Started thread engine...");
	initThreads(iaddr);
	logMsg("done\n");
  
  current_day		= 0;
	current_week	= 0;
	current_month	= 0;
	
  return;
}

/** Just sets the ReadyToShutdown flag to true.
  */

void Contest::preparetoShutdown() {
	ReadyToShutdown = true;
}

/** The deallocation of resources happens here.
    It first shuts down the database connection, cancels all the
    (remaining) timers and deactivates the reactor.
  */
void Contest::shutdown() {
  logMsg("Contest:: Shutting down...\n");
	TimePeriod tp (SOC.getTimeStamp(), EOC.getTimeStamp());

	if (dbconnection) {
		logMsg("Closing Database...");
		delete dbconnection;
 		logMsg("done\n");
	}

  logMsg("Unregistering Timers...");
 	ACE_Reactor::instance()->cancel_timer(this);
  logMsg("done\n");

  logMsg("Closing down the Reactor...");
  ACE_Reactor::instance()->close();
  logMsg("done\n");
  delete iaddr;

/*  if (peer_acceptor) {
		logMsg("Shuting down the acceptor...");
		delete peer_acceptor;
 		logMsg("done\n");
	}
  if (processor) {
		logMsg("Stopping Processor Object...");
		delete processor;
 		logMsg("done\n");
	}
  if (logger) {
    logMsg("Delete logger object...");
    delete logger;
    logMsg("done\n");
  }*/

  exit(EXIT_SUCCESS);
}
  
/** handle_timeout() is called every time a timer is triggered.
    Because timers are given an argument, which in this case
    is an ID number regarding the kind of the timer (Start of
    Contest, End of Contest, End of Day, End of Week, End of
    Month), handle_timeout can prepare the necessary structures
    to create the next Day object, or start/end the contest.
    Because this works in a multithreaded environment, we have
    included everything in an ACE_Guard scope.
    At the beginning of the timeout handling all transactions are
    disabled and are re-enabled at the end.
*/

int Contest::handle_timeout(const ACE_Time_Value &tv,
																			const void *argument) {

	ACE_Guard<ACE_Thread_Mutex> guard(mutex_); {
    // Disable Transactions as we're about to write data to the DB
    if (dbconnection)
  	  dbconnection->disableTransactions();

		TimePeriod tp;
    int arg = (int)(argument);

#ifdef DEBUG
    stringstream debug;
    debug.str("");
#endif

		if (arg == ID_SOC) {
			logMsg("Contest::handle_timeout(): Start of Contest timer\n");
      init();
  	}
		
	 	if (arg == ID_EOC) {
 			logMsg("Contest::handle_timeout(): End of Contest timer\n");
 			preparetoShutdown();
	 	}

 		if ((arg == ID_EOD) || (arg == ID_EOW) || (arg == ID_EOM)) {
			logMsg("Contest::handle_timeout(): End of day timer\n");
			tp = TimePeriod(EOD[current_day], EOD[current_day+1]);
      current_day++;
 		}

	 	if (arg == ID_EOW) {
 			logMsg("Contest::handle_timeout(): End of week timer\n");
     	tp = TimePeriod(EOW[current_week], EOW[current_week+1]);
			current_week++;
 		}

	 	if (arg == ID_EOM) {
 			logMsg("Contest::handle_timeout(): End of month timer\n");
     	tp = TimePeriod(EOM[current_month], EOM[current_month+1]);
			current_month++;
		}

		if (arg != ID_SOC) {
			tp = TimePeriod(SOC, tp.getEndTS());
      if (tp.getEndTS().nextDay() >= time(0)) {
#ifdef EXTRADEBUG
        debug.str("");
        debug << "Contest::handle_timeout(): tp.getEndTS().nextDay() = " << tp.getEndTS().nextDay()
              << ", time(0) = " << time(0) << endl;
		    logMsg(debug.str());
#endif
      }
      dbconnection->changeLogFilename();
		}

    if (current_day < EOD.size()) {
			TimeStamp ts(EOD[current_day]);	
			// Create Day Object (the constructor writes to the DB)
#ifdef DEBUG
      debug.str("");
      debug << "Transactions enabled: " << boolalpha
            << dbconnection->transactionsEnabled() << endl;
      logMsg(debug.str());
#endif
			Day *newday = new Day(this, ts);
		  ContestDays.push_back(*newday);
#ifdef DEBUG
      debug.str("");
      debug << "Contest::handle_timeout(): ContestDays.size() = " << ContestDays.size() << endl;
      debug << "Waiting for today's traffic..." << endl;
		  logMsg(debug.str());
#endif 
  		delete newday;
    }
    // Enable Transactions again
    if (dbconnection)
      dbconnection->enableTransactions();
	}

  return 0;
}

/** This one is responsible for handling the signal INT, TERM and QUIT.
    The only reason we want these caught and handled is to know when the
    software is terminated on a user's command. The action is logged in
    evalserver.log. The only thing we do is set ReadyToShutdown to true.
  */
int Contest::handle_signal(int signum, siginfo_t*, ucontext_t*)
{
  stringstream debug("");

  switch(signum) {
    case SIGQUIT:
      if (basethreadpid == ACE_OS::getpid()) {
        debug << "Received SIGQUIT signal. Shutting down gracefully" << endl;
  		  logMsg(debug.str());
        preparetoShutdown();
      }
      break;
    case SIGTERM:
      if (basethreadpid == ACE_OS::getpid()) {
        debug << "Received SIGTERM signal. Shutting down gracefully" << endl;
        logMsg(debug.str());
	preparetoShutdown();
      }
      break;
    case SIGINT:
      if (basethreadpid == ACE_OS::getpid()) {
        debug << "Received SIGINT signal. Shutting down gracefully" << endl;
	  	  logMsg(debug.str());
	preparetoShutdown();
      }
      break;
  }
  return 0;
}

/** This method initializes the EOD, EOW, EOM, SOC, EOC
    TimeStamp objects (or vectors of them) and registers these
    with the reactor instance. We also have to make sure
    that no timers overlap. That is, if a day timer overlaps with
    a week or month timer, we exclude the day timer from the
    registered timers in ACE, but NOT from the EOD vector.
*/  

bool Contest::initTimers() {

  int arg = ID_SOC;
	SOC = TimeStamp(2003, 7, 1);
	ACE_Reactor::instance()->schedule_timer(this,
									(const void *) arg,
									SOC.getTimeStamp() - time(0));

  arg = ID_EOC;
	EOC = TimeStamp(2004, 1, 1, 1);
	ACE_Reactor::instance()->schedule_timer(this,
									(const void *) arg,
									EOC.getTimeStamp() - time(0));

	TimeStamp start(SOC), end(EOC), current(start);

  arg = ID_EOW;
	EOW.push_back(current);
	current.setTimeStamp(current.nextWeek());
	while (current < end) {
		EOW.push_back(current);
 		ACE_Reactor::instance()->schedule_timer(this,
  								(const void *) arg,
	  							current.getTimeStamp() - time(0));
		current.setTimeStamp(current.nextWeek());

  } 	

	vector<TimeStamp>::iterator i;
	i = EOW.begin();
  stringstream debug;
  debug << "Weekly timers: (arg = " << arg << ")" << endl;
	logMsg(debug.str());
	while (i != EOW.end()) {
		logMsg(i->toString());
		i++;
	}

  arg = ID_EOM;
  debug.str("");
 	debug << "Monthly timers: (arg = " << arg << ")" << endl;
  logMsg(debug.str());
	EOM.push_back(TimeStamp(2003, 7, 1));
	EOM.push_back(TimeStamp(2003, 8, 1));
  EOM.push_back(TimeStamp(2003, 9, 1));
  EOM.push_back(TimeStamp(2003, 10, 1));
  EOM.push_back(TimeStamp(2003, 11, 1));
  EOM.push_back(TimeStamp(2003, 12, 1));
  EOM.push_back(TimeStamp(2004, 1, 1));
	i = EOM.begin();
	i++;
	while (i != EOM.end()) {
		ACE_Reactor::instance()->schedule_timer(this,
									(const void *) arg,
									i->getTimeStamp() - time(0));
		logMsg(i->toString());
		i++;
	}	

  arg = ID_EOD;
	end = EOC;
	current = SOC;
	EOD.push_back(current);
	current++;
	while (current < end) {
 		EOD.push_back(current);
    if ((find(EOW.begin(), EOW.end(), current) == EOW.end()) &&
         find(EOM.begin(), EOM.end(), current) == EOM.end()) {
    		ACE_Reactor::instance()->schedule_timer(this,
	    								(const void *) arg,
		    							current.getTimeStamp() - time(0));
    }
		current++;
	}

	i = EOD.begin();
  debug.str("");
	debug << "Daily timers: (arg = " << arg << ")" << endl;
  logMsg(debug.str());
	while (i != EOD.end()) {
		logMsg(i->toString());
		i++;
	}

	logMsg("Start of Contest timer: ");
	logMsg(SOC.toString());

	logMsg("End of Contest timer: ");
	logMsg(EOC.toString());

	return true;
}

/** Basically calls the handle_events() of the reactor
    which is responsible for handling all the events
    (socket connection, timer triggering)
*/

void Contest::start() {

	while (ReadyToShutdown == false)	{		// Start the reactor's event loop
		ACE_Reactor::instance()->handle_events();
  }

  shutdown();
}

/** Creates the acceptor object which uses the Strategy
    feature of ACE to create a thread per socket connection
    and runs svc() in that thread to process the connection.
*/

bool Contest::initThreads(ACE_INET_Addr *iaddr) {
  peer_acceptor = new Client_Acceptor(this);

  if (peer_acceptor->open (*iaddr, ACE_Reactor::instance()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "open"),
                      -1);

	return true;
}

/** Set up the signal handler to catch signals.
    The program should catch SIGINT (Ctrl-C), SIGQUIT
    (Ctrl-\) and if possible SIGKILL and log the
    action before quiting.
*/

bool Contest::initSignal() {

  //Register the handler asking to call back when either SIGWINCH
  //or SIGINT signals occur. Note that in both the cases we asked the
  //Reactor to call back the same Event_Handler i.e., MyEventHandler.
  //This is the reason why we had to write a switch statement in the
  //handle_signal() method above. Also note that the ACE_Reactor is
  //being used as a Singleton object (Singleton pattern)

  ACE_Reactor::instance()->register_handler(SIGTERM,this);
  ACE_Reactor::instance()->register_handler(SIGQUIT,this);
  ACE_Reactor::instance()->register_handler(SIGINT,this);

  return true;
}

/** Set up the prizes for the contest, give them identifiers
    and register them in the Prizes table (SQL) of the database.
*/

bool Contest::setupPrizes(void) {

#ifdef DEBUG
  stringstream debug;
#endif

	PrizeNames.push_back("Nothing");
  PrizeNames.push_back("Ringtones");
	PrizeNames.push_back("Obsolete");
	PrizeNames.push_back("Philips MP3 player");
	PrizeNames.push_back("Philips Home Cinema");
	PrizeNames.push_back("Peugeot 206 CC");

	size_t ind = 0;
	TimePeriod tp;
	giftDetails p;
	stringstream index;
	while (ind < EOD.size() -1) {
		tp = TimePeriod(EOD[ind], EOD[ind+1]);
		index << EOD[ind].toString(true) << ":" << giftDetails::ID_MINUTEPRIZES;
		p = giftDetails(giftDetails::ID_MINUTEPRIZES, PrizeNames[giftDetails::ID_MINUTEPRIZES], tp, MINUTEPRIZES, MINUTEPRIZES);
		Prizes.insertObject(index.str(), p);
#ifdef EXTRADEBUG
		debug << index.str() << ": ";
		debug << p.insertString() << endl;
	  logMsg(debug.str());
	  debug.str("");
#endif

/*    index.str("");
		index << current.toString(true) << ":" << giftDetails::ID_HOURLYPRIZES;
		p = giftDetails(giftDetails::ID_HOURLYPRIZES, PrizeNames[giftDetails::ID_HOURLYPRIZES], tp, HOURLYPRIZES, HOURLYPRIZES);
		Prizes.insertObject(index.str(), p);
#ifdef EXTRADEBUG
		debug << index.str() << ": ";
		debug << p.insertString() << endl;
	  logMsg(debug.str());
	  debug.str("");
#endif*/

    index.str("");
		index << EOD[ind].toString(true) << ":" << giftDetails::ID_DAILYPRIZES;
		p = giftDetails(giftDetails::ID_DAILYPRIZES, PrizeNames[giftDetails::ID_DAILYPRIZES], tp, DAILYPRIZES, DAILYPRIZES);
		Prizes.insertObject(index.str(), p);
#ifdef EXTRADEBUG
		debug << index.str() << ": ";
		debug << p.insertString() << endl;
	  logMsg(debug.str());
	  debug.str("");
#endif
		ind++;
	  index.str("");
	}

	ind = 0;
 	while (ind < EOW.size()-1) {
		tp = TimePeriod(EOW[ind], EOW[ind+1]);
		index << EOW[ind].toString(true) << ":" << giftDetails::ID_WEEKLYPRIZES;
		p = giftDetails(giftDetails::ID_WEEKLYPRIZES, PrizeNames[giftDetails::ID_WEEKLYPRIZES], tp, WEEKLYPRIZES, WEEKLYPRIZES);
		Prizes.insertObject(index.str(), p);
#ifdef EXTRADEBUG
		debug << index.str() << ": ";
		debug << p.insertString() << endl;
	  logMsg(debug.str());
	  debug.str("");
#endif
		ind++;
	  index.str("");
	}
  
	tp = TimePeriod(EOW[ind], EOC);
 	index << EOW[ind].toString(true) << ":" << giftDetails::ID_WEEKLYPRIZES;
 	p = giftDetails(giftDetails::ID_WEEKLYPRIZES, PrizeNames[giftDetails::ID_WEEKLYPRIZES], tp, WEEKLYPRIZES, WEEKLYPRIZES);
 	Prizes.insertObject(index.str(), p);
#ifdef EXTRADEBUG
  debug << index.str() << ": ";
  debug << p.insertString() << endl;
  logMsg(debug.str());
  debug.str("");
#endif
 	ind++;
  index.str("");

	ind = 0;
 	while (ind < EOM.size() -1) {
		tp = TimePeriod(EOM[ind], EOM[ind+1]);
		index << EOM[ind].toString(true) << ":" << giftDetails::ID_MONTHLYPRIZES;
		p = giftDetails(giftDetails::ID_MONTHLYPRIZES, PrizeNames[giftDetails::ID_MONTHLYPRIZES], tp, MONTHLYPRIZES, MONTHLYPRIZES);
		Prizes.insertObject(index.str(), p);
#ifdef EXTRADEBUG
		debug << index.str() << ": ";
		debug << p.insertString() << endl;
	  logMsg(debug.str());
	  debug.str("");
#endif
		ind++;
	  index.str("");
	}

#ifdef DEBUG
	debug.str("");
	string colname = "qty_rem";
	string iname = "gid";
	stringstream val;
  for (u_int i = giftDetails::ID_MINUTEPRIZES; i <= giftDetails::ID_MONTHLYPRIZES; i++) {
    val.str("");
    val << i;
   	debug << dec << "There are " << Prizes.sumColumn(val.str(), iname, colname)
	  			<< " " << PrizeNames[i] << " available" << endl;
  }
  logMsg(debug.str());
#endif

	return true;
}

/** Returns a pointer to the current day of the contest
  */

Day *Contest::getCurrentDay() {
	if (current_day < ContestDays.size())
	return &ContestDays[current_day];
	else
		return NULL;
}

/** Returns a pointer to the last day of the contest
    or NULL if the contest has just started (so that it
    has no last day)
*/

Day *Contest::getLastDay() {
	if (current_day > 0)
		return &ContestDays[current_day -1];
	else
		return NULL;
}

/// Returns the vector that holds the Days of the Contest

vector<class Day> &Contest::getContestDays() {
	return ContestDays;
}

/** Given a TimePeriod object and a gift ID, it returns
    true if the gift is given in that period, false otherwise
*/

bool Contest::giftIsGiven(TimePeriod tp, u_int gid) {

  bool status = false;
  stringstream debug;

  string prefix = "period", qtyname = "qty_rem";
	map<string, giftDetails> *
  dayprizes = Prizes.selectObjects(tp.getBeginTS(),
                                    prefix, qtyname);

	map<string, giftDetails>::iterator g;
	g = dayprizes->begin();

	while (g != dayprizes->end()) {
		if (gid == g->second.getGiftId() && g->second.getCurrentQuantity() == 0) {
      debug.str("");
      debug << "Contest::giftIsGiven(): gift " << gid << " is already given in this period." << endl;
      logMsg(debug.str());
      return true;
    }
  }

	return status;
}

/** initLogger creates and opens the logger object.
    The logger object is a thread that runs asynchronously to the other
    threads and does all the logging (that is all I/O that is relevant to
    logging).
*/
bool Contest::initLogger(void) {
  logger = new Logger(OUTPUT_BOTH, LOGFILENAME);

  logger->open(0);

  return true;
}

/** initProcessor creates and opens the Processor object.
    This is the object that the processing, that is the draw
    is done.
*/
bool Contest::initProcessor(void) {
  processor = new Processor(this);

  processor->open(0);

  return true;
}

/** Here we open the database connection and create the tables if
    we have to. When we create a table, we use two vector<string>
    objects. fieldnames and fieldtypes for the names and types
    of the fields of the SQL table respectively. Then we initialize
    the SQLTable using the appropriate templates and we also
    pass the logger object as an argument, since we want to have
    one global logger.
  */
bool Contest::initDB() {
	dbconnection = new SQLiteConnection(DBNAME, logger, true);

	if (dbconnection->isReady()) {
  	string iname("code");
  	string tsname("ts");

    vector<string> fieldnames, fieldtypes, indexnames;
    fieldnames.push_back(iname);
    fieldtypes.push_back("TEXT");
    fieldnames.push_back("gid");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("msisdn");
    fieldtypes.push_back("TEXT");
    fieldnames.push_back(tsname);
    fieldtypes.push_back("INTEGER");

    indexnames.push_back(tsname);
    indexnames.push_back("gid");

  	string tablename = "CODES";

		Participants = SQLTable<string, partDetails>(dbconnection, tablename,
 													  											iname, tsname, fieldnames,
                                                  fieldtypes, indexnames, true);
                           
		tablename = "WINNINGCODES";
		Winners = SQLTable<string, partDetails>(dbconnection, tablename,
																						iname, tsname, fieldnames,
                                            fieldtypes, indexnames, true);

    tablename = "PRIZES";
    fieldnames.clear();
    fieldtypes.clear();
    indexnames.clear();
 		iname = "ID";
		tsname = "periodbegin";
    fieldnames.push_back(iname);
    fieldtypes.push_back("TEXT");
    fieldnames.push_back("gid");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("name");
    fieldtypes.push_back("TEXT");
    fieldnames.push_back(tsname);
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("periodend");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("qty_init");
    fieldtypes.push_back("INTEGER");
    fieldnames.push_back("qty_rem");
    fieldtypes.push_back("INTEGER");

    indexnames.push_back(iname);
    indexnames.push_back("periodbegin");
    indexnames.push_back("periodend");

    if (dbconnection->existsTable(tablename) == true)
      SetupPrizes = false;

 		Prizes = SQLTable<string, giftDetails>(dbconnection, tablename,
  																					iname, tsname, fieldnames,
                                             fieldtypes, indexnames, true);

		tablename = "COUNTERS";
    fieldnames.clear();
    fieldtypes.clear();
    indexnames.clear();
		iname = "date";
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

    indexnames.push_back(iname);

		Counters = SQLTable<string, Day>(dbconnection, tablename,
																			iname, tsname, fieldnames,
                                      fieldtypes, indexnames, true);

    if (Participants.isReady() &&
        Winners.isReady() &&
        Counters.isReady() &&
        Prizes.isReady())
      return true;
    else
      return false;
	}

  dbconnection->enableTransactions();
	return true;
}

/** Load the code column of the CODES table into the codes_ vector.
    For efficiency reasons, we will trade off some memory (apprx. 12MB)
    for 1M codes, to check for an existing code.
  */

bool Contest::loadCodes() {

  Participants.selectAllObjects(codes_);
  stringstream debug;
  debug << "Loaded " << codes_.size() << " codes.";
  logMsg(debug.str());
  return true;
}    

/** Instead of using directly the logmsg method from the logger object,
    we wrap it with another one, so that if it (the logger) is not available
    (at the start or the end of execution) we will still have a logging
    mechanism available via cout.
  */
  
void Contest::logMsg(string msg) {
  if (logger)
    logger->logMsg(msg);
  else
    cout << msg;

  return;
}
