/***************************************************************************
                          giftdetails.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef GIFTDETAILS_H
#define GIFTDETAILS_H

using namespace std;

/*!
  * \class giftDetails
	* \brief Wrapper class to deal with the prizes
  *
  * This class provides an easy way to hold prize information,
  * and is been used for the value part of the SQL table
  * PRIZES that is been used, and provides access to info, such
  * as the prize id, its name, the initial quantity, remaining
  * and the actual period for which it is available.
  */

#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "timeperiod.h"

class giftDetails {
  /// The prize id
	u_int giftid;

  /// The prize name
	string giftName;

  /// A TimePeriod during which the prizes will be available
	TimePeriod period;

  /// Initial quantity of the prizes
	int initial;

  /// Current quantity of the prizes
	int remaining;

  /// boolean for SQLTable<> template class
	bool empty;

public:
  /// These enums are used to distinguish the types of the prizes
  enum {  ID_NOGIFT = 0, ID_MINUTEPRIZES = 1, ID_HOURLYPRIZES = 2,
          ID_DAILYPRIZES = 3, ID_WEEKLYPRIZES = 4, ID_MONTHLYPRIZES = 5,
          ID_USEDCODE = 10 };

  /// dummy constructor that just creates an empty structure.
	giftDetails();

  /// Default constructor
	giftDetails(int gid, string newgiftName, TimePeriod tp, int initQ, int rem);

  /// Constructor that uses a string representation of the parameters to create the object
	giftDetails(string giftstr);

  /// Dummy destructor 
	~giftDetails();

  /// Returns the id of the prize, usually one of the enums ID_*
	u_int getGiftId();

  /// Returns the name of prize as a string
	string getName();

  /// Returns the initial quantity of the current prize
	size_t getInitialQuantity();

  /// Returns the current quantity (available prizes)
	size_t getCurrentQuantity();

  /// Returns the time period object
  TimePeriod &getTimePeriod();

  /// Sets current quantity to the given number
	void setCurrentQuantity(int curQ);

  /// The operator -- decreases the current quantity of the prize by 1
	void operator--(int);

  /// Sets the name to the given string
  void setName(string name);

  /// Sets the initial quantity to the given number
  void setInitialQuantity(int initQ);

	/// This is for the SQLTable<> template class
  string insertString();

  /// This is for the SQLTable<> template class
	string updateString();

  /// boolean method to be used in SQLTable<> methods
	bool isEmpty();

	/// Return duration of specific gift.
	u_int getGiftDuration();
};

#endif
