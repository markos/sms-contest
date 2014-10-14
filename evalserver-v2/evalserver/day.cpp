/***************************************************************************
                          day.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "day.h"

/**
  * The Day constructor. It takes the Contest object pointer,
  * the starting TimeStamp.
  * It estimates the number of messages for the specific
  * time period and resizes the DayMessages vector.
  * Firstly, it checks the COUNTERS table to see if a record for the
  * particular day already exists, and if so, retrieves the counters
  * from the table. Then the system will continue from the previous
  * state.
  * Otherwise it will try to estimate the number of messages for this day
  * using calculateEstNoMsgs().
  * Whatever the case it will have a valid value for estNoMsgs and will
  * resize DayMessages vector with that size.
  * Then it will assign the available prizes (taking into account the COUNTERS
  * variable prizes) into random positions in DayMessages.
  */

Day::Day(Contest *myContest, TimeStamp &start_ts) {
	parentContest = myContest;
	day_period = TimePeriod(start_ts.getTimeStamp(), start_ts.nextDay());

#ifdef DEBUG
  stringstream debug;
  debug << "Day::day_period = " << day_period.toString(true) << endl;
  parentContest->logger->logMsg(debug.str());
#endif
	Day storedday = parentContest->Counters[day_period.getBeginTS().toString(true)];
	if (storedday.isEmpty()) {
		counter.set(0);
		unique_counter.set(0);
		prizes = 0;
		estNoMsgs = calculateEstNoMsgs();
	} else {
    debug.str("");
    debug << "Day:: storedday -> " << storedday.getCounter() << ", " << storedday.getEstNoMsgs() << endl;
    parentContest->logger->logMsg(debug.str());
    
		counter.set(storedday.getCounter());
		unique_counter.set(storedday.getUniqueCounter());
		estNoMsgs = storedday.getEstNoMsgs();
		prizes = storedday.getNoPrizes();
	}
  parentContest->logger->logMsg("creating counter object...");
	parentContest->Counters.insertObject(day_period.getBeginTS().toString(true), *this);
  parentContest->logger->logMsg("done\n");

#ifdef DEBUG
  	debug.str("");
		debug << "Day::estMsgs " << estNoMsgs << endl;
  	parentContest->logger->logMsg(debug.str());
#endif
						
	DayMessages.resize(estNoMsgs);
#ifdef DEBUG
  debug.str("");
	debug << "Day::DayMessages resized: " << DayMessages.size() << endl;
  debug << "Day::counter set to : " << counter.val() << endl;
  parentContest->logger->logMsg(debug.str());
#endif

	assignPrizes();
}

/**
  * Take a string representation of the variables in the
  * COUNTERS table, counter, unique_counter, estNoMsgs and
  * prizes, and resets the day, setting the empty boolean
  * to true as well.
  */
  
Day::Day(string daystr) {
	if (daystr.compare("") != 0) {

  	vector<string> result;
  	strTokenizer(daystr, result, ",");

  	counter.set(strtol(result[0].c_str(), NULL, 10));
  	estNoMsgs = strtol(result[1].c_str(), NULL, 10);
  	unique_counter.set(strtol(result[2].c_str(), NULL, 10));
  	prizes = strtol(result[3].c_str(), NULL, 10);
  	empty = false;
	} else
		empty = true;
}

/**
  * assignPrizes() is responsible for attaching a gift to a position
  * in the DayMessages vector. Basically, DayMessages is a vector
  * whose current position will give the next given prize. Since
  * probabilities are floating point numbers and we have to
  * correspond these to gift ids, we have to iterate over all
  * available presents for this day and assign them to the
  * DayMessages vector.
  * Espessially for the weekly and monthly gifts, we don't force
  * them to be assigned in the first n-1 days of their respective
  * periods but we do force them to be assigned in the last day.
  */

