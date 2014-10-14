/***************************************************************************
                          logmsg_mo.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "logmsg_mo.h"

/**
  * The default constructor. It just initializes the member variables of the object
  * (logger_, msg_ and future_result_).
  */

logMsg_MO::logMsg_MO(Logger *logger, string msg, ACE_Future<u_long> &future_result)
:logger_(logger), msg_(msg), future_result_(future_result)
{
}

/**
  * Dummy destructor, we don't allocate anything dynamically.
  */

logMsg_MO::~logMsg_MO(){
}

/**
  * We don't have a lot to do. We just create a future object containing the
  * result of logMsg_i().
  * What this means is that the logMsg_MO will finish immediately, but the result
  * (the return value of call()) will remain uninitialized until the Logger object
  * MO queue actually reaches this object.
  */

int logMsg_MO::call(void) {
  return this->future_result_.set(this->logger_->logMsg_i(this->msg_));
}
