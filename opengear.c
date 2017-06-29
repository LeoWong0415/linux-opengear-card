
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>


#include "can_lv.h"
#include "opengear.h"
#include "ogpdef.h"
#include "parameters.h"
#include "menus.h"

//#define DEBUG 1

//============================================================================================
//Global variables

extern unsigned char my_id;
extern char my_mac[];

// Table used for computing the CRC
const int crcTable[] =
{
	0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
	0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
	0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
	0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
	0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
	0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
	0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
	0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
	0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
	0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
	0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
	0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
	0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
	0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
	0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
	0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
	0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
	0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
	0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
	0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
	0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
	0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
	0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
	0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
	0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
	0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
	0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
	0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
	0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
	0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
	0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
	0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};
//----------------------------------------------------------------------------------------
OgpLongFrame assemblyData;
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//============================================================================================
//FUNCTIONS
//---------------------------------------------------------------------------------------------
/**
* Compute the CRC for a message (array of bytes).
* @param data - byte array to process
* @param length - number of bytes to process
* @param crc - the starting crc value
* @return - the updated crc value
*/
int getMessageCrc(char* data, int length, int crc)
{
	int ix;
	for (ix = 0; ix < length; ix++)
	crc = updateCrc(data[ix], crc);
	return crc;
}
//---------------------------------------------------------------------------------------------
void OGP_SendData( BYTE dest, BYTE msgType,BYTE * buffer,BYTE size)
{
short nFrag,odd;
short i,j,index;
int crc,ret;
UINT16 msgCRC,msgLen;
OgpShortFrame fr;

	index=0;
	msgLen=size;
	nFrag=(short)CEILING((size+4)/8.0);
	odd=(short)((size+4)%8);
	if(odd==0) odd=8;
//	printf("Last segment Leng: %d\r\n",odd);
	if(size>8){
		crc=getMessageCrc(buffer,size,0);
		msgCRC=(UINT16)crc;
		fr.header.source=my_id;
		fr.header.dest=dest;
		fr.header.msgType=msgType;
		fr.header.version=0;
		fr.header.priority=0;
		for(i=0;i<nFrag;i++){

			if(i==0){	//First fragmentation
				fr.header.flags=0x02;
                fr.length=8;

				fr.data[0]=(BYTE)((msgLen>>8)&0xFF);
				fr.data[1]=(BYTE)(msgLen&0xFF);
				fr.data[2]=(BYTE)((msgCRC>>8)&0xFF);
				fr.data[3]=(BYTE)(msgCRC&0xFF);
				for(j=0;j<4;j++){
					fr.data[j+4]=buffer[j];
				}
				index+=4;
			}
			else if (i<(nFrag-1)){
				for(j=0;j<8;j++){
					fr.data[j]=buffer[index];
					index++;
				}
				fr.header.flags=0x00;
                fr.length=8;
			}
			else if (i==(nFrag-1)){	//Last fragmentation			
				for(j=0;index<size;j++){
					fr.data[j]=buffer[index];
					index++;
				}
				fr.header.flags=0x01;
                fr.length=odd;
			}
            ret = OgpWrite(&fr);
            if(ret < 0)
                printf("Error Return: %d bytes\r\n",ret);
		}
	}
	else{//Message len less than 8 bytes, so it transfered as 1 CAN frame
		fr.header.source=my_id;
		fr.header.dest=dest;
		fr.header.msgType=msgType;
		fr.header.version=0;
		fr.header.priority=0;
		fr.header.flags=0x03;
        fr.length=size;

		for(j=0;j<size;j++){
			fr.data[j]=buffer[j];
		}
        ret = OgpWrite(&fr);
        if(ret < 0)
            printf("Error Return: %d bytes\r\n",ret);
	}
}
//---------------------------------------------------------------------------------------------
/**
 *  write the value to string, MSB first
 */
void writeUint16(INT16 val, unsigned char* dest)
{
   dest[0] = (val >> 8) & 0xff;
   dest[1] = val & 0xff;
}
//---------------------------------------------------------------------------------------------
/**
 *  read an unsigned int from the byte stream, MSB first
 */
