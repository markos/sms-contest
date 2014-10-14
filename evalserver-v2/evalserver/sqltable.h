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

/*!
  * \class SQLTable
	* \brief Template class to provide an easy way to access an SQL table.
  *
  * This class is one of the most important of the program.
  * It allows a uniform and more or less consistent way of accessing the database
  * independently of the actual information in the tables.
  * It deals with most of the important SQL commands in a consistent way. These
  * include INSERT, UPDATE, DELETE, SELECT. It offers a number of methods to assist
  * programming in most of the usual ways one may call the SQL commands.
  * It is designed to complement the SQLiteConnection class and right now it cannot
  * be used with another adapter.
  *
  * The class is a templated class. It uses two abstract classes, namely a key class
  * and a data class. The key class is the one that is used as the index of the SQL
  * table. It can be a PRIMARY KEY or not, this is declared on the construction of
  * the object (and the creation of the respective table).
  * The only requirement for the key class is to provide a c_str() method that returns
  * a pointer to a C string (that is null-terminated).
  * As for the data class there are a few requirements as well:
  * - It must provide a method insertString(), which returns the part of the SQL INSERT
  *   command, without the key. For example, if the INSERT command would be:\n
  *
  *   INSERT INTO CODES VALUES('76138768176', 1, '309641768762', 1231444534);\n
  *
  *   then the insertString() method should return:\n
  *
  *   1, '309641768762', 1231444534\n
  *
  *   This should then be used to form the complete INSERT command to be executed.
  * - Likewise, the data class must provide an updateString() to be used when
  *   calling UPDATE on the data.
  * - For information reasons only, it may provide a toHTMLString() method,
  *   that displays a table row of its data.
  * - Equally important is the existance of two constructors, a dummy that sets a
  *   boolean variable "empty" to true, and a constructor that takes as an argument
  *   the string representation of the data. The data is first read using a SELECT
  *   command, is output in a string as comma-separated values and passed as a
  *   parameter to the data class constructor. The constructor should then tokenize
  *   the string and use these values to initialize itself. Of course it has to set
  *   the empty boolean to false afterwards.
  *   It is a generic way and can be used for pretty much any object that can be put
  *   in a table.
  */

#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <ace/OS.h>

#include "sqliteconnection.h"
#include "timeperiod.h"

