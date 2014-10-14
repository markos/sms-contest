/***************************************************************************
                          logger.h  -  description
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

#ifndef LOGGER_H
#define LOGGER_H


/**
  *@author Alex Davelos
  */

#include <string>
using namespace std;

/* This class is declared in order to leave the sqltable class
    untouced, and to avoid using the evalserver logger class
    which includes ace features . The only method used by sqltable class
    is log_msg which in this imlementation does nothing at all...
*/

enum output_t { OUTPUT_STDOUT, OUTPUT_BOTH, OUTPUT_FILEONLY};
                                                                                
class Logger {
public: 
	Logger();
   Logger(int,string& str){ /*used just for consistency reasons */  }
	~Logger();
  void setLogFileName(const string& str){ /*used just for consistency reasons */  }
  void open(int lo){ /*used just for consistency reasons */  }   
   void logMsg(const string& str) { /*used just for consistency reasons */ }
};

#endif
