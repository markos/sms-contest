/***************************************************************************
                          connectionmsgblock.cpp  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#include "connectionmsgblock.h"

connectionMsgBlock::connectionMsgBlock(long long code, class partDetails &pd){

	data = new unsigned char[getDataSize()];
	unsigned char *data_p = data;

	// Copy the code to the data buffer
	memcpy(data_p, &code, sizeof(code));
	data_p += sizeof(code);

	// Copy the timestamp
	time_t ts = pd.getTimestamp();
	memcpy(data_p, &ts, sizeof(time_t));
	data_p += sizeof(time_t);

	// Copy the MSDN
	memcpy(data_p, pd.getMSDN(), pd.sizeofMSDN());
	data_p += pd.sizeofMSDN()-1;
	
	// Copy the Gift Id
	unsigned char gid = pd.getGiftId();
	memcpy(data_p, &gid, sizeof(unsigned char));
	data_p += sizeof(unsigned char);

	int crc = crc32.getCRC(data, DATA_WO_CRC);
	memcpy(data_p, &crc, sizeof(int));
}

connectionMsgBlock::connectionMsgBlock(unsigned char *newdata) {
	data = new unsigned char[getDataSize()];
	memcpy(data, newdata, DATA_WO_CRC);
	int crc = crc32.getCRC(data, DATA_WO_CRC);
	memcpy(data + CRC_OFFSET, &crc, sizeof(int));
}

int connectionMsgBlock::getDataSize() {
	return DATASIZE;
}

bool connectionMsgBlock::operator==(connectionMsgBlock &cmb) {
	unsigned char *data2 = cmb.getData();
	int crc1 = this->retrieveCRC();
	int crc2 = cmb.retrieveCRC();

	
	if (memcmp(data, data2, getDataSize()) == 0 && (crc1 == crc2))
		return true;
	else
		return false;
}

unsigned char *connectionMsgBlock::getData() {
	return data;
}

connectionMsgBlock::~connectionMsgBlock(){
	delete data;
}

int connectionMsgBlock::retrieveCRC() {
	int crc;
	memcpy(&crc, data +CRC_OFFSET, sizeof(int));

	return crc;
}

bool connectionMsgBlock::isValid() {
	int crc = retrieveCRC();

	int compcrc = crc32.getCRC(data, DATA_WO_CRC);
	if (crc == compcrc)
		return true;
	else
		return false;
}

pair<long long, partDetails> connectionMsgBlock::getParticipant() {
	partDetails pd;

	long long code;
	time_t ts;
	char msdn[12], gid;

	unsigned char *data_p = data;

	// Copy the code
	memcpy(&code, data_p, sizeof(code));
	data_p += sizeof(code);

	// Copy the timestamp
	memcpy(&ts, data_p, sizeof(time_t));
	data_p += sizeof(time_t);
	pd.setTimestamp(ts);

	// Copy the MSDN
	memcpy(msdn, data_p, pd.sizeofMSDN());
	data_p += pd.sizeofMSDN()-1;
	pd.setMSDN(string(msdn));

	// Copy the Gift Id
	memcpy(&gid, data_p, sizeof(char));
	pd.setGiftId(gid);
	
	return pair<long long, partDetails> (code, pd);	
}

pair<string, partDetails> connectionMsgBlock::getParticipantStr() {
	pair<long long, partDetails> t = getParticipant();

	stringstream code;
	code << t.first;
	return pair<string, partDetails>(code.str(), t.second);
}