template<class key, class data> class SQLTable {
  /// This boolean declares whether a table is ready to be used.
	bool ready;

  /// Pointer to the SQLiteConnection object
	SQLiteConnection *dbcon;

  /// The name of the table
	string tablename;

  /// The name of the key field of the table
  string indexname;

  /// The name of the timestamp field
  string tsname;

  /// A vector that holds the names of all the fields
  vector<string> fieldnames;

  /// This vector holds the names of the _types)_ of the fields
  vector<string> fieldtypes;

  /// This vector holds the names of the fields to be indexed.
  vector<string> indices;

  /// If true then the index is also a PRIMARY KEY
	bool hasPrimaryKey;

  /// We typedef the map<key, vector<data> > type to sqlmap.
	typedef map<key, vector<data> > sqlmap;
public:

  /// Dummy constructor
  SQLTable() { ready = false; }
  
  /// The default constructor.
	SQLTable(SQLiteConnection *db,
						const string &tname, const string &iname,
						const string &ts, vector<string> &fnames,
            vector<string> &ftypes, vector<string> &indexnames, bool primarykey);

  /// Copy constructor, copies a SQLTable object to another.
	SQLTable(const SQLTable &source);

  /// We overload the assignment operator. Basically the same procedure as the copy constructor
	SQLTable &operator=(const SQLTable &source);

  /// The default destructor.
	~SQLTable() { };

  /// Creates the indices described in the vector indices.
  bool createIndices();

  /// Drops the current table from the database
	bool drop();

  /// Returns a key, data pair of the record that exists in the position index.
	pair<key, data> selectObject(size_t index);

  /// Returns the data object of the record where the field iname = val.
	data selectObject(key val, const string &iname);

  /// Returns a map of <key, data> objects where iname field is in the range (from, to).
	map<key, data> * selectObjects(const key &from, const key &to, const string &iname);

  /// Returns a map of <key, data> objects that have timestamps in the period tp
	map<key, data> * selectObjects(TimePeriod &tp);

  /// Returns a map of <key, data> objects that their timeperiods include timestamp ts and qtyname != 0
	map<key, data> * selectObjects(	TimeStamp &ts, const string &tpprefix,
																		const string &qtyname);

  /// Return a map of <key, data> objects where the field iname, has values from the vector objs.
	map<key, data> * selectObjects(vector<key> &objs, const string &iname);

  /// Return a map of <key, data> of all objects.
	map<key, data> * selectAllObjects();

  /// Fill given vector with all objects of indexname.
  void selectAllObjects(vector<string> &keys);

  /// Return a map of the UNIQUE objects (using field iname) in the timeperiod tp.
	map<key, data> * selectDistinctObjects(TimePeriod &tp, const string &iname);

  /// Return a map of all UNIQUE objects (using field iname)
	map<key, data> * selectDistinctObjects(const string &iname);

  /// Write the UNIQUE objects (using field iname) in the timeperiod tp in the stream out as HTML
	void selectDistinctObjectsMap(ostream& out, TimePeriod &tp, const string &iname);

  /// Write all UNIQUE objects (using field iname) in the stream out as HTML
	void selectDistinctObjectsMap(ostream& out, const string &iname);

  /// Inserts the object pair (val, obj) in the table.
	void insertObject(key val, data &obj);

  /// Inserts the given map of objects in the table.
	void insertObjects(map<key, data> &objs);

  /// Updates the object where field iname has the value val according to the values of data obj.
	void updateObject(const key &val, data &obj, const string &iname);

  /// Updates the map of objects, using the index field iname.
	void updateObjects(map<key, data> &objs, const string &iname);

  /// Deletes the object where field iname has value val
	void deleteObject(key &val, const string &iname);

  /// Deletes the objects where field iname has values in the range from-to
	void deleteObjects(const key &from, const key &to, const string &iname);

  /// Delete the objects where the field iname takes values from the vector objs.
	void deleteObjects(vector<key> &objs, const string &iname);

  /// Returns the number of records where field iname has value val
	size_t size(key val, const string &iname);

  /// Returns the number of records where field iname has value in the range from-to
	size_t size(key from, key to, const string &iname);

  /// Returns the number of records where the field tsname has values in the timeperiod tp.
	size_t size(TimePeriod &tp);

  /// Returns the number of records where their timeperiods include timestamp ts and qtyname != 0
	size_t size(const TimeStamp &ts, const string &tpprefix, const string &qtyname);

  /// Return the size of the table
	size_t size();

  /// Return the size of UNIQUE objects (using index iname) in the timeperiod tp
	size_t sizeofDistinctObjects(TimePeriod &tp, const string &iname);

  /// Return the size of UNIQUE objects (using index iname)
	size_t sizeofDistinctObjects(const string &iname);

  /// Return the sum of field colname of records where field iname is in the range from-to.
	string sumColumn(key from, key to, const string &iname, const string &colname);

  /// Return the sum of field colname of records where field iname is equal to val and in the timeperiod tp
	string sumColumn(TimePeriod &tp, key val, const string &iname, const string &colname);

  /// Return the sum of field colname of records where field iname is equal to val and their timeperiods include timestamp ts and qtyname != 0
	string sumColumn(	const TimeStamp &ts, const string &tpprefix,
										key val, const string &iname, const string &colname);

  /// Return the sum of field colname of records where field iname is equal to val
	string sumColumn(key val, const string &iname, const string &colname);

  /// Overload the operator [] to access the object where indexname is equal to x.
	data operator[](key x) {
		return selectObject(x, indexname);
	}

  /// Overload the operator [] to access the object in the position ind.
	pair<key, data> operator[](size_t ind) {
		return selectObject(ind);
	}

  /// Return true if the table is opened and operational
	bool isReady() { return ready; }

  /// Wrapper around logger object's logMsg
  void logMsg(string msg);
};

/**
  * This is the copy constructor, copies the contents of a source SQLTable
  * to another
  */
  
