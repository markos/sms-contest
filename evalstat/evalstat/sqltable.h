/***************************************************************************
                          sqltable.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef SQLTABLE_H
#define SQLTABLE_H

using namespace std;

#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <ace/OS.h>

#include "sqliteconnection.h"
#include "timeperiod.h"

/**
  *@author Konstantinos Margaritis
  */

template<class key, class data> class SQLTable {
	bool ready;
	SQLiteConnection *dbcon;
	Logger *logger;
	string tablename, indexname, tsname;
//	map<string, string> fieldnames;
  vector<string> fieldnames, fieldtypes;
	bool hasPrimaryKey;

	typedef map<key, vector<data> > sqlmap;
public:
	SQLTable() { ready = false; }
	SQLTable(SQLiteConnection *db, Logger *mylogger,
						const string &tname, const string &iname,
						const string &ts, vector<string> &fnames,
            vector<string> &ftypes, bool primarykey);
	SQLTable(const SQLTable &source);
	SQLTable &operator=(const SQLTable &source);
	~SQLTable() { };

	bool exists();
	bool drop();
	pair<key, data> selectObject(size_t index);
	data selectObject(key val, const string &iname);
	map<key, data> * selectObjects(const key &from, const key &to, const string &iname);
	map<key, data> * selectObjects(timePeriod &tp);
	map<key, data> * selectObjects(	TimeStamp &ts, const string &tpprefix,
																		const string &qtyname);
	map<key, data> * selectObjects(vector<key> &objs, const string &iname);
	map<key, data> * selectAllObjects();
	map<key, data> * selectDistinctObjects(timePeriod &tp, const string &iname);
	map<key, data> * selectDistinctObjects(const string &iname);
	void selectDistinctObjectsMap(ostream& out, timePeriod &tp, const string &iname);
	void selectDistinctObjectsMap(ostream& out, const string &iname);
	void insertObject(key val, data &obj);
	void insertObjects(map<key, data> &objs);
	void updateObject(const key &val, data &obj, const string &iname);
	void updateObjects(map<key, data> &objs, const string &iname);
	void deleteObject(key &val, const string &iname);
	void deleteObjects(const key &from, const key &to, const string &iname);
	void deleteObjects(vector<key> &objs, const string &iname);

	size_t size(key val, const string &iname);
	size_t size(key from, key to, const string &iname);
	size_t size(timePeriod &tp);
	size_t size(const TimeStamp &ts, const string &tpprefix, const string &qtyname);
	size_t size();
	size_t sizeofDistinctObjects(timePeriod &tp, const string &iname);
	size_t sizeofDistinctObjects(const string &iname);

	string sumColumn(key from, key to, const string &iname, const string &colname);
	string sumColumn(timePeriod &tp, key val, const string &iname, const string &colname);
	string sumColumn(	const TimeStamp &ts, const string &tpprefix,
										key val, const string &iname, const string &colname);
	string sumColumn(key val, const string &iname, const string &colname);

	data operator[](key x) {
		return selectObject(x, indexname);
	}

	pair<key, data> operator[](size_t ind) {
		return selectObject(ind);
	}

	bool isReady() { return ready; }
};

template<class key, class data>
SQLTable<key, data>::SQLTable(const SQLTable &source)
:dbcon(source.dbcon), logger(source.logger),
tablename(source.tablename),
indexname(source.indexname),
tsname(source.tsname),
fieldnames(source.fieldnames),
fieldtypes(source.fieldtypes),
ready(source.ready),
hasPrimaryKey(source.hasPrimaryKey) { }

template<class key, class data> SQLTable<key, data>&
SQLTable<key, data>::operator=(const SQLTable &source) {
	this->dbcon = source.dbcon;
	this->logger = source.logger;
	this->tablename = source.tablename;
	this->indexname = source.indexname;
	this->tsname = source.tsname;
	this->fieldnames = source.fieldnames;
 	this->fieldtypes = source.fieldtypes;
	this->ready = source.ready;
	this->hasPrimaryKey = source.hasPrimaryKey;

	return *this;
}

template<class key, class data> 
SQLTable<key, data>::SQLTable<key, data>(SQLiteConnection *db, Logger *mylogger,
																					const string &tname, const string &iname,
																					const string &ts,
																					vector<string> &fnames,
                                          vector<string> &ftypes,
																					bool primarykey)
