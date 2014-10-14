/***************************************************************************
                          logmsg_mo.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef LOGMSG_MO_H
#define LOGMSG_MO_H

using namespace std;

/*!
  * \class logMsg_MO
	* \brief This class is the method object that is queued by the Logger object.
  *
  * The philosophy behind Method Objects is described in ACE Tutorial and the C++ NP.
  * In practice, a method object returns a future object containing the result object
  * of the actual implementation method that is called in call().
  *
  * The logMsg_MO class is the method object that is queued by the logger. When ready,
  * the logger object (or actually its svc() method) calls the call() method in the MO
  * object which in turn calls the logMsg_i() method of the Logger.
  */

#include <ace/Method_Object.h>

#include "logger.h"

class logMsg_MO : public ACE_Method_Object  {
public:
  /// The default Constructor, takes a pointer to the logger, the msg string and a future object
	logMsg_MO(Logger *logger, string msg, ACE_Future<u_long> &future_result);

  /// Destructor
	virtual ~logMsg_MO();

  /// The call() method will be called by the svc() of the Logger Active Object
  virtual int call(void);
private:
  /// Pointer to the Logger object
  Logger *logger_;

  /// The message to be logged
  string msg_;

  /// The ACE Future object that is returned
  ACE_Future<u_long> future_result_;
};

#endif
