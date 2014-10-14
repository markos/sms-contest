/***************************************************************************
                          sqliteconnection.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "sqliteconnection.h"
 
SQLiteConnection::SQLiteConnection(string dbname, Logger *mylogger, bool usetranslog) {
	SQLiteConnection(dbname.c_str(), mylogger, usetranslog);
}

SQLiteConnection::SQLiteConnection(char *dbname, Logger *mylogger, bool usetranslog)
:logger(mylogger), DBname(dbname), use_transactions_log(usetranslog) {
	zErrMsg = 0;
	sqlitedb = sqlite_open(dbname, 0, &zErrMsg);
	if (sqlitedb == 0) {
		logger->logMsg("Cannot open database\n");
		exit(EXIT_FAILURE);
	}

  if (use_transactions_log) {
    string filename = pickFilename();
    transactions_log = new Logger(OUTPUT_FILEONLY, filename);
    transactions_log->open(0);
  }

	transactions = 0;
	conflicts = 0;
	transactions_enabled = false;
}

SQLiteConnection::~SQLiteConnection() {
	if (sqlitedb)
		sqlite_close(sqlitedb);
}

string SQLiteConnection::pickFilename() {
  string basefilename = TRANSLOGPATH + TimeStamp(time(0)).toString(true) + "_";
  stringstream filename;
  int counter = 1;
  struct stat dummy;
  do {
    filename.str("");
    filename << basefilename << counter << TRANSLOGSUFFIX;
    counter++;
  } while (stat(filename.str().c_str(), &dummy) == 0);
  return filename.str();
}

void SQLiteConnection::changeLogFilename() {
  if (use_transactions_log)
    transactions_log->setLogFileName(pickFilename());
}

sqlite *SQLiteConnection::getdb() {
	return sqlitedb;
}

bool SQLiteConnection::isReady() {

	if (sqlitedb)
		return true;
	else
		return false;
}

void SQLiteConnection::reconnect() {
	zErrMsg = 0;
	if (sqlitedb)
		sqlite_close(sqlitedb);
		
	sqlitedb = sqlite_open(DBname.c_str(), 0, &zErrMsg);
	if (sqlitedb == 0) {
		logger->logMsg("Cannot open database\n");
		exit(EXIT_FAILURE);
	}

	transactioncmd.str("");
	transactions = 0;
	
	if (transactions_enabled)
  	transactions_enabled = true;
	else
  	transactions_enabled = false;
}

void SQLiteConnection::enableTransactions() {

	transactioncmd.str("");
	transactions_enabled = true;
	transactions = 0;
}

void SQLiteConnection::disableTransactions(bool commit) {

	transactions_enabled = false;
	if (commit && transactions)
		commitTransaction();
	transactioncmd.str("");
	transactions = 0;
}

bool SQLiteConnection::transactionsEnabled() {
  return transactions_enabled;
}

bool SQLiteConnection::beginTransaction() {

	if (!transactions)
		transactioncmd << "BEGIN TRANSACTION ON CONFLICT IGNORE;" << endl;

	return true;
}

bool SQLiteConnection::commitTransaction() {
	int rc;

#ifdef DEBUG
    stringstream debug;
#endif

  if (transactions)  
  	transactioncmd << "COMMIT TRANSACTION;" << endl;

  logTransaction(transactioncmd.str());
#ifdef EXTRADEBUG
	logger->logMsg(transactioncmd.str());
#endif

  do {
  	rc = sqlite_exec(sqlitedb, transactioncmd.str().c_str(),
	  									 NULL, NULL, &zErrMsg);
  	if (rc == SQLITE_OK) {
#ifdef EXTRADEBUG
	  	logger->logMsg("Transaction commited\n");
#endif
  		return true;
	  } else {
      if (rc == SQLITE_MISUSE) {
    		logger->logMsg("Reopenning database...");
	    	reconnect();
		    logger->logMsg("done\n");
      }

      conflicts++;
#ifdef DEBUG
      debug << "Unable to commit transaction. Error CODE: " << rc << endl;
	  	debug << "Conflicts = " << conflicts << endl;
  	  debug << "Command executed: " << endl << transactioncmd.str() << endl;
	  	debug << "Error Msg : " << zErrMsg << endl;
  		logger->logMsg(debug.str());
#endif

      if (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
        logger->logMsg("Retrying...\n");
        sleep(1);
      }
    }
	} while (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE);
  return false;
}

size_t SQLiteConnection::getTransactions() {
	return transactions;
}

void SQLiteConnection::logTransaction(string cmdstr) {
  if (use_transactions_log)
    transactions_log->logMsg(cmdstr);
}