:dbcon(db), logger(mylogger), tablename(tname),
indexname(iname), tsname(ts), fieldnames(fnames), fieldtypes(ftypes),
hasPrimaryKey(primarykey) {

#ifdef DEBUG
		stringstream debug;
#endif

	if (!exists()) {
		int rc;
   	char *zErrMsg = 0;

		stringstream sqlcmd;
//		map<string, string>::iterator i;
    size_t i = 0;

		sqlcmd << "CREATE TABLE " << tablename;
    sqlcmd << " ( " << fieldnames[i] << " " << fieldtypes[i];
    if (hasPrimaryKey)
      sqlcmd << " PRIMARY KEY";
    i++;
		while (i < fieldnames.size()) {
			sqlcmd << ", " << fieldnames[i] << " " << fieldtypes[i];
			i++;
		}
		sqlcmd << ");" << endl;
    
/*		sqlcmd << " ( " << indexname << " " << fieldnames[indexname];
		if (hasPrimaryKey)
			sqlcmd << " PRIMARY KEY";
		fieldnames.erase(indexname);
		i = fieldnames.begin();
		while (i != fieldnames.end()) {
			sqlcmd << ", " << i->first << " " << i->second;
			i++;
		}
		sqlcmd << ");" << endl;*/

#ifdef DEBUG
		logger->logMsg(sqlcmd.str());
#endif

    do {
  		rc = sqlite_exec (dbcon->getdb(), sqlcmd.str().c_str(), NULL, NULL, &zErrMsg);
	  	if (rc == SQLITE_OK)
        dbcon->logTransaction(sqlcmd.str());
      else {
#ifdef DEBUG
	  		debug << "Unable to create table: Error CODE = " << rc << endl;
  	  	debug << "Command executed: " << endl << sqlcmd.str() << endl;
 	  	  debug << "Error Msg : " << zErrMsg << endl;
		  	logger->logMsg(debug.str());
#endif
      }
		} while (rc == SQLITE_BUSY);
	} else {
#ifdef DEBUG
		logger->logMsg(string("Table " + tablename + " exists\n"));
#endif
	}

	ready = true;
}

template<class key, class data> bool
SQLTable<key, data>::drop() {
	int rc;
#ifdef DEBUG
	stringstream debug;
#endif

	stringstream sqlcmd;
	sqlcmd << "DROP TABLE " << tablename << ";" << endl;
#ifdef DEBUG
	logger->logMsg(sqlcmd.str());
#endif

  do {
  	rc = sqlite_exec (dbcon->getdb(), sqlcmd.str().c_str(), NULL, NULL, NULL);
	  if (rc == SQLITE_OK) {
      dbcon->logTransaction(sqlcmd.str());
      return true;
    } else {
#ifdef DEBUG
		  	debug << "Unable to drop table: Error CODE = " << rc << endl;
			  logger->logMsg(debug.str());
#endif
  		return false;
    }
	} while (rc == SQLITE_BUSY);
}

template<class key, class data> bool
SQLTable<key, data>::exists() {

	int rc, nrow, ncol;
	char **result;

#ifdef DEBUG
    stringstream debug;
#endif

	rc = sqlite_get_table_printf(dbcon->getdb(),
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

template<class key, class data> pair<key, data>
SQLTable<key, data>::selectObject(size_t index) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s LIMIT 1 OFFSET %d;\n",
													tablename.c_str(), index);
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i < ncol; i++) {
		  	objstr << result[ncol +i];
			  if (i < ncol -1)
				  objstr << ",";
			}
  		objstr << endl;
#ifdef EXTRADEBUG
			logger->logMsg(objstr.str());
#endif
  		return pair<key, data> (result[ncol], data(objstr.str()));
		}

		sqlite_free_table(result);
  } else
		return pair<key, data> ("", data());

	return pair<key, data> ("", data());
}

template<class key, class data> data
SQLTable<key, data>::selectObject(key val, const string &iname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s WHERE %s = %Q LIMIT 1;\n",
													tablename.c_str(), iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
   free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i < ncol; i++) {
		  	objstr << result[ncol +i];
			  if (i < ncol -1)
				  objstr << ",";
			}
  		objstr << endl;
