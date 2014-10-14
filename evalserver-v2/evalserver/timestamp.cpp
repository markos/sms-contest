/***************************************************************************
                          timestamp.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "timestamp.h"

/**
  * The default constructor, creates a TimeStamp object given the actual date
  * as parameters. You may omit the hour, mins and secs.
  */

TimeStamp::TimeStamp(int year, int month, int mday, int hour, int mins, int secs) {
	struct tm tinfo;

	tinfo.tm_hour = hour;
	tinfo.tm_min = mins;
	tinfo.tm_sec = secs;
	tinfo.tm_wday = 0;
	tinfo.tm_isdst = 0;
	tinfo.tm_mday = mday;
	tinfo.tm_mon = month - 1;
	tinfo.tm_year = year -1900;

	timestamp = mktime(&tinfo);
	timeinfo = tinfo;
}

/**
  * Another constructor, it can use a timeinfo structure to create the TimeStamp.
  */
  
TimeStamp::TimeStamp(struct tm *ti) {
	struct tm *tinfo;
	
	timestamp = mktime(ti);
	tinfo = localtime(&timestamp);
	timeinfo = *tinfo;
}

/**
  * This constructor uses the actual timestamp (number of seconds since Jan 1, 1970)
  * to create the TimeStamp object.
  */
  
TimeStamp::TimeStamp(time_t ts){
	struct tm *tinfo;
	
	timestamp = ts;
	tinfo = localtime(&ts);
	timeinfo = *tinfo;
}

/**
  * The copy constructor
  */
  
TimeStamp::TimeStamp(const TimeStamp &source){
	struct tm *tinfo;

	timestamp = source.timestamp;
	tinfo = localtime(&timestamp);
	timeinfo = *tinfo;
}

/**
  * Dummy destructor.
  */
  
TimeStamp::~TimeStamp(){
}

/**
  * We have overloaded the ++ operator to increase the date by one day.
  */
  
