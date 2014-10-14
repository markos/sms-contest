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

/*!
  * \class partDetails
	* \brief Wrapper class to deal with the participations
  *
  * This class provides a way to simplify the access and processing of
  * participation data. Along with the other classes giftDetails and Day
  * it provides an interface to the SQLTable class, and we use this
  * facility to populate the SQL tables CODES and WINNINGCODES.
  */

#include <vector>
#include <string>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>

#include "timestamp.h"
#include "giftdetails.h"

class partDetails {
public:
  /// The size of the string holding the MSISDN is an enum
  enum { MSISDNSIZE = 13};

private:
  /// The MSISDN is kept as a C string
	char msisdn[MSISDNSIZE+1];

  /// The timestamp of the participation
	time_t timestamp;

  /// The id of the prize won (see giftdetails.h for details)
	unsigned char giftid;

  /// boolean for SQLTable<> template class
	bool empty;

public:
  /// dummy constructor that just creates an empty structure.
	partDetails();

  /// The default constructor, takes the MSISDN as string, the timestamp and the prize id
	partDetails(string newmsisdn, time_t ts, int gid);

  /// This constructor builds a partDetails object from a string representation of the parameters.
	partDetails(string pdstr);

  /// Dummy destructor, we don't allocate anything dynamically
	~partDetails();

  /// We overload the operator << to allow a partDetails to be output to a C++ stream.
	friend ostream& operator<<(ostream &out,partDetails &pd);

  /// Set the MSISDN to the given string
	void setMSISDN(string newmsisdn);

  /// Returns the MSISDN as a C string (null terminated).
	char *getMSISDN();

  /// Returns the MSISDN as a C++ string object.
	string getMSISDNStr();

  /// Sets the timestamp to the given timestamp
  void setTimestamp(time_t ts);

  /// Returns the timestamp as a time_t object.
	time_t getTimestamp();

  /// Sets the Prize Id to the given one.
	void setGiftId(int gid);

  /// Returns the Prize id of the participation
	unsigned char getGiftId();

  /// This is for the SQLTable<> template class' insert* methods
  string insertString();

  /// This is for the SQLTable<> template class' update* methods
  string updateString();

  /// This is for the SQLTable<> template class, to create a HTML table
  string toHTMLString();

  /// boolean method to be used in SQLTable<> methods
  bool isEmpty();
};

#endif
