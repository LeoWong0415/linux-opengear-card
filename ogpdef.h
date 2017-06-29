/*******************************************************************************
* FILENAME:
* ogpdef.h
* Adapted from canmsg.h
******************************************************************************/
#ifndef __OGPDEF_H
#define __OGPDEF_H

// maximum length of OGP messages
#define OGP_MTU                         260

// mask limiting OGP addresses to 6 bits
#define OGP_ADDRESS_MASK                0x3f

// well-defined OGP addresses
#define OGP_ADDR_BCAST                  0x3F 		// OGP broadcast address
#define OGP_ADDR_UPLOAD                 0x3E 		// OGP multicast address for file upload
#define OGP_ADDR_NONE                   0x00 		// Invalid OGP address for cards
#define OGP_ADDR_FINC                   0x00 		// OGP address for the FINC
#define OGP_ADDR_PRINT                  0x01 		// OGP address for debug messages
#define OGP_ADDR_TRAP                   0x02 		// OGP address for inbound messages (e.g. traps)

// this is used to map a request to a response
#define OGP_RESPONSE                    0x80

// messages broadcast to openGear devices
#define OGP_TIME 			0x01 		// UTC time broadcast

// messages broadcast by openGear devices
#define OGP_PRINT 			0x00 		// debug / print message
#define OGP_DEBUG 			0x00
#define OGP_REPORT_PARAM                0x10 		// report the value of a param
#define OGP_TRAP 			0x11 		// report a status event
#define OGP_EVENT_LOG                   0x12 		// report a log message - not implemented
#define OGP_BOOTLOAD                    0x13 		// boot loader is running
#define OGP_BUSTEST                     0x14 		// message for stress-test � do not use

// messages related to file upload (software upgrade)
#define OGP_START_UPLOAD                0x40 		// start the upload process
#define OGP_UPLOAD_PAGE                 0x41 		// send a single page of code
#define OGP_VERIFY_UPLOAD               0x42 		// verify the upload was successful
#define OGP_REBOOT 			0x43 		// reboot the processor
#define OGP_REBOOT_RESPONE              0xC3		// respone to command reboot processor
// messages related to file upload (software upgrade)
#define OGP_START_UPLOAD_RESPONE 	0xC0 		// start the upload process
#define OGP_UPLOAD_PAGE_RESPONE 	0xC1 		// send a single page of code
#define OGP_VERIFY_UPLOAD_RESPONE 	0xC2 		// verify the upload was successful

// command-line interface
#define OGP_COMMAND                     0x44 		// UTF-8 command

// messages to exchange parameter info
#define OGP_GET_NUMPARAMS               0x45 		// ask how many params in table
#define OGP_GET_PARAM_OIDS              0x46 		// request the object IDs
#define OGP_GET_DESCRIPTOR              0x47 		// request the descriptor for a param
#define OGP_GET_PARAM_NAME              0x48 		// request the parameter name
#define OGP_GET_PARAM                   0x49 		// request the value of a param
#define OGP_SET_PARAM                   0x4A 		// set the value of a param (absolute)
#define OGP_SET_RELATIVE                0x4B 		// set the value of a param (obsolete)
#define OGP_GET_ARRAY_ELEMENT           0x4C 		// get the value of an array element
#define OGP_SET_ARRAY_ELEMENT           0x4D 		// set the value of an array element

// Respone message types
#define OGP_GET_NUMPARAMS_RESPONE 	0xC5            //respone to request to get number of param
#define OGP_GET_PARAM_OIDS_RESPONE	0xC6            //respone to request to get table of OIDs
#define OGP_GET_DESCRIPTOR_RESPONE 	0xC7            //respone to request the descriptor for a param
#define OGP_GET_PARAM_NAME_RESPONE 	0xC8            // request the parameter name
#define OGP_GET_PARAM_RESPONE		0xC9            // respone to request get the value of param
#define OGP_SET_PARAM_RESPONE 		0xCA 		// set the value of a param (absolute)
#define OGP_GET_ARRAY_ELEMENT_RESPONE   0xCC 		// get the value of an array element
#define OGP_SET_ARRAY_ELEMENT_RESPONE   0xCD 		// set the value of an array element

#define OGP_GET_MENUSET_NAME_RESPONE	0xD0            // respone to request get MenuSet name
#define OGP_GET_MENU_COUNT_RESPONE 	0xD1 		// get the number of TAB in a MENU group
#define OGP_GET_MENU_NAME_RESPONE 	0xD2 		// get the name of a menu
#define OGP_GET_MENU_OIDS_RESPONE 	0xD3 		// get the object IDs for a menu
#define OGP_GET_MENU_STATE_RESPONE 	0xDA 		// requests the display state for the specified menu
// spare 0x4E and 0x4F

// messages related to the menu structure
#define OGP_GET_MENUSET_NAME            0x50 		// get the name of a menu group
#define OGP_GET_MENU_COUNT 		0x51 		// get the number of menus in a group
#define OGP_GET_MENU_NAME 		0x52 		// get the name of a menu
#define OGP_GET_MENU_OIDS 		0x53 		// get the object IDs for a menu
#define OGP_GET_MENU_STATE 		0x5A 		// requests the display state for the specified menu

