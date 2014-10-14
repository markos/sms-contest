/***************************************************************************
                          partdetails.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef PARTDETAILS_H
#define PARTDETAILS_H

using namespace std;

#include <vector>
#include <string>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>

#include "timestamp.h"

#define MSDNSIZE	13
#define NO_GIFT		0

/**
  *@author Konstantinos Margaritis
  */

class partDetails {
private:
	char msdn[MSDNSIZE+1];
	time_t timestamp;
	unsigned char giftid;
	bool empty;
public:
	partDetails();
	partDetails(string newmsdn, time_t ts, int gid);
	partDetails(string pdstr);
//	partDetails(partDetails &);
	~partDetails();

	friend ostream& operator<<(ostream &out,partDetails &pd);
	void setMSDN(string newmsdn);
	char *getMSDN();
	string getMSDNStr();
	static size_t sizeofMSDN();
	void setTimestamp(time_t ts);
	time_t getTimestamp();
	void setGiftId(int gid);
	unsigned char getGiftId();
  string getStringTimestamp();
  string insertString();
  string updateString();
  string toHTMLString();
  bool isEmpty();
};

#endif
