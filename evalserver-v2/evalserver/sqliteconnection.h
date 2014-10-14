/***************************************************************************
                          sqliteconnection.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/
#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

using namespace std;

/*!
  * \class SQLiteConnection
	* \brief Provide a C++ class for the C API of SQLite
  *
  * SQLite does not provide a C++ API, only a C API, so
  * I had to write one for C++. It is basically a wrapper
  * that encapsulates the sqlitedb pointer and provides a
  * simple way to access the db and execute transactions.
  * There are a few things that have to be pointed out about
  * the use of this class.
  * After you create the class, transactions have to be enabled
  * with beginTransaction() and disabled with disableTransaction().
  * A transaction is commited with commitTransaction().
  * This class also provides a logging facility. All SQL commands
  * are also recorded into date-named files in directory TRANSLOGPATH.
  * In the event of an error, it possible to reconstruct the db
  * from these files with the reconstructdb command.
  */

#include <ace/Synch.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Object.h>

#include <sqlite.h>
#include <sstream>

#include <map>
#include <vector>
#include <algorithm>

#include "logger.h"
#include "timestamp.h"

#define TRANSLOGPATH    "TransactionLogs/"
#define TRANSLOGSUFFIX  ".sql"

class SQLiteConnection {
  /// some enumerations that keep some default values
	enum { transactions_threshold = 100, max_conflicts = 10 };

  /// Instead of using true/false, we use the doCommit/dontCommit enums.
  enum { doCommit = true, dontCommit = false};

  /// The pointer to the sqlite handler.
	sqlite *sqlitedb;

  /// This string holds the actuall SQLite message in the event of an error
	char *zErrMsg;

  /// Pointer to the original Logger object and the private one.
	Logger *logger, *transactions_log;

  /// The name of the DB file
	string DBname;

  /// The number of transactions held and the conflicts
	size_t transactions, conflicts;

  /// The complete SQL transaction that is going to be executed.
	stringstream transactioncmd;

  /// Booleans that decide if transactions are going to be used and logged
	bool transactions_enabled, use_transactions_log;

  /// Declare SQLTable class as a friend so that it can access private members.
	template<class key, class data> friend class SQLTable;
public:

  /// Constructor that uses a string as input
	SQLiteConnection(string dbname, Logger *mylogger, bool usetranslog = false);

  /// Constructor that uses a normal C string as input.
	SQLiteConnection(char *dbname, Logger *mylogger, bool usetranslog = false);

  /// Default destructor
	~SQLiteConnection();

  /// Returns the pointer to the sqlite handler.
	sqlite *getdb();

  /// Is the db ready? That is, are we open?
	bool isReady();

  /// Close and Reopen the database.
	void reconnect();

  /// Begins the transaction
	bool beginTransaction();

  /// Commit the transaction
	bool commitTransaction();

  /// Enable the transactions
	void enableTransactions();

  /// Disable (and optionally commit) the current transaction
	void disableTransactions(bool commit = doCommit);

  /// Returns true if transactions are enabled.
  bool transactionsEnabled();

  /// Logs the current transaction to the date-named file.
  void logTransaction(string cmdstr);

  /// Checks for the existence of an SQL table.
  bool existsTable(string tablename);

  /// Checks for the existence of an index for a table.
  bool existsIndex(string index, string tablename);

  /// Returns the number of SQL commands in the current transaction.
	size_t getTransactions();

  /// Automatically picks a filename for the SQL log files.
  string pickFilename();

  /// Change the current log filename.
  void changeLogFilename();
};

#endif
