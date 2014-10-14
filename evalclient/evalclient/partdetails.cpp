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
#include "giftdetails.h"

partDetails::partDetails():timestamp(time(0)), giftid(0) {
	setMSDN("");
	empty = true;
}

partDetails::partDetails(string newmsdn, time_t ts, int gid)
:timestamp(ts), giftid(gid) {
	setMSDN(newmsdn);
	empty = false;
}

partDetails::partDetails(string pdstr) {

	vector<string> result;
	strTokenizer(pdstr, result, ",");

	setGiftId(atol(result[0].c_str()));
	setMSDN(result[1]);
	setTimestamp(atol(result[2].c_str()));

	empty = false;
}

partDetails::~partDetails(){
}

ostream& operator<<(ostream &out,partDetails& pd) {
	TimeStamp ts(pd.getTimestamp());
	string no(pd.getMSDN());
	out << no << "\tGift = " << (int)(pd.getGiftId()) << "\tDate = " << ts.toString();
	return out;
}

void partDetails::setMSDN(string newmsdn) {
	strcpy(msdn, newmsdn.c_str());
}

char *partDetails::getMSDN() {
	return msdn;
}

string partDetails::getMSDNStr() {
	return string(msdn);
}

size_t partDetails::sizeofMSDN() {
	return MSDNSIZE;
}

void partDetails::setTimestamp(time_t ts) {
	timestamp = ts;
}

time_t partDetails::getTimestamp() {
	return timestamp;
}

void partDetails::setGiftId(int gid) {
	giftid = gid;
}

unsigned char partDetails::getGiftId() {
	return giftid;
}

string partDetails::getStringTimestamp() {
  char str[32];
  sprintf(str,"%ld",timestamp);
  return str;
}

string partDetails::insertString() {
	stringstream result;
	string no(getMSDN());
	result << (int)(getGiftId()) << ", " << no << ", " << timestamp ;
	return result.str();
}

string partDetails::updateString() {
	stringstream result;
	string no(getMSDN());
	result << " gid = " << (int)(getGiftId())
				 <<	" msisdn = " << no << ", "
				 << " ts = " << timestamp << ", ";
	return result.str();
}

string partDetails::toHTMLString() {
	stringstream te_str;

	te_str << "<TD>" << endl;
	te_str.fill('0');
	te_str.width(12);
	te_str << getMSDN() << "</TD>\n\t<TD>" << endl;
	te_str << TimeStamp(getTimestamp()).toString() << "</TD>\n\t<TD>" << endl;
	te_str << PrizeNames.size() << ", " << PrizeNames[getGiftId()] << "</TD>" << endl;

	return te_str.str();
}

bool partDetails::isEmpty() {
	return empty;
}
