/***************************************************************************
                          partdetails.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "partdetails.h"
#include "strtokenizer.h"

/**
  * Dummy constructor. Just creates an empty object.
  */
  
partDetails::partDetails():timestamp(time(0)), giftid(0) {
	setMSISDN("");
	empty = true;
}

/**     
  * The default constructor. Creates the partDetails object
  * from the given parameters (msisdn, timestamp and prize id).
  * It also sets the boolean empty to true.
  */

partDetails::partDetails(string newmsisdn, time_t ts, int gid)
:timestamp(ts), giftid(gid) {
	setMSISDN(newmsisdn);
	empty = false;
}

/**
  * This constructor creates a partDetails object, given a string
  * representation of the parameters. These must comma-separated and
  * with the following order:
  * - giftid
  * - MSISDN
  * - timestamp (as type time_t)
  */

partDetails::partDetails(string pdstr) {

	vector<string> result;
	strTokenizer(pdstr, result, ",");

	setGiftId(atol(result[0].c_str()));
	setMSISDN(result[1]);
	setTimestamp(atol(result[2].c_str()));

	empty = false;
}

/**
  * Dummy destructor. We don't allocate anything dynamically.
  */

partDetails::~partDetails(){
}

/**
  * We overload the operator << to allow a partDetails object
  * to be output to a C++ stream. The output will be of the form:
  * MSISDN\tGID\tDATESTRING
  */

ostream& operator<<(ostream &out,partDetails& pd) {
	TimeStamp ts(pd.getTimestamp());
	string no(pd.getMSISDN());
	out << no << "\tGift = " << (int)(pd.getGiftId()) << "\tDate = " << ts.toString();
	return out;
}

/**
  * Set the MSISDN of the partdetails object to the given string
  */

void partDetails::setMSISDN(string newmsisdn) {
	strcpy(msisdn, newmsisdn.c_str());
}

/**
  * Return the MSISDN as a C string (null terminated).
  */

char *partDetails::getMSISDN() {
	return msisdn;
}

/**
  * Return the MSISDN as a string object.
  */

string partDetails::getMSISDNStr() {
	return string(msisdn);
}

/**
  * Set the timestamp of the partdetails object to the given value.
  */

void partDetails::setTimestamp(time_t ts) {
	timestamp = ts;
}

/**
  * Return the timestamp of the partdetails object.
  */

time_t partDetails::getTimestamp() {
	return timestamp;
}

/**
  * Set the prize id of the partdetails object to the given value.
  */

void partDetails::setGiftId(int gid) {
	giftid = gid;
}

/**
  * Return the prize id the partdetails object.
  */

unsigned char partDetails::getGiftId() {
	return giftid;
}

/**
  * Returns a string suitable to be used in insert* methods
  * of SQLTable.
  */

string partDetails::insertString() {
	stringstream result;
	string no(getMSISDN());
	result << (int)(getGiftId()) << ", " << no << ", " << timestamp ;
	return result.str();
}

/**
  * Returns a string suitable to be used in update* methods
  * of SQLTable.
  */

string partDetails::updateString() {
	stringstream result;
	string no(getMSISDN());
	result << " gid = " << (int)(getGiftId())
				 <<	" msisdn = " << no << ", "
				 << " ts = " << timestamp << ", ";
	return result.str();
}

/**
  * Returns a string suitable to be used in select* methods
  * of SQLTable class to create a HTML table.
  */

string partDetails::toHTMLString() {
	stringstream te_str;

	te_str << "<TD>" << endl;
	te_str.fill('0');
	te_str.width(12);
	te_str << getMSISDN() << "</TD>\n\t<TD>" << endl;
	te_str << TimeStamp(getTimestamp()).toString() << "</TD>\n\t<TD>" << endl;
	te_str << (int)getGiftId() << "</TD>" << endl;

	return te_str.str();
}

/**
  * This is to check if a giftDetails object has been created
  * with the dummy constructor giftDetails().
  */

bool partDetails::isEmpty() {
	return empty;
}
