/***************************************************************************
                          timestamp.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

using namespace std;

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

/**
  *@author Konstantinos Margaritis
  */

class TimeStamp{
private:
	struct tm timeinfo;
	time_t timestamp;
public:
	TimeStamp(const TimeStamp &source);
	TimeStamp(int year, int month, int mday, int hour = 0, int mins = 0, int secs = 0);
	TimeStamp(struct tm *timeinfo);
	TimeStamp(time_t ts = 0);
	~TimeStamp();

	void operator++(int);
	bool operator>(time_t ts);
	bool operator>(TimeStamp ts);
	bool operator<(time_t ts);
	bool operator<(TimeStamp ts);
	bool operator==(TimeStamp ts);
	bool operator==(time_t ts);
	bool operator!=(TimeStamp ts);
	bool operator!=(time_t ts);
	
	const time_t getTimeStamp();
	void setTimeStamp(time_t ts);
  time_t nextDay();
 	time_t previousDay();
	time_t nextWeek();
	time_t nextMonth();

	string toString(bool isShort = false);
};

#endif
