/***************************************************************************
                          timestamp.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "timestamp.h"

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

TimeStamp::TimeStamp(struct tm *ti) {
	struct tm *tinfo;
	
	timestamp = mktime(ti);
	tinfo = localtime(&timestamp);
	timeinfo = *tinfo;
}

TimeStamp::TimeStamp(time_t ts){
	struct tm *tinfo;
	
	timestamp = ts;
	tinfo = localtime(&ts);
	timeinfo = *tinfo;
}

TimeStamp::TimeStamp(const TimeStamp &source){
	struct tm *tinfo;

	timestamp = source.timestamp;
	tinfo = localtime(&timestamp);
	timeinfo = *tinfo;
}

TimeStamp::~TimeStamp(){
}

void TimeStamp::operator++(int) {
	timestamp += 86400;
	timeinfo = *(localtime(&timestamp));
}

bool TimeStamp::operator<(time_t ts) {
	if (timestamp < ts)
		return true;
	else
		return false;
}

bool TimeStamp::operator<(TimeStamp ts) {
	if (timestamp < ts.getTimeStamp())
		return true;
	else
		return false;
}

bool TimeStamp::operator>(time_t ts) {
	if (timestamp > ts)
		return true;
	else
		return false;
}

bool TimeStamp::operator>(TimeStamp ts) {
	if (timestamp > ts.getTimeStamp())
		return true;
	else
		return false;
}

bool TimeStamp::operator==(TimeStamp ts) {
	if (timestamp == ts.getTimeStamp())
		return true;
	else
		return false;
}

bool TimeStamp::operator==(time_t ts) {
	if (timestamp == ts)
		return true;
	else
		return false;
}

bool TimeStamp::operator!=(TimeStamp ts) {
	if (timestamp != ts.getTimeStamp())
		return true;
	else
		return false;
}

bool TimeStamp::operator!=(time_t ts) {
	if (timestamp != ts)
		return true;
	else
		return false;
}

const time_t TimeStamp::getTimeStamp() {
	return timestamp;
}

void TimeStamp::setTimeStamp(time_t ts){
	struct tm *tinfo;

	timestamp = ts;
	tinfo = localtime(&ts);
	timeinfo = *tinfo;
}

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
		return asctime(&timeinfo);
}
