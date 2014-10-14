/***************************************************************************
                          connectionmsgblock.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifndef CONNECTIONMSGBLOCK_H
#define CONNECTIONMSGBLOCK_H


/**
  *@author Konstantinos Margaritis
  */

#include <vector>
#include <iostream>
  
#include "partdetails.h"
#include "crc_32.h"

#define DATASIZE		29
#define DATA_WO_CRC	25
#define CRC_OFFSET		25
  
class connectionMsgBlock {
	CRC_32 crc32;
	unsigned char *data;
public: 
	connectionMsgBlock(long long code, class partDetails &pd);
	connectionMsgBlock(unsigned char *data);
	~connectionMsgBlock();

	static int getDataSize();
	int retrieveCRC();
	bool isValid();
	bool operator==(connectionMsgBlock &cmb);
	unsigned char *getData();
	pair<long long, partDetails> getParticipant();
	pair<string, partDetails> getParticipantStr();
};

#endif
