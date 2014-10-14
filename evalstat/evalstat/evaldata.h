/***************************************************************************
                          evaldata.h  -  description
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

#ifndef EVALDATA_H
#define EVALDATA_H

#include <string>               
#include   <vector>
#include <iostream>
#include <strtokenizer.h>
#include "timestamp.h"

using namespace std;

/**
  *@author Alex Davelos
  */

/* The class that holds the data that are retrieved from
    the database...            */
class EvalData {
public: 
   EvalData();
   EvalData(const EvalData& e);
  ~EvalData();
  EvalData(const string& sqlite_str);
  string getData() { return data; }
  
    /** Convert CSV String data to vector of string dataVector */
  bool tokenizeData();
  /** Convert the string vector to HTML Row string
     argument cont specifies whether a <TR> html tag is used or not
   */
  void setHTMLRow(int cont);
  /** No descriptions */
  string toHTMLString();
  /** Get the vector of strings in order to be passed into 
the HTML Editor object to write rows... */
  vector<string> getVectorData();

  /** Set the timestamp of data */
  void setTimestamp(const string& t);
  /** Add links to the data vector */
  void addLinks(vector<string> links);
  
private:
   /* initial data */
   string data;
   /* data as a string vector */
   vector<string> dataVector;
   /* data as an html row */
   string dataHTML;

};


#endif
