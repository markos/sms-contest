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

/**
  *@author Konstantinos Margaritis
  */

class SQLiteConnection {
	enum { transactions_threshold = 20 };
  enum { doCommit = true, dontCommit = false};
	sqlite *sqlitedb;
	char *zErrMsg;
	Logger *logger, *transactions_log;
	string DBname;
	size_t transactions, conflicts;
	stringstream transactioncmd;
	bool transactions_enabled, use_transactions_log;

	template<class key, class data> friend class SQLTable;
public:
	SQLiteConnection(string dbname, Logger *mylogger, bool usetranslog = false);
	SQLiteConnection(char *dbname, Logger *mylogger, bool usetranslog = false);
	~SQLiteConnection();

	sqlite *getdb();
	bool isReady();
	void reconnect();

	bool beginTransaction();
	bool commitTransaction();
	void enableTransactions();
	void disableTransactions(bool commit = doCommit);
  bool transactionsEnabled();
  void logTransaction(string cmdstr);

	size_t getTransactions();
  string pickFilename();
  void changeLogFilename();
};

#endif
