/***************************************************************************
                          strtokenizer.cpp  -  description
                             -------------------
    begin                : Wed Jan 29 2003
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

#include "strtokenizer.h"

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
