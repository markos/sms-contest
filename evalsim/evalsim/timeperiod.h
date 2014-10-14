/***************************************************************************
                          timeperiod.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef TIMEPERIOD_H
#define TIMEPERIOD_H

using namespace std;

#include "timestamp.h"

/**
  *@author Konstantinos Margaritis
  */

class timePeriod {
	TimeStamp beginTimeStamp, endTimeStamp;
public:
	timePeriod(const timePeriod &source);
	timePeriod(TimeStamp begints, TimeStamp endts);
	timePeriod(long begints = 0, long endts = 0 +86400);
	~timePeriod();

	TimeStamp &getBeginTS();
	TimeStamp &getEndTS();
	bool dateInPeriod(TimeStamp ts);
	bool dateInPeriod(time_t ts);
	string toString(bool shortString = false);
};

#endif
