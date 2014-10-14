/***************************************************************************
                          logger.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "logger.h"
#include "logmsg_mo.h"

/**
  * The constructor. It's simple in form and its only important
  * function, other than to set the member variables of the object,
  * is to open the logfile for writing (if it is specified in out_).
  */

Logger::Logger(output_t out, string logfilename)
:	out_(out), logfilename_(logfilename) {

	if (out_ == OUTPUT_BOTH || out_ == OUTPUT_FILEONLY)
		logFile.open(logfilename_.c_str(), ios::out );
}

/**
  * The destructor just calls the method close().
  */

Logger::~Logger(){
  close();
}

/**
  * The open() method where the active object is activated
  * Create a detached thread to handle the logging.
  */
int Logger::open(void *) {
  return this->activate(THR_NEW_LWP|THR_DETACHED);
}

/**
  * Called when the active object is destroyed.
  * Just closes the file if it is open.
  */
  
int Logger::close(u_long flags) {
 	if (out_ == OUTPUT_BOTH || out_ == OUTPUT_FILEONLY)
    logFile.close();

  return 0;
}

/** The svc() method is the one that does all the work.
    The thread created will run in an infinite loop waiting for
    method objects to be enqueued on the private activation queue.
    Once a method object is inserted in the queue the thread wakes up
    dequeues the object and then invokes the call() method on the object
    it just dequeued. If there are no method objects on the activation
    queue the task blocks and falls asleep.
*/

int Logger::svc(void) {
  while (1) {
    // Dequeue the next method object from the activation queue
    // we use an auto pointer in case an exception is thrown in the <call>
    auto_ptr<ACE_Method_Object> mo(this->activation_queue_.dequeue());

    // Call it.
    if (mo->call() == -1)
      break;

    // Destructor automatically deletes it.
  }
  return 0;
}

/**
  * Set the log filename to the given one.
  * Also, close and reopen the logfile with the new
  * filename.
  */
void Logger::setLogFileName(string filename) {
  logfilename_ = filename;
  
 	if (out_ == OUTPUT_BOTH || out_ == OUTPUT_FILEONLY) {
    logFile.close();
		logFile.open(logfilename_.c_str(), ios::out );
  }
}

// Return the log filename
string Logger::getLogFileName() {
  return logfilename_;
}

/**
  * This method is called asynchronously. It logs the message.
  * It actually creates a future object that will hold the result
  * of the action and puts the method object (of type logMsg_MO) to
  * the activation_queue_ of the Logger object.
  * This in turn is handled by svc() and the actual logging method
  * logMsg_i() is called to do the logging.
  */

ACE_Future<u_long> Logger::logMsg(string msg) {
  ACE_Future<u_long> resultant_future;

  this->activation_queue_.enqueue(new logMsg_MO(this, msg, resultant_future));

  return resultant_future;
}

/**
  * The actual method to do the logging. It uses ACE_DEBUG to write the content
  * to stdout, and timestamped C++ stream I/O for the file.
  */

u_long Logger::logMsg_i(string msg) {
  ACE_Guard<ACE_Thread_Mutex> guard(mutex_); {
		if (out_ == OUTPUT_BOTH || out_ == OUTPUT_FILEONLY) {
      logFile << "-- " << TimeStamp(time(0)).toString();
			logFile << msg;
      logFile.flush();
     }

    if (out_ == OUTPUT_BOTH || out_ == OUTPUT_STDOUT)
      ACE_DEBUG((LM_DEBUG, "%s", msg.c_str()));
  }
  return 10;
}
