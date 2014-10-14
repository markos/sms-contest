/***************************************************************************
                          strTokenizer.h  -  description
                             -------------------
    begin                : Tue Jan 14 2003
    copyright            : (C) 2003 by Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

using namespace std;
 
#include <string>
#include <vector>

#ifndef STRTOKENIZER_H
#define STRTOKENIZER_H

/// Separates a given string using the given delimiters and outputs the result to a vector
bool strTokenizer( string &str, vector<string>& splitted, const string delimiter);

#endif
