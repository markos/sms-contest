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

/*!
  * \class TimePeriod
	* \brief Class to handle timeperiods consistently
  *
  * Since we're not dealing with single timestamps, but with time periods,
  * that is, with pairs of timestamps (begin, end) we needed something more
  * than the TimeStamp class. Hence the TimePeriod class
  */

#include "timestamp.h"

class TimePeriod {
  /// The Timestamp of the start of the period
	TimeStamp beginTimeStamp;

  /// The Timestamp of the start of the period
  TimeStamp endTimeStamp;
public:
  /// The copy constructor
	TimePeriod(const TimePeriod &source);

  /// Constructor, accepting two TimeStamp objects as parameters
	TimePeriod(TimeStamp begints, TimeStamp endts);

  /// Default constructor, accepting two time_t objects as parameters, or no parameters at all.
  TimePeriod(long begints = 0, long endts = 86400);

  /// Dummy destructor
	~TimePeriod();

  /// Returns a reference to the begin TimeStamp
	TimeStamp &getBeginTS();

  /// Returns a reference to the end TimeStamp
	TimeStamp &getEndTS();

  /// Returns true if given timestamp (TimeStamp object) is _in_ the current TimePeriod object
	bool dateInPeriod(TimeStamp ts);

  /// Returns true if given timestamp (time_t) is _in_ the current TimePeriod object
	bool dateInPeriod(time_t ts);

  /// Returns the number of days in the time period
  int periodInDays();

  /// Returns a short or long string representation of the time period.
	string toString(bool shortString = false);
};

#endif