template<class key, class data>
SQLTable<key, data>::SQLTable(const SQLTable &source)
:dbcon(source.dbcon),
tablename(source.tablename),
indexname(source.indexname),
tsname(source.tsname),
fieldnames(source.fieldnames),
fieldtypes(source.fieldtypes),
indices(source.indices),
ready(source.ready),
hasPrimaryKey(source.hasPrimaryKey) { }

/**
  * This is the assignment operator =, works just like the copy constructor
  */

template<class key, class data> SQLTable<key, data>&
SQLTable<key, data>::operator=(const SQLTable &source) {
	this->dbcon = source.dbcon;
	this->tablename = source.tablename;
	this->indexname = source.indexname;
	this->tsname = source.tsname;
	this->fieldnames = source.fieldnames;
 	this->fieldtypes = source.fieldtypes;
  this->indices = source.indices;
	this->ready = source.ready;
	this->hasPrimaryKey = source.hasPrimaryKey;

	return *this;
}

/**
  * This is the constructor. Apart from initializing the member variables
  * dbcon, tablename, indexname, tsname, fieldnames, fieldtypes and
  * hasPrimaryKey, it is responsible to check for the existense of the table
  * and create it if needed.
  * The creation of the table uses the following information:
  * - The key of the table (the first field) is indexname
  * - It is declared as PRIMARY KEY if hasPrimaryKey is set to true
  * - All the fields are created using the names and types in fieldnames and types vectors
  * Upon successfull creation the ready flag is set to true.
  */

template<class key, class data> 
SQLTable<key, data>::SQLTable<key, data>(SQLiteConnection *db,
																					const string &tname, const string &iname,
																					const string &ts,
																					vector<string> &fnames,
                                          vector<string> &ftypes,
                                          vector<string> &indexnames,
																					bool primarykey)
:dbcon(db), tablename(tname),
indexname(iname), tsname(ts), fieldnames(fnames), fieldtypes(ftypes),
indices(indexnames), hasPrimaryKey(primarykey) {

#ifdef DEBUG
		stringstream debug;
#endif

	if (!dbcon->existsTable(tablename)) {
		int rc;
   	char *zErrMsg = 0;

		stringstream sqlcmd;
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
    
#ifdef DEBUG
		logMsg(sqlcmd.str());
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
        logMsg(debug.str());
        ACE_OS::sleep(1);
#endif
      }
		} while (rc == SQLITE_BUSY);
	} else {
#ifdef DEBUG
		logMsg(string("Table " + tablename + " exists\n"));
#endif
	}

  createIndices();
	ready = true;
}

/**
  * This method creates the necessary indices for the current table, if needed.
  * First it checks for the existence of the indices with existsIndex().
  * If it exists it does a CREATE INDEX on the table, using a UNIQUE index
  * if the field is a PRIMARY KEY.
  * Upon successfull creation the ready flag is set to true.
  * The indices are VERY important to the performance of the database, esp.
  * with very large datasets.
  */


template<class key, class data> bool
SQLTable<key, data>::createIndices() {

#ifdef DEBUG
		stringstream debug;
#endif
  stringstream index, sqlcmd;
  int rc;
  char *zErrMsg = 0;
  
  vector<string>::iterator i = indices.begin();
  
  while (i != indices.end()) {
    sqlcmd.str("");
    index.str("");
    index << tablename << "_index_" << *i;

    if (dbcon->existsIndex(index.str(), tablename)) {
  		logMsg(string("Index " + index.str() + " exists\n"));
    } else {
      logMsg(string("Index " + index.str() + " does not exist\n"));

      if (hasPrimaryKey && indexname.compare(*i) == 0)
        sqlcmd << "CREATE UNIQUE INDEX ";
      else
     		sqlcmd  << "CREATE INDEX ";

      sqlcmd  << index.str() << " ON " << tablename
              << "(" << *i << ");" << endl;

#ifdef DEBUG
  		logMsg(sqlcmd.str());
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
          logMsg(debug.str());
          ACE_OS::sleep(1);
  #endif
        }
  		} while (rc == SQLITE_BUSY);
    }
    i++;
  }

  return true;
}

/**
  * Drops the table, NOT a destuctor. The destructor should not delete the table from
  * the database.
  */

