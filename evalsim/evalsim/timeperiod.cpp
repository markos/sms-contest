/***************************************************************************
                          timeperiod.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "timeperiod.h"

timePeriod::timePeriod(const timePeriod &source) {
	beginTimeStamp = source.beginTimeStamp;
	endTimeStamp = source.endTimeStamp;
}

timePeriod::timePeriod(TimeStamp begints, TimeStamp endts)
:beginTimeStamp(begints), endTimeStamp(endts) {
}

timePeriod::timePeriod(long begints, long endts)
:beginTimeStamp(begints), endTimeStamp(endts) {
}

timePeriod::~timePeriod(){
}

bool timePeriod::dateInPeriod(TimeStamp ts) {
	if (ts > beginTimeStamp && ts < endTimeStamp)
		return true;
	else
		return false;
}

bool timePeriod::dateInPeriod(time_t ts) {
	if (beginTimeStamp < ts && endTimeStamp > ts)
		return true;
	else
		return false;
}

TimeStamp &timePeriod::getBeginTS() {
	return beginTimeStamp;
}

TimeStamp &timePeriod::getEndTS() {
	return endTimeStamp;
}

string timePeriod::toString(bool shortString) {
	return string("From : " + beginTimeStamp.toString(shortString)
										+ ", To   : " + endTimeStamp.toString(shortString) +" ");
}

