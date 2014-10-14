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

/*!
  * \class TimeStamp
	* \brief TimeStamp manipulation and handling class
  *
  * Since we are keeping all timing information in UNIX
  * timestamp form, we have created a class to handle time and
  * date information in that form. This class allows a creation
  * of an object given a date, a timeinfo struct, a timestamp.
  * It also allows simple arithmetic operations and comparisons to
  * be performed between TimeStamp objects.
  * One of the most useful methods is the toString() which returns
  * a string representation in short or long form.
  */

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

class TimeStamp{
private:
  /// Internally the class keeps time information in a timeinfo struct
	struct tm timeinfo;

  /// ... and a unix timestamp.
	time_t timestamp;
public:
  /// The copy constructor
	TimeStamp(const TimeStamp &source);

  /// Construct a TimeStamp object given a date
	TimeStamp(int year, int month, int mday, int hour = 0, int mins = 0, int secs = 0);

  /// Construct a TimeStamp given a timeinfo structure
	TimeStamp(struct tm *timeinfo);

  /// Construct a TimeStamp object given a UNIX timestamp.
	TimeStamp(time_t ts = 0);

  /// Typical destructor
	~TimeStamp();

  /// Add a day to the current timestamp
	void operator++(int);

  /// Comparison operator between current TimeStamp and an integer
	bool operator>(time_t ts);

  /// Comparison operator between current and another TimeStamp object
	bool operator>(TimeStamp ts);

  /// Comparison operator between current TimeStamp and an integer
	bool operator<(time_t ts);

  /// Comparison operator between current and another TimeStamp object
	bool operator<(TimeStamp ts);

  /// Equality operator between current and another TimeStamp object
	bool operator==(TimeStamp ts);

  /// Comparison operator between current TimeStamp and an integer
	bool operator==(time_t ts);

  /// Difference operator between current and another TimeStamp object
	bool operator!=(TimeStamp ts);
  
  /// Comparison operator between current TimeStamp and an integer
	bool operator!=(time_t ts);

  /// Returns the current timestamp in time_t format
	const time_t getTimeStamp();
 
  /// Sets current timestamp to the given value
	void setTimeStamp(time_t ts);

  /// Returns the current timestamp + 1 day in time_t format
  time_t nextDay();

  /// Returns the current timestamp - 1 day in time_t format
 	time_t previousDay();

  /// Returns the current timestamp + 1 week in time_t format
	time_t nextWeek();

  /// Returns the current timestamp + 1 month in time_t format
	time_t nextMonth();

  /// Returns a string representation (short or long form)
	string toString(bool isShort = false);
};

#endif
