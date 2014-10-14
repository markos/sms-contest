/***************************************************************************
                          processor.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef PROCESSOR_H
#define PROCESSOR_H

using namespace std;

/*!
  * \class Processor
	* \brief Process the connectionMsgBlock messages.
  *
  * This class works in exactly the same way as Logger.
  * Its use is slightly different, in that it is called
  * from Client_Handler to process the messages that are
  * sent to the server, calls Day::executeDraw() on the object
  * and returns the result to the handler, which in turn
  * returns it to evalclient and closes the connection.
  */

#include <ace/Synch.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Object.h>

#include "connectionmsgblock.h"

class Contest;

class Processor : public ACE_Task<ACE_MT_SYNCH>  {
public:
  /// The standard constructor is passed a pointer to the parent Contest object
	Processor(Contest *myContest);

  /// Typical destructor
	virtual ~Processor();

  /// Virtual open, starts the thread in which the processing takes place.
  virtual int open(void *);

  /// Closes the thread.
  virtual int close(u_long flags = 0);

  /// This method handles the dequeueing of the messages
  virtual int svc(void);

  /// Process the message, or rather queue it for processing
  ACE_Future<int> process(connectionMsgBlock *cmb);

  /// Actual implementation of the processing method
  int process_i(connectionMsgBlock *cmb);

private:
  /// Pointer to the parent Contest object
  Contest *parentContest;

  /// The mutex mechanism, we use ACE's Guard
  ACE_Thread_Mutex mutex_;

  /// The queue to keep the method objects
  ACE_Activation_Queue activation_queue_;

  /// Last 100 codes or so to be used when dealing with transactions.
  vector<string> cached_codes_;
};

#endif
