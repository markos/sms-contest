/***************************************************************************
                          sqliteconnection.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "sqliteconnection.h"

/**
  * This constructor allows the use of string name for the dbname, instead of
  * just plain C strings.
  */

SQLiteConnection::SQLiteConnection(string dbname, Logger *mylogger, bool usetranslog) {
	SQLiteConnection(dbname.c_str(), mylogger, usetranslog);
}

/**
  * The default constructor. It opens the SQLite DB (residing in the file dbname),
  * Initializes the transactions logger object and the transactions logfiles.
  * It also resets the counters transactions and conflicts.
  * Transactions are disabled by default.
  */

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

/**
  * Closes the DB connection.
  */
  
SQLiteConnection::~SQLiteConnection() {
	if (sqlitedb)
		sqlite_close(sqlitedb);
}

/**
  * Returns a filename to be used for logging.
  * Its path will be on directory TRANSLOGPATH, and
  * the basename will be the current date (YYYYMMDD format).
  * There will be no overwriting of existing files, instead a
  * counter appended to the filename will be used.
  * the suffix will be TRANSLOGSUFFIX (default .sql).
  */
  
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

/**
  * If transaction logging is enabled then set the log filename
  * using the method in pickFilename().
  */
  
void SQLiteConnection::changeLogFilename() {
  if (use_transactions_log)
    transactions_log->setLogFileName(pickFilename());
}

/**
  * Returns the pointer to the SQLite connection.
  */
  
sqlite *SQLiteConnection::getdb() {
	return sqlitedb;
}

/**
  * Returns true if the connection is successful.
  */

bool SQLiteConnection::isReady() {

	if (sqlitedb)
		return true;
	else
		return false;
}

/**
  * Closes and reopens the database. Should be used if a problem
  * occurs with the execution of some SQL commands. Also resets
  * the counters.
  */

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
}

/**
  * Enables the transactions. Resets counters and sets
  * transactions_enabled to true.
  */

void SQLiteConnection::enableTransactions() {

	transactioncmd.str("");
	transactions_enabled = true;
	transactions = 0;
}

/**
  * Disables transactions. Optionally, commits the current transaction
  * Before disabling.
  */
  
void SQLiteConnection::disableTransactions(bool commit) {

	transactions_enabled = false;
	if (commit && transactions)
		commitTransaction();
	transactioncmd.str("");
	transactions = 0;
}

/**
  * Returns true if transactions are enabled.
  */
  
bool SQLiteConnection::transactionsEnabled() {
  return transactions_enabled;
}

/**
  * Begins the current transaction. Basically
  * insert 'BEGIN TRANSACTION' in the transaction string.
  */
  
bool SQLiteConnection::beginTransaction() {

	if (!transactions)
		transactioncmd << "BEGIN TRANSACTION ON CONFLICT IGNORE;" << endl;

	return true;
}

/**
  * Commits the current transaction. 
  * Error handling:
  * if there are no available file descriptors or there is
  * an error with the execution of the command executed (SQLITE_MISUSE
  * or SQLITE_CANTOPEN) then the DB is reopened with reconnect().
  * Then the command is re-executed after a period of one second.
  * If the number of conflicts exceeds max_conflicts then the program
  * gives up with executing this transaction. The number of conflicts
  * is reset after each successfull transaction.
  */
  
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
      conflicts = 0;
#ifdef EXTRADEBUG
	  	logger->logMsg("Transaction commited\n");
#endif
      transactions = 0;
      transactioncmd.str("");

  		return true;
	  } else {
      if (rc == SQLITE_MISUSE || rc == SQLITE_CANTOPEN) {
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
        ACE_OS::sleep(1);
      }
    }
	} while (conflicts < max_conflicts && (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE));

  if (conflicts >= max_conflicts) {
    logger->logMsg("Giving up, waiting 1 sec\n");
    ACE_OS::sleep(1);
 		logger->logMsg("Reopenning database...");
   	reconnect();
    logger->logMsg("done\n");
  }

  transactions = 0;
  transactioncmd.str("");

  return false;
}

/**
  * Returns the number of SQL commands in the transaction string.
  */
  
size_t SQLiteConnection::getTransactions() {
	return transactions;
}

/**
  * Logs the current transaction to the transaction logfile.
  */
  
void SQLiteConnection::logTransaction(string cmdstr) {
  if (use_transactions_log)
    transactions_log->logMsg(cmdstr);
}

/**
  * Returns true if the specified table exists in the database.
  */
  
bool SQLiteConnection::existsTable(string tablename) {

	int rc, nrow, ncol;
	char **result;

#ifdef DEBUG
    stringstream debug;
#endif

	rc = sqlite_get_table_printf(sqlitedb,
  	"SELECT name FROM sqlite_master WHERE type='table' AND name=%Q;",
	  &result, &nrow, &ncol, NULL, tablename.c_str());
	if (rc == SQLITE_OK) {
  	sqlite_free_table(result);
	  if (nrow)
	    return true;
		else
  		return false;
	} else
  	return false;
}

/**
  * Returns true if the specified index for the table exists in the database.
  */

bool SQLiteConnection::existsIndex(string index, string tablename) {

	int rc, nrow, ncol;
	char **result;

#ifdef DEBUG
    stringstream debug;
#endif

	rc = sqlite_get_table_printf(sqlitedb,
  	"SELECT name FROM sqlite_master WHERE type='index' AND name=%Q AND tbl_name = %Q;",
	  &result, &nrow, &ncol, NULL, index.c_str(), tablename.c_str());
	if (rc == SQLITE_OK) {
  	sqlite_free_table(result);
	  if (nrow)
	    return true;
		else
  		return false;
	} else
  	return false;
}
