/***************************************************************************
                          simbatchtimerhandle.cpp  -  description
                             -------------------
    begin                : Fri Feb 7 2003
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

#include "simbatchtimerhandle.h"

SimBatchTimerHandle::SimBatchTimerHandle(){
}
SimBatchTimerHandle::~SimBatchTimerHandle(){
}

void SimBatchTimerHandle::setBatchConnectionTimer(int index)
{
  
}
                                                                            
int SimBatchTimerHandle::handle_timeout(const ACE_Time_Value& expire_time,const void* sim_flag)
{
        return 0;
}