template<class key, class data> bool
SQLTable<key, data>::drop() {
	int rc;
#ifdef DEBUG
	stringstream debug;
#endif

	stringstream sqlcmd;
	sqlcmd << "DROP TABLE " << tablename << ";" << endl;
#ifdef DEBUG
	logMsg(sqlcmd.str());
#endif

  do {
  	rc = sqlite_exec (dbcon->getdb(), sqlcmd.str().c_str(), NULL, NULL, NULL);
	  if (rc == SQLITE_OK) {
      dbcon->logTransaction(sqlcmd.str());
      return true;
    } else {
#ifdef DEBUG
		  	debug << "Unable to drop table: Error CODE = " << rc << endl;
			  logMsg(debug.str());
#endif
  		return false;
    }
	} while (rc == SQLITE_BUSY);
}

/**
  * Returns the pair <key, data> of the record which is in the position
  * index of the TABLE. Basically it uses the OFFSET parameter in SELECT.
  */

template<class key, class data> pair<key, data>
SQLTable<key, data>::selectObject(size_t index) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s LIMIT 1 OFFSET %d;\n",
													tablename.c_str(), index);
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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
			logMsg(objstr.str());
#endif
  		return pair<key, data> (result[ncol], data(objstr.str()));
		}

		sqlite_free_table(result);
  } else
		return pair<key, data> ("", data());

	return pair<key, data> ("", data());
}

/**
  * Returns the pair <key, data> of the record where field iname
  * has value val.
  */

template<class key, class data> data
SQLTable<key, data>::selectObject(key val, const string &iname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s WHERE %s = %Q LIMIT 1;\n",
													tablename.c_str(), iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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
	  	logMsg(objstr.str());
#endif
		  return data(objstr.str());
		}

  	sqlite_free_table(result);
	} else
  	return data();
  
	return data();
}

/**
  * Returns the pair <key, data> of the record where field iname
  * has value in the range (from, to).
  */

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectObjects(const key &from, const key &to, const string &iname) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s WHERE (%s >= %Q AND %s <= %Q);\n",
							tablename.c_str(), iname.c_str(), from.c_str(), iname.c_str(), to.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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
	  		logMsg(objstr.str());
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

/**
  * Returns the pair <key, data> of the record where field tsname
  * (which is used to hold timestamp information) is inside the timeperiod
  * tp.
  */

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectObjects(TimePeriod &tp) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s WHERE (%s >= %d AND %s <= %d);\n",
							tablename.c_str(), tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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
	  		logMsg(objstr.str());
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

/**
  * Returns the pair <key, data> of the record where the given ts
  * is inside the values of the fields {tpprefix}_begin and {tpprefix}_end
  * and where the value of the field qtyname is != 0.
  */

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
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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
		  	logMsg(objstr.str());
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

/**
  * Returns the map of objects <key, data> of the records where
  * the field iname takes values given in the vector<key> objs.
  */

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