void TimeStamp::operator++(int) {

	timeinfo.tm_mday++;

	timestamp = mktime(&timeinfo);
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the operator <
  */

bool TimeStamp::operator<(time_t ts) {
	if (timestamp < ts)
		return true;
	else
		return false;
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the operator <
  */

bool TimeStamp::operator<(TimeStamp ts) {
	if (timestamp < ts.getTimeStamp())
		return true;
	else
		return false;
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the operator >
  */

bool TimeStamp::operator>(time_t ts) {
	if (timestamp > ts)
		return true;
	else
		return false;
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the operator >
  */

bool TimeStamp::operator>(TimeStamp ts) {
	if (timestamp > ts.getTimeStamp())
		return true;
	else
		return false;
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the equality operator ==.
  */

bool TimeStamp::operator==(TimeStamp ts) {
	if (timestamp == ts.getTimeStamp())
		return true;
	else
		return false;
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the equality operator ==.
  */

bool TimeStamp::operator==(time_t ts) {
	if (timestamp == ts)
		return true;
	else
		return false;
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the non-equality operator !=.
  */

bool TimeStamp::operator!=(TimeStamp ts) {
	if (timestamp != ts.getTimeStamp())
		return true;
	else
		return false;
}

/**
  * A TimeStamp may be compared to another TimeStamp object or a UNIX timestamp
  * (which is of time_t).
  * This overloads the non-equality operator !=.
  */

bool TimeStamp::operator!=(time_t ts) {
	if (timestamp != ts)
		return true;
	else
		return false;
}

/**
  * Returns the UNIX timestamp of the current object
  */

const time_t TimeStamp::getTimeStamp() {
	return timestamp;
}

/**
  * Sets the timestamp of the current object to the given one.
  * Instead of just copying the timestamp, it also synchronizes
  * the timeinfo structure as well.
  */

void TimeStamp::setTimeStamp(time_t ts){
	struct tm *tinfo;

	timestamp = ts;
	tinfo = localtime(&ts);
	timeinfo = *tinfo;
}

/**
  * Returns the UNIX timestamp of the next day of the current object,
  * that is 24 hours from the timestamp of this object.
  * Note: In contrast to the operator ++ and --,
  * this method does not change the current object
  * but creates a copy of it.
  */

time_t TimeStamp::nextDay() {
	struct tm tinfo;

	tinfo.tm_hour = timeinfo.tm_hour;
	tinfo.tm_min = timeinfo.tm_min;
	tinfo.tm_sec = timeinfo.tm_sec;
	tinfo.tm_wday = timeinfo.tm_wday;
	tinfo.tm_isdst = timeinfo.tm_isdst;
	tinfo.tm_mday = timeinfo.tm_mday +1;
	tinfo.tm_mon = timeinfo.tm_mon;
	tinfo.tm_year = timeinfo.tm_year;

	time_t ts = mktime(&tinfo);
	return ts;
}

/**
  * Returns the UNIX timestamp of the previous day of the current object,
  * that is 24 hours before the timestamp of this object.
  * The same notes apply as in nextDay().
  */

time_t TimeStamp::previousDay() {
	struct tm tinfo;

	tinfo.tm_hour = timeinfo.tm_hour;
	tinfo.tm_min = timeinfo.tm_min;
	tinfo.tm_sec = timeinfo.tm_sec;
	tinfo.tm_wday = timeinfo.tm_wday;
	tinfo.tm_isdst = timeinfo.tm_isdst;
	tinfo.tm_mday = timeinfo.tm_mday -1;
	tinfo.tm_mon = timeinfo.tm_mon;
	tinfo.tm_year = timeinfo.tm_year;

	time_t ts = mktime(&tinfo);
	return ts;
}

/**
  * Returns the UNIX timestamp of the next week of the current object,
  * that is 7 days after the timestamp of this object.
  * The same notes apply as in nextDay().
  */

time_t TimeStamp::nextWeek() {
	struct tm tinfo;

	tinfo.tm_hour = timeinfo.tm_hour;
	tinfo.tm_min = timeinfo.tm_min;
	tinfo.tm_sec = timeinfo.tm_sec;
	tinfo.tm_wday = timeinfo.tm_wday;
	tinfo.tm_isdst = timeinfo.tm_isdst;
	tinfo.tm_mday = timeinfo.tm_mday +7;
	tinfo.tm_mon = timeinfo.tm_mon;
	tinfo.tm_year = timeinfo.tm_year;

	time_t ts = mktime(&tinfo);
	return ts;
}

/**
  * Returns the UNIX timestamp of the next month of the current object,
  * that is 1 month after the timestamp of this object.
  * This is better than adding 30 days, because it also takes care of
  * shorter months such as February.
  * The same notes apply as in nextDay().
  */

time_t TimeStamp::nextMonth() {
	struct tm tinfo;

	tinfo.tm_hour = timeinfo.tm_hour;
	tinfo.tm_min = timeinfo.tm_min;
	tinfo.tm_sec = timeinfo.tm_sec;
	tinfo.tm_wday = timeinfo.tm_wday;
	tinfo.tm_isdst = timeinfo.tm_isdst;
	tinfo.tm_mday = timeinfo.tm_mday;
	tinfo.tm_mon = timeinfo.tm_mon +1;
	tinfo.tm_year = timeinfo.tm_year;

	time_t ts = mktime(&tinfo);
	return ts;
}

/**
  * Returns a string representation of the current TimeStamp object.
  * If isShort is set to false, the representation will be analytical
  * (just as the output of the command date), while if it is true, the
  * string will be of the form YYYYMMDD. This is very useful in filenames
  * and in statistics.
  */

string TimeStamp::toString(bool isShort) {
	if (isShort) {
		stringstream tsstr;
		tsstr <<  timeinfo.tm_year+1900;
		tsstr << setw(2) << setfill('0');
		tsstr << timeinfo.tm_mon +1;
		tsstr << setw(2) << setfill('0');
		tsstr << timeinfo.tm_mday;
		return tsstr.str();
	} else
		return ctime(&timestamp);
}