#ifdef EXTRADEBUG
	  	logger->logMsg(objstr.str());
#endif
		  return data(objstr.str());
		}

  	sqlite_free_table(result);
	} else
  	return data();
  
	return data();
}

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectObjects(const key &from, const key &to, const string &iname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s WHERE (%s >= %Q AND %s <= %Q);\n",
							tablename.c_str(), iname.c_str(), from.c_str(), iname.c_str(), to.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i <= nrow; i++) {
		  	for (int j=1; j < ncol; j++) {
					objstr << result[i*ncol +j];
  				if (j < ncol -1)
	  				objstr << ",";
				}
  			objstr << endl;
#ifdef EXTRADEBUG
	  		logger->logMsg(objstr.str());
#endif
				data *newobj = new data(objstr.str());
  			string *index = new string(result[i*ncol]);
	  		if (newobj) {
					newmap->insert(pair<key, data>(*index, *newobj));
  				delete newobj;
	  		}
				objstr.str("");
  		}
		}

  	sqlite_free_table(result);
	  return newmap;
	} else
  	return NULL;
    
	return NULL;
}

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectObjects(timePeriod &tp) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s WHERE (%s >= %d AND %s <= %d);\n",
							tablename.c_str(), tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i <= nrow; i++) {
		  	for (int j=1; j < ncol; j++) {
			  	objstr << result[i*ncol +j];
				  if (j < ncol -1)
					  objstr << ",";
				}
  			objstr << endl;
#ifdef EXTRADEBUG
	  		logger->logMsg(objstr.str());
#endif
		  	data *newobj = new data(objstr.str());
			  string *index = new string(result[i*ncol]);
				if (newobj) {
  				newmap->insert(pair<key, data>(*index, *newobj));
	  			delete newobj;
		  	}
			  objstr.str("");
			}
  	}

	  sqlite_free_table(result);
		return newmap;
  } else
	  return NULL;
  
	return NULL;
}

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectObjects(	TimeStamp &ts,
																		const string &tpprefix,
																		const string &qtyname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;
	string tpbegin = tpprefix + "begin";
	string tpend = tpprefix + "end";

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s WHERE (%s <= %d AND %d < %s AND %s != 0);\n",
							tablename.c_str(), tpbegin.c_str(), ts.getTimeStamp(),
							ts.getTimeStamp(), tpend.c_str(), qtyname.c_str());
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i <= nrow; i++) {
		  	for (int j=1; j < ncol; j++) {
			  	objstr << result[i*ncol +j];
				  if (j < ncol -1)
						objstr << ",";
  			}
	  		objstr << endl;
#ifdef EXTRADEBUG
		  	logger->logMsg(objstr.str());
#endif
			  data *newobj = new data(objstr.str());
				string *index = new string(result[i*ncol]);
  			if (newobj) {
	  			newmap->insert(pair<key, data>(*index, *newobj));
		  		delete newobj;
			  }
				objstr.str("");
  		}
	  }

		sqlite_free_table(result);
  	return newmap;
	} else
  	return NULL;
  
	return NULL;
}

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectObjects(vector<key> &objs, const string &iname) {
	typename vector<key>::iterator i = objs.begin();

	map<key, data> *newmap = new map<key, data>;

	while (i != objs.end()) {
		data *newobj = selectObject(indexname.c_str(), *i);
		if (newobj) {
			newmap->insert(pair<key, data>(*i, *newobj));
			delete newobj;
		}
	}
	return newmap;
}

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectAllObjects() {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s;\n", tablename.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i <= nrow; i++) {
		  	for (int j=1; j < ncol; j++) {
			  	objstr << result[i*ncol +j];
				  if (j < ncol -1)
						objstr << ",";
  			}
	  		objstr << endl;
#ifdef EXTRADEBUG
		  	logger->logMsg(objstr.str());
#endif
			  data *newobj = new data(objstr.str());
				string *index = new string(result[i*ncol]);
  			if (newobj) {
	  			newmap->insert(pair<key, data>(*index, *newobj));
		  		delete newobj;
			  	delete index;
				}
  			objstr.str("");
	  	}
		}

  	sqlite_free_table(result);
	  return newmap;
	} else
  	return NULL;
  
	return NULL;
}

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectDistinctObjects(timePeriod &tp, const string &iname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf(
						"SELECT * FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s WHERE (%s >= %d AND %s <= %d);\n",
							tablename.c_str(), iname.c_str(), iname.c_str(),
							tablename.c_str(), tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i <= nrow; i++) {
		  	for (int j=1; j < ncol; j++) {
					objstr << result[i*ncol +j];
  				if (j < ncol -1)
	  				objstr << ",";
		  	}
			  objstr << endl;
#ifdef EXTRADEBUG
				logger->logMsg(objstr.str());
#endif
  			data *newobj = new data(objstr.str());
	  		string *index = new string(result[i*ncol]);
		  	if (newobj) {
			  	newmap->insert(pair<key, data>(*index, *newobj));
				  delete newobj;
				}
  			objstr.str("");
	  	}
		}

  	sqlite_free_table(result);
	  return newmap;
	} else
  	return NULL;
  
	return NULL;
}

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectDistinctObjects(const string &iname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf(
						"SELECT * FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s);\n",
							tablename.c_str(), iname.c_str(),
							iname.c_str(), tablename.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i <= nrow; i++) {
		  	for (int j=1; j < ncol; j++) {
			  	objstr << result[i*ncol +j];
				  if (j < ncol -1)
						objstr << ",";
  			}
	  		objstr << endl;
#ifdef EXTRADEBUG
		  	logger->logMsg(objstr.str());
#endif
				data *newobj = new data(objstr.str());
  			string *index = new string(result[i*ncol]);
	  		if (newobj) {
		  		newmap->insert(pair<key, data>(*index, *newobj));
			  	delete newobj;
				}
  			objstr.str("");
	  	}
		}

  	sqlite_free_table(result);
	  return newmap;
	} else
  	return NULL;

	return NULL;
}