/**
  * Returns the map of objects <key, data> of all the records.
  * This one should be used with care, esp. with large databases.
  */

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectAllObjects() {

	int rc, nrow, ncol;
	char *sqlcmd, **result;
	stringstream objstr;

	map<key, data> *newmap = new map<key, data>;

	sqlcmd = sqlite_mprintf("SELECT * FROM %s;\n", tablename.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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
		  	logMsg(objstr.str());
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

/** Fill the given vector keys, with all the objects of the column indexname.
  */
template<class key, class data> void
SQLTable<key, data>::selectAllObjects(vector<string> &keys) {

	int rc, nrow, ncol;
	char *sqlcmd, **result;

  sqlcmd = sqlite_mprintf("SELECT %s FROM %s;\n", indexname.c_str(), tablename.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
#endif

	rc = sqlite_get_table(dbcon->getdb(), sqlcmd, &result, &nrow, &ncol, NULL);
  free(sqlcmd);
	if (rc == SQLITE_OK) {
  	if (nrow) {
	  	for (int i=1; i <= nrow; i++) {
#ifdef EXTRADEBUG
		  	logMsg(objstr.str());
#endif
				string *index = new string(result[i*ncol]);
        keys.push_back(*index);
		  	delete index;
	  	}
		}
  	sqlite_free_table(result);
	}
  return;
}


/**
  * Returns the map of objects <key, data> of the UNIQUE records where
  * the field tsname has values inside the period tp. We check for
  * uniqueness using the field iname.
  */

template<class key, class data> map<key, data> *
SQLTable<key, data>::selectDistinctObjects(TimePeriod &tp, const string &iname) {

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
	logMsg(sqlcmd);
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
				logMsg(objstr.str());
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

/**
  * Returns the map of objects <key, data> of the UNIQUE records using the field iname.
  */

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
	logMsg(sqlcmd);
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
		  	logMsg(objstr.str());
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

/**
  * This does not return anything. Instead we provide a filestream descriptor
  * and it writes an HTML table of the UNIQUE records (using the field iname)
  * where their tsname field is inside the timeperiod tp.
  * The class data is required to provide a toHTMLString() method.
  * The performance is much greater than creating a map of the objects in RAM,
  * and then writing it to disk.
  */

template<class key, class data> void
SQLTable<key, data>::selectDistinctObjectsMap(ostream& out, TimePeriod &tp, const string &iname) {

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
							iname.c_str());

#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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
			  logMsg(objstr.str());
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

/**
  * This does not return anything. Instead we provide a filestream descriptor
  * and it writes an HTML table of the UNIQUE records (using the field iname).
  * The class data is required to provide a toHTMLString() method.
  * The performance is much greater than creating a map of the objects in RAM,
  * and then writing it to disk.
  */

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
	logMsg(sqlcmd);
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
			  logMsg(objstr.str());
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

/**
  * Inserts the object (val, obj.insertString()) to the table.
  * The data class is required to provide an insertString() method.
  * The key class is required to provide a c_str() method.
  * The same procedure is followed as in SQLiteConnection::commitTransaction().
  */

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
	logMsg(sqlcmd);
#endif

	if (dbcon->transactions_enabled) {
#ifdef EXTRADEBUG
    logMsg("Queueing into transaction...\n");
#endif
		dbcon->transactioncmd << sqlcmd;
		dbcon->transactions++;
	} else {
    do {
  		rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, 	&zErrMsg);

	  	if (rc == SQLITE_OK) {
        dbcon->conflicts = 0;
        dbcon->logTransaction(sqlcmd);
#ifdef EXTRADEBUG
  			logMsg("Data inserted successfully\n");
#endif
			} else {

        dbcon->conflicts++;
        if (rc == SQLITE_MISUSE || rc == SQLITE_CANTOPEN) {
      		logMsg("Reopenning database...");
          ACE_OS::sleep(1);
	      	dbcon->reconnect();
		      logMsg("done\n");
        }
        
#ifdef DEBUG
		  	debug << "Unable to INSERT data. Error CODE: " << rc << endl;
   	  	debug << "Command executed: " << sqlcmd << endl;
	  	  debug << "Error Msg : " << zErrMsg << endl;
  		  logMsg(debug.str());
#endif
  		}
      if (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
        logMsg("Retrying...\n");
        ACE_OS::sleep(1);
      }
	  } while (dbcon->conflicts < dbcon->max_conflicts && (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE));
  }

	if (dbcon->transactions == dbcon->transactions_threshold)
		dbcon->commitTransaction();
  
	free(sqlcmd);
	return;
}

/**
  * Inserts the objects (val, obj.insertString()) included in the given
  * map to the table. Calls insertObject() for each object.
  */

template<class key, class data> void
SQLTable<key, data>::insertObjects(map<key, data> &objs) {

	typename map<key, data>::iterator i = objs.begin();
	while (i != objs.end()) {
		this->insertObject(i->first, i->second);
		i++;
	}
}

/**
  * Deletes the object where field has value val from the table.
  * The key class is required to provide a c_str() method.
  */

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
	logMsg(sqlcmd);
#endif

  do {
  	rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, NULL);
	  free(sqlcmd);
  	if (rc == SQLITE_OK) {
        dbcon->logTransaction(sqlcmd.str());
#ifdef EXTRADEBUG
		  	logMsg("Entry deleted successfully\n");
#endif
  	} else {
      if (rc == SQLITE_MISUSE) {
    		logMsg("Reopenning database...");
	    	dbcon->reconnect();
		    logMsg("done\n");
      }

#ifdef DEBUG
	  	debug << "Unable to DELETE data. Error CODE: " << rc << endl;
		  logMsg(debug.str());
#endif
  	}
    if (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
      logMsg("Retrying...\n");
      ACE_OS::sleep(1);
    }
	} while (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE);

	return;
}

