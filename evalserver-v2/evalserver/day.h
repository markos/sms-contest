/***************************************************************************
                          day.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef DAY_H
#define DAY_H

/*!
  * \class Day
	* \brief	This class holds info about a day in the competition
  *
  * The day class is used for the following reasons:
  * - Hold statistical information for the participations daily.
  * - Hold the array of prizes to be given daily.
  * - execute the actual draw of the competition, that is decide whether
  *   this participation wins or not, and what.\n
  * There are three possible ways to create a Day object, using three possible
  * constructors.
  * The first Day() is just a dummy constructor that just creates an empty shell
  * to be populated later. It is not usable.
  * The second takes a string representation of a Day's details, actually the result
  * from an SQL SELECT command in the COUNTERS table, so that it can construct
  * the Day again. It's only used in statistics.
  * The third constructor, takes the time period, a flag to signify whether we are
  * at the start of a new week or month, and performs the following actions:
  * - Estimate the number of the participations for the particular day.
  * - Assign the prizes in fixed positions in an array that holds the messages.
  * - provide a method to be called from the Processor object, to execute the actual
  *   draw of the competition.\n
  * The class Day is also used as a template class for SQLTable<>. For this reason,
  * we have created two methods insertString() and updateString(), that return
  * the daily counters as strings representations, to be used in the insert* and
  * update* SQLTable methods respectively.
  * About the DayMessages vector: This one has a "current index" variable, a cursor,
  * that moves forward one position for each participation. At the start of the day,
  * this vector is cleared and resized to hold the estimated messages for this day.
  * For each participation, it picks the value at the current position and returns it
  * to the participant. The value is either 0 (for no prize), or the id of the prize
  * itself.
  */

using namespace std;

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <cmath>
#include <ace/Synch.h>

#include "contest.h"
#include "timeperiod.h"
#include "Count.h"
#include "strtokenizer.h"

class Day {
private:
  /// An enumerator object for the calculation of the estimated number of messages
	enum {default_avg_threshold = 5};

  /// Enumeration objects for the default and minimum number of est. number of messages.
	enum {default_estNoMsgs = 10000, min_estNoMsgs = 1600};

  /// A pointer to the Contest part of which is this.
	class Contest *parentContest;

  /// The vector that holds the Prizes that are given to the participants.
	vector<u_int> DayMessages;

  /// The Processor is a friend class and can access the private members of Day.
	friend class Processor;

  /// Counter of the current number of participants.
	Count counter;

  /// The unique players for today
  Count unique_counter;

  /// The estimated number of messages for this day.
	u_int estNoMsgs;

  /// The number of prizes won so far in the current day
  u_int prizes;

  /// The current day period
	TimePeriod day_period;

  /// boolean for SQLTable<> template class
  bool empty;
public:
	///	The class constructor
	Day(class Contest *myContest, TimeStamp &ts);

	/// Class constructor for SQLTable<> support
	Day(string daystr);

  /// dummy constructor that just creates an empty structure
	Day() {	empty = true; }

	/// return the current day TimePeriod object
	TimePeriod getTimePeriod();

	/// Sets the time period
	void setTimePeriod(TimePeriod &tp);

	/// Assign the prizes to the DayMessages.
	void assignPrizes();
	
	/// Calculate estimated Number of Messages for this day
	u_int calculateEstNoMsgs();

	/// Return estimated Number of Messages for this day
	u_int getEstNoMsgs();

	/// Execute draw for the given partDetails object.
	bool executeDraw(partDetails &pd);

  /// Set counter
  void setCounter(u_int &val);
  
	/// return counter
	u_int getCounter();

	/// return unique counter
	u_int getUniqueCounter();

	/// return no of prizes
	u_int getNoPrizes();

	/// This is for the SQLTable<> template class
	string insertString();

	/// This is for the SQLTable<> template class
	string updateString();

	/// boolean method to be used in SQLTable<> methods
	bool isEmpty();
};

#endif