template<class key, class data> void
SQLTable<key, data>::selectDistinctObjectsMap(ostream& out, timePeriod &tp, const string &iname) {

	int rc, nrow, ncol, counter = 0;
	char *sqlcmd, **result;
	stringstream objstr;
	string index, oldindex("");
	data newobj;

	sqlcmd = sqlite_mprintf(
						"SELECT %s,* FROM %s WHERE (%s >= %d AND %s <= %d) order by %s;\n",
							iname.c_str(), tablename.c_str(),
							tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp(),
                        "TS" );
							//iname.c_str());
              
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	int excluded;
		  for (int j=0; j < ncol; j++) {
			  if (iname.compare(result[j]) == 0)
				  excluded = j;
			}
  		for (int i=1; i <= nrow; i++) {
	  		for (int j=2; j < ncol; j++) {
		  		if (j == excluded)
			  		objstr << result[i*ncol+1];
				  else
					  objstr << result[i*ncol+j];
					if (j < ncol -1)
  					objstr << ",";
	  		}
		  	objstr << endl;
#ifdef EXTRADEBUG
			  logger->logMsg(objstr.str());
#endif
				newobj = data(objstr.str());
  			index = result[i*ncol];
	  		out << "<TR>" << endl << "<TD>" << i << "<TD>";
		  	if (index.compare(oldindex) != 0) {
			  	counter = 1;
				  out << index;
				} else
  				counter++;
	  		oldindex = index;
		  	out << "<TD>" << counter << data(objstr.str()).toHTMLString();
				objstr.str("");
  		}
	  }

		sqlite_free_table(result);
  }
	
	return;
}

template<class key, class data> void
SQLTable<key, data>::selectDistinctObjectsMap(ostream& out, const string &iname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;
	string index;
	data newobj;

	sqlmap *newmap = new map<key, vector<data> >;
	typename sqlmap::iterator p;

	sqlcmd = sqlite_mprintf("SELECT %s,* FROM %s order by %s;\n",
													iname.c_str(),tablename.c_str(), iname.c_str());

#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	int excluded;
		  for (int j=0; j < ncol; j++) {
			  if (iname.compare(result[j]) == 0)
				  excluded = j;
			}
  		for (int i=1; i <= nrow; i++) {
	  		for (int j=2; j < ncol; j++) {
		  		if (j == excluded)
			  		objstr << result[i*ncol+1];
				  else
					  objstr << result[i*ncol+j];
					if (j < ncol -1)
  					objstr << ",";
	  		}
		  	objstr << endl;
#ifdef EXTRADEBUG
			  logger->logMsg(objstr.str());
#endif
				newobj = data(objstr.str());
  			index = result[i*ncol];
	  		p = newmap->find(index);
		  	if (p != newmap->end())
			  	p->second.push_back(newobj);
				else {
  				vector<data> v;
	  			v.push_back(newobj);
		  		newmap->insert(pair<key, vector<data> >(index, v));
			  }
				objstr.str("");
  		}
	  }

		sqlite_free_table(result);
  	return newmap;
	} else
  	return NULL;
  
	return NULL;
}