/**
  * Deletes the object where field has value val in the range (from,to).
  * The key class is required to provide a c_str() method.
  */

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
	logMsg(sqlcmd);
#endif

  do {
  	rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, NULL);
	  free(sqlcmd);
  	if (rc == SQLITE_OK) {
        dbcon->logTransaction(sqlcmd.str());
#ifdef DEBUG
		  	logMsg("Entries deleted successfully\n");
#endif
  	} else {
#ifdef DEBUG
	  	debug << "Unable to DELETE data. Error CODE: " << rc << endl;
		  logMsg(debug.str());
#endif
  	}
    if (rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
      logMsg("Retrying...\n");
//      ACE_OS::sleep(1);
    }
	} while (rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE);

	return;
}

/**
  * Deletes the objects included in the given vector objs from the table.
  * Calls deleteObject() for each object.
  */

template<class key, class data> void
SQLTable<key, data>::deleteObjects(vector<key> &objs, const string &iname) {

	typename vector<key>::iterator i = objs.begin();
	while (i != objs.end()) {
		deleteObject(indexname.c_str(), *i);
		i++;
	}
}

/**
  * Updates the object where field iname has value iname, using the values in the
  * object data.
  * The data class is required to provide for an updateString() method.
  * The same procedure is followed as in SQLiteConnection::commitTransaction().
  */

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
	logMsg(sqlcmd);
#endif

	if (dbcon->transactions_enabled) {
		dbcon->transactioncmd << sqlcmd;
		dbcon->transactions++;
	} else {
    do {
    	rc = sqlite_exec(dbcon->getdb(), sqlcmd, NULL, NULL, NULL);

    	if (rc == SQLITE_OK) {
        dbcon->conflicts = 0;
        dbcon->logTransaction(sqlcmd);
#ifdef EXTRADEBUG
		  	logMsg("Data updated successfully\n");
#endif
  		} else {
        dbcon->conflicts++;
    
        if (rc == SQLITE_MISUSE || rc == SQLITE_CANTOPEN) {
      		logMsg("Reopenning database...");
	      	dbcon->reconnect();
		      logMsg("done\n");
        }

#ifdef DEBUG
	  		debug << "Unable to UPDATE data. Error CODE: " << rc << endl;
		  	logMsg(debug.str());
#endif
  		}
      if (rc == SQLITE_ERROR || rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE) {
        logMsg("Retrying...\n");      
        ACE_OS::sleep(1);
      }
	  } while (dbcon->conflicts < dbcon->max_conflicts && (rc == SQLITE_BUSY || rc == SQLITE_CANTOPEN || rc == SQLITE_MISUSE));
	}

	if (dbcon->transactions == dbcon->transactions_threshold)
		dbcon->commitTransaction();

	free(sqlcmd);
	return;
}

/**
  * Updates the objects (val, obj.updateString()) included in the given
  * map to the table. Calls updateObject() for each object.
  */

template<class key, class data> void
SQLTable<key, data>::updateObjects(map<key, data> &objs, const string &iname) {
	typename map<key, data>::iterator i = objs.begin();
	while (i != objs.end()) {
		updateObject(indexname.c_str(), i->first, i->second);
		i++;
	}
}

/**
  * Returns the number of all the records whom the field iname has
  * value equal to val.
  */

