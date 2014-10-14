/***************************************************************************
                          processor_mo.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef PROCESSOR_MO_H
#define PROCESSOR_MO_H

using namespace std;

/*!
  * \class processor_MO
	* \brief This class is the method object that is queued by the Processor object.
  *
  * The philosophy behind Method Objects is described in ACE Tutorial and the C++ NP.
  * In practice, a method object returns a future object containing the result object
  * of the actual implementation method that is called in call().
  *
  * The processor_MO class is the method object that is queued by the Processor. When ready,
  * the processor object (or actually its svc() method) calls the call() method in the MO
  * object which in turn calls the process_i() method of the Processor.
  */

#include <ace/Method_Object.h>

#include "processor.h"
#include "connectionmsgblock.h"

class processor_MO : public ACE_Method_Object  {
public:
  /// The default Constructor, takes a pointer to the Processor, the message block and a future object
	processor_MO(Processor *processor, connectionMsgBlock *cmb, ACE_Future<int> &future_result);

  /// Dummy destructor
  virtual ~processor_MO();

  /// The call() method will be called by the svc() of the Logger Active Object
  virtual int call(void);
private:
  /// Pointer to the Processor object
  Processor *processor_;

  /// Pointer to the connectionMsgBlock
  connectionMsgBlock *cmb_;

  /// The ACE Future object that is returned
  ACE_Future<int> future_result_;
};

#endif