void Day::assignPrizes() {
	double rpos_d, r;
  u_int rpos, diff;
  bool status;
  TimeStamp ts(0L);
  u_int gid, loops;
#ifdef DEBUG
  stringstream debug;
	parentContest->logger->logMsg("Day::assigning gifts to messages\n");
#endif

	TimeStamp t(day_period.getBeginTS());
	string prefix = "period", qtyname = "qty_rem";
	map<string, giftDetails> *
	dayprizes = parentContest->Prizes.selectObjects(day_period.getBeginTS(),
																								prefix, qtyname);
	map<string, giftDetails>::iterator g;
	g = dayprizes->begin();

	while (g != dayprizes->end()) {
		gid = g->second.getGiftId();
#ifdef DEBUG
    debug.str("");
    debug << "gid: " << gid << "\trem:" << g->second.getCurrentQuantity() << "\tinit:"
          << g->second.getInitialQuantity() << "\tduration: " << g->second.getGiftDuration() << endl;
    parentContest->logger->logMsg(debug.str());
#endif
		for (size_t i = 0; i < g->second.getCurrentQuantity(); i++) {
      status = false;
      loops = 0;
      do {
				r = (double)(rand())/(double)(RAND_MAX);

        diff = max(0, (int)(DayMessages.size() - counter.val()));
        if (diff == 0 || diff < i)
          status = true;
          
  			rpos_d = r * g->second.getGiftDuration() * diff + counter.val();
        rpos = (u_int) (floor(rpos_d));

#ifdef EXTRADEBUG
          debug.str("");
     			debug << "size = " << DayMessages.size() << ", counter = " << counter.val()
                << ", diff = " << diff << ", i = " << i << endl;
     			debug << "rpos = " << rpos << endl;
          parentContest->logger->logMsg(debug.str());
#endif
                                                
        if ( rpos >= counter.val() && rpos < DayMessages.size() &&
            DayMessages[rpos] == giftDetails::ID_NOGIFT) {
			  	DayMessages[rpos] = gid;
          status = true;
#ifdef DEBUG
          debug.str("");
          if (gid != 1) {
       			debug << i << "\t" << rpos << "\t/" << DayMessages.size() << "\t" << DayMessages[rpos] << endl;
            parentContest->logger->logMsg(debug.str());
          }
#endif
        }
        
        if (gid == giftDetails::ID_WEEKLYPRIZES || gid == giftDetails::ID_MONTHLYPRIZES) {
            ts = g->second.getTimePeriod().getEndTS().getTimeStamp()-1;
            
          if (status == false) {
  					if ((gid == giftDetails::ID_WEEKLYPRIZES && day_period.dateInPeriod(ts)) ||
		  				 (gid == giftDetails::ID_MONTHLYPRIZES && day_period.dateInPeriod(ts)))
				  		status = false;
  					else
  	          status = true;
          }
        }
        loops++;
      } while (status == false && loops < 100);
		}
		g++;
	}

#ifdef DEBUG
  debug.str("");
  for (gid = giftDetails::ID_MINUTEPRIZES; gid <= giftDetails::ID_MONTHLYPRIZES; gid++)
    debug << "Distributed " << count(DayMessages.begin(), DayMessages.end(), gid)
          << " prizes of type " << gid << endl;
  parentContest->logger->logMsg(debug.str());
#endif
}

/// Returns the current time period object
TimePeriod Day::getTimePeriod() {
	return day_period;
}

/// Sets the current time period object
void Day::setTimePeriod(TimePeriod &tp) {
	day_period = tp;
}

/// Return the estNoMsgs varible (note: does not calculate the value).
u_int Day::getEstNoMsgs() {
	return estNoMsgs;
}

/**
  * Calculates an average of the past {default_avg_threshold} days' counters,
  * and returns the result. If the Contest has just started then it
  * returns {default_estNoMsgs}, and if the result of 0.8 * average is less
  * than {min_estNoMsgs} it returns {min_estNoMsgs} instead.
  */

