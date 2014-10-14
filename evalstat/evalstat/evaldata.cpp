/***************************************************************************
                          evaldata.cpp  -  description
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

#include "evaldata.h"

EvalData::EvalData()
{
   data = "";
   dataHTML = "";
   dataVector.clear();
}

EvalData::EvalData(const EvalData& copy_data)
{
  data = copy_data.data;
  dataHTML = copy_data.dataHTML;
  dataVector = copy_data.dataVector;
}

EvalData::EvalData(const string& sqlite_str) : data ( sqlite_str )
{
   dataHTML="";
   dataVector.clear();
}

EvalData::~EvalData(){
}
/** No descriptions */
bool EvalData::tokenizeData()
{
  string delim = ",";
  return strTokenizer(data,dataVector,delim);
}
/** No descriptions */
void EvalData::setHTMLRow(int cont)
{
     if (!cont)
     {
        dataHTML += "<TR>";
     }
     vector<string>::iterator i;
     for ( i = dataVector.begin(); i !=dataVector.end(); i++)
     {
            dataHTML+="<TD>"+*i+"</TD>";
     }
     dataHTML+=  "</TR>";
      
}
/** No descriptions */
string EvalData::toHTMLString(){
  tokenizeData();
  string tempstring = dataVector[2];
  int ts = atoi ( tempstring.c_str() );
  TimeStamp tt (ts);
  dataVector[2] = tt.toString( false );
  setHTMLRow(1);


  
  return dataHTML;
}
/** Get the vector of strings in order to be passed into 
the HTML Editor object to write rows... */
vector<string> EvalData::getVectorData()
{
   return dataVector;
}
/** Set the timestamp of data */
void EvalData::setTimestamp(const string& t)
{

  dataVector.insert(dataVector.begin(),t);
}


/** Add links to the data vector */
void EvalData::addLinks(vector<string> links)
{
    for ( int i=0 ; i < links.size() ; i++ )
         dataVector.push_back ( links[i] );
}
