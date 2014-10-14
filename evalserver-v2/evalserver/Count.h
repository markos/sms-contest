/***************************************************************************
                          Count.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/


#ifndef COUNT_H
#define COUNT_H

/**
	*@author	Konstantinos Margaritis
	*@brief	a thread safe mutex'ed counter object
    When working in a multithreaded environment, we have to use
    special structures to make sure that a variable has not changed
    value when we are trying to read it. the Count object does this
    very thing. It wraps an integer with an ACE_Mutex object so that
    no two threads can access the variable at the same time. We use
    this class for the counters table.
  */
  
using namespace std;

#include <ace/Version.h>
#if ACE_MAJOR_VERSION >= 5 && ACE_MINOR_VERSION >= 2
#  include <ace/Atomic_Op.h>
#else
#  include <ace/Synch_T.h>
#endif
#include <string>
#include <sstream>

typedef ACE_Atomic_Op<ACE_Mutex, unsigned int> counter_t;

class Count {
protected:
  /** The actual counter object. It is of type ACE_Atomic_Op
      using templates ACE_Mutex and unsigned int
    */
  counter_t counter_;
public:
  /** A simple initializing constructor, just sets counter_ to 0.
    */
  Count() { counter_ = 0; }

  /** The copy constructor.
    */
	Count(counter_t &counter): counter_(counter) {}

  /** A constructor that takes a number in a string and converts to
      an integer.
    */
	Count(string str) {
    counter_ = atol(str.c_str());
	}

  /** Overloaded ++ operator, increases counter_ by 1.
    */
	void operator++(int) { ++counter_; }

  /** Overloaded -- operator, decreases counter_ by 1.
    */
	void operator--(int) { --counter_; }
  
  /** Retrieves current value of counter_.
    */
	unsigned int val() { return counter_.value(); }

  /** Sets value of counter_ to given val.
    */
  void set(unsigned int val) { counter_ = val; }

  /** This method exists so that Count object can
      be used as a templated class for the insert*
      methods in SQLTable. What it basically does
      is return a string representation of its contents.
    */
	string insertString() {
		stringstream result;
		result << counter_.value();
		return result.str();
	}

  /** This method exists so that Count object can
      be used as a templated class for the update*
      methods in SQLTable. Like the insertString()
      method it returns a string that should contain
      the part of the command to update the counter
      value in the SQL table.
    */
	string updateString() {
		stringstream result;
		result << " count = " << counter_.value();
		return result.str();
	}
};

#endif // COUNT_H
