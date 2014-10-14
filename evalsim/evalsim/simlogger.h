/***************************************************************************
                          simlogger.h  -  description
                             -------------------
    begin                : Mon Feb 10 2003
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

#ifndef SIMLOGGER_H
#define SIMLOGGER_H


/**
  *@author Alex Davelos
  */

class SimLogger {
public: 
	SimLogger();
	~SimLogger();

  int getTotalCount() { return message_counter; }
  int getCorrectCount() { return correct_message_counter; }
  void newCount(int correct_flag); 

private:
  int message_counter;
  int correct_message_counter;
};

#endif
