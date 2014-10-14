/***************************************************************************
                          strtokenizer.cpp  -  description
                             -------------------
    begin                : Wed Jan 29 2003
    copyright            : (C) 2003 by Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "strtokenizer.h"

/**
  * This helper function, takes a given string (str), separates it using the
  * given delimiters, and outputs the result to the given vector<string> (splitted).
  */
  
bool strTokenizer( string &str, vector<string>& splitted, const string delimiter)
{
	string::size_type lastPos = str.find_first_not_of(delimiter, 0);

 	string::size_type pos     = str.find_first_of(delimiter, lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {
        splitted.push_back(str.substr(lastPos, pos - lastPos));

        lastPos = str.find_first_not_of(delimiter, pos);

        pos = str.find_first_of(delimiter, lastPos);
    }

	return true;
}