template<class key, class data> void
SQLTable<key, data>::insertObject(key val, data &obj) {

	int rc;
	char *sqlcmd;
 	char *zErrMsg = 0;

#ifdef DEBUG
    stringstream debug;
#endif

	if (dbcon->transactions_enabled && dbcon->transactions == 0)
		dbcon->beginTransaction();

	sqlcmd = sqlite_mprintf("INSERT INTO %s VALUES(%Q, %s);\n",
		tablename.c_str(), val.c_str(), obj.insertString().c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	if (dbcon->transactions_enabled) {
#ifdef EXTRADEBUG
    logger->logMsg("Queueing into transaction...\n");
#endif
		dbcon->transactioncmd << sqlcmd;
		dbcon->transactions++;
	} else {
    do {
  		rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, 	&zErrMsg);

	  	if (rc == SQLITE_OK) {
        dbcon->logTransaction(sqlcmd);
#ifdef EXTRADEBUG
  			logger->logMsg("Data inserted successfully\n");
#endif
			} else {
        if (rc == SQLITE_MISUSE) {
      		logger->logMsg("Reopenning database...");
          sleep(1);
	      	dbcon->reconnect();
		      logger->logMsg("done\n");
        }
        
#ifdef DEBUG
		  	debug << "Unable to INSERT data. Error CODE: " << rc << endl;
   	  	debug << "Command executed: " << sqlcmd << endl;
	  	  debug << "Error Msg : " << zErrMsg << endl;
  		  logger->logMsg(debug.str());
#endif
  		}
      if (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
        logger->logMsg("Retrying...\n");
        sleep(1);
      }
	  } while (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE);
  }

	if (dbcon->transactions == dbcon->transactions_threshold) {
		dbcon->commitTransaction();
		dbcon->transactions = 0;
		dbcon->transactioncmd.str("");
	}
	free(sqlcmd);
	return;
}

template<class key, class data> void
SQLTable<key, data>::insertObjects(map<key, data> &objs) {

	typename map<key, data>::iterator i = objs.begin();
	while (i != objs.end()) {
		this->insertObject(i->first, i->second);
		i++;
	}
}

template<class key, class data> void
SQLTable<key, data>::deleteObject(key &val, const string &iname) {

	int rc;
	char *sqlcmd;
	stringstream objstr;

#ifdef DEBUG
    stringstream debug;
#endif

	sqlcmd = sqlite_mprintf("DELETE FROM %s WHERE %s = %Q;\n",
													tablename.c_str(), iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

  do {
  	rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, NULL);
	  free(sqlcmd);
  	if (rc == SQLITE_OK) {
        dbcon->logTransaction(sqlcmd.str());
#ifdef EXTRADEBUG
		  	logger->logMsg("Entry deleted successfully\n");
#endif
  	} else {
      if (rc == SQLITE_MISUSE) {
    		logger->logMsg("Reopenning database...");
	    	dbcon->reconnect();
		    logger->logMsg("done\n");
      }

#ifdef DEBUG
	  	debug << "Unable to DELETE data. Error CODE: " << rc << endl;
		  logger->logMsg(debug.str());
#endif
  	}
    if (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
      logger->logMsg("Retrying...\n");
      sleep(1);
    }
	} while (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE);

	return;
}

