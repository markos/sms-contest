/***************************************************************************
                          processor.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "processor.h"
#include "processor_mo.h"
#include "contest.h"

/**
  * The constructor just initializes the Contest pointer.
  */

Processor::Processor(Contest *contest)
:parentContest(contest){}

/**
  * Dummy destructor just calls close().
  */
  
Processor::~Processor(){
  close();
}

/**
  * The open() method where the active object is activated
  * Create a detached thread to handle the processing.
  */
  
int Processor::open(void *) {
  return this->activate(THR_NEW_LWP|THR_DETACHED);
}

/**
  * Called when the active object is destroyed.
  * A no-op actually.
  */

int Processor::close(u_long flags) {
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

int Processor::svc(void) {
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
  * This method is called asynchronously. It processes the message
  * It actually creates a future object that will hold the result
  * of the action and puts the method object (of type processor_MO) to
  * the activation_queue_ of the Processor object.
  * This in turn is handled by svc() and the actual processing method
  * process_i() is called to do the processing.
  */

ACE_Future<int> Processor::process(connectionMsgBlock *cmb) {
  ACE_Future<int> resultant_future;

  this->activation_queue_.enqueue(new processor_MO(this, cmb, resultant_future));

  return resultant_future;
}

/**
  * The actual method to do the processing. It keeps everything inside an
  * ACE Guard scope for safety reasons.
  * This one is actually a very important method so we'll do a more thorough
  * analysis:
  * - First the message is converted into a string and is checked in the Participants
  *   table if it exists. If so ID_USEDCODE is returned.
  * - If we proceed, we have to know the current day, so we use getCurrentDay() from
  *   Contest object.
  * - Increase counter by 1.
  * - We check if this MSISDN has been used in this day already and if so we increase
  *   unique_counter as well.
  * - Now that we have a valid partDetails object we call Day::executeDraw on this and
  *   we have the result of the draw in the same partDetails object.
  * - If it is a prize, we disable transactions, insert the record in the CODES and
  *   WINNINGCODES tables, update the PRIZES table and re-enable the transactions.
  * - We also update the COUNTERS table, do some logging and return the id of the prize
  *   to the calling function/method (Client_Handler::svc() actually).
  */

int Processor::process_i(connectionMsgBlock *cmb) {

  stringstream debug;
 	int gid = 0;

	static string dummy = "";
  stringstream index;
  giftDetails gd;
  TimeStamp current;

  pair<string, partDetails> result = cmb->getParticipantStr();

  parentContest->logger->logMsg("\n--------------------------->\n");
  /*
   * If we're dealing with transactions if a user submits the same code within
   * a few seconds, then it may be possible that it has not yet been written
   * in the database.
   * This is a problem. So, we have decided to use a small cache of the last few
   * codes that have not been written in the database.
   * The cache is cleared every time a transaction is commited.
   */
  if (find(cached_codes_.begin(), cached_codes_.end(), result.first) != cached_codes_.end()) {
    parentContest->logger->logMsg("Already used code\n");
    return giftDetails::ID_USEDCODE;
  }
  // If it's not in the cache, put it in.
  cached_codes_.push_back(result.first);
  
  /* Comment this as we're not doing the search in the DB anymore.*/
  if (!parentContest->Participants[result.first].isEmpty()) {
		parentContest->logger->logMsg("Already used code\n");
    return giftDetails::ID_USEDCODE;
  }

  /* Instead use the codes_ vector
  if (find(parentContest->codes_.begin(), parentContest->codes_.end(),
            result.first) != parentContest->codes_.end()) {
    parentContest->logger->logMsg("Already used code\n");
    return giftDetails::ID_USEDCODE;
  }
  If it's not in the cache, put it in.
  parentContest->codes_.push_back(result.first);*/

	Day *curday = parentContest->getCurrentDay();
	if (curday) {
		partDetails dummypd = parentContest->Participants.selectObject(result.second.getMSISDN(),
                                                                   "msisdn");
		if (dummypd.isEmpty()) {
   		curday->unique_counter++;
		}

		current = curday->getTimePeriod().getBeginTS();
 		curday->executeDraw(result.second);
 		gid = result.second.getGiftId();
		if (gid) {
			// Update WINNINGCODES table
			parentContest->Winners.insertObject(result.first, result.second);
			parentContest->Participants.insertObject(result.first, result.second);

      // Update Prizes table
      index << current.toString(true) << ":" << gid;
      gd = parentContest->Prizes[index.str()];
	    gd--;
			parentContest->Prizes.updateObject(index.str(), gd, dummy);

      // Update Counters table
			curday->prizes++;

      // Commit the current transaction to ensure save.
			parentContest->dbconnection->commitTransaction();

      // Now clear the cached codes.
      cached_codes_.clear();
		} else
			parentContest->Participants.insertObject(result.first, result.second);

#ifdef DEBUG
  	debug << result.first << "\t" << result.second << endl;
    parentContest->logger->logMsg(debug.str());
#endif

  	// Update Counters table
     parentContest->Counters.updateObject(current.toString(true),
    																			*curday, dummy);

    debug.str("");
    debug << "Today\'s traffic (so far)       : " << curday->updateString() << endl;
		debug << "Total Number of Participations : " << parentContest->Participants.size() << endl;
		parentContest->logger->logMsg(debug.str());
  }
  
  return gid;
}