u_int Day::calculateEstNoMsgs() {
  size_t avg_threshold = 0;
	u_long avg = 0;
  size_t size = parentContest->getContestDays().size();
#ifdef DEBUG
  stringstream debug;
#endif

	if (size == 0)
		return default_estNoMsgs;

  vector<class Day>::iterator i = parentContest->getContestDays().begin();

	while (i != parentContest->getContestDays().end()) {
    avg_threshold++;
    if (size - avg_threshold < default_avg_threshold) {
#ifdef EXTRADEBUG
      debug << "avg_threshold = " << avg_threshold << "\tsize = " << size << endl;
      debug << "avg = " << avg << "\t+" << i->getCounter() << endl;
#endif
  		avg += i->getCounter();
    }
		i++;
	}
#ifdef EXTRADEBUG
  parentContest->logger->logMsg(debug.str());
#endif

	avg /= min((size_t)default_avg_threshold, size);
#ifdef DEBUG
    debug.str("");
    debug << "Day::getEstNoMsgs(): avg = " << avg << endl;
    parentContest->logger->logMsg(debug.str());
#endif

	return max((u_long)(0.8 * avg), (u_long) min_estNoMsgs);
}

/**
  * The actual draw engine is here. Given a partDetails object
  * which holds information on the code, the MSISDN, and the timestamp,
  * fill the appropriate gift id in this object.
  * We take the gift id from the current value in DayMessages vector
  * in the position pointed to by counter,
  * if of course, counter is less than the size of DayMessages. If it is
  * bigger, then the prize won is nothing.
  * At the end a check is made to confirm that we don't give extra prizes.
  */

bool Day::executeDraw(partDetails &pd) {

#ifdef DEBUG
  stringstream debug;
#endif

  static string dummy = "";

  u_int current = counter.val();
  counter++;
	if ( current < DayMessages.size()) {
    if (DayMessages[current]) {
#ifdef DEBUG
      debug << "DayMessages[" << current << "] = " << DayMessages[current] << endl;
      parentContest->logger->logMsg(debug.str());
#endif
   		pd.setGiftId(DayMessages[current]);
    }
	}	else        
		pd.setGiftId(giftDetails::ID_NOGIFT);

/*  u_int gid = pd.getGiftId();
	if (gid) {
    TimePeriod tp;
    switch (gid) {
    case giftDetails::ID_HOURLYPRIZES:
    case giftDetails::ID_DAILYPRIZES:
      tp = day_period;
      break;
    case giftDetails::ID_WEEKLYPRIZES:
      tp = TimePeriod(parentContest->EOW[parentContest->current_week],
                      parentContest->EOW[parentContest->current_week+1]);
      break;
    case giftDetails::ID_MONTHLYPRIZES:
      tp = TimePeriod(parentContest->EOM[parentContest->current_month],
                      parentContest->EOM[parentContest->current_month+1]);
      break;
    default:
      break;
    }
    if (gid == giftDetails::ID_MONTHLYPRIZES) {
      bool alreadyGivenPrize = parentContest->giftIsGiven(tp, gid);
	  	if (alreadyGivenPrize == true) {
        debug << "Setting Prizes type to NO_GIFT (0)." << endl;
        pd.setGiftId(giftDetails::ID_NOGIFT);
      }
    }
  }*/

	return true;
}

/**
  * Returns the current value of the counter
  */
u_int Day::getCounter() {
	return counter.val();
}

/**
  * Sets the value of the counter. Used for recovery purposes.
  */
void Day::setCounter(u_int &val) {
  if (counter.val() != val) {
    counter.set(val);
  }
}

/**
  * Returns the current value of the counter
  */
u_int Day::getUniqueCounter() {
	return unique_counter.val();
}

/**
  * Returns the current value 
  */
u_int Day::getNoPrizes() {
	return prizes;
}

/**
  * returns a string suitable to be used in insert* methods
  * of SQLTable.
  */

string Day::insertString() {
	stringstream result;
	result	<< counter.val()
					<< ", " << estNoMsgs
					<< ", " << unique_counter.val()
					<< ", " << prizes;
	return result.str();
}

/**
  * returns a string suitable to be used in update* methods
  * of SQLTable.
  */

string Day::updateString() {
	stringstream result;
	result	<< "codes = " << counter.val()
					<< ", estcodes = " << estNoMsgs
					<< ", msisdns = " << unique_counter.val()
					<< ", prizes = " << prizes;
	return result.str();
}

/**
  * This is to check if a Day object has been created
  * with the dummy constructor Day().
  */

bool Day::isEmpty() {
	return empty;
}
