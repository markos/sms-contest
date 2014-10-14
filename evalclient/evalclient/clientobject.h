/***************************************************************************
                          clientobject.h  -  description
                             -------------------
    begin                : Mon Sep 2 2002
    copyright            : (C) 2002 by Alex Davelos
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

#include <iostream>
#ifndef CLIENTOBJECT_H
#define CLIENTOBJECT_H

using namespace std;

#include <ace/Thread_Manager.h>
#include <ace/Get_Opt.h>
/**
  *@author Alex Davelos
  */

class clientObject {
public: 
	clientObject();
	~clientObject();
  void init();
};

#endif