template<class key, class data> void
SQLTable<key, data>::deleteObjects(const key &from, const key &to, const string &iname) {

	int rc;
	char *sqlcmd;
	stringstream objstr;

#ifdef DEBUG
    stringstream debug;
#endif

	sqlcmd = sqlite_mprintf("DELETE FROM %s WHERE (%s >= %Q AND %s <= %Q);\n",
							tablename.c_str(), iname.c_str(), from.c_str(), iname.c_str(), to.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

  do {
  	rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, NULL);
	  free(sqlcmd);
  	if (rc == SQLITE_OK) {
        dbcon->logTransaction(sqlcmd.str());
#ifdef DEBUG
		  	logger->logMsg("Entries deleted successfully\n");
#endif
  	} else {
#ifdef DEBUG
	  	debug << "Unable to DELETE data. Error CODE: " << rc << endl;
		  logger->logMsg(debug.str());
#endif
  	}
    if (rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
      logger->logMsg("Retrying...\n");
        sleep(1);
    }
	} while (rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE);

	return;
}

template<class key, class data> void
SQLTable<key, data>::deleteObjects(vector<key> &objs, const string &iname) {

	typename vector<key>::iterator i = objs.begin();
	while (i != objs.end()) {
		deleteObject(indexname.c_str(), *i);
		i++;
	}
}

template<class key, class data> void
SQLTable<key, data>::updateObject(const key &val, data &obj, const string &iname) {
	int rc;
	char *sqlcmd;

#ifdef DEBUG
    stringstream debug;
#endif
  string index;
	if (iname.compare("") == 0)
		index = indexname;
	else
		index = iname;

	if (dbcon->transactions_enabled && dbcon->transactions == 0)
		dbcon->beginTransaction();

	sqlcmd = sqlite_mprintf("UPDATE %s SET %s WHERE %s = %Q;\n",
		tablename.c_str(), obj.updateString().c_str(), index.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	if (dbcon->transactions_enabled) {
		dbcon->transactioncmd << sqlcmd;
		dbcon->transactions++;
	} else {
    do {
    	rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, NULL);

    	if (rc == SQLITE_OK) {
        dbcon->logTransaction(sqlcmd);
#ifdef EXTRADEBUG
		  	logger->logMsg("Data updated successfully\n");
#endif
  		} else {
        if (rc == SQLITE_MISUSE) {
      		logger->logMsg("Reopenning database...");
	      	dbcon->reconnect();
		      logger->logMsg("done\n");
        }

#ifdef DEBUG
	  		debug << "Unable to UPDATE data. Error CODE: " << rc << endl;
		  	logger->logMsg(debug.str());
#endif
  		}
      if (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
        logger->logMsg("Retrying...\n");      
        sleep(1);
      }
	  } while (rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE);
	}

	if (dbcon->transactions == dbcon->transactions_threshold) {
		dbcon->commitTransaction();
		dbcon->transactions = 0;
		dbcon->transactioncmd.str("");
	}

	free(sqlcmd);
	return;
}

template<class key, class data> void
SQLTable<key, data>::updateObjects(map<key, data> &objs, const string &iname) {
	typename map<key, data>::iterator i = objs.begin();
	while (i != objs.end()) {
		updateObject(indexname.c_str(), i->first, i->second);
		i++;
	}
}

template<class key, class data> size_t
SQLTable<key, data>::size(key val, const string &iname) {
	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s WHERE %s = %Q;\n",
													tablename.c_str(), iname.c_str(), val.c_str());
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			count = strtoul(result[ncol], NULL, 10);
		}
		sqlite_free_table(result);
	}
	
	return count;
}

template<class key, class data> size_t
SQLTable<key, data>::size(key from, key to, const string &iname) {
	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s WHERE (%s >= %Q AND %s <= %Q);\n",
													tablename.c_str(), iname.c_str(), from.c_str(), iname.c_str(), to.c_str());
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			count = strtoul(result[ncol], NULL, 10);
		}
		sqlite_free_table(result);
	}

	return count;
}

template<class key, class data> size_t
SQLTable<key, data>::size(timePeriod &tp) {

	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s WHERE (%s >= %d AND %s <= %d);\n",
							tablename.c_str(), tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp());
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			count = strtoul(result[ncol], NULL, 10);
		}
		sqlite_free_table(result);
	}

	return count;
}