// messages to request product information
#define OGP_GET_PRODUCT_INFO            0x54 		// get the product info (internal use only)
#define OGP_GET_PRODUCT_KEY             0x55 		// get the product key (internal use only)

// messages to request SNMP mappings
#define OGP_GET_SNMP_BASE 		0x56 		// get the SNMP base OID
#define OGP_GET_SNMP_OID 		0x57 		// get the SNMP OID for a param
#define OGP_GET_SNMP_TRAP 		0x58 		// get the SNMP TRAP for a param

#define OGP_GET_SNMP_BASE_RESPONE 	0xD6 		// get the SNMP base OID reply


// messages to request external objects
#define OGP_GET_EXTERNAL_OBJ            0x59 		// get the object with specified OID

// message to request menu state
#define OGP_GET_MENU_STATE 		0x5A 		// get the visibility of a menu
// spare 0x5B to 0x7F

// generic error codes
#define OGP_OK 				0x00 		// no error
#define OGP_UNSUPPORTED 		0x01 		// unsupported OGP message
#define OGP_INVALID_LENGTH 		0x02 		// message is wrong length
#define OGP_REQUEST_DENIED 		0x03 		// request denied (provide reason)
#define OGP_NOTFOUND 			0x11 		// parameter or menu not found
#define OGP_NO_RESP 			0xFF 		// no response received

// error codes for parameter messages (should not be here)
#define OGP_PARAM_NOTFOUND 		0x11 		// parameter not found
#define OGP_PARAM_BADVAL 		0x12 		// parameter value out of range
#define OGP_PARAM_READONLY 		0x13 		// parameter is read only
#define OGP_PARAM_LOCKED 		0x14 		// parameter is read only
#define OGP_PARAM_BADINDEX 		0x15 		// array index out of bounds

// error codes for file upload (should not be here)
#define OGP_INVALID_PRODUCT             0x11 		// wrong product name for upload
#define OGP_INVALID_UPLOAD 		0x12 		// unsupported file type
#define OGP_INVALID_STATE 		0x13 		// wrong message for current state
#define OGP_UPLOAD_WAIT 		0x14 		// try again after N milliseconds
#define OGP_INVALID_PAGE 		0x15 		// data out of range (past length)
#define OGP_INVALID_CRC 		0x16 		// CRC mismatch
#define OGP_INVALID_FILESIZE            0x17 		// upload file size is wrong size
#define OGP_INVALID_KEY 		0x18 		// product key mismatch

// well-defined trap messages (to be used with OGP_TRAP
#define OGP_RESTART 			0x01 		// product has just booted up
#define OGP_PARAM_CHANGED 		0x02 		// a parameter descriptor has changed
#define OGP_MENU_CHANGED 		0x03 		// a menu descriptor has changed
#define OGP_EXTOBJ_CHANGED 		0x04 		// an external object has changed

// macros defining the allowable content for OGP_PARAM_CHANGED trap
#define OGP_PARAM_ACCESS_READONLY 	0 		// access changed to read only
#define OGP_PARAM_ACCESS_READWRITE 	1 		// access changed to read/write
#define OGP_PARAM_DESC_CHANGED 		2 		// descriptor changed

// macros defining the allowable content for OGP_MENU_CHANGED trap
#define OGP_MENU_HIDDEN 		0 		// menu is hidden
#define OGP_MENU_READONLY 		1 		// menu is visible, but disabled
#define OGP_MENU_NORMAL 		2 		// menu is visible and enabled
#define OGP_MENU_DESC_CHANGED 		3 		// descriptor changed

// length of fixed-length messages (should not be here)
#define OGP_START_UPLOAD_LEN 		40

// parameter types supported
#define INT8U_PARAM_TYPE 		1 		// not implemented
#define INT16_PARAM_TYPE 		2
#define INT16U_PARAM_TYPE 		3 		// not implemented
#define INT32_PARAM_TYPE 		4
#define INT32U_PARAM_TYPE 		5 		// not implemented
#define FLOAT_PARAM_TYPE 		6
#define STRING_PARAM_TYPE 		7
#define BOOLEAN_PARAM_TYPE 		8 		// not implemented
#define COMMAND_PARAM_TYPE 		9 		// not implemented
#define ALARM_PARAM_TYPE 		10 		// not implemented
#define INT16_ARRAY_TYPE 		12 		// array of 16-bit integers
#define INT32_ARRAY_TYPE 		14 		// array of 32-bit integers
#define FLOAT_ARRAY_TYPE 		16 		// array of 32-bit floats
#define STRING_ARRAY_TYPE 		17 		// array of strings

// length for each parameter type
#define INT8U_PARAM_SIZE 		1
#define INT16_PARAM_SIZE 		2
#define INT16U_PARAM_SIZE 		2
#define INT32_PARAM_SIZE 		4
#define INT32U_PARAM_SIZE 		4
#define FLOAT_PARAM_SIZE 		4
#define STRING_PARAM_SIZE 		1		// use the maximum permitted string length
#define BOOLEAN_PARAM_SIZE 		1
#define COMMAND_PARAM_SIZE 		1
#define ALARM_PARAM_SIZE 		2
#define NULL_LENGTH 			0		// length of a NULL object

