/***************************************************************************
                          logger.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

using namespace std;

/*!
  * \class Logger
	* \brief Log a message to stdout and/or to a file.
  *
  * This class offers a totally independent way to keep
  * logs in a program. It allows asynchronous logging
  * to the stdout and/or a file (timestamped).
  * It runs in a separate thread and uses ACE's queueing
  * mechanisms to avoid thrashing of concurrently written
  * messages.
  * Its use is simple, just call the logmsg() method with
  * a string or a char * object.
  */

#include <ace/Synch.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Object.h>
#include <string>
#include <memory>
#include <fstream>
#include <iomanip>

#include "timestamp.h"

/// Enum to specify the type of the output method.  
enum output_t { OUTPUT_STDOUT, OUTPUT_BOTH, OUTPUT_FILEONLY};

class Logger : public ACE_Task<ACE_MT_SYNCH>  {
  /// Specify the output method
  output_t out_;

  /// The filename to write the logs into.
  string logfilename_;

  /// The standard C++ ofstream of the logfile.
  ofstream logFile;
public:
  /// The standard constructor, can be called with no arguments
	Logger(output_t out = OUTPUT_BOTH, string logfilename = "");

  /// Typical destructor
	virtual ~Logger();

  /// Virtual open, starts the thread and opens the file.
  virtual int open(void *);

  /// Virtual close, stops the thread and closes the file.
  virtual int close(u_long flags = 0);

  /// This method handles the dequeueing of the messages
  virtual int svc(void);

  /// Changes the filename of the logfile
  void setLogFileName(string filename);

  /// Returns the filename of the logfile
  string getLogFileName();
  
  /// Log a message
  ACE_Future<u_long> logMsg(string msg);

  /// Actual implementation of the Logger
  u_long logMsg_i(string msg);

private:
  /// The mutex mechanism, we use ACE's Guard
  ACE_Thread_Mutex mutex_;

  /// The queue to keep the method objects
  ACE_Activation_Queue activation_queue_;
};

#endif
