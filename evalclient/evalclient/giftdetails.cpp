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

giftDetails::giftDetails()
:giftid(0), giftName(""), period(), initial(0), remaining(0) {
	empty = true;
}


giftDetails::giftDetails(	int gid, string newgiftName, timePeriod tp,
													int initQ, int rem)
:giftid(gid), giftName(newgiftName), period(tp), initial(initQ), remaining(rem) {
	empty = false;
}

giftDetails::giftDetails(string gstr) {

	vector<string> result;
	strTokenizer(gstr, result, ",");

	giftid = strtol(result[0].c_str(), NULL, 10);
	setName(result[1]);
	TimeStamp start = TimeStamp(strtol(result[2].c_str(), NULL, 10));
	TimeStamp end = TimeStamp(strtol(result[3].c_str(), NULL, 10));
	period = timePeriod(start, end);
	setInitialQuantity(strtol(result[4].c_str(), NULL, 10));
	setCurrentQuantity(strtol(result[5].c_str(), NULL, 10));
	empty = false;
}

giftDetails::~giftDetails(){
}

int giftDetails::getGiftId() {
	return giftid;
}

string giftDetails::getName() {
	return giftName;
}

size_t giftDetails::getInitialQuantity() {
	return initial;
}

size_t giftDetails::getCurrentQuantity() {
	return remaining;
}

void giftDetails::setCurrentQuantity(int curQ) {
	remaining = curQ;
}

void giftDetails::operator--(int) {
	if (remaining > 0)
		remaining--;
}

void giftDetails::setInitialQuantity(int initQ) {
  initial=initQ;
}

void giftDetails::setName(string name) {
  giftName=name;
}

/** Returns the duration that a specific gift is available in days.
*/
u_int giftDetails::getGiftDuration() {
	switch (giftid) {
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
		}
}

string giftDetails::insertString() {
	stringstream result;
	result	<< (int)(giftid)
					<< ", \"" << giftName
					<< "\", " << period.getBeginTS().getTimeStamp()
					<< ", " << period.getEndTS().getTimeStamp()
					<< ", " << initial
					<< ", " << remaining;
	return result.str();
}

string giftDetails::updateString() {
	stringstream result;
	result	<< "gid = " << (int)(giftid)
					<< ", name = \"" << giftName << "\""
					<< ", periodbegin = " << period.getBeginTS().getTimeStamp()
					<< ", periodend = " << period.getEndTS().getTimeStamp()
					<< ", qty_init = " << initial
					<< ", qty_rem = " << remaining;
	return result.str();
}

bool giftDetails::isEmpty() {
	return empty;
}
