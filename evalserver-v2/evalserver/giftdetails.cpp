/***************************************************************************
                          giftdetails.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "giftdetails.h"
#include "strtokenizer.h"

/**
  * Dummy constructor. Just creates an empty object.
  */

giftDetails::giftDetails()
:giftid(0), giftName(""), period(), initial(0), remaining(0) {
	empty = true;
}

/**
  * The default constructor creates a giftDetails object given the
  * parameters. Apart from initializing the variables, it also sets
  * empty to false.
  */

giftDetails::giftDetails(	int gid, string newgiftName, TimePeriod tp,
													int initQ, int rem)
:giftid(gid), giftName(newgiftName), period(tp), initial(initQ), remaining(rem) {
	empty = false;
}

/**
  * This constructor creates a giftDetails object, given a string
  * representation of the parameters. These must comma-separated and
  * with the following order:
  * - giftid
  * - name
  * - start of period
  * - end of period
  * - initial quantity
  * - current quantity
  */

giftDetails::giftDetails(string gstr) {

	vector<string> result;
	strTokenizer(gstr, result, ",");

	giftid = strtol(result[0].c_str(), NULL, 10);
	setName(result[1]);
	TimeStamp start = TimeStamp(strtol(result[2].c_str(), NULL, 10));
	TimeStamp end = TimeStamp(strtol(result[3].c_str(), NULL, 10));
	period = TimePeriod(start, end);
	setInitialQuantity(strtol(result[4].c_str(), NULL, 10));
	setCurrentQuantity(strtol(result[5].c_str(), NULL, 10));
	empty = false;
}

/**
  * Dummy destructor
  */
  
giftDetails::~giftDetails(){
}

/**
  * Return the prize id
  */
  
u_int giftDetails::getGiftId() {
	return giftid;
}

/**
  * Return the name of the prize as a string object
  */
  
string giftDetails::getName() {
	return giftName;
}

/**
  * Return the initial quantity of the current prize.
  */
  
size_t giftDetails::getInitialQuantity() {
	return initial;
}

/**
  * Return the current quantity of the current prize.
  */

size_t giftDetails::getCurrentQuantity() {
	return remaining;
}

/**
  * Return the time period that the prize is available in.
  */

TimePeriod &giftDetails::getTimePeriod() {
	return period;
}

/**
  * Sets the current quantity of the current prize to the given number curQ
  */

void giftDetails::setCurrentQuantity(int curQ) {
	remaining = curQ;
}

/**
  * Overloads the -- operator for this class.
  * It decreases the current quantity of this prize by 1.
  */

void giftDetails::operator--(int) {
	if (remaining > 0)
		remaining--;
}

/**
  * Sets the initial quantity of the current prize to the given number initQ
  */

void giftDetails::setInitialQuantity(int initQ) {
  initial=initQ;
}

/**
  * Sets the name of the prize to the given name
  */

void giftDetails::setName(string name) {
  giftName=name;
}

/**
  * Returns the duration that a specific gift is available in days.
  */
  
u_int giftDetails::getGiftDuration() {
/*	switch (giftid) {
    case ID_MINUTEPRIZES:
		case ID_HOURLYPRIZES:
		case ID_DAILYPRIZES:
			return 1;
			break;
		case ID_WEEKLYPRIZES:
			return 7;
			break;
		case ID_MONTHLYPRIZES:
			return 30;
			break;
		default:
			return 0;
			break;
		}*/
    return period.periodInDays();
}

/**
  * returns a string suitable to be used in insert* methods
  * of SQLTable.
  */
  
string giftDetails::insertString() {
	stringstream result;
	result	<< (int)(giftid)
					<< ", \'" << giftName
					<< "\', " << period.getBeginTS().getTimeStamp()
					<< ", " << period.getEndTS().getTimeStamp()
					<< ", " << initial
					<< ", " << remaining;
	return result.str();
}

/**
  * returns a string suitable to be used in update* methods
  * of SQLTable.
  */

string giftDetails::updateString() {
	stringstream result;
	result	<< "gid = " << (int)(giftid)
					<< ", name = \'" << giftName << "\'"
					<< ", periodbegin = " << period.getBeginTS().getTimeStamp()
					<< ", periodend = " << period.getEndTS().getTimeStamp()
					<< ", qty_init = " << initial
					<< ", qty_rem = " << remaining;
	return result.str();
}

/**
  * This is to check if a giftDetails object has been created
  * with the dummy constructor giftDetails().
  */

bool giftDetails::isEmpty() {
	return empty;
}