template<class key, class data> size_t
SQLTable<key, data>::sizeofDistinctObjects(timePeriod &tp, const string &iname) {

	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;

	sqlcmd = sqlite_mprintf(
							"SELECT COUNT(*) FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s WHERE (%s >= %d AND %s <= %d));\n",
							tablename.c_str(), iname.c_str(), iname.c_str(),
							tablename.c_str(), tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp());
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			count = strtoul(result[ncol], NULL, 10);
		}
		sqlite_free_table(result);
	}

	cout << "count = " << count << endl;
	return count;
}

template<class key, class data> size_t
SQLTable<key, data>::sizeofDistinctObjects(const string &iname) {

	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;

	sqlcmd = sqlite_mprintf(
							"SELECT COUNT(*) FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s);\n",
							tablename.c_str(), iname.c_str(),
							iname.c_str(), tablename.c_str());
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			count = strtoul(result[ncol], NULL, 10);
		}
		sqlite_free_table(result);
	}

	cout << "count = " << count << endl;
	return count;
}

template<class key, class data> size_t
SQLTable<key, data>::size(	const TimeStamp &ts,
														const string &tpprefix,
														const string &qtyname) {

	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;
	string tpbegin = tpprefix + "begin";
	string tpend = tpprefix + "end";

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s WHERE (%s <= %d AND %d < %s AND %q != 0);\n",
							tablename.c_str(),
							tpbegin.c_str(), ts.getTimeStamp(),
							ts.getTimeStamp(), tpend.c_str(), qtyname.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			count = strtoul(result[ncol], NULL, 10);
		}
		sqlite_free_table(result);
	}

	return count;
}


template<class key, class data> size_t
SQLTable<key, data>::size() {
	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s;\n", tablename.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			count = strtoul(result[ncol], NULL, 10);
		}
		sqlite_free_table(result);
	}
	
	return count;
}

template<class key, class data> string
SQLTable<key, data>::sumColumn(key from, key to, const string &iname, const string &colname) {
	int rc, nrow, ncol;
	char *sqlcmd, **result;
	string res;

	sqlcmd = sqlite_mprintf("SELECT SUM(%s) FROM %s WHERE (%s >= %Q AND %s <= %Q);\n",
													colname.c_str(), tablename.c_str(),
													iname.c_str(), from.c_str(),
													iname.c_str(), to.c_str());
#ifdef DEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			res = result[ncol];
		} else
			res = "N/A";
		sqlite_free_table(result);
	}

	return res;
}

template<class key, class data> string
SQLTable<key, data>::sumColumn(	timePeriod &tp, key val,
																const string &iname, const string &colname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;
	string res;

	sqlcmd = sqlite_mprintf("SELECT SUM(%s) FROM %s WHERE (%s >= %d AND %s <= %d AND %s = %Q);\n",
							colname.c_str(), tablename.c_str(),
							tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp(),
							iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			res = result[ncol];
		} else
			res = "N/A";
		sqlite_free_table(result);
	}

	return res;
}

template<class key, class data> string
SQLTable<key, data>::sumColumn(	const TimeStamp &ts,
														const string &tpprefix,
														key val, const string &iname, 
														const string &colname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;
	string tpbegin = tpprefix + "begin";
	string tpend = tpprefix + "end";
	string res;

	map<key, data> *newmap = new map<key, data>;

		sqlcmd = sqlite_mprintf("SELECT SUM(%s) FROM %s WHERE (%s <= %d AND %d < %s AND %s != %Q);\n",
							colname.c_str(), tablename.c_str(),
							tpbegin.c_str(), ts.getTimeStamp(),
							ts.getTimeStamp(), tpend.c_str(),
							iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			res = result[ncol];
		} else
			res = "N/A";
		sqlite_free_table(result);
	}

	return res;
}


template<class key, class data> string
SQLTable<key, data>::sumColumn(key val, const string &iname, const string &colname) {
	int rc, nrow, ncol;
	char *sqlcmd, **result;
	string res;

	sqlcmd = sqlite_mprintf("SELECT SUM(%s) FROM %s WHERE %s = %Q;\n",
													colname.c_str(), tablename.c_str(),
													iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logger->logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
	free(sqlcmd);
	if (rc == SQLITE_OK) {
		if (ncol) {
			res = result[ncol];
		} else
			res = "N/A";
		sqlite_free_table(result);
	}

	return res;
}

#endif
