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

using namespace std;

/*!
  * \class connectionMsgBlock
	* \brief A wrapper class for the message block that holds participation information
  *
  * The information that is exchanged between the evalclient and the evalserver programs,
  * is encapsulated in a connectionMsgBlock structure. This structure is of the following form:
  *
  * - partiticipation code (type long long: 8 bytes)
  * - timestamp (type time_t: 4 bytes)
  * - MSISDN (string (not null terminated) : partDetails::MSISDNSIZE, 13 bytes)
  * - prize id (unsigned char: 1 byte)
  * - Checksum of the structure (CRC32 : 4 bytes)
  *
  * Methods are provided to check for the validity of the structure, and
  * the retrieval of the information inside the block.
  */

#include <vector>
#include <iostream>
  
#include "partdetails.h"
#include "crc_32.h"
  
class connectionMsgBlock {
  /// The object used to calculate the CRC of the message block
	CRC_32 crc32;

  /// Pointer to the data holding the message block (size: DATASIZE).
	unsigned char *data;
public:
  /// These enums give a more insightful view of the data positions inside the block
  enum { DATASIZE = 29, DATA_WO_CRC = 25, CRC_OFFSET = 25 };

  /// This constructor takes the code and the partDetails object, and creates a message block
	connectionMsgBlock(long long code, class partDetails &pd);

  /// This constructor takes a buffer and creates a message block WITH the right CRC
	connectionMsgBlock(unsigned char *data);

  /// Default destructor, just deallocates memory for the buffer data
	~connectionMsgBlock();

  /// Retrieve the CRC from the buffer.
	int retrieveCRC();

  /// Checks for the validity of the message block.
	bool isValid();

  /// Compares two message blocks
	bool operator==(connectionMsgBlock &cmb);

  /// Returns the raw buffer of the message block
	unsigned char *getData();

  /// Returns a pair containing the code (as a long long) and the corresponding partDetails
	pair<long long, partDetails> getParticipant();

  /// Returns a pair containing the code (as a string) and the corresponding partDetails 
	pair<string, partDetails> getParticipantStr();
};

#endif
