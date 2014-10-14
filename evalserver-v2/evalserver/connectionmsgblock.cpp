/***************************************************************************
                          connectionmsgblock.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "connectionmsgblock.h"

/**
  * This constructor constructs a message block, given the components separetely.
  * It takes the code (long long type) and the partDetails object containing the
  * rest of the data (must be created beforehand).
  * It then puts the data into place and calculates the CRC for the block.
  * The result is a valid connectionMsgBlock object.
  */

connectionMsgBlock::connectionMsgBlock(long long code, class partDetails &pd){

	data = new unsigned char[DATASIZE];
	unsigned char *data_p = data;

	// Copy the code to the data buffer
	memcpy(data_p, &code, sizeof(code));
	data_p += sizeof(code);

	// Copy the timestamp
	time_t ts = pd.getTimestamp();
	memcpy(data_p, &ts, sizeof(time_t));
	data_p += sizeof(time_t);

	// Copy the MSISDN
	memcpy(data_p, pd.getMSISDN(), partDetails::MSISDNSIZE);
	data_p += partDetails::MSISDNSIZE-1;
	
	// Copy the Gift Id
	unsigned char gid = pd.getGiftId();
	memcpy(data_p, &gid, sizeof(unsigned char));
	data_p += sizeof(unsigned char);

	int crc = crc32.getCRC(data, DATA_WO_CRC);
	memcpy(data_p, &crc, sizeof(int));
}

/**
  * This constructor constructs a message block from the raw data.
  * It takes an existing buffer (without the CRC part) and appends
  * the correct CRC.
  */

connectionMsgBlock::connectionMsgBlock(unsigned char *newdata) {
	data = new unsigned char[DATASIZE];
	memcpy(data, newdata, DATA_WO_CRC);
	int crc = crc32.getCRC(data, DATA_WO_CRC);
	memcpy(data + CRC_OFFSET, &crc, sizeof(int));
}

/**
  * Compares two connectionMsgBlocks (using the operator ==)
  * It compares the buffers AND the crcs of each object.
  * Returns true if the objects are equal.
  */

bool connectionMsgBlock::operator==(connectionMsgBlock &cmb) {
	unsigned char *data2 = cmb.getData();
	int crc1 = this->retrieveCRC();
	int crc2 = cmb.retrieveCRC();

	
	if (memcmp(data, data2, DATASIZE) == 0 && (crc1 == crc2))
		return true;
	else
		return false;
}

/**
  * Returns the buffer holding the data
  */

unsigned char *connectionMsgBlock::getData() {
	return data;
}

/**
  * The destructor has no other purpose than to deallocate the
  * memory used by the buffer.
  */
  
connectionMsgBlock::~connectionMsgBlock(){
	delete data;
}

/**
  * Retrieves the CRC of the current object,
  * as held in the buffer.
  */
  
int connectionMsgBlock::retrieveCRC() {
	int crc;
	memcpy(&crc, data +CRC_OFFSET, sizeof(int));

	return crc;
}

/**
  * Validity of the object is checked by calculating the CRC
  * of the data in the buffer and comparing it to the CRC _in_
  * the buffer. Returns true if these are equal.
  */
  
bool connectionMsgBlock::isValid() {
	int crc = retrieveCRC();

	int compcrc = crc32.getCRC(data, DATA_WO_CRC);
	if (crc == compcrc)
		return true;
	else
		return false;
}

/**
  * This method returns a pair object containing the code and respective
  * partDetails object of the current connectionMsgBlock object.
  * The code is of type long long.
  */
  
pair<long long, partDetails> connectionMsgBlock::getParticipant() {
	partDetails pd;

	long long code;
	time_t ts;
	char msisdn[12], gid;

	unsigned char *data_p = data;

	// Copy the code
	memcpy(&code, data_p, sizeof(code));
	data_p += sizeof(code);

	// Copy the timestamp
	memcpy(&ts, data_p, sizeof(time_t));
	data_p += sizeof(time_t);
	pd.setTimestamp(ts);

	// Copy the MSISDN
	memcpy(msisdn, data_p, partDetails::MSISDNSIZE);
	data_p += partDetails::MSISDNSIZE-1;
	pd.setMSISDN(string(msisdn));

	// Copy the Gift Id
	memcpy(&gid, data_p, sizeof(char));
	pd.setGiftId(gid);
	
	return pair<long long, partDetails> (code, pd);	
}

/**
  * This is similar to getParticipant(). Likewise, it returns a pair
  * object containing the code and respective partDetails object of
  * the current connectionMsgBlock object.
  * This time the code is of type string.
  */

pair<string, partDetails> connectionMsgBlock::getParticipantStr() {
	pair<long long, partDetails> t = getParticipant();

	stringstream code;
	code << t.first;
	return pair<string, partDetails>(code.str(), t.second);
}
