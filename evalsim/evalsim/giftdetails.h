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

#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "timeperiod.h"

/**
  *@author Konstantinos Margaritis
  */

const static int	ID_HOURLYPRIZES = 1,
									ID_DAILYPRIZES = 2,
									ID_WEEKLYPRIZES = 3,
									ID_MONTHLYPRIZES = 4,
									ID_USEDCODE = 10;

enum giftType {Hourly, Daily, Weekly, Monthly};

/// The names of the prizes
static map<int, string> PrizeNames;
				
class giftDetails {
	int giftid;
	string giftName;
	timePeriod period;
	int initial;
	int remaining;
	bool empty;

public:
	giftDetails();
	giftDetails(int gid, string newgiftName, timePeriod tp, int initQ, int rem);
	giftDetails(string giftstr);
	~giftDetails();

	int getGiftId();
	string getName();
	size_t getInitialQuantity();
	size_t getCurrentQuantity();
	void setCurrentQuantity(int curQ);
	void operator--(int);
  void setName(string name);
  void setInitialQuantity(int initQ);
  string insertString();
	string updateString();
	bool isEmpty();

	/// Return duration of specific gift.
	u_int getGiftDuration();

};

#endif
