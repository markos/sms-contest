/***************************************************************************
                          crc_32.cpp  -  description
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

#include "crc_32.h"

/** The constructor. It basically creates the crc32 table used for calculating
    the checksums.
*/

CRC_32::CRC_32(){
	// This is the official polynomial used by CRC-32 in PKZip, WinZip and Ethernet
	u_long ulPolynomial = 0x04c11db7;
	int i, j;

	// 256 values representing ASCII character codes;
	for ( i=0; i < 0xFF; i++) {
		crc32_table[i] = reflect(i, 8) << 24;
		for (j=0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
		crc32_table[i] = reflect(crc32_table[i], 32);
	}
}

/** Helper method used by the constructor. It swaps the bit orientation
    in ch, based on ref.
*/

u_long CRC_32::reflect(u_long ref, unsigned char ch) {
	// Used only by init()

	u_long value(0);

	// Swap bit 0 for bit 7, bit 1 for bit 6, etc.
	for (int i=0; i < ch+1; i++) {
		if (ref & 1)
			value |= 1 << (ch -i);
		ref >>= 1;
	}
	return value;
}

/// It calculates and returns the CRC of the buffer in data.

int CRC_32::getCRC(unsigned char *data, size_t size) {
	// Pass a data buffer to this function and it will return the CRC

	// Once the lookup table has been filled in by the two functions above
	// this function creates all CRCs using only the lookup table

	// Be sure to use unsigned variables, because negative values introduce high
	// bits where zero bits are required

	// Start out with all bits set high
	unsigned char *data_p = data;
	u_long ulCRC(0xffffffff);
	int len = size;

	// Perform the algorithm on each character in the buffer,
	// using the lookup table values
	while (len--) 
		ulCRC = (ulCRC >> 8) ^ crc32_table[((ulCRC & 0xFF) ^ *data_p++)-1];
	// Exclusive OR the result with the beginning value
	return ulCRC ^ 0xffffffff;
}
