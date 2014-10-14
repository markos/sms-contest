/***************************************************************************
                          simlogger.cpp  -  description
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

#include "simlogger.h"

SimLogger::SimLogger()
{    
    correct_message_counter = 0;
    message_counter = 0;
}
SimLogger::~SimLogger()
{
}

void SimLogger::newCount(int correct_flag)
{
  if ( correct_flag )
  {
    correct_message_counter ++;
    message_counter ++;
  }
  else
  {
    message_counter ++;
  }

}
