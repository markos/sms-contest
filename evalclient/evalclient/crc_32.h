/***************************************************************************
                          crc_32.h  -  description
                             -------------------
    begin                : Fri Aug 30 2002
    copyright            : (C) 2002 by Konstantinos Margaritis
    email                : markos@bullet-adv.gr
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

/**
	*@author	Konstantinos Margaritis
	*@brief	Class to return CRC-32 error detection
	*@description
          It is used by connectionMsgBlock class to ensure that
					data is correctly transferred. Although the chances of
					data sent incorrectly are quite minimal, especially in
					this particular case, where the server runs on the same
					machine as the client, it still is good practice.				
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
