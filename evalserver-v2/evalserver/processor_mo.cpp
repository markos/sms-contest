/***************************************************************************
                          processor_mo.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "processor_mo.h"
#include "contest.h"

/**
  * The default constructor. It just initializes the member variables of the object
  * (processor_, cmb_ and future_result_).
  */

processor_MO::processor_MO(Processor *processor, connectionMsgBlock *cmb,
                            ACE_Future<int> &future_result)
:processor_(processor), cmb_(cmb), future_result_(future_result)
{
}

/**
  * Dummy destructor, we don't allocate anything dynamically.
  */

processor_MO::~processor_MO(){
}

/**
  * We don't have a lot to do. We just create a future object containing the
  * result of process_i().
  * What this means is that the processor_MO will finish immediately, but the result
  * (the return value of call()) will remain uninitialized until the Processor object
  * MO queue actually reaches this object.
  */

int processor_MO::call(void) {
  return this->future_result_.set(this->processor_->process_i(this->cmb_));
}
