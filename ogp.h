/*
 * linux/ogp.h
 *
 * Definitions for OpenGear Layer
 *
 * Authors: Ho Viet Sau, sau@hdvietnam.com.vn
  */

#ifndef OGP_H
#define OGP_H

//Defines
#define OGP_MTU 260

/*
 * OpenGear structure
 *
 */
//Define header for OpenGear Protocol
struct sOgpHeader{
	unsigned char 	priority;
	unsigned char 	version;
	unsigned char 	source;
	unsigned char 	dest;
	unsigned char 	msgType;
	unsigned char 	flags;
	unsigned int	value;
};
typedef struct sOgpHeader OgpHeader;
//Define structure of Short OpenGear Frame
struct sOgpShortFrame{
	OgpHeader	header;
	unsigned char 	length;
	unsigned char 	data[8] __attribute__((aligned(8)));
};
typedef struct sOgpShortFrame OgpShortFrame;

//Define structure of Long OpenGear Frame
struct sOgpLongFrame{
	OgpHeader	header;
	unsigned short 	length;
	unsigned short 	msgCrc;
	unsigned char 	nFrag;
	unsigned short 	wrIndex;
	unsigned short 	rdIndex;
	unsigned char 	data[OGP_MTU];
};
typedef struct sOgpLongFrame OgpLongFrame;
// this defines an object ID
typedef unsigned short ObjectID;
struct sReportObject{
    ObjectID objectId;
    int interval;
    int count;
};
typedef struct sReportObject ReportObject;

// this associates an object ID with a name.
struct sNamedOid 
{
   char* 	name;
   ObjectID    	oid;
};
typedef struct sNamedOid NamedOid;

// generic parameter
struct sParam
{
   char* 	name;       	// descriptive name for this param
   ObjectID    	oid;        	// object ID used to identify this param
} ;
typedef struct sParam Param;

// integer parameter
struct sInt16Param
{
   char* 		name;       // descriptive name for this param
   ObjectID    		oid;        // object ID used to identify this param
   unsigned char	access;     // read only or read/write
   unsigned char	widget;		// widget type to display
   short            value;      // value of the param
   unsigned char   	ctype;      // constraint type
   unsigned char   	clen;       // constraint length
   void*       		cdata;      // pointer to constraint structure
}; 
typedef struct sInt16Param Int16Param;
// integer 16bit array parameter
struct sInt16ArrayParam
{
   char*            name;       // descriptive name for this param
   ObjectID    		oid;        // object ID used to identify this param
   unsigned char	access;     // read only or read/write
   unsigned char	widget;     // widget type to display
   short*           value;      // value of the param
   unsigned char   	ctype;      // constraint type
   unsigned char   	clen;       // constraint length
   void*       		cdata;      // pointer to constraint structure
   unsigned char        datasize;       // number of elements in array
}; 
typedef struct sInt16ArrayParam Int16ArrayParam;
//String Array Param
struct sStrArrayParam
{
   char*            name;       // descriptive name for this param
   ObjectID    		oid;        // object ID used to identify this param
   unsigned char	access;     // read only or read/write
   unsigned char	widget;     // widget type to display
   char **          value;      // pointer to array of content
   unsigned char    precision;  // Maximum number of characters in an element
   unsigned char    datasize;       // number of elements in array
}; 
typedef struct sStrArrayParam StrArrayParam;

// integer 32 bit parameter
struct sInt32Param
{
   char* 		name;       // descriptive name for this param
   ObjectID    		oid;        // object ID used to identify this param
   unsigned char	access;     // read only or read/write
   unsigned char	widget;		// widget type to display
   int              value;      // value of the param
   unsigned char	ctype;      // constraint type
   unsigned char	clen;       // constraint length
   void*       		cdata;      // pointer to constraint structure
}; 
typedef struct sInt32Param Int32Param;

// string parameter (read only since value is const)
struct sStringParam
{
   const char* 		name;       // descriptive name for this param
   ObjectID    		oid;        // object ID used to identify this param
   unsigned char	access;     // read only or read/write
   char* 		value;      // value
};
typedef struct sStringParam StringParam;

// floating point parameter
struct sFloatParam
{
   char* 		name;       // descriptive name for this param
   ObjectID    		oid;        // object ID used to identify this param
   unsigned char	access;     // read only or read/write
   unsigned char	widget;     // widget type to display
   float                value;      // value of the param
   unsigned char   	ctype;      // constraint type
   unsigned char   	clen;       // constraint length
   void*       		cdata;      // pointer to constraint structure
}; 
typedef struct sFloatParam FloatParam;

/******************************************************************************/
/* structures for choice constraint                                           */
/******************************************************************************/
struct sNamedChoice
{
   unsigned char   	value;
   char* 		name;
};
typedef struct sNamedChoice NamedChoice;

struct sChoiceConstraint
{
   NamedChoice*  	choices;
   unsigned char	numChoice;
};
typedef struct sChoiceConstraint ChoiceConstraint;
/******************************************************************************/
/* structures for range constraint                                           */
/******************************************************************************/
struct sRangeConstraint
{
   short       min;
   short	max;
};
typedef struct sRangeConstraint RangeConstraint;
struct sFloatRangeConstraint
{
   float        min;
   float        max;
};
typedef struct sFloatRangeConstraint FloatRangeConstraint;
struct sInt32RangeConstraint
{
    int min;
    int max;
};
typedef struct sInt32RangeConstraint Int32RangeConstraint;
/******************************************************************************/
/* structures for alarm table constraint                                           */
/******************************************************************************/
struct sAlarmNamedChoice
{
   unsigned char   	value;
   const char* 		name;
   unsigned char	display;
};
typedef struct sAlarmNamedChoice AlarmNamedChoice;

struct sAlarmConstraint
{
   AlarmNamedChoice*	choices;
   unsigned char	numChoice;
};
typedef struct sAlarmConstraint AlarmConstraint;

/******************************************************************************/
/* structures to deal with menus                                              */
/******************************************************************************/
// this defines the name and content for a single menu page.
struct sMenuPage
{
   char*  name;
   ObjectID*    	items;
   unsigned char	numItems;
};
typedef struct sMenuPage MenuPage;

// this defines a set of menus to be grouped in a tabbed page.
struct sMenuGroup
{
   const char*  	name;
   MenuPage*    	menus;
   unsigned char	numMenus;
};
typedef struct sMenuGroup MenuGroup;
struct sOgpFilter
{
    unsigned source;
    unsigned dest;
};
    //ogpheader: PPVV SSSS SSD | D DDDD MMMM MMMM FFFF Fzzz
    //sid: PPV VSSS SSSD ;000 0 010 0000
    //eid: DD DDDM MMMM MMMF FFFF	; 11 1110 0000 0000 0000 = 0x3E000

typedef struct sOgpFilter OgpFilter;

//Time Service Structure
struct sOgpTime
{
    unsigned int    utcTime;    //UTC time from 01/01/1970
    unsigned short  mSec;       //milli-second
    unsigned char   sync;       //flag indicate it is synchronized from NTP (bit 0)
};
typedef struct sOgpTime OgpTime;

#endif /* OGP_H */
