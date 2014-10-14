/***************************************************************************
                          timeperiod.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "timeperiod.h"

/**
  * The copy constructor. Initializes a TimePeriod object
  * using the contents of another.
  */
  
TimePeriod::TimePeriod(const TimePeriod &source) {
	beginTimeStamp = source.beginTimeStamp;
	endTimeStamp = source.endTimeStamp;
}

/**
  * A constructor. Takes two TimeStamp objects to create
  * a TimePeriod object.
  */

TimePeriod::TimePeriod(TimeStamp begints, TimeStamp endts)
:beginTimeStamp(begints), endTimeStamp(endts) {
}

/**
  * The default constructor. Takes two timestamps (but this
  * time of type time_t) to create a TimePeriod object.
  * These values can be ommitted, taking defaults of 0 and 86400
  * respectively, meaning the TimePeriod is one day, starting
  * Jan 1, 1970.
  */

TimePeriod::TimePeriod(long begints, long endts)
:beginTimeStamp(begints), endTimeStamp(endts) {
}

/**
  * Dummy destructor. We don't allocate anything dynamically
  */

TimePeriod::~TimePeriod(){
}

/**
  * If ts (type TimeStamp object) is between beginTimeStamp
  * and endTimeStamp then the method returns true, otherwise
  * it returns false.
  */
  
bool TimePeriod::dateInPeriod(TimeStamp ts) {
	if (ts > beginTimeStamp && ts < endTimeStamp)
		return true;
	else
		return false;
}

/**
  * If ts (type time_t) is between beginTimeStamp
  * and endTimeStamp then the method returns true, otherwise
  * it returns false.
  */

bool TimePeriod::dateInPeriod(time_t ts) {
	if (beginTimeStamp < ts && endTimeStamp > ts)
		return true;
	else
		return false;
}

/**
  * Returns the number of days of this time period
  */

int TimePeriod::periodInDays() {
  int days = (int)ceil((double)(endTimeStamp.getTimeStamp() - beginTimeStamp.getTimeStamp())/(double)(86400));
  return days;
}

/**
  * Returns the beginning of the period TimeStamp object
  */
  
TimeStamp &TimePeriod::getBeginTS() {
	return beginTimeStamp;
}

/**
  * Returns the end of the period TimeStamp object
  */

TimeStamp &TimePeriod::getEndTS() {
	return endTimeStamp;
}

/**
  * Returns a string representation of the TimePeriod object.
  * If the parameter shortString is false, then it just returns the
  * locale string representation of the TimeStamp objects
  * (just as date command would) separated by '-'.
  * Otherwise the dates are of the form YYYYMMDD.
  */

string TimePeriod::toString(bool shortString) {
	return string("From : " + beginTimeStamp.toString(shortString)
										+ ", To   : " + endTimeStamp.toString(shortString) +" ");
}

