/***************************************************************************
                          strTokenizer.h  -  description
                             -------------------
    begin                : Tue Jan 14 2003
    copyright            : (C) 2003 by Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

using namespace std;
#include <string>
#include <vector>

#ifndef STRTOKENIZER_H
#define STRTOKENIZER_H

bool strTokenizer( string &str, vector<string>& splitted, const string delimiter);

#endif