// settings for the access flag
#define ACCESS_READWRITE 		0x01
#define ACCESS_READONLY 		0x00

// precision is used for numerical displays
#define PRECISION_PCT 			0x80 		// display as percent - not implemented
#define PRECISION_NONE 			0x00
#define PRECISION_0 			0x00 		// display whole numbers only
#define PRECISION_1 			0x01 		// display with one digit after the decimal
#define PRECISION_2 			0x02
#define PRECISION_3 			0x03

// constraint types
#define NULL_CONSTRAINT 		0		// unconstrained
#define RANGE_CONSTRAINT 		1		// constrained by min and max
#define CHOICE_CONSTRAINT 		2		// choice of several values (max 255)
#define EXTENDED_CHOICE 		3		// choice of several values (more than 255)
#define STRING_CHOICE 			4		// choice of several strings
#define RANGE_STEP_CONSTRAINT           5		// constraint by min/max with defined step size
#define ALARM_TABLE 			10		// alarm encoded as a bit field
#define EXTERNAL_CONSTRAINT             11		// reference to constraint table

// external object types
#define CONSTRAINT_OBJECT 		1		// external object containing a constraint

// widget hints for all parameter types
#define WIDGET_DEFAULT 			0		// let DashBoard decide
#define WIDGET_TEXT_DISPLAY             1		// display as text, read only
#define WIDGET_HIDDEN 			2		// do not display

// widget hints for numeric types with NULL_CONSTRAINT or RANGE_CONSTRAINT
#define WIDGET_SLIDER_HORIZONTAL 	3 		// slider (RANGE only)
#define WIDGET_SLIDER_VERTICAL 		4 		// slider (RANGE only)
#define WIDGET_SPINNER 			5 		// spinner
#define WIDGET_TEXTBOX 			6 		// numeric entry field
#define WIDGET_PROGRESS_BAR 		17 		// progress bar (RANGE only)
#define WIDGET_AUDIO_METER_VERTICAL 19      //Readonly

// widget hints for integer types with CHOICE_CONSTRAINT
#define WIDGET_COMBO_BOX 		7               // combo box - usually the default
#define WIDGET_CHECKBOX 		8               // two choices
#define WIDGET_RADIO_HORIZONTAL 	9 		// radio buttons
#define WIDGET_RADIO_VERTICAL 		10 		// radio buttons
#define WIDGET_BUTTON_PROMPT 		11 		// single choice
#define WIDGET_BUTTON_NO_PROMPT 	12 		// single choice
#define WIDGET_BUTTON_TOGGLE 		13 		// two choices
#define WIDGET_FILE_DOWNLOAD 		18 		// external object OID/filename pairs

// widget hints for INT32_PARAM
#define WIDGET_IP_ADDRESS 		14 		// nnn.nnn.nnn.nnn

// widget hints for integer arrays
#define WIDGET_ARRAY_HEADER_VERTICAL 	15              // array layout specification
#define WIDGET_ARRAY_HEADER_HORIZONTAL 	16              // array layout specification

// widget hints for STRING_PARAM
#define WIDGET_TEXT_ENTRY 		3 		// normal text entry field
#define WIDGET_PASSWORD 		4 		// uses password entry field
#define WIDGET_TITLE_LINE 		5 		// layout hint - read only
#define WIDGET_LINE_ONLY 		6 		// layout hint - read only
#define WIDGET_TITLE_ONLY 		7 		// layout hint - read only
#define WIDGET_PAGE_TAB 		8 		// layout hint - read only
#define WIDGET_LICENSE 			9 		// RossKeys license adapter
#define WIDGET_TITLE_HEADER             10 		// layout hint - read only
#define WIDGET_COMBO_ENTRY 		11 		// combo box plus entry field
#define WIDGET_ICON_DISPLAY             12 		// icon plus text display
#define WIDGET_RICH_LABEL 		13 		// multi-line display (html format)
#define WIDGET_MULTILINE_TEXT_ENTRY     14 		// multi-line text entry (non-html)

// widget hints for STRING_PARAM (used with special OID 255.1)
#define WIDGET_NAME_OVERRIDE_APPEND 	0
#define WIDGET_NAME_OVERRIDE_REPLACE 	1

// deprecated names - here for backward compatibility
#define WIDGET_NONE 	WIDGET_DEFAULT
#define WIDGET_COMBO 	WIDGET_COMBO_BOX
#define WIDGET_RADIO 	WIDGET_RADIO_HORIZONTAL
#define WIDGET_HSLIDER 	WIDGET_SLIDER_HORIZONTAL
#define WIDGET_VSLIDER 	WIDGET_SLIDER_VERTICAL

// define for assembly a long frame
#define OGP_FIRST_FRAME     0x02
#define OGP_MID_FRAME       0x00
#define OGP_LAST_FRAME      0x01

#define OGP_SHORT_FRAME     0x03

#endif