INT16 readUint16(unsigned char* dest)
{
   return ((dest[0] & 0xff) << 8) |
          ((dest[1] & 0xff));
   
}
//---------------------------------------------------------------------------------------------
/**
 *  write the value to string, MSB first
 */
void writeUint32(UINT32 val, unsigned char* dest)
{
   writeUint16(val >> 16, dest);
   writeUint16(val, &dest[2]);
}
//---------------------------------------------------------------------------------------------
/**
 *  read an unsigned int from the byte stream, MSB first
 */
UINT32 readUint32(unsigned char* dest)
{
   UINT32 val = (readUint16(&dest[0]) & 0xffff);
   val = (val << 16) | (readUint16(&dest[2]) & 0xffff);
   return val;
}
//---------------------------------------------------------------------------------------------
/**
 *  write the value to string, MSB first
 */
void writeFloat(float val, unsigned char* dest)
{
    UINT32 *pInt=(UINT32 *) &val;
    writeUint32(*pInt,dest);
}
//---------------------------------------------------------------------------------------------
float readFloat(unsigned char* dest)
{
    float data;
    UINT32 *pval=(UINT32 *)&data;
    *pval = readUint32(dest);
    usleep(1);
    return (data);
}
//---------------------------------------------------------------------------------------------
BYTE encodeInt16Constraint(Int16Param* param, char* buffer)
{
   // choice constraint - encode the values and names
   if (param->ctype == CHOICE_CONSTRAINT)
   {
      ChoiceConstraint *constraint = (ChoiceConstraint *) param->cdata;
      NamedChoice *choices = constraint->choices;
      BYTE numChoice = constraint->numChoice;
      BYTE ix;
      BYTE offset = 0;
      
      // encode the number of choices
      buffer[offset] = numChoice;
      offset++;
      
      // encode each choice as value, namelen, name
      for (ix = 0; ix < numChoice; ix++)
      {
         const char* name = choices[ix].name;
         BYTE namelen = strlen(name) + 1;
         
         writeUint16(choices[ix].value, &buffer[offset]);
         offset += 2;
         
         buffer[offset] = namelen;
         offset++;
         
         strcpy(&buffer[offset], name);
         offset += namelen;
      }
      return offset;
   }
// Alarm table constraint
   else if (param->ctype == ALARM_TABLE)
   {
      AlarmConstraint *constraint = (AlarmConstraint *) param->cdata;
      AlarmNamedChoice *choices = constraint->choices;
      BYTE numChoice = constraint->numChoice;
      BYTE ix;
      BYTE offset = 0;
      
      // encode the number of choices
      buffer[offset] = numChoice;
      offset++;
      
      // encode each choice as value, namelen, name
      for (ix = 0; ix < numChoice; ix++)
      {
         const char* name = choices[ix].name;
         BYTE namelen = strlen(name) + 1;
         buffer[offset] = choices[ix].value;
         offset++;
         buffer[offset] = choices[ix].display;
         offset++;
         
         buffer[offset] = namelen;
         offset++;
         
         strcpy(&buffer[offset], name);
         offset += namelen;
      }
      return offset;
   }
   // Range constraint has no data
   else if (param->ctype == RANGE_CONSTRAINT)
   {
       RangeConstraint * range= (RangeConstraint*) param->cdata;
       writeUint16(range->min,&buffer[0]);
       writeUint16(range->max,&buffer[2]);
       return 4;
   }
   
   // Null constraint has no data
   else if (param->ctype == NULL_CONSTRAINT)
      return 0;
   // nothing else is supported
   else
      return 0;
}
//---------------------------------------------------------------------------------------------
BYTE encodeInt16ArrConstraint(Int16ArrayParam* param, char* buffer)
{
   // choice constraint - encode the values and names
   if (param->ctype == CHOICE_CONSTRAINT)
   {
      ChoiceConstraint *constraint = (ChoiceConstraint *) param->cdata;
      NamedChoice *choices = constraint->choices;
      BYTE numChoice = constraint->numChoice;
      BYTE ix;
      BYTE offset = 0;
      
      // encode the number of choices
      buffer[offset] = numChoice;
      offset++;
      
      // encode each choice as value, namelen, name
      for (ix = 0; ix < numChoice; ix++)
      {
         const char* name = choices[ix].name;
         BYTE namelen = strlen(name) + 1;
         
         writeUint16(choices[ix].value, &buffer[offset]);
         offset += 2;
         
         buffer[offset] = namelen;
         offset++;
         
         strcpy(&buffer[offset], name);
         offset += namelen;
      }
      return offset;
   }
// Alarm table constraint
   else if (param->ctype == ALARM_TABLE)
   {
      AlarmConstraint *constraint = (AlarmConstraint *) param->cdata;
      AlarmNamedChoice *choices = constraint->choices;
      BYTE numChoice = constraint->numChoice;
      BYTE ix;
      BYTE offset = 0;
      
      // encode the number of choices
      buffer[offset] = numChoice;
      offset++;
      
      // encode each choice as value, namelen, name
      for (ix = 0; ix < numChoice; ix++)
      {
         const char* name = choices[ix].name;
         BYTE namelen = strlen(name) + 1;
         buffer[offset] = choices[ix].value;
         offset++;
         buffer[offset] = choices[ix].display;
         offset++;
         
         buffer[offset] = namelen;
         offset++;
         
         strcpy(&buffer[offset], name);
         offset += namelen;
      }
      return offset;
   }
   else if (param->ctype == RANGE_CONSTRAINT)
   {
       RangeConstraint * range= (RangeConstraint*) param->cdata;
       writeUint16(range->min,&buffer[0]);
       writeUint16(range->max,&buffer[2]);
       return 4;
   }
   // Null constraint has no data
   else if (param->ctype == NULL_CONSTRAINT)
      return 0;
   // nothing else is supported
   else
      return 0;
}
//---------------------------------------------------------------------------------------------
BYTE encodeFloatConstraint(FloatParam* param, char* buffer)
{
   // Range constraint has no data
   if (param->ctype == RANGE_CONSTRAINT)
   {
       FloatRangeConstraint * range= (FloatRangeConstraint*) param->cdata;
       writeFloat(range->min,&buffer[0]);
       writeFloat(range->max,&buffer[4]);
       return 8;
   }
   else
      return 0;
}
//---------------------------------------------------------------------------------------------
BYTE encodeInt32Constraint(Int32Param* param, char* buffer)
{
   // Range constraint has no data
   if (param->ctype == RANGE_CONSTRAINT)
   {
       Int32RangeConstraint * range= (Int32RangeConstraint*) param->cdata;
       writeUint32(range->min,&buffer[0]);
       writeUint32(range->max,&buffer[4]);
       return 8;
   }
   else
      return 0;
}