template<class key, class data> size_t
SQLTable<key, data>::size(key val, const string &iname) {
	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s WHERE %s = %Q;\n",
													tablename.c_str(), iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/**
  * Returns the number of all the records whom the field iname has
  * value in the range (from, to).
  */

template<class key, class data> size_t
SQLTable<key, data>::size(key from, key to, const string &iname) {
	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s WHERE (%s >= %Q AND %s <= %Q);\n",
													tablename.c_str(), iname.c_str(), from.c_str(), iname.c_str(), to.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/**
  * Returns the number of all the records whom the field tsname (which
  * holds timestamp information) is inside the timeperiod tp.
  */

template<class key, class data> size_t
SQLTable<key, data>::size(TimePeriod &tp) {

	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;
	stringstream objstr;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s WHERE (%s >= %d AND %s <= %d);\n",
							tablename.c_str(), tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/**
  * Returns the number of all the unique records (using the index iname) whom
  * the field tsname (whichholds timestamp information) is inside the timeperiod tp.
  */

template<class key, class data> size_t
SQLTable<key, data>::sizeofDistinctObjects(TimePeriod &tp, const string &iname) {

	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;

	sqlcmd = sqlite_mprintf(
							"SELECT COUNT(*) FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s WHERE (%s >= %d AND %s <= %d));\n",
							tablename.c_str(), iname.c_str(), iname.c_str(),
							tablename.c_str(), tsname.c_str(), tp.getBeginTS().getTimeStamp(),
							tsname.c_str(), tp.getEndTS().getTimeStamp());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/**
  * Returns the number of all the unique records (using the index iname).
  */

template<class key, class data> size_t
SQLTable<key, data>::sizeofDistinctObjects(const string &iname) {

	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;

	sqlcmd = sqlite_mprintf(
							"SELECT COUNT(*) FROM %s WHERE %s IN (SELECT DISTINCT %s FROM %s);\n",
							tablename.c_str(), iname.c_str(),
							iname.c_str(), tablename.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/**
  * Returns the number of the records where the given ts
  * is inside the values of the fields {tpprefix}_begin and {tpprefix}_end
  * and where the value of the field qtyname is != 0.
  */

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
	logMsg(sqlcmd);
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

/**
  * Returns the number of all records.
  */

template<class key, class data> size_t
SQLTable<key, data>::size() {
	int rc, nrow, ncol;
	size_t count = 0;
	char *sqlcmd, **result;

	sqlcmd = sqlite_mprintf("SELECT COUNT(*) FROM %s;\n", tablename.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/**
  * Returns the sum of the field colname, of all the records for which the field iname has
  * value in the range (from, to).
  */

template<class key, class data> string
SQLTable<key, data>::sumColumn(key from, key to, const string &iname, const string &colname) {
	int rc, nrow, ncol;
	char *sqlcmd, **result;
	string res;

	sqlcmd = sqlite_mprintf("SELECT SUM(%s) FROM %s WHERE (%s >= %Q AND %s <= %Q);\n",
													colname.c_str(), tablename.c_str(),
													iname.c_str(), from.c_str(),
													iname.c_str(), to.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/**
  * Returns the sum of the field colname, of all the records for which the field iname is
  * equal to val and the field tsname is inside the timeperiod tp.
  */

template<class key, class data> string
SQLTable<key, data>::sumColumn(	TimePeriod &tp, key val,
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
	logMsg(sqlcmd);
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

/**
  * Returns the sum of the field colname, of all the records for which the given ts 
  * is inside the values of the fields {tpprefix}_begin and {tpprefix}_end
  * and where the value of the field qtyname is != 0.
  */

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
	logMsg(sqlcmd);
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

/**
  * Returns the sum of the field colname, of all the records for which the file iname
  * is equal to val.
  */

template<class key, class data> string
SQLTable<key, data>::sumColumn(key val, const string &iname, const string &colname) {
	int rc, nrow, ncol;
	char *sqlcmd, **result;
	string res;

	sqlcmd = sqlite_mprintf("SELECT SUM(%s) FROM %s WHERE %s = %Q;\n",
													colname.c_str(), tablename.c_str(),
													iname.c_str(), val.c_str());
#ifdef EXTRADEBUG
	logMsg(sqlcmd);
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

/** Instead of using directly the logmsg method from the logger object,
    we wrap it with another one, so that if it (the logger) is not available
    (at the start or the end of execution) we will still have a logging
    mechanism available via cout.
  */

template<class key, class data> void
SQLTable<key, data>::logMsg(string msg) {
  if (dbcon->logger)
    dbcon->logger->logMsg(msg);
  else
    cout << msg;

  return;
}


#endif
