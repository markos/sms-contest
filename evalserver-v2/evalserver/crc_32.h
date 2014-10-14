/***************************************************************************
                          crc_32.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 Bullet S.A.
    creator              : Konstantinos Margaritis
    email                : markos@bullet.gr
    version              : $ver
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CRC_32_H
#define CRC_32_H

using namespace std;

/*!
  * \class CRC_32
	* \brief	Class to return CRC-32 error detection
  *
  *  It is used by connectionMsgBlock class to ensure that
  *  data is correctly transferred. Although the chances of
  *  data sent incorrectly are quite minimal, especially in
  *  this particular case, where the server runs on the same
  *  machine as the client, it still is good practice.
  *  The usage is simple. After initialization of an object, one
  *  can use it to retrieve the CRC of a given buffer with
  *  getCRC().
  */

#include <sys/types.h>
#include <iostream>
  
class CRC_32 {
protected:
	/// Lookup table array
	u_long crc32_table[256];

	/// Reflects CRC bits in the lookup table
	u_long reflect(u_long ref, unsigned char ch);

public:
	/// Basic Constructor, it initializes the crc32_table.
	CRC_32();

	/// Creates a CRC from a data buffer
	int getCRC(unsigned char *data, size_t size);
};

#endif
