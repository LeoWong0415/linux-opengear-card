/* 
 * File:   opengear.h
 * Author: sau
 *
 * Created on August 28, 2012, 9:55 AM
 */

#ifndef OPENGEAR_H
#define	OPENGEAR_H

//Included
#include "types1.h"
#include "ogp.h"
//Defines


#define CEILING(X) (X-(int)(X) > 0 ? (int)(X+1) : (int)(X))

//-----------------------------------------------------------------------
//Extern variables

//Function Prototypes
/** Compute the CRC for a byte - private inline version */
#define updateCrc(data, crc) (((crc >> 8) & 0xff) ^ crcTable [(crc ^ data) & 0xff])

//Calculate CRC check for message
int getMessageCrc(char* data, int length, int crc);
void writeUint16(INT16 val, unsigned char* dest);
INT16 readUint16(unsigned char* dest);
void writeUint32(UINT32 val, unsigned char* dest);
UINT32 readUint32(unsigned char* dest);
void writeFloat(float val, unsigned char* dest);
float readFloat(unsigned char* dest);
BYTE encodeInt16Constraint(Int16Param* param, char* buffer);
BYTE encodeInt16ArrConstraint(Int16ArrayParam* param, char* buffer);
BYTE encodeInt32Constraint(Int32Param* param, char* buffer);
BYTE encodeFloatConstraint(FloatParam* param, char* buffer);
BYTE encodeIntArrConstraint(Int16ArrayParam* param, char* buffer);
void DisplayRxMessage(OgpShortFrame * fr);

void OGP_CopyHeader(OgpHeader* source,OgpHeader* dest);
BOOL OGP_CompareHeader(OgpHeader* source,OgpHeader* dest);

void OGP_SendData( BYTE dest, BYTE msgType,BYTE * buffer,BYTE size);
void OGP_Process(void);
void OGP_ProcessLongFrame(OgpShortFrame * fr);
void OGP_RespondMessage(OgpShortFrame * fr);
void OGP_SendRestartState(void);
void ProcessBroadcastMessage(OgpShortFrame * fr);

#endif	/* OPENGEAR_H */