//---------------------------------------------------------------------------------------------
//#define CAN_HEADER
#define OPENGEAR_HEADER
#define SOURCE
#define DEST
#define MSG_TYPE
#define FLAGS
#define DATA

void DisplayRxMessage(OgpShortFrame * fr) 
{
    static int disp_count=0;
    printf("MESSAGE(%d)\r\n", ++disp_count);
//    printf("Messages remain in buffer: %d\t%d(S)\t%d(L)\r\n",msg_count,frs_count,frl_count);
#ifdef OPENGEAR_HEADER
    printf("OPENGEAR HEADER\r\n");
#ifdef ID  
    printf("\tID:\t0x%08X\r\n", fr->header.value);
#endif
#ifdef PRIORITY
    printf("\tPriority:\t%d\r\n", fr->header.priority);
#endif
#ifdef VERSION
    printf("\tVersion:\t%d\r\n", fr->header.version);
#endif
#ifdef SOURCE
    printf("\tSource:\t0x%02X\r\n", fr->header.source);
#endif
#ifdef DEST
    printf("\tDest:\t0x%02X\r\n", fr->header.dest);
#endif
#ifdef MSG_TYPE
    printf("\tMsgType:\t0x%02X\r\n", fr->header.msgType);
#endif
#ifdef FLAGS
    printf("\tFlags:\t0x%02X\r\n", fr->header.flags);
#endif  
#endif
#ifdef DATA
    int i;
    printf("DATA:\r\n\t");
    for (i = 0; i < fr->length; i++)
        printf("0x%02x ", fr->data[i]);
#endif  
    printf("\r\n------------------------------------\r\n");
}
//---------------------------------------------------------------------------------------------
//This is main function for Opengear processing, it's block function, so it should be run
//in a independent thread
void OGP_Process(void)
{
    int ret;
    OgpShortFrame fr;
    ret = OgpRead(&fr);
    if(ret < 0){
        printf("Ogp read failed\r\n");
        return;
    }
    if(fr.header.dest == my_id){
//        printf("Packet for me\r\n");
        //Check if it is a short frame?
        if(fr.header.flags == OGP_SHORT_FRAME)
            OGP_RespondMessage(&fr);
        else
            OGP_ProcessLongFrame(&fr);
    }
    else {
        //Process packet not for me, such as broadcast packet
//        printf("Broadcast packet\r\n");
        ProcessBroadcastMessage(&fr);
    }
}
//---------------------------------------------------------------------------------------------
//Process all request for my product
void OGP_RespondMessage(OgpShortFrame * fr)
{
	BYTE group,menu;
	UINT16 oid;
	INT16 temp_data;
	BYTE dest,index,nparam,type;
    
    //Process
    switch(fr->header.msgType){
        case OGP_GET_PARAM: {	//Received request to get a parameter (0x49)
            dest=fr->header.source;
            oid=(((UINT16)(fr->data[1])<<8)| (UINT16)(fr->data[2]));
#ifdef DEBUG
            printf("GetParam: 0x%04X\r\n",oid);
#endif
//                connected=TRUE;
            OGP_GetParamRespone(dest,oid);
        } break;		
        case OGP_GET_NUMPARAMS: {	//Received request to get number of parameters
#ifdef DEBUG
            printf("GetNumParam\r\n");
#endif
            dest=fr->header.source;
            OGP_GetNumOfParamRespone(dest);
        } break;
        case OGP_GET_PARAM_OIDS: {	//Received request to get a table of OIDs
#ifdef DEBUG
            printf("GetParamOid\r\n");
#endif
            dest=fr->header.source;
            index=fr->data[2];
            nparam=fr->data[3];

            OGP_GetParamOidsRespone(dest,index,nparam);
        } break;	
        case OGP_GET_DESCRIPTOR: {	//Received request to get a descriptor of OID
            dest=fr->header.source;
            oid=((fr->data[1]<<8)|fr->data[2]);
#ifdef DEBUG
            printf("GetDescriptor: 0x%04X\r\n",oid);
#endif
            OGP_GetDescriptorRespone(dest,oid);
        } break;	
        case OGP_GET_MENUSET_NAME: { 	//Received request to get MenuSet name
            dest=fr->header.source;
            group=fr->data[1];
#ifdef DEBUG
            printf("GetMenuSetName for Group: %02x\r\n",group);
#endif						
            OGP_GetMenuSetNameRespone(dest,group);
        }break;	
        case OGP_GET_MENU_COUNT: { 	//Received request to get number of tab on Menu
            dest=fr->header.source;
            group=fr->data[1];
#ifdef DEBUG
            printf("GetMenuCount for Group: %02x\r\n",group);
#endif						
            OGP_GetMenuCountRespone(dest,group);
        }break;	
        case OGP_GET_MENU_NAME: { 	//Received request to get name of Menu
            dest=fr->header.source;
            group=fr->data[1];
            menu=fr->data[2];
#ifdef DEBUG
            printf("GetMenuName for Group: %02x\r\n",group);
#endif						
            OGP_GetMenuNameRespone(dest,group,menu);
        }break;	
        case OGP_GET_MENU_OIDS: { 	//Received request to get OIDs on Menu
            dest=fr->header.source;
            group=fr->data[1];
            menu=fr->data[2];
#ifdef DEBUG
            printf("GetMenuOid for Group: %02x\r\n",group);
#endif						
            OGP_GetMenuOidsRespone(dest,group,menu);
        }break;	
        case OGP_GET_MENU_STATE: { 	//display state for the specified menu
            dest=fr->header.source;
            group=fr->data[1];
            menu=fr->data[2];
#ifdef DEBUG
            printf("GetMenuState for Group: %02x\r\n",group);
#endif						
            OGP_GetMenuStateRespone(dest,group,menu);
        }break;	
        case OGP_SET_PARAM: {	//received request to set a parameter
#ifdef DEBUG
            printf("Set Parameters request\r\n");
#endif						
            OGP_SetParamRespond(fr);
        } break;
        case OGP_REBOOT: {
            dest=fr->header.source;
#ifdef DEBUG
            printf("Send Respone to Reset command\r\n");
#endif
            OGP_RebootRespone(dest);
            sleep(1);
            system("reboot");
//						delay_ms(100);
//						SoftReset();	
        }
        case OGP_VERIFY_UPLOAD: { 	//Upload finished, request to verify data
            dest=fr->header.source;
            type=fr->data[1];
#ifdef DEBUG
            printf("Send Respone to Verify Upload\r\n");
#endif						
            OGP_VerifyUploadRespone(dest,type);
        }break;	
        case OGP_GET_SNMP_BASE: { 	//get the SNMP base OID
            dest=fr->header.source;
#ifdef DEBUG
            printf("Send Respone to Get SNMP base OID\r\n");
#endif						
            OGP_GetSnmpBaseOidRespone(dest);
        }break;	
        case OGP_GET_ARRAY_ELEMENT: { //request single element of array
            dest=fr->header.source;
            oid=(((UINT16)fr->data[1]<<8)|(UINT16)fr->data[2]);
            temp_data = (((INT16)fr->data[3]<<8)|(INT16)fr->data[4]);
#ifdef DEBUG_LEVEL0
            printf("Send Respone to Get Element of Array\r\n");
#endif						
            OGP_SendInt16ArrayElementValue(dest,oid,temp_data);
        } break;
        case OGP_SET_ARRAY_ELEMENT: { //request to set a single element of array
#ifdef DEBUG_LEVEL0
            printf("Send Respone to Set Element of Array\r\n");
#endif						
            OGP_SetArrayElementRespone(fr);
        } break;
        default : {
            //Display for testing
#ifdef DEBUG
            printf("Other\r\n");
#endif
            DisplayRxMessage(fr);
        } break;	
    }	
    
}
//---------------------------------------------------------------------------------------------
void OGP_SendRestartState(void)
{
	BYTE buffer[4];
	buffer[0]=0x01;
	buffer[1]=0;
	buffer[2]=0;
	buffer[3]=0;
#ifdef DEBUG	
	printf("Send OGP Restart\r\n");
#endif	
	OGP_SendData(OGP_ADDR_TRAP,OGP_TRAP,buffer,1);
}
//---------------------------------------------------------------------------------------------
BYTE encodeIntArrConstraint(Int16ArrayParam* param, char* buffer)
{
   // choice constraint - encode the values and names
   if (param->ctype == CHOICE_CONSTRAINT)
   {
      ChoiceConstraint *constraint = (ChoiceConstraint *) param->cdata;
      NamedChoice *choices = constraint->choices;
      BYTE numChoice = constraint->numChoice;
      BYTE ix;
      BYTE offset = 0;
      
      // encode the number of choices
      buffer[offset] = numChoice;
      offset++;
      
      // encode each choice as value, namelen, name
      for (ix = 0; ix < numChoice; ix++)
      {
         const char* name = choices[ix].name;
         BYTE namelen = strlen(name) + 1;
         
         writeUint16(choices[ix].value, &buffer[offset]);
         offset += 2;
         
         buffer[offset] = namelen;
         offset++;
         
         strcpy(&buffer[offset], name);
         offset += namelen;
      }
      return offset;
   }
// Alarm table constraint
   else if (param->ctype == ALARM_TABLE)
   {
      AlarmConstraint *constraint = (AlarmConstraint *) param->cdata;
      AlarmNamedChoice *choices = constraint->choices;
      BYTE numChoice = constraint->numChoice;
      BYTE ix;
      BYTE offset = 0;
      
      // encode the number of choices
      buffer[offset] = numChoice;
      offset++;
      
      // encode each choice as value, namelen, name
      for (ix = 0; ix < numChoice; ix++)
      {
         const char* name = choices[ix].name;
         BYTE namelen = strlen(name) + 1;
         buffer[offset] = choices[ix].value;
         offset++;
         buffer[offset] = choices[ix].display;
         offset++;
         
         buffer[offset] = namelen;
         offset++;
         
         strcpy(&buffer[offset], name);
         offset += namelen;
      }
      return offset;
   }
   else if (param->ctype == RANGE_CONSTRAINT)
   {
       RangeConstraint * range= (RangeConstraint*) param->cdata;
       writeUint16(range->min,&buffer[0]);
       writeUint16(range->max,&buffer[2]);
       return 4;
   }
   // Null constraint has no data
   else if (param->ctype == NULL_CONSTRAINT)
      return 0;
   // nothing else is supported
   else
      return 0;
}
//---------------------------------------------------------------------------------------------
#include <time.h>
void ProcessBroadcastMessage(OgpShortFrame * fr)
{
    struct tm * t;
    OgpTime oTime;
    time_t tim;
    
    if(fr->header.msgType == OGP_TIME)
    {
        tim = (time_t) readUint32(&fr->data[0]);//+25569;
        //tim = time(NULL);
        t = localtime(&tim);
//        printf("Time Service: %d\r\n",tim);
       // printf("Time: %02d:%02d:%02d %02d/%02d/%04d\r\n", t->tm_hour,t->tm_min,t->tm_sec,
         //       t->tm_mday,t->tm_mon+1,t->tm_year+1900);
/*
        printf("MacAddr: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                my_mac[0],my_mac[1],my_mac[2],my_mac[3],my_mac[4],my_mac[5]);
*/
        
    }
}
//---------------------------------------------------------------------------------------------
void OGP_CopyHeader(OgpHeader* source,OgpHeader* dest)
{
    dest->priority = source->priority;
    dest->version = source->version;
    dest->source = source->source;
    dest->dest = source->dest;
    dest->msgType = source->msgType;
    dest->flags = source->flags;
    dest->value = source->value;
}
//---------------------------------------------------------------------------------------------
BOOL OGP_CompareHeader(OgpHeader* source,OgpHeader* dest)
{
    printf("S.val: 0x%08X D.val: 0x%08X\r\n",(source->value & 0xFFFFF00),(dest->value & 0xFFFFF00));
    return ((source->value & 0xFFFFF00)==(dest->value & 0xFFFFF00));
}
//---------------------------------------------------------------------------------------------
void OGP_ProcessLongFrame(OgpShortFrame * fr)
{
    printf("Long Message\r\n");
    switch (fr->header.flags)
    {
        case OGP_FIRST_FRAME: {
            OGP_CopyHeader(&fr->header,&assemblyData.header);
            assemblyData.length = readUint16(&fr->data[0]);
            assemblyData.msgCrc = readUint16(&fr->data[2]);
            memcpy(&assemblyData.data[0],&fr->data[4],4);
            assemblyData.wrIndex = 4;
        }break;
        case OGP_MID_FRAME: {
            if(OGP_CompareHeader(&fr->header,&assemblyData.header)){
                memcpy(&assemblyData.data[assemblyData.wrIndex],&fr->data[0],8);
                assemblyData.wrIndex += 8;
            }
            else{
                printf("Wrong message header\r\n");
            }
        }break;
        case OGP_LAST_FRAME: {
            if(OGP_CompareHeader(&fr->header,&assemblyData.header)){
                memcpy(&assemblyData.data[assemblyData.wrIndex],&fr->data[0],fr->length);
                assemblyData.wrIndex += fr->length;
                //Finished, so starting process it
                
                OGP_StartProcessLongData(&assemblyData);
            }
            else{
                printf("Wrong message header\r\n");
            }
        }break;
        default: {
            printf("Strange flag\r\n");
        }break;
    }
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
