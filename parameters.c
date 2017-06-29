

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "opengear.h"
#include "parameters.h"
#include "oids.h"
#include "ogpdef.h"
#include "ogp_ctrl.h"
#include "utilities.h"

//from Hien
#include "config.h"
#include "control.h"

//#define DEBUG 1
//Extern
extern BOOL ctrlPanelIsAvailable;      //from main.c; indicate that there is a Control Panel connected
extern BOOL setModeAlready;             //indicate action state of setting mode for Hardware/Sofware control

extern struct agc_item alc_param;
extern DELAY_T audio_delay_param;
extern ObjectID audioLevelGroup[];
extern ObjectID audioDelayGroup[];
//========================================================================================
//GLOBAL VARIABLES

/*
 * struct agc_item{
double        op_threshold;             // muc am thanh mong muon -40 -> 0
double        gate_threshold;           // duoc muc nay  khong xu ly -60 -> -20
double        transient_range;          // dai am thanh se tang hoac giam 0 -> 30
double        transient_bypass_range;       // dai am thanh khong xu ly, neu muc do am thanh nam trong khoang nay thi khong xu ly 0 -> 30 db
double        transient_time;           // so dbfs/s, toc do thay doi, muc decibel thay doi tren 1 giay

double          agc_range;              // muc tang hoac giam agc, tu 0 den 40 decibel 0-> 40 db
double          agc_bypass_range;        // trong muc nay thi se khong tang hoac giam 0->40 db
int             agc_time;               // tu 0 den 2 gio, tinh bang giay, 0 -> 7200
int             agc_rms_time;           // thoi gian tinh rms, tu 0 -> 3600 giay.
int             enabled;                 // bypass xu ly am luong
int             mode;
};
 */
//Constants for Leveler modes
const struct agc_item lvDefaultModes[3] = {
    {-18.0, -50.0, 6.0, 6.0, 2, 3,    5, 40, 60, 1},
    {-18.0, -50.0, 6.0, 6.0, 4, 6.0, 3.0, 30, 30, 1},
    {-18.0, -50.0, 6.0, 6.0, 1, 6.0, 3.0, 120, 60, 1}
};


/******************************************************************************/
/* array holding the oids for return to control system                        */
/******************************************************************************/
ObjectID oids[OID_TABLE_SIZE] = { 
    //System status 5
	DEVICE_NAME_OID,   SUPPLIER_NAME_OID,      SOFTWARE_REV_OID, 
	NAME_OVERRIDE_OID, FPGA_VERSION_OID,
    //FrameSync control tab 11
    FRAMESYNC_ENABLE_OID, FRAMESYNC_STATUS_OID, VIDEO_FORMAT_OID,
    SDI_SIGNAL_FORMAT_OID, REFERENCE_OID, ON_LOSS_INPUT_SIG_OID,ON_LOSS_FRAMESYNC_OID,
    VERTICAL_DELAY_OID, HORIZONTAL_DELAY_OID, AUDIO_OFFSET_OID,
     //"Key" control tab 13
    KEY_ENABLE_OID, KEY_CLIP_OID, KEY_GAIN_OID, KEY_MAKE_LINEAR_OID, KEY_INVERT_OID,
    KEY_TYPE_OID, KEY_ALPHA_OID, KEY_TRANSPARENT_OID, KEY_BOXMASK_OID, KEY_MAX_TOP_OID,
    KEY_MAX_BOT_OID, KEY_MAX_LEFT_OID, KEY_MAX_RIGHT_OID, KEY_STATE_OID,
    //"Audio" control tab 20
    AUDIO_VOL_TAB_OID,AUDIO_CTR_VIA_PANEL,AUDIO_VOL_HEADER_OID, AUDIO_VOL_DATA_OID,AUDIO_VOL_MIXL_OID,AUDIO_VOL_MIXR_OID,
    //Leveler control tab 13
    LEVEL_TAB_OID, LEVEL_BYPASS_OID, LEVEL_MODE_OID, LEVEL_OPTHR_OID, LEVEL_GATETHR_OID, LEVEL_TRRANGE_OID,
    LEVEL_TRBYPASS_OID, LEVEL_TRTIME_OID, LEVEL_AGCRANGE_OID,LEVEL_AGCBPRANGE_OID, LEVEL_AGCTIME_OID,
    LEVEL_AGCRMSTIME_OID,
    //Audio delay subTab
    DL_TAB_OID, DL_BYPASS_OID, DL_TIME_OID,
    //Audio state
    AUDIO_PROC_OID,
    //Logo control tab 10
    LOGO_PROC_OID,
    LOGO_STATITL_OID, LOGO_TOTAL_OID, LOGO_ACTIDX_OID, LOGO_ACTNAME_OID, LOGO_CURTITL_OID,
    LOGO_CURIDX_OID, LOGO_CURNAME_OID, LOGO_CURPOSX_OID, LOGO_CURPOSY_OID, LOGO_ACTIVATE_OID,
    //Admin control tab 13
    ADMIN_TITLE_OID, ADMIN_UNAME_OID, ADMIN_PASSWD_OID, ADMIN_LOGIN_OID, ADMIN_NETTITL_OID,
    ADMIN_NETMAC_OID, ADMIN_NETIP_OID, ADMIN_NETCANCEL_OID, ADMIN_NETAPPLY_OID, ADMIN_LICTITL_OID,
    ADMIN_LICINFO_OID, ADMIN_NEWLIC_OID, ADMIN_LICUPDATE_OID
};
//----------------------------------------------------------------------------------------
ReportObject reportItems[N_REPORT_ITEMS]=
{
    {SDI_SIGNAL_FORMAT_OID,4,1},
    {FRAMESYNC_STATUS_OID,4,0},
    {KEY_STATE_OID, 4, 2}
};
//----------------------------------------------------------------------------------------
AlarmNamedChoice frameSyncStatusItems[3] =
{
   { 0, "On", 0 },
   { 1, "Off", 0 },
   { 2, "Off no valid reference detected", 1 }
};
AlarmConstraint frameSyncStatusAlarm = { frameSyncStatusItems, 3 };
Int16Param frameSyncStatusParam =
{
   "FrameSync",         	// name
   FRAMESYNC_STATUS_OID,      	// oid
   ACCESS_READONLY,        	// access
   0,						// widget type
   2,                      	// value
   ALARM_TABLE,      	   	// ctype
   0,                      	// clen
   &frameSyncStatusAlarm             	// constraint data
};
//----------------------------------------------------------------------------------------
AlarmNamedChoice keyStatusItems[2] =
{
   { 0, "On", 0 },  //bit num, "state_char", display_type
   { 1, "Off", 0 },
};
AlarmConstraint keyStatusAlarm = { keyStatusItems, 2 };
Int16Param keyStatusParam =
{
   "Key",         	// name
   KEY_STATE_OID,      	// oid
   ACCESS_READONLY,        	// access
   0,						// widget type
   2,                      	// value
   ALARM_TABLE,      	   	// ctype
   0,                      	// clen
   &keyStatusAlarm             	// constraint data
};
//----------------------------------------------------------------------------------------
AlarmNamedChoice audioStatusItems[2] =
{
   { 0, "On", 0 },  //bit num, "state_char", display_type
   { 1, "Off", 0 },
};
AlarmConstraint audioStatusAlarm = { audioStatusItems, 2 };
Int16Param audioStatusParam =
{
   "Audio",         	// name
   AUDIO_PROC_OID,      	// oid
   ACCESS_READONLY,        	// access
   0,						// widget type
   2,                      	// value
   ALARM_TABLE,      	   	// ctype
   0,                      	// clen
   &audioStatusAlarm             	// constraint data
};
//----------------------------------------------------------------------------------------
AlarmNamedChoice logoStatusItems[2] =
{
   { 0, "On", 0 },  //bit num, "state_char", display_type
   { 1, "Off", 0 },
};
AlarmConstraint logoStatusAlarm = { logoStatusItems, 2 };
Int16Param logoStatusParam =
{
   "LogoInsert",         	// name
   LOGO_PROC_OID,      	// oid
   ACCESS_READONLY,        	// access
   0,						// widget type
   2,                      	// value
   ALARM_TABLE,      	   	// ctype
   0,                      	// clen
   &logoStatusAlarm             	// constraint data
};

//----------------------------------------------------------------------------------------
NamedChoice frameSyncEnableItems[2]=
{
    { 0,    "Off" },
    { 1,    "On" }
};
ChoiceConstraint frameSyncEnableChoice = { frameSyncEnableItems, 2 };
Int16Param frameSyncEnableCheckboxParam =
{
   "FrameSync Enable",            // name
   FRAMESYNC_ENABLE_OID,          // oid
   ACCESS_READWRITE,       	// access
   WIDGET_CHECKBOX,         // widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &frameSyncEnableChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice referenceItems[2] =
{
   { 0, "Input Video" },
   { 1, "External" }
};
ChoiceConstraint referenceChoice = { referenceItems, 2 };
Int16Param referenceParam =
{
   "Reference",        // name
   REFERENCE_OID,            // oid
   ACCESS_READWRITE,       	// access
   WIDGET_COMBO_BOX,		// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &referenceChoice      // constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice onLossSignalItems[3] =
{
   { 0, "Disable Outputs" },
   { 1, "Freeze Last Frame" },
   { 2, "Freeze to Color" }
};
ChoiceConstraint onLossInputSignalChoice = { onLossSignalItems, 3 };
Int16Param onLossInputSignalParam =
{
   "On Loss Input Signal",        // name
   ON_LOSS_INPUT_SIG_OID,            // oid
   ACCESS_READWRITE,       	// access
   WIDGET_COMBO_BOX,		// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &onLossInputSignalChoice      // constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice onLossFrameSyncColorItems[9] =
{
   { 0, "White" },
   { 1, "Black" },
   { 2, "Gray 50%" },
   { 3, "Red" },
   { 4, "Green" },
   { 5, "Blue" },
   { 6, "Yellow" },
   { 7, "Magenta" },
   { 8, "Cyan" },
};
ChoiceConstraint onLossFrameSyncColorChoice = { onLossFrameSyncColorItems, 9 };
Int16Param onLossFrameSyncColorParam =
{
   "On Loss FrameSync Freeze Color",        // name
   ON_LOSS_FRAMESYNC_OID,            // oid
   ACCESS_READWRITE,       	// access
   WIDGET_COMBO_BOX,		// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &onLossFrameSyncColorChoice      // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint verticalDelayRange = { -1124, 1124 };
Int16Param verticalDelayParam =
{
    "Vertical Delay (lines)",        // name
   VERTICAL_DELAY_OID,            // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,		// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &verticalDelayRange      // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint horizontalDelayRange = { -64.0, 64.0 };
FloatParam horizontalDelayParam =
{
   "Horizontal Delay (us)",            // name
   HORIZONTAL_DELAY_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   -10.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &horizontalDelayRange          // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint audioOffsetRange = { -575.0, 575.0 };
FloatParam audioOffsetParam =
{
   "Audio Offset from Video (ms)",            // name
   AUDIO_OFFSET_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   -20.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &audioOffsetRange          // constraint data
};
//----------------------------------------------------------------------------------------
StringParam deviceNameParam =
{
   "Product Name",         	// name
   DEVICE_NAME_OID,        	// oid
   ACCESS_READONLY,        	// access
   "HD-FX1 "         		// value
};
//----------------------------------------------------------------------------------------
StringParam supplierNameParam =
{
   "Supplier Name",         // name
   SUPPLIER_NAME_OID,       // oid
   ACCESS_READONLY,        	// access
   "HD Vietnam Ltd.,Co."   	// value
};
//----------------------------------------------------------------------------------------
char fpga_version[8]="Unknown";
StringParam fpgaVersionParam =
{
   "FPGA Rev",         	// name
   FPGA_VERSION_OID,      	// oid
   ACCESS_READONLY,        	// access
   fpga_version   				// value
};//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
StringParam softwareRevParam =
{
   "Software Rev",         	// name
   SOFTWARE_REV_OID,        // oid
   ACCESS_READONLY,        	// access
   "3.10"   				// value    /*Last update 9:24 AM 02/02/2013*/
};
//----------------------------------------------------------------------------------------
StringParam nameOverrideParam =
{
   "Name Override",         // name
   NAME_OVERRIDE_OID,       // oid
   ACCESS_READONLY,        // access
   "Universal Video Processing"         // value
};
//----------------------------------------------------------------------------------------
NamedChoice formatChoiceItems[3] =
{
   { 0, "HD 1080i" },
   { 1, "SD 576i" },
   { 2, "Auto" }
};
ChoiceConstraint videoFormatChoice = { formatChoiceItems, 3 }; 
Int16Param videoFormatParam =
{
   "Input Format",                	// name
   VIDEO_FORMAT_OID,        	// oid
   ACCESS_READWRITE,       	// access
   WIDGET_COMBO_BOX,						// widget type
   2,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &videoFormatChoice            	// constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice formatSignalItems[3] =
{
   { 0, "Unknown format" },
   { 1, "SD 576i" },
   { 2, "HD 1080i" }
};
ChoiceConstraint formatChoice = { formatSignalItems, 3 };

Int16Param formatSignalParam =
{
   "Signal format",        	// name
   SDI_SIGNAL_FORMAT_OID,  	// oid
   ACCESS_READONLY,        	// access
   0,						// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,  	   	// ctype
   0,                      	// clen
   &formatChoice            // constraint data
};
//----------------------------------------------------------------------------------------
//KEY TAB
//----------------------------------------------------------------------------------------
NamedChoice keyEnableItems[2]=
{
    { 0,    "Off" },
    { 1,    "On" }
};
ChoiceConstraint keyEnableChoice = { keyEnableItems, 2 };
Int16Param keyEnableCheckboxParam =
{
   "Key Enable",            // name
   KEY_ENABLE_OID,          // oid
   ACCESS_READWRITE,       	// access
   WIDGET_CHECKBOX,         // widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &keyEnableChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint keyClipRange = { 0, 1920 };
Int16Param keyClipParam =
{
   "Clip",                  // name
   KEY_CLIP_OID,            // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &keyClipRange            // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint keyGainRange = { 0, 100 };
Int16Param keyGainParam =
{
   "Gain",                  // name
   KEY_GAIN_OID,            // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   100,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &keyGainRange            // constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice clipAndGainItems[1]=
{
    { 0,    "Make Linear"}
};
ChoiceConstraint clipAndGainChoice = { clipAndGainItems, 1 };
Int16Param keyMakeLinearButtonParam =
{
   "Clip & Gain",           // name
   KEY_MAKE_LINEAR_OID,     // oid
   ACCESS_READONLY,       	// access
   WIDGET_BUTTON_NO_PROMPT,	// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &clipAndGainChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice keyInvertItems[2]=
{
    { 0,    "Off" },
    { 1,    "On" }
};
ChoiceConstraint keyInvertChoice = { keyInvertItems, 2 };
Int16Param keyInvertCheckboxParam =
{
   "Key Invert",            // name
   KEY_INVERT_OID,          // oid
   ACCESS_READONLY,       	// access
   WIDGET_CHECKBOX,         // widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &keyInvertChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice keyTypeItems[2]=
{
    { 0,    "Auto Select"},
    { 1,    "Manual"},
};
ChoiceConstraint keyTypeChoice = { keyTypeItems, 2 };
Int16Param keyTypeParam =
{
   "Key Type",              // name
   KEY_TYPE_OID,            // oid
   ACCESS_READONLY,       	// access
   WIDGET_COMBO_BOX,		// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &keyTypeChoice           // constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice keyAlphaTypeItems[2]=
{
    { 0,    "Shaped"},
    { 1,    "Unshaped"},
};
ChoiceConstraint keyAlphaTypeChoice = { keyAlphaTypeItems, 2 };
Int16Param keyAlphaTypeParam =
{
   "Key Alpha Type",        // name
   KEY_ALPHA_OID,            // oid
   ACCESS_READONLY,       	// access
   WIDGET_COMBO_BOX,		// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &keyAlphaTypeChoice      // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint keyTransparentRange = { 0, 100 };
Int16Param keyTransparentParam =
{
   "Transparency",          // name
   KEY_TRANSPARENT_OID,     // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &keyTransparentRange     // constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice checkBoxItems[2]=
{
    { 0,    "Off" },
    { 1,    "On" }
};
ChoiceConstraint checkBoxChoice = { checkBoxItems, 2 };
Int16Param keyBoxMaskParam =
{
   "Box Mask",             // name
   KEY_BOXMASK_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_COMBO_BOX,		// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &checkBoxChoice        // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint keyMaskTopRange = { 0, 1080 };
Int16Param keyMaskTopParam =
{
   "Mask Top Edge",         // name
   KEY_MAX_TOP_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &keyMaskTopRange         // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint keyMaskBottomRange = { 0, 1080 };
Int16Param keyMaskBottomParam =
{
   "Mask Bottom Edge",      // name
   KEY_MAX_BOT_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &keyMaskBottomRange      // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint keyMaskLeftRange = { 0, 1920 };
Int16Param keyMaskLeftParam =
{
   "Mask Left Edge",        // name
   KEY_MAX_LEFT_OID,        // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &keyMaskLeftRange         // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint keyMaskRightRange = { 0, 1920 };
Int16Param keyMaskRightParam =
{
   "Mask Right Edge",       // name
   KEY_MAX_RIGHT_OID,       // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &keyMaskRightRange         // constraint data
};
//----------------------------------------------------------------------------------------
//"Audio TAB
//----------------------------------------------------------------------------------------
StringParam audioVolumeTabParam =
{
    "tab0",
    AUDIO_VOL_TAB_OID,
    ACCESS_READONLY,
    "Mixer"
};
//----------------------------------------------------------------------------------------
NamedChoice ctrlPanelItems[2]=
{
    { 0,    "Software" },
    { 1,    "CtrlPanel" }
};
ChoiceConstraint ctrlPanelChoice = { ctrlPanelItems, 2 };
Int16Param ctrlPanelCheckboxParam =
{
   "Control Panel",            // name
   AUDIO_CTR_VIA_PANEL,          // oid
   ACCESS_READONLY,       	// access
   WIDGET_CHECKBOX,         // widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &ctrlPanelChoice      	// constraint data
};

//----------------------------------------------------------------------------------------
NamedChoice volumeHeaderItems[4] =
{
    { 0,    "Channel 1" },
    { 1,    "Channel 2" },
    { 2,    "Channel 3" },
    { 3,    "Master1 Out" },
};
ChoiceConstraint audioVolumeHeaderChoice = { volumeHeaderItems, 4 };
UINT16 volumeHeaderData[4] = {0,1,2,3};
Int16ArrayParam audioVolumeHeaderParam =
{
    "Channel 1",              // name
    AUDIO_VOL_HEADER_OID,   // oid
    ACCESS_READONLY,        // access
    WIDGET_ARRAY_HEADER_HORIZONTAL,   // widget type
    volumeHeaderData,       // value
    CHOICE_CONSTRAINT,      // ctype
    0,                      // clen
    &audioVolumeHeaderChoice,       // constraint data
    4                       // number of elements
};
//----------------------------------------------------------------------------------------
RangeConstraint audioVolumeRange = { -80, 40 };
INT16 volumeData[4] = {0,0,0,0};
Int16ArrayParam audioVolumeDataParam =
{
    "Volume",              // name
    AUDIO_VOL_DATA_OID,   // oid
    ACCESS_READWRITE,        // access
    WIDGET_SLIDER_VERTICAL,   // widget type
    volumeData,       // value
    RANGE_CONSTRAINT,      // ctype
    0,                      // clen
    &audioVolumeRange,       // constraint data
    4                       // number of elements
};
//----------------------------------------------------------------------------------------
UINT16 volumeMixLCheckBox[4]={0,0,0,0};
Int16ArrayParam audioVolumeMixLeftParam =
{
    "Mix Left",              // name
    AUDIO_VOL_MIXL_OID,   // oid
    ACCESS_READWRITE,        // access
    WIDGET_CHECKBOX,   // widget type
    volumeMixLCheckBox,       // value
    CHOICE_CONSTRAINT,      // ctype
    0,                      // clen
    &checkBoxChoice,       // constraint data
    3                       // number of elements
};
//----------------------------------------------------------------------------------------
UINT16 volumeMixRCheckBox[4]={0,0,0,0};
Int16ArrayParam audioVolumeMixRightParam =
{
    "Mix Right",              // name
    AUDIO_VOL_MIXR_OID,   // oid
    ACCESS_READWRITE,        // access
    WIDGET_CHECKBOX,   // widget type
    volumeMixRCheckBox,       // value
    CHOICE_CONSTRAINT,      // ctype
    0,                      // clen
    &checkBoxChoice,       // constraint data
    3                       // number of elements
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
NamedChoice volumeHeader2Items[4] =
{
    { 0,    "Channel 1" },
    { 1,    "Channel 2" },
    { 2,    "Channel 3" },
    { 3,    "Master2 Out" },
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
RangeConstraint audioLevelRange = {-40, 20};
//----------------------------------------------------------------------------------------
//Leveler tab
//----------------------------------------------------------------------------------------
StringParam levelerTabParam =
{
    "tab2",
    LEVEL_TAB_OID,
    ACCESS_READONLY,
    "Leveler"
};
//----------------------------------------------------------------------------------------
NamedChoice levelerBypassItems[2]=
{
    { 0,    "On" },
    { 1,    "Bypass" }
};
ChoiceConstraint levelerBypassChoice = { levelerBypassItems, 2 };
Int16Param levelerBypassCheckboxParam =
{
   "Bypass",            // name
   LEVEL_BYPASS_OID,          // oid
   ACCESS_READWRITE,       	// access
   WIDGET_CHECKBOX,         // widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &levelerBypassChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice leverlerModeItems[4] =
{
   { 0, "TV" },
   { 1, "TV Live" },
   { 2, "TV Movie" },
   { 3, "User Define" }
};
ChoiceConstraint leverlerModeChoices = { leverlerModeItems, 4 }; 
Int16Param leverlerModeParam =
{
   "Mode Select",                	// name
   LEVEL_MODE_OID,        	// oid
   ACCESS_READWRITE,       	// access
   WIDGET_COMBO_BOX,						// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &leverlerModeChoices            	// constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint lvOpThresholdRange = { -40.0, 0.0 };
FloatParam lvOpThresholdParam =
{
   "OP Threshold (dB)",            // name
   LEVEL_OPTHR_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   -18.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &lvOpThresholdRange          // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint lvGateThresholdRange = { -60.0, 20.0 };
FloatParam lvGateThresholdParam =
{
   "Gate Threshold (dB)",            // name
   LEVEL_GATETHR_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   -40.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &lvGateThresholdRange          // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint lvTransientRange = { 0.0, 30.0 };
FloatParam lvTransientRangeParam =
{
   "Transient Range (dB)",            // name
   LEVEL_TRRANGE_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   10.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &lvTransientRange          // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint lvTransientBypassRange = { 0.0, 30.0 };
FloatParam lvTransientBypassParam =
{
   "Transient Bypass (dB)",            // name
   LEVEL_TRBYPASS_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   10.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &lvTransientBypassRange          // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint lvTransientTimeRange = { 0.0, 30.0 };
FloatParam lvTransientTimeParam =
{
   "Transient Time (dBFS/sec)",         // name
   LEVEL_TRTIME_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   2.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &lvTransientTimeRange         // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint lvAgcRange = { 0.0, 40.0 };
FloatParam lvAgcRangeParam =
{
   "AGC Range (dB)",            // name
   LEVEL_AGCRANGE_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   10.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &lvAgcRange          // constraint data
};
//----------------------------------------------------------------------------------------
FloatRangeConstraint lvAgcBypassRange = { 0.0, 40.0 };
FloatParam lvAgcBypassRangeParam =
{
   "AGC Bypass Range (dB)",            // name
   LEVEL_AGCBPRANGE_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   10.0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &lvAgcBypassRange          // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint lvAgcTimeRange = { 0, 7200 };
Int16Param lvAgcTimeParam =
{
   "AGC Time (sec)",         // name
   LEVEL_AGCTIME_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   40,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &lvAgcTimeRange         // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint lvAgcRmsTimeRange = { 0, 3600 };
Int16Param lvAgcRmsTimeParam =
{
   "AGC RMS Time (sec)",         // name
   LEVEL_AGCRMSTIME_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_SLIDER_HORIZONTAL,// widget type
   60,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &lvAgcRmsTimeRange         // constraint data
};

//----------------------------------------------------------------------------------------
//Audio delay subTab
//----------------------------------------------------------------------------------------
StringParam dlTabParam =
{
    "tab3",
    DL_TAB_OID,
    ACCESS_READONLY,
    "Delay"
};
//----------------------------------------------------------------------------------------
NamedChoice dlBypassItems[2]=
{
    { 0,    "On" },
    { 1,    "Bypass" }
};
ChoiceConstraint dlBypassChoice = { dlBypassItems, 2 };
Int16Param dlBypassCheckboxParam =
{
   "Bypass",            // name
   DL_BYPASS_OID,          // oid
   ACCESS_READWRITE,       	// access
   WIDGET_CHECKBOX,         // widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &dlBypassChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
Int32RangeConstraint dlConstraint = {0, 40000};
Int32Param dlTimeParam =
{
   "Delay Time (ms)",        	// name
   DL_TIME_OID,          // oid
   ACCESS_READWRITE,       	// access
   WIDGET_SLIDER_HORIZONTAL,		// widget type
   0,                       //value
   RANGE_CONSTRAINT,         // ctype
   0,                      	// clen
   &dlConstraint                     // constraint data
};

//----------------------------------------------------------------------------------------
//LOGO TAB
//----------------------------------------------------------------------------------------
StringParam logoStaTitleParam =
{
   "lstatitl",         	// name
   LOGO_STATITL_OID,        	// oid
   ACCESS_READONLY,        	// access
   "Logo status"         		// value
};
//----------------------------------------------------------------------------------------
RangeConstraint logoTotalRange = { 0, 500 };
Int16Param logoTotalParam =
{
   "Total of Logos",         // name
   LOGO_TOTAL_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_TEXTBOX,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &logoTotalRange         // constraint data
};
//----------------------------------------------------------------------------------------
Int16Param logoActIdxParam =
{
   "Activated Logo Index",         // name
   LOGO_ACTIDX_OID,         // oid
   ACCESS_READONLY,       	// access
   WIDGET_TEXTBOX,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &logoTotalRange         // constraint data
};
//----------------------------------------------------------------------------------------
char actLogoName[64]= "Test Logo File Name";
StringParam logoActNameParam =
{
   "Activated Logo Name",         	// name
   LOGO_ACTNAME_OID,        	// oid
   ACCESS_READONLY,        	// access
   actLogoName         		// value
};
//----------------------------------------------------------------------------------------
StringParam logoCurTitleParam =
{
   "lcurtitl",         	// name
   LOGO_CURTITL_OID,        	// oid
   ACCESS_READONLY,        	// access
   "Settings for Logo"         		// value
};
//----------------------------------------------------------------------------------------
Int16Param logoCurIdxParam =
{
   "Current Logo Index",         // name
   LOGO_CURIDX_OID,         // oid
   ACCESS_READWRITE,       	// access
   WIDGET_SPINNER,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &logoTotalRange         // constraint data
};
//----------------------------------------------------------------------------------------
char curLogoName[64]= "Test Logo File Name 2";
StringParam logoCurNameParam =
{
   "Current Logo Name",         	// name
   LOGO_CURNAME_OID,        	// oid
   ACCESS_READONLY,        	// access
   curLogoName         		// value
};
//----------------------------------------------------------------------------------------
RangeConstraint logoPosXRange = {0,1920};
Int16Param logoCurPosXParam =
{
   "Current Logo PosX",         // name
   LOGO_CURPOSX_OID,         // oid
   ACCESS_READWRITE,       	// access
   WIDGET_SPINNER,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &logoPosXRange         // constraint data
};
//----------------------------------------------------------------------------------------
RangeConstraint logoPosYRange = {0,1080};
Int16Param logoCurPosYParam =
{
   "Current Logo PosY",         // name
   LOGO_CURPOSY_OID,         // oid
   ACCESS_READWRITE,       	// access
   WIDGET_SPINNER,// widget type
   0,                      	// value
   RANGE_CONSTRAINT,      	// ctype
   4,                      	// clen
   &logoPosYRange         // constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice logoActivateItems[2]=
{
    { 0,    "Off" },
    { 1,    "On" }
};
ChoiceConstraint logoActivateChoice = { logoActivateItems, 2 };
Int16Param logoActivateParam =
{
   "Activate",            // name
   LOGO_ACTIVATE_OID,          // oid
   ACCESS_READWRITE,       	// access
   WIDGET_CHECKBOX,         // widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &logoActivateChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
//Admin tab
//----------------------------------------------------------------------------------------
StringParam adminUserTitleParam =
{
   "admtitl",         	// name
   ADMIN_TITLE_OID,        	// oid
   ACCESS_READONLY,        	// access
   "Administrator Information"         		// value
};
//----------------------------------------------------------------------------------------
char adminUserName[32]="";
StringParam adminUserNameParam =
{
   "User Name",         	// name
   ADMIN_UNAME_OID,        	// oid
   ACCESS_READWRITE,        	// access
   adminUserName         		// value
};
//----------------------------------------------------------------------------------------
char adminUserPasswd[32]="";
StringParam adminUserPasswdParam =
{
   "Password",         	// name
   ADMIN_PASSWD_OID,        	// oid
   ACCESS_READWRITE,        	// access
   adminUserPasswd         		// value
};
//----------------------------------------------------------------------------------------
NamedChoice adminLoginItems[2]=
{
    { 0,    "    Login    "},
    { 1,    "    Logout    "},
};
ChoiceConstraint adminLoginChoice = { adminLoginItems, 2 };
Int16Param adminLoginButtonParam =
{
   "Login / Logout",           // name
   ADMIN_LOGIN_OID,     // oid
   ACCESS_READWRITE,       	// access
   WIDGET_BUTTON_TOGGLE,	// widget type
   0,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &adminLoginChoice      	// constraint data
};
//----------------------------------------------------------------------------------------
StringParam adminNetTitleParam =
{
   "adntitl",         	// name
   ADMIN_NETTITL_OID,        	// oid
   ACCESS_READONLY,        	// access
   "Network Information"         		// value
};
//----------------------------------------------------------------------------------------
char adminNetMac[20]="";
StringParam adminNetMacParam =
{
   "MAC Address",         	// name
   ADMIN_NETMAC_OID,        	// oid
   ACCESS_READONLY,        	// access
   adminNetMac         		// value
};
//----------------------------------------------------------------------------------------
INT32 adminOldIpAddr=0;
Int32Param adminIpAddrParam =
{
   "IP Address",        	// name
   ADMIN_NETIP_OID,          // oid
   ACCESS_READONLY,       	// access
   WIDGET_IP_ADDRESS,		// widget type
   0,//0xC0A8017B,              // value//192.168.1.123
   NULL_CONSTRAINT,         // ctype
   0,                      	// clen
   NULL                     // constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice cancelItems[1]=
{
    { 1,    "Cancel"}
};
ChoiceConstraint cancelChoice = { cancelItems, 1 };
Int16Param adminNetCancelButtonParam =
{
   "",                      // name
   ADMIN_NETCANCEL_OID,       // oid
   ACCESS_READONLY,       	// access
   WIDGET_BUTTON_NO_PROMPT,	// widget type
   1,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &cancelChoice           	// constraint data
};
//----------------------------------------------------------------------------------------
NamedChoice applyItems[1]=
{
    { 1,    "Apply Changes and Reboot"}
};
ChoiceConstraint applyChoice = { applyItems, 1 };
Int16Param adminNetApplyButtonParam =
{
   "",                      // name
   ADMIN_NETAPPLY_OID,        // oid
   ACCESS_READONLY,       	// access
   WIDGET_BUTTON_PROMPT,	// widget type
   1,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &applyChoice           	// constraint data
};
//----------------------------------------------------------------------------------------
StringParam adminLicTitleParam =
{
   "adlic",         	// name
   ADMIN_LICTITL_OID,        	// oid
   ACCESS_READONLY,        	// access
   "License Information"         		// value
};
//----------------------------------------------------------------------------------------
char adminLicInfo[32]="Standard";
StringParam adminLicInfoParam =
{
   "Current License",         	// name
   ADMIN_LICINFO_OID,        	// oid
   ACCESS_READONLY,        	// access
   adminLicInfo         		// value
};
//----------------------------------------------------------------------------------------
char adminLicNew[32]="";
StringParam adminLicNewParam =
{
   "New License",         	// name
   ADMIN_NEWLIC_OID,        	// oid
   ACCESS_READONLY,        	// access
   adminLicNew         		// value
};
//----------------------------------------------------------------------------------------
NamedChoice licUpdateItems[1]=
{
    { 1,    "Update"}
};
ChoiceConstraint licUpdateChoice = { licUpdateItems, 1 };
Int16Param adminLicUpdateButtonParam =
{
   "",                      // name
   ADMIN_LICUPDATE_OID,       // oid
   ACCESS_READONLY,       	// access
   WIDGET_BUTTON_NO_PROMPT,	// widget type
   1,                      	// value
   CHOICE_CONSTRAINT,      	// ctype
   0,                      	// clen
   &licUpdateChoice           	// constraint data
};
//----------------------------------------------------------------------------------------
extern unsigned char audio_save_context;

extern double rms_ch12_in;
extern double peak_ch12_in;
extern double gain_ch12_out;

extern double rms_ch12_out;
extern double peak_ch12_out;
extern double peak_com_ch12_in;
extern double peak_com_ch12_out;

//========================================================================================
//FUNCTIONS
//----------------------------------------------------------------------------------------
void OGP_GetParamRespone(unsigned char dest,ObjectID oid)
{
StringParam* sparam;	
Int16Param* iparam;
Int32Param* i32param;
Int16ArrayParam* iaparam;
//ChoiceStringParam* csparam;
FloatParam* fparam;
StrArrayParam * saparam;

	switch (oid){
        //FRAMESYNC
		case FRAMESYNC_ENABLE_OID: {
			iparam = &frameSyncEnableCheckboxParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
        case FRAMESYNC_STATUS_OID: {
            iparam = &frameSyncStatusParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        case REFERENCE_OID: {
            iparam = &referenceParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        case ON_LOSS_INPUT_SIG_OID: {
            iparam = &onLossInputSignalParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        case ON_LOSS_FRAMESYNC_OID: {
            iparam = &onLossFrameSyncColorParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        case VERTICAL_DELAY_OID: {
            iparam = &verticalDelayParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        case HORIZONTAL_DELAY_OID: {
            fparam = &horizontalDelayParam;
            OGP_SendFloatParamValue(dest, fparam);
        }break;
        case AUDIO_OFFSET_OID: {
            fparam = &audioOffsetParam;
            OGP_SendFloatParamValue(dest, fparam);
        }break;
            
		case DEVICE_NAME_OID: {
			sparam = &deviceNameParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case SUPPLIER_NAME_OID: {
			sparam = &supplierNameParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case FPGA_VERSION_OID: {
			sparam = &fpgaVersionParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case SOFTWARE_REV_OID: {
			sparam = &softwareRevParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case NAME_OVERRIDE_OID: {
			sparam = &nameOverrideParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case VIDEO_FORMAT_OID: {
			iparam = &videoFormatParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case SDI_SIGNAL_FORMAT_OID: {
			iparam = &formatSignalParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
        //KEY
        case KEY_STATE_OID: {
            iparam = &keyStatusParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        
		case KEY_ENABLE_OID: {
			iparam = &keyEnableCheckboxParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_CLIP_OID: {
			iparam = &keyClipParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_GAIN_OID: {
			iparam = &keyGainParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_MAKE_LINEAR_OID: {
			iparam = &keyMakeLinearButtonParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_INVERT_OID: {
			iparam = &keyInvertCheckboxParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_TYPE_OID: {
			iparam = &keyTypeParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_ALPHA_OID: {
			iparam = &keyAlphaTypeParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_TRANSPARENT_OID: {
			iparam = &keyTransparentParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_BOXMASK_OID: {
			iparam = &keyBoxMaskParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_MAX_TOP_OID: {
			iparam = &keyMaskTopParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_MAX_BOT_OID: {
			iparam = &keyMaskBottomParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_MAX_LEFT_OID: {
			iparam = &keyMaskLeftParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case KEY_MAX_RIGHT_OID: {
			iparam = &keyMaskRightParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;
        //AUDIO-volume
        case AUDIO_PROC_OID: {
            iparam = &audioStatusParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
		case AUDIO_VOL_TAB_OID: {
			sparam = &audioVolumeTabParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case AUDIO_CTR_VIA_PANEL: {
			iparam = &ctrlPanelCheckboxParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case AUDIO_VOL_HEADER_OID: {
			iaparam = &audioVolumeHeaderParam;
			OGP_SendInt16ArrParamValue(dest, iaparam);
		} break;	
		case AUDIO_VOL_DATA_OID: {
			iaparam = &audioVolumeDataParam;
			OGP_SendInt16ArrParamValue(dest, iaparam);
		} break;	
		case AUDIO_VOL_MIXL_OID: {
			iaparam = &audioVolumeMixLeftParam;
			OGP_SendInt16ArrParamValue(dest, iaparam);
		} break;	
		case AUDIO_VOL_MIXR_OID: {
			iaparam = &audioVolumeMixRightParam;
			OGP_SendInt16ArrParamValue(dest, iaparam);
		} break;	

        
        //Leveler
		case LEVEL_TAB_OID: {
			sparam = &levelerTabParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case LEVEL_BYPASS_OID: {
			iparam = &levelerBypassCheckboxParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;
		case LEVEL_MODE_OID: {
			iparam = &leverlerModeParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
                case LEVEL_OPTHR_OID: {
                    fparam = &lvOpThresholdParam;
                    OGP_SendFloatParamValue(dest, fparam);
                }break;
                case LEVEL_GATETHR_OID: {
                    fparam = &lvGateThresholdParam;
                    OGP_SendFloatParamValue(dest, fparam);
                }break;
                case LEVEL_TRRANGE_OID: {
                    fparam = &lvTransientRangeParam;
                    OGP_SendFloatParamValue(dest, fparam);
                }break;
                case LEVEL_TRBYPASS_OID: {
                    fparam = &lvTransientBypassParam;
                    OGP_SendFloatParamValue(dest, fparam);
                }break;
                case LEVEL_TRTIME_OID: {
                    fparam = &lvTransientTimeParam;
                    OGP_SendFloatParamValue(dest, fparam);
                }break;
                case LEVEL_AGCRANGE_OID: {
                    fparam = &lvAgcRangeParam;
                    OGP_SendFloatParamValue(dest, fparam);
                }break;
                case LEVEL_AGCBPRANGE_OID: {
                    fparam = &lvAgcBypassRangeParam;
                    OGP_SendFloatParamValue(dest, fparam);
                }break;
                case LEVEL_AGCTIME_OID: {
                    iparam = &lvAgcTimeParam;
                    OGP_SendInt16ParamValue(dest, iparam);
                }break;
                case LEVEL_AGCRMSTIME_OID: {
                    iparam = &lvAgcRmsTimeParam;
                    OGP_SendInt16ParamValue(dest, iparam);
                }break;
        //Audio Delay
		case DL_TAB_OID: {
			sparam = &dlTabParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case DL_BYPASS_OID: {
			iparam = &dlBypassCheckboxParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;
		case DL_TIME_OID: {
			i32param = &dlTimeParam;
			OGP_SendInt32ParamValue(dest, i32param);
		} break;	
        
        //Logo
        case LOGO_PROC_OID: {
            iparam = &logoStatusParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        
		case LOGO_STATITL_OID: {
			sparam = &logoStaTitleParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
        case LOGO_TOTAL_OID: {
            iparam = &logoTotalParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        case LOGO_ACTIDX_OID: {
            iparam = &logoActIdxParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
		case LOGO_ACTNAME_OID: {
			sparam = &logoActNameParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case LOGO_CURTITL_OID: {
			sparam = &logoCurTitleParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
        case LOGO_CURIDX_OID: {
            iparam = &logoCurIdxParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
		case LOGO_CURNAME_OID: {
			sparam = &logoCurNameParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
        case LOGO_CURPOSX_OID: {
            iparam = &logoCurPosXParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
        case LOGO_CURPOSY_OID: {
            iparam = &logoCurPosYParam;
            OGP_SendInt16ParamValue(dest, iparam);
        }break;
		case LOGO_ACTIVATE_OID: {
			iparam = &logoActivateParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
        //Admin tab
		case ADMIN_TITLE_OID: {
			sparam = &adminUserTitleParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_UNAME_OID: {
			sparam = &adminUserNameParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_PASSWD_OID: {
			sparam = &adminUserPasswdParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_LOGIN_OID: {
			iparam = &adminLoginButtonParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case ADMIN_NETTITL_OID: {
			sparam = &adminNetTitleParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_NETMAC_OID: {
			sparam = &adminNetMacParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_NETIP_OID: {
			i32param = &adminIpAddrParam;
			OGP_SendInt32ParamValue(dest, i32param);
		} break;	
		case ADMIN_NETCANCEL_OID: {
			iparam = &adminNetCancelButtonParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case ADMIN_NETAPPLY_OID: {
			iparam = &adminNetApplyButtonParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
		case ADMIN_LICTITL_OID: {
			sparam = &adminLicTitleParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_LICINFO_OID: {
			sparam = &adminLicInfoParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_NEWLIC_OID: {
			sparam = &adminLicNewParam;
			OGP_SendStringParamValue(dest, sparam);
		} break;	
		case ADMIN_LICUPDATE_OID: {
			iparam = &adminLicUpdateButtonParam;
			OGP_SendInt16ParamValue(dest, iparam);
		} break;	
        case 0xFF09: {   //Name override
            OGP_SendGetParamNotFound(dest,oid);
        }break;
		default: {
			OGP_SendGetParamNotFound(dest,oid);
            printf("GetParam not defined yet: 0x%04x\r\n",oid);
            while(1);
		}
	}	
}
//----------------------------------------------------------------------------------------
void OGP_SendInt16ParamValue(unsigned char dest, Int16Param* param)
{
    BYTE buffer[6];
    INT16 oid,data;
   
    oid  = param->oid;
    data = param->value;
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid,&buffer[1]);
    buffer[3]  = 2;                			//data length
    writeUint16(data,&buffer[4]);
#ifdef DEBUG	
    //printf("GetParam oid 0x%04X: 0x%04X\r\n",oid,data);
#endif
    OGP_SendData(dest,OGP_GET_PARAM_RESPONE,buffer,6);
}  
//----------------------------------------------------------------------------------------
void OGP_SendFloatParamValue(unsigned char dest, FloatParam* param)
{
    BYTE buffer[8];
    UINT16 oid;
    float data;
   
    oid  = param->oid;
    data = param->value;
   // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid,&buffer[1]);            //oid
    buffer[3]  = 4;                			//data length
    writeFloat(data,&buffer[4]);
#ifdef DEBUG	
    //printf("GetParam oid 0x%04X: %f\r\n",oid,data);
#endif
    OGP_SendData(dest,OGP_GET_PARAM_RESPONE,buffer,8);
}  
//----------------------------------------------------------------------------------------
void OGP_SendStringParamValue(unsigned char dest,StringParam* param)
{
    char buffer[64];
    BYTE namelen;
    UINT16 oid;
   
    oid  = param->oid;
    buffer[0]  = OGP_OK;
    writeUint16(oid,&buffer[1]);
    strcpy(&buffer[4], param->value);
    namelen = strlen(&buffer[4]) + 1;
    buffer[3]  = namelen;
#ifdef DEBUG   
    //printf("Param Value: %s\r\n",param->value);
#endif   
    OGP_SendData(dest,OGP_GET_PARAM_RESPONE,buffer,(namelen+4));
}
//----------------------------------------------------------------------------------------
void OGP_SendGetParamNotFound(unsigned char dest, ObjectID oid)
{
	BYTE buffer[3];
	buffer[0] = OGP_PARAM_NOTFOUND;
    writeUint16(oid,&buffer[1]);
#ifdef DEBUG	
	//printf("GetParam with oid 0x%04X not found!\r\n",oid);
#endif
	OGP_SendData(dest,OGP_GET_PARAM_RESPONE,buffer,3);	
}
//----------------------------------------------------------------------------------------
void OGP_GetNumOfParamRespone(unsigned char dest)
{
BYTE buffer[3];
UINT16 size;
	size=(UINT16)OID_TABLE_SIZE;
	buffer[0]=OGP_OK;
    writeUint16(size,&buffer[1]);
#ifdef DEBUG	
	//printf("Number of Param: %d\r\n",size);
#endif
	OGP_SendData(dest,OGP_GET_NUMPARAMS_RESPONE,buffer,3);
}
//----------------------------------------------------------------------------------------
void OGP_GetParamOidsRespone(unsigned char dest,unsigned char index,unsigned char nParam)
{
unsigned char buffer[OGP_MTU];
unsigned short i,size,tsize,idx;
UINT16 temp;

	if(((index+nParam)>OID_TABLE_SIZE)||(nParam>128)){
		buffer[0]=OGP_PARAM_NOTFOUND;
		buffer[1]=0;
		buffer[2]=index;
		buffer[3]=nParam;
#ifdef DEBUG	
		printf("GetParamOids invalid param\r\n");
#endif
		OGP_SendData(dest,OGP_GET_PARAM_OIDS_RESPONE,buffer,4);
	}
	else{
		if(nParam==0) size= OID_TABLE_SIZE;
		else size=nParam;
		
		buffer[0]=OGP_OK;	
		buffer[1]=0;
		buffer[2]=index;
		buffer[3]=size;
		idx=index;
		for(i=0;i<size;i++){
			temp = oids[idx];
            writeUint16(temp, &buffer[i*2+4]);
			idx++;
		}
		tsize=size*2+4;
#ifdef DEBUG	
		printf("GetParamOidsRespone: from index %d to %d\r\n",index,index+size-1);
#endif
		OGP_SendData(dest,OGP_GET_PARAM_OIDS_RESPONE,buffer,tsize);
	}
}	
//----------------------------------------------------------------------------------------
void OGP_GetDescriptorRespone(unsigned char dest,ObjectID oid)
{
StringParam* sparam;	
//ChoiceStringParam* csparam;	
Int16Param* iparam;
Int32Param* i32param;
Int16ArrayParam* iaparam;
StrArrayParam * saparam;
FloatParam* fparam;

	switch (oid){
        //FRAMESYNC
		case FRAMESYNC_ENABLE_OID: {
			iparam = &frameSyncEnableCheckboxParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
        case FRAMESYNC_STATUS_OID : {
            iparam = &frameSyncStatusParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
        case REFERENCE_OID : {
            iparam = &referenceParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
        case ON_LOSS_INPUT_SIG_OID : {
            iparam = &onLossInputSignalParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
        case ON_LOSS_FRAMESYNC_OID : {
            iparam = &onLossFrameSyncColorParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
        case VERTICAL_DELAY_OID : {
            iparam = &verticalDelayParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);

        }break;
        case HORIZONTAL_DELAY_OID : {
            fparam = &horizontalDelayParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",fparam->name);
#endif
            OGP_SendFloatParamDesc(dest, fparam);

        }break;
        case AUDIO_OFFSET_OID : {
            fparam = &audioOffsetParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",fparam->name);
#endif
            OGP_SendFloatParamDesc(dest, fparam);

        }break;
            
		case DEVICE_NAME_OID: {
			sparam = &deviceNameParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendStringParamDesc(dest, sparam);
		} break;	
		case SUPPLIER_NAME_OID: {
			sparam = &supplierNameParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendStringParamDesc(dest, sparam);
		} break;	
		case FPGA_VERSION_OID: {
			sparam = &fpgaVersionParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendStringParamDesc(dest, sparam);
		} break;	
		case SOFTWARE_REV_OID: {
			sparam = &softwareRevParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendStringParamDesc(dest, sparam);
		} break;	
		case NAME_OVERRIDE_OID: {
			sparam = &nameOverrideParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendStringParamDesc(dest, sparam);
		} break;	
		case VIDEO_FORMAT_OID: {
			iparam = &videoFormatParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",iparam->name);
#endif
			OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case SDI_SIGNAL_FORMAT_OID: {
			iparam = &formatSignalParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",iparam->name);
#endif
			OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
        //KEY
        case KEY_STATE_OID : {
            iparam = &keyStatusParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
        
		case KEY_ENABLE_OID: {
			iparam = &keyEnableCheckboxParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_CLIP_OID: {
			iparam = &keyClipParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_GAIN_OID: {
			iparam = &keyGainParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_MAKE_LINEAR_OID: {
			iparam = &keyMakeLinearButtonParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_INVERT_OID: {
			iparam = &keyInvertCheckboxParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_TYPE_OID: {
			iparam = &keyTypeParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_ALPHA_OID: {
			iparam = &keyAlphaTypeParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_TRANSPARENT_OID: {
			iparam = &keyTransparentParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_BOXMASK_OID: {
			iparam = &keyBoxMaskParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_MAX_TOP_OID: {
			iparam = &keyMaskTopParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_MAX_BOT_OID: {
			iparam = &keyMaskBottomParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_MAX_LEFT_OID: {
			iparam = &keyMaskLeftParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case KEY_MAX_RIGHT_OID: {
			iparam = &keyMaskRightParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;
        //AUDIO
        case AUDIO_PROC_OID : {
            iparam = &audioStatusParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;

        //audio volume
		case AUDIO_VOL_TAB_OID: {
			sparam = &audioVolumeTabParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendTabParamDesc(dest, sparam);
		} break;	
		case AUDIO_CTR_VIA_PANEL: {
			iparam = &ctrlPanelCheckboxParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case AUDIO_VOL_HEADER_OID: {
			iaparam = &audioVolumeHeaderParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",iaparam->name);
#endif
			OGP_SendInt16ArrParamDesc(dest, iaparam);
		} break;	
		case AUDIO_VOL_DATA_OID: {
			iaparam = &audioVolumeDataParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",iaparam->name);
#endif
			OGP_SendInt16ArrParamDesc(dest, iaparam);
		} break;	
		case AUDIO_VOL_MIXL_OID: {
			iaparam = &audioVolumeMixLeftParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",iaparam->name);
#endif
			OGP_SendInt16ArrParamDesc(dest, iaparam);
		} break;	
		case AUDIO_VOL_MIXR_OID: {
			iaparam = &audioVolumeMixRightParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",iaparam->name);
#endif
			OGP_SendInt16ArrParamDesc(dest, iaparam);
		} break;	

        //Leveler
		case LEVEL_TAB_OID: {
			sparam = &levelerTabParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendTabParamDesc(dest, sparam);
		} break;	
		case LEVEL_BYPASS_OID: {
			iparam = &levelerBypassCheckboxParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
                        OGP_SendInt16ParamDesc(dest, iparam);
		} break;
		case LEVEL_MODE_OID: {
			iparam = &leverlerModeParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",iparam->name);
#endif
			OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
                case LEVEL_OPTHR_OID : {
                    fparam = &lvOpThresholdParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",fparam->name);
        #endif
                    OGP_SendFloatParamDesc(dest, fparam);

                }break;
                case LEVEL_GATETHR_OID : {
                    fparam = &lvGateThresholdParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",fparam->name);
        #endif
                    OGP_SendFloatParamDesc(dest, fparam);

                }break;
                case LEVEL_TRRANGE_OID : {
                    fparam = &lvTransientRangeParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",fparam->name);
        #endif
                    OGP_SendFloatParamDesc(dest, fparam);
                }break;
                case LEVEL_TRBYPASS_OID : {
                    fparam = &lvTransientBypassParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",fparam->name);
        #endif
                    OGP_SendFloatParamDesc(dest, fparam);
                }break;
                case LEVEL_TRTIME_OID : {
                    fparam = &lvTransientTimeParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",iparam->name);
        #endif
                    OGP_SendFloatParamDesc(dest, fparam);
                }break;
                case LEVEL_AGCRANGE_OID : {
                    fparam = &lvAgcRangeParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",fparam->name);
        #endif
                    OGP_SendFloatParamDesc(dest, fparam);
                }break;
                case LEVEL_AGCBPRANGE_OID : {
                    fparam = &lvAgcBypassRangeParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",fparam->name);
        #endif
                    OGP_SendFloatParamDesc(dest, fparam);
                }break;
                case LEVEL_AGCTIME_OID : {
                    iparam = &lvAgcTimeParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",iparam->name);
        #endif
                    OGP_SendInt16ParamDesc(dest, iparam);
                }break;
                case LEVEL_AGCRMSTIME_OID : {
                    iparam = &lvAgcRmsTimeParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",iparam->name);
        #endif
                    OGP_SendInt16ParamDesc(dest, iparam);
                }break;
        //Audio Delay
		case DL_TAB_OID: {
			sparam = &dlTabParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendTabParamDesc(dest, sparam);
		} break;	
		case DL_BYPASS_OID: {
			iparam = &dlBypassCheckboxParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
                        OGP_SendInt16ParamDesc(dest, iparam);
		} break;
		case DL_TIME_OID: {
			i32param = &dlTimeParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",i32param->name);
#endif
            OGP_SendInt32ParamDesc(dest, i32param);
		} break;	
        
        //Logo
        case LOGO_PROC_OID : {
            iparam = &logoStatusParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;

		case LOGO_STATITL_OID: {
			sparam = &logoStaTitleParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TITLE_HEADER);
		} break;	
        case LOGO_TOTAL_OID : {
            iparam = &logoTotalParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
        case LOGO_ACTIDX_OID : {
            iparam = &logoActIdxParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
		case LOGO_ACTNAME_OID: {
			sparam = &logoActNameParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TEXT_ENTRY);
		} break;	
		case LOGO_CURTITL_OID: {
			sparam = &logoCurTitleParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TITLE_HEADER);
		} break;	
                case LOGO_CURIDX_OID : {
                    iparam = &logoCurIdxParam;
        #ifdef DEBUG	
                    printf("GetDescriptor %s\r\n",iparam->name);
        #endif
                    OGP_SendInt16ParamDesc(dest, iparam);
                }break;
		case LOGO_CURNAME_OID: {
			sparam = &logoCurNameParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TEXT_ENTRY);
		} break;	
        case LOGO_CURPOSX_OID : {
            iparam = &logoCurPosXParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
        case LOGO_CURPOSY_OID : {
            iparam = &logoCurPosYParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
        }break;
		case LOGO_ACTIVATE_OID: {
			iparam = &logoActivateParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
        //Admin tab
		case ADMIN_TITLE_OID: {
			sparam = &adminUserTitleParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TITLE_HEADER);
		} break;	        
		case ADMIN_UNAME_OID: {
			sparam = &adminUserNameParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TEXT_ENTRY);
		} break;	        
		case ADMIN_PASSWD_OID: {
			sparam = &adminUserPasswdParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_PASSWORD);
		} break;	        
		case ADMIN_LOGIN_OID: {
			iparam = &adminLoginButtonParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case ADMIN_NETTITL_OID: {
			sparam = &adminNetTitleParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TITLE_HEADER);
		} break;	        
		case ADMIN_NETMAC_OID: {
			sparam = &adminNetMacParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TEXT_ENTRY);
		} break;	        
		case ADMIN_NETIP_OID: {
			i32param = &adminIpAddrParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",i32param->name);
#endif
            OGP_SendInt32ParamDesc(dest, i32param);
		} break;	
		case ADMIN_NETCANCEL_OID: {
			iparam = &adminNetCancelButtonParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case ADMIN_NETAPPLY_OID: {
			iparam = &adminNetApplyButtonParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	
		case ADMIN_LICTITL_OID: {
			sparam = &adminLicTitleParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TITLE_HEADER);
		} break;	        
		case ADMIN_LICINFO_OID: {
			sparam = &adminLicInfoParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TEXT_ENTRY);
		} break;	        
		case ADMIN_NEWLIC_OID: {
			sparam = &adminLicNewParam;
#ifdef DEBUG	
			printf("GetDescriptor %s\r\n",sparam->name);
#endif
			OGP_SendWidgetStringParamDesc(dest, sparam,WIDGET_TEXT_ENTRY);
		} break;	        
		case ADMIN_LICUPDATE_OID: {
			iparam = &adminLicUpdateButtonParam;
#ifdef DEBUG	
            printf("GetDescriptor %s\r\n",iparam->name);
#endif
            OGP_SendInt16ParamDesc(dest, iparam);
		} break;	

		default: {
			OGP_SendParamNotFound(dest,oid);
            printf("GetDescriptor not defined yet: 0x%04x\r\n",oid);
            while(1);
		}break;
	}	
}	
//----------------------------------------------------------------------------------------
void OGP_SendInt16ParamDesc(unsigned char dest, Int16Param* param)
{
    char buffer[OGP_MTU];
    BYTE clen;
    // length of the name (including terminator)
    BYTE namelen = strlen(param->name) + 1;
    // location where the constraint data is written
    BYTE cstart = 13 + namelen;
    // write the constraint data there and store its length
    clen = encodeInt16Constraint(param, &buffer[cstart]);
    // total reponse length
    BYTE resplen = cstart + clen;
    UINT16 oid;

    oid  = param->oid;
    #ifdef DEBUG	
    printf("Clen=%d\r\n",clen);
    #endif
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);           //oid
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = INT16_PARAM_TYPE;           // type
    buffer[6]  = INT16_PARAM_SIZE;           // data size
    buffer[7]  = param->access;              // access
    buffer[8]  = PRECISION_NONE;             // precision
    buffer[9]  = param->widget;              // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);       	// name
    buffer[11 + namelen] = param->ctype;     // ctype
    buffer[12 + namelen] = clen;             // clen
    #ifdef DEBUG2	
    printf("String Descriptor lenght: %d\r\n",buffer[3]);
    for(i=0;i<resplen;i++)
        printf("D%d: %02x\r\n",i,buffer[i]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SendFloatParamDesc(unsigned char dest, FloatParam* param)
{
    char buffer[OGP_MTU];
    BYTE clen;
    // length of the name (including terminator)
    BYTE namelen = strlen(param->name) + 1;
    // location where the constraint data is written
    BYTE cstart = 13 + namelen;
    // write the constraint data there and store its length
    clen = encodeFloatConstraint(param, &buffer[cstart]);
    // total reponse length
    BYTE resplen = cstart + clen;
    UINT16 oid;

    oid  = param->oid;
    #ifdef DEBUG	
    printf("Clen=%d\r\n",clen);
    #endif
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);           //oid
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = FLOAT_PARAM_TYPE;           // type
    buffer[6]  = FLOAT_PARAM_SIZE;           // data size
    buffer[7]  = param->access;              // access
    buffer[8]  = PRECISION_1;                // precision
    buffer[9]  = param->widget;              // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);       	// name
    buffer[11 + namelen] = param->ctype;     // ctype
    buffer[12 + namelen] = clen;             // clen
    #ifdef DEBUG2	
    printf("String Descriptor lenght: %d\r\n",buffer[3]);
    for(i=0;i<resplen;i++)
        printf("D%d: %02x\r\n",i,buffer[i]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SendStringParamDesc(unsigned char dest, StringParam* param)
{
    char buffer[OGP_MTU];
    BYTE namelen = strlen(param->name) + 1;
    BYTE clen = 0; 
    BYTE resplen = 13 + namelen + clen;
    UINT16 oid;

    oid  = param->oid;
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);           //oid
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = STRING_PARAM_TYPE;          // type
    buffer[6]  = 32;                         // nominal data size
    buffer[7]  = param->access;              // access
    buffer[8]  = PRECISION_NONE;             // precision
    buffer[9]  = WIDGET_NONE;                // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);        // name
    buffer[11 + namelen] = NULL_CONSTRAINT;  // ctype
    buffer[12 + namelen] = clen;             // clen
    #ifdef DEBUG	
    printf("String Descriptor length: %d\r\n",buffer[3]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SendWidgetStringParamDesc(unsigned char dest, StringParam* param,unsigned char widget)
{
    char buffer[OGP_MTU];
    BYTE namelen = strlen(param->name) + 1;
    BYTE clen = 0; 
    BYTE resplen = 13 + namelen + clen;
    UINT16 oid;

    oid  = param->oid;
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);           //oid
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = STRING_PARAM_TYPE;          // type
    buffer[6]  = 32;                         // nominal data size
    buffer[7]  = param->access;              // access
    buffer[8]  = PRECISION_NONE;             // precision
    buffer[9]  = widget;                // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);        // name
    buffer[11 + namelen] = NULL_CONSTRAINT;  // ctype
    buffer[12 + namelen] = clen;             // clen
    #ifdef DEBUG	
    printf("String Descriptor length: %d\r\n",buffer[3]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SendParamNotFound(unsigned char dest, ObjectID oid)
{
	BYTE buffer[3];
	buffer[0]=OGP_PARAM_NOTFOUND;
    writeUint16(oid, &buffer[1]);           //oid
#ifdef DEBUG	
	printf("Param with oid 0x%04X not found!\r\n",oid);
#endif
	OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,3);	
}
//----------------------------------------------------------------------------------------
void OGP_SetInt16ParamRespone(unsigned char dest, Int16Param* param)
{
    BYTE buffer[6];
    INT16 oid,data;

    oid  = param->oid;
    data = param->value;
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);           //oid
    buffer[3]  = 2;                			//data length
    writeUint16(data, &buffer[4]);          //data
    #ifdef DEBUG	
    printf("SetParam oid 0x%04X: %d\r\n",oid,data);
    #endif
    OGP_SendData(dest,OGP_SET_PARAM_RESPONE,buffer,6);
}  
//----------------------------------------------------------------------------------------
void OGP_SetInt16ArrayParamRespone(unsigned char dest, Int16ArrayParam* param)
{
    BYTE buffer[6];
    INT16 oid,data;

    oid  = param->oid;
    data = param->value[0];
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);           //oid
    buffer[3]  = 2;                			//data length
    writeUint16(data, &buffer[4]);          //data
    #ifdef DEBUG	
    printf("SetParam oid 0x%04X: %d\r\n",oid,data);
    #endif
    OGP_SendData(dest,OGP_SET_PARAM_RESPONE,buffer,6);
}  
//----------------------------------------------------------------------------------------
void OGP_SetFloatParamRespone(BYTE dest,FloatParam * param)
{
    BYTE buffer[8];
    UINT16 oid;
    float data;

    oid  = param->oid;
    data = param->value;
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid,&buffer[1]);
    buffer[3]  = 4;                			//data length
    writeFloat(data,&buffer[4]);
    #ifdef DEBUG	
    printf("SetParam oid 0x%04X: %f\r\n",oid,data);
    #endif
    OGP_SendData(dest,OGP_SET_PARAM_RESPONE,buffer,8);
    
}
//----------------------------------------------------------------------------------------
void OGP_SetStringParamRespone(unsigned char dest,UINT16 oid,char* data,unsigned char len)
{
BYTE buffer[OGP_MTU];
	buffer[0]=OGP_OK;
    writeUint16(oid, &buffer[1]);
	buffer[3]=len;
	strcpy(&buffer[4],data);
#ifdef DEBUG	
	printf("Acknowledge to Set String Param\r\n");
#endif
	OGP_SendData(dest,OGP_SET_PARAM_RESPONE,buffer,len+4);
}
//----------------------------------------------------------------------------------------
void OGP_RebootRespone(BYTE dest)
{
BYTE buffer[4];
	buffer[0]=OGP_OK;
	buffer[1]='O';
	buffer[2]='k';
	buffer[3]=0;
#ifdef DEBUG	
	printf("Processor is rebooting now!\r\n");
#endif
	OGP_SendData(dest,OGP_REBOOT_RESPONE,buffer,1);	
}
//----------------------------------------------------------------------------------------
void OGP_VerifyUploadRespone(BYTE dest,BYTE type)
{
BYTE buffer[3];
int crc;
//	crc=getMessageCrc(udata,file_length,0);
//	if(crc==file_crc){
		buffer[0]=OGP_OK;		
//	}
//	else{
//		buffer[0]=OGP_INVALID_CRC;	
//	}
	buffer[1]=type;
	buffer[2]=0;
#ifdef DEBUG	
	printf("Verify Upload OK\r\n");
#endif	
	OGP_SendData(dest,OGP_VERIFY_UPLOAD_RESPONE,buffer,3);
}
//----------------------------------------------------------------------------------------
void OGP_GetSnmpBaseOidRespone(BYTE dest)
{
BYTE buffer[3];
	buffer[0]=OGP_UNSUPPORTED;	
	buffer[1]=0;
	buffer[2]=0;
#ifdef DEBUG	
	printf("SNMP Unsupported in this version\r\n");
#endif	
	OGP_SendData(dest,OGP_GET_SNMP_BASE_RESPONE,buffer,3);
}
//----------------------------------------------------------------------------------------
void OGP_SetParamRespond(OgpShortFrame * fr)
{
    BYTE dest;
    UINT16 oid;
    BYTE dlen;
    INT16 temp_data;
    INT32 i32_data;
    float fval;
      
    dest=fr->header.source;
    oid=((fr->data[1]<<8)|fr->data[2]);
    dlen=fr->data[3];

    switch(oid){
        case REFERENCE_OID: {
            temp_data=readUint16(&fr->data[4]);
            referenceParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&referenceParam);
            DoSetReference(temp_data);
        } break;
        case ON_LOSS_INPUT_SIG_OID: {
            temp_data=readUint16(&fr->data[4]);
            onLossInputSignalParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&onLossInputSignalParam);
            DoSetOnLossInputSignalOption(temp_data);
        } break;
        case FRAMESYNC_ENABLE_OID: {
            temp_data=readUint16(&fr->data[4]);

            frameSyncEnableCheckboxParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&frameSyncEnableCheckboxParam);
            DoSetFrameSyncEnable(temp_data);
        } break;
        case ON_LOSS_FRAMESYNC_OID: {
            temp_data=readUint16(&fr->data[4]);
            onLossFrameSyncColorParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&onLossFrameSyncColorParam);
            DoSetOnLossFrameSyncOption(temp_data);
        } break;
        case VERTICAL_DELAY_OID: {
            temp_data=readUint16(&fr->data[4]);
            verticalDelayParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&verticalDelayParam);
            DoSetVerticalDelay(temp_data);
        } break;
        case HORIZONTAL_DELAY_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            horizontalDelayParam.value=fval;
            OGP_SetFloatParamRespone(dest,&horizontalDelayParam);
            DoSetHorizontalDelay(fval);
        } break;
        case AUDIO_OFFSET_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            audioOffsetParam.value=fval;
            OGP_SetFloatParamRespone(dest,&audioOffsetParam);
            DoSetAudioOffset(fval);
        } break;

        case NAME_OVERRIDE_OID: {
            strcpy(&nameOverrideParam.value[0],&fr->data[4]);
#ifdef DEBUG
                printf("Changed to %s\r\n",nameOverrideParam.value);
#endif
            OGP_SetStringParamRespone(dest,oid,&fr->data[4],dlen);
        } break;
        case VIDEO_FORMAT_OID: {
            temp_data=readUint16(&fr->data[4]);

            videoFormatParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&videoFormatParam);
            DoSetInputVideoFormat(temp_data);
        } break;
        //KEY
        case KEY_ENABLE_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyEnableCheckboxParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyEnableCheckboxParam);
            DoSetKeyEnable(temp_data);
        } break;
        case KEY_CLIP_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyClipParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyClipParam);
            DoSetKeyClip(temp_data);
        } break;
        case KEY_GAIN_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyGainParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyGainParam);
            DoSetKeyGain(temp_data);
        } break;
        case KEY_MAKE_LINEAR_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyMakeLinearButtonParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyMakeLinearButtonParam);
            DoSetKeyMakeLine(temp_data);
        } break;
        case KEY_INVERT_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyInvertCheckboxParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyInvertCheckboxParam);
            DoSetKeyInvert(temp_data);
        } break;
        case KEY_TYPE_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyTypeParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyTypeParam);
            DoSetKeyType(temp_data);
        } break;
        case KEY_ALPHA_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyAlphaTypeParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyAlphaTypeParam);
            DoSetKeyAlphaType(temp_data);
        } break;
        case KEY_TRANSPARENT_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyTransparentParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyTransparentParam);
            DoSetKeyTransparent(temp_data);
        } break;
        case KEY_BOXMASK_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyBoxMaskParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyBoxMaskParam);
            DoSetKeyBoxMask(temp_data);
        } break;
        case KEY_MAX_TOP_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyMaskTopParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyMaskTopParam);
            DoSetKeyBoxMaskTop(temp_data);
        } break;
        case KEY_MAX_BOT_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyMaskBottomParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyMaskBottomParam);
            DoSetKeyBoxMaskBot(temp_data);
        } break;
        case KEY_MAX_LEFT_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyMaskLeftParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyMaskLeftParam);
            DoSetKeyBoxMaskLeft(temp_data);
        } break;
        case KEY_MAX_RIGHT_OID: {
            temp_data=readUint16(&fr->data[4]);

            keyMaskRightParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&keyMaskRightParam);
            DoSetKeyBoxMaskRight(temp_data);
        } break;
        //Audio

        //Leveler
        case LEVEL_BYPASS_OID: {
            temp_data=readUint16(&fr->data[4]);

            levelerBypassCheckboxParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&levelerBypassCheckboxParam);
            DoSetLevelerBypass(temp_data);
        } break;
        case LEVEL_MODE_OID: {
            temp_data=readUint16(&fr->data[4]);

            leverlerModeParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&leverlerModeParam);
            DoSetLeverlerMode(temp_data);
        } break;   
        case LEVEL_OPTHR_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            lvOpThresholdParam.value=fval;
            OGP_SetFloatParamRespone(dest,&lvOpThresholdParam);
            alc_param.op_threshold = (double) fval;
            audio_save_context=1;
        } break;
        case LEVEL_GATETHR_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            lvGateThresholdParam.value=fval;
            OGP_SetFloatParamRespone(dest,&lvGateThresholdParam);
            alc_param.gate_threshold = (double)fval;
            audio_save_context=1;
            //DoSetLvGateThreshold(fval);
        } break;
        case LEVEL_TRRANGE_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            lvTransientRangeParam.value=fval;
            OGP_SetFloatParamRespone(dest,&lvTransientRangeParam);
            alc_param.transient_range = (double)fval;
            audio_save_context=1;
            //DoSetLvTransientRange(fval);
        } break;
        case LEVEL_TRBYPASS_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            lvTransientBypassParam.value=fval;
            OGP_SetFloatParamRespone(dest,&lvTransientBypassParam);
            alc_param.transient_bypass_range = (double)fval;
            audio_save_context=1;
            //DoSetLvTransientBypass(fval);
        } break;
        case LEVEL_TRTIME_OID: {
            fval=readFloat(&fr->data[4]);
            lvTransientTimeParam.value=fval;
            OGP_SetFloatParamRespone(dest,&lvTransientTimeParam);
            alc_param.transient_time = (double)fval;
            audio_save_context=1;
            //DoSetLvTransientTime(temp_data);
        } break;
        case LEVEL_AGCRANGE_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            lvAgcRangeParam.value=fval;
            OGP_SetFloatParamRespone(dest,&lvAgcRangeParam);
            alc_param.agc_range = (double)fval;
            audio_save_context=1;
            //DoSetLvAgcRange(fval);
        } break;
        case LEVEL_AGCBPRANGE_OID: {
            fval=readFloat(&fr->data[4]);
            //printf("Set Float value: %f\r\n",fval);
            lvAgcBypassRangeParam.value=fval;
            OGP_SetFloatParamRespone(dest,&lvAgcBypassRangeParam);
            alc_param.agc_bypass_range = (double)fval;
            audio_save_context=1;
            //DoSetLvAgcBypassRange(fval);
        } break;
        case LEVEL_AGCTIME_OID: {
            temp_data=readUint16(&fr->data[4]);

            lvAgcTimeParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&lvAgcTimeParam);
            alc_param.agc_time = (int) temp_data;
            audio_save_context=1;
            //DoSetLvAgcTime(temp_data);
        } break;
        case LEVEL_AGCRMSTIME_OID: {
            temp_data=readUint16(&fr->data[4]);

            lvAgcRmsTimeParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&lvAgcRmsTimeParam);
            alc_param.agc_rms_time = (int)temp_data;
            audio_save_context=1;
            //DoSetLvAgcRmsTime(temp_data);
        } break;
        //Audio delay
        case DL_BYPASS_OID: {
            temp_data=readUint16(&fr->data[4]);

            dlBypassCheckboxParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&dlBypassCheckboxParam);
            DoSetDelayBypass(temp_data);
        } break;
        case DL_TIME_OID: {
            i32_data=readUint32(&fr->data[4]);
            dlTimeParam.value=i32_data;
            OGP_SetInt32ParamRespone(dest,&dlTimeParam);
            DoSetDelayTime(i32_data);
        } break;
        
        //Logo control
        case LOGO_CURIDX_OID: {
            temp_data=readUint16(&fr->data[4]);

            logoCurIdxParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&logoCurIdxParam);
            DoSetLogoIndex(temp_data);
        } break;
        case LOGO_CURPOSX_OID: {
            temp_data=readUint16(&fr->data[4]);

            logoCurPosXParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&logoCurPosXParam);
            DoSetLogoPosX(temp_data);
        } break;
        case LOGO_CURPOSY_OID: {
            temp_data=readUint16(&fr->data[4]);

            logoCurPosYParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&logoCurPosYParam);
            DoSetLogoPosY(temp_data);
        } break;
        case LOGO_ACTIVATE_OID: {
            temp_data=readUint16(&fr->data[4]);

            logoActivateParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&logoActivateParam);
            DoSetLogoActivate(temp_data);
        } break;
        //Admin tab
        case ADMIN_UNAME_OID: {
            strcpy(adminUserNameParam.value,&fr->data[4]);
            OGP_SetStringParamRespone(dest,oid,adminUserNameParam.value,sizeof(adminUserNameParam.value)+1);
        }break;
        case ADMIN_PASSWD_OID: {
            strcpy(adminUserPasswdParam.value,&fr->data[4]);
            OGP_SetStringParamRespone(dest,oid,adminUserPasswdParam.value,sizeof(adminUserPasswdParam.value)+1);
        }break;
        case ADMIN_LOGIN_OID: {
            temp_data=readUint16(&fr->data[4]);

            adminLoginButtonParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&adminLoginButtonParam);
            DoSetAdminLogin(temp_data);
        } break;
        case ADMIN_NETIP_OID: {
            i32_data=readUint32(&fr->data[4]);
            adminOldIpAddr = adminIpAddrParam.value;
            adminIpAddrParam.value=i32_data;
            OGP_SetInt32ParamRespone(dest,&adminIpAddrParam);
        } break;
        case ADMIN_NETCANCEL_OID: {
            temp_data=readUint16(&fr->data[4]);

            adminNetCancelButtonParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&adminNetCancelButtonParam);
            DoSetCancelNetConfig();
        } break;
        case ADMIN_NETAPPLY_OID: {
            temp_data=readUint16(&fr->data[4]);

            adminNetApplyButtonParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&adminNetApplyButtonParam);
            DoSetApplyNetConfig();
        } break;
        case ADMIN_NEWLIC_OID: {
            strcpy(adminLicNewParam.value,&fr->data[4]);
            OGP_SetStringParamRespone(dest,oid,adminLicNewParam.value,sizeof(adminLicNewParam.value)+1);
        }break;
        case ADMIN_LICUPDATE_OID: {
            temp_data=readUint16(&fr->data[4]);

            adminLicUpdateButtonParam.value=temp_data;
            OGP_SetInt16ParamRespone(dest,&adminLicUpdateButtonParam);
            DoSetNewLicenseUpdate();
        } break;
        
        default: {
#ifdef DEBUG
            printf("Set Other Param\r\n");
#endif
            DisplayRxMessage(fr);
        } break;	
    }						
    
}
//----------------------------------------------------------------------------------------
void OGP_ReportInt16Param(Int16Param* param)
{
BYTE buffer[6];
INT16 oid,data;
	oid=param->oid;
	data=param->value;
	buffer[0]=0x00;
    writeUint16(oid, &buffer[1]);
	buffer[3]=2;	//data size
    writeUint16(data, &buffer[4]);
#ifdef DEBUG_LEVEL0	
	printf("Report param for oid: 0x%04X\r\n",oid);
#endif
	OGP_SendData(OGP_ADDR_TRAP,OGP_REPORT_PARAM,buffer,6);
}
//----------------------------------------------------------------------------------------
void OGP_ReportInt16ArrayParam(Int16ArrayParam* param)
{
BYTE buffer[64];
INT16 oid,size,i,dlen;
INT16 * data;
size = param->datasize;
	oid=param->oid;
	data=param->value;
    dlen = 2*size + 4;
	buffer[0]=0x00;
    writeUint16(oid, &buffer[1]);
	buffer[3]=2*size;	//data size
    for(i=0;i<size;i++){
        writeUint16(data[i], &buffer[i*2+4]);
    }
#ifdef DEBUG_LEVEL0	
	printf("Report param for oid: 0x%04X\r\n",oid);
#endif
	OGP_SendData(OGP_ADDR_TRAP,OGP_REPORT_PARAM,buffer,dlen);
}
//----------------------------------------------------------------------------------------
void OGP_ReportInt32Param(Int32Param* param)
{
BYTE buffer[8];
UINT16 oid;
INT32 data;
	oid=param->oid;
	data=param->value;
	buffer[0]=0x00;
    writeUint16(oid,&buffer[1]);
	buffer[3]=4;	//data size
    writeUint32(data,&buffer[4]);
#ifdef DEBUG_LEVEL0	
	printf("Report param for oid: 0x%04X\r\n",oid);
#endif
	OGP_SendData(OGP_ADDR_TRAP,OGP_REPORT_PARAM,buffer,8);
}
//----------------------------------------------------------------------------------------
void OGP_ReportFloatParam(FloatParam* param)
{
BYTE buffer[8];
UINT16 oid;
float data;
	oid=param->oid;
	data=param->value;
	buffer[0]=0x00;
    writeUint16(oid,&buffer[1]);
	buffer[3]=4;	//data size
    writeFloat(data,&buffer[4]);
#ifdef DEBUG_LEVEL0	
	printf("Report param for oid: 0x%04X\r\n",oid);
#endif
	OGP_SendData(OGP_ADDR_TRAP,OGP_REPORT_PARAM,buffer,8);
}
//----------------------------------------------------------------------------------------
void OGP_ReportObjectParam(ObjectID * pOid)
{
    Int16Param * pInt16;
    StringParam * sparam;
    Int16ArrayParam * pInt16Arr;
    switch (*pOid)
    {
        case SDI_SIGNAL_FORMAT_OID: {
            pInt16 = &formatSignalParam;
            OGP_ReportInt16Param(pInt16);
        }break;
        case FRAMESYNC_STATUS_OID: {
            pInt16 = &frameSyncStatusParam;
            OGP_ReportInt16Param(pInt16);
        }break;
        case KEY_STATE_OID: {
            pInt16 = &keyStatusParam;
            OGP_ReportInt16Param(pInt16);
        }break;
        case AUDIO_PROC_OID: {
            pInt16 = &audioStatusParam;
            OGP_ReportInt16Param(pInt16);
        }break;
        case LOGO_PROC_OID: {
            pInt16 = &logoStatusParam;
            OGP_ReportInt16Param(pInt16);
        }break;
        default: {
            //printf("Not implement yet\r\n");
        }break;
    }
}
//----------------------------------------------------------------------------------------
#include <sys/time.h>
#include <time.h>
void OGP_CyclicReportParams(ReportObject * robjs,int size)
{
    struct timeval tv;
    static time_t sec_old=0, usec_old=0;
    time_t sec, usec;
    BOOL expired = FALSE;
    int i;
    
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec;
    usec = tv.tv_usec;
    
    
    //Check if time expired or not?
    if((sec != sec_old)||(abs(usec - usec_old) > 500000 ))
    {
        sec_old = sec;
        usec_old = usec;
        expired = TRUE;
    }
    if(expired)
    {
        CheckVariablesChanged();
        //Update Meter's value
        //Check if any parameters want to report
        for (i=0;i<size;i++)
        {
//            robjs[i].count++;
            if(++(robjs[i].count) >= robjs[i].interval) //It's time to report
            {
                robjs[i].count = 0;
                OGP_ReportObjectParam(&robjs[i].objectId);
            }
        }
        //SaveAudio if needed
        if(audio_save_context){
            SaveAudioParam();
            audio_save_context=0;
        }
        expired = FALSE;
    }
    usleep(100000);
}
//----------------------------------------------------------------------------------------
void OGP_SendTabParamDesc(unsigned char dest, StringParam* param)
{
    char buffer[OGP_MTU];
    BYTE namelen = strlen(param->name) + 1;
    BYTE clen = 0; 
    BYTE resplen = 13 + namelen + clen;
    UINT16 oid;

    oid  = param->oid;
    // fill in the response message
    buffer[0]  = OGP_OK;
    buffer[1]  = (BYTE)((oid>>8)&0xFF);
    buffer[2]  = (BYTE)(oid&0xFF);       	// oid
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = STRING_PARAM_TYPE;          // type
    buffer[6]  = 32;                         // nominal data size
    buffer[7]  = param->access;              // access
    buffer[8]  = PRECISION_NONE;             // precision
    buffer[9]  = WIDGET_PAGE_TAB;            // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);        // name
    buffer[11 + namelen] = NULL_CONSTRAINT;  // ctype
    buffer[12 + namelen] = clen;             // clen
    #ifdef DEBUG	
    printf("String Descriptor length: %d\r\n",buffer[3]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SendInt16ArrayElementValue(BYTE dest,UINT16 oid,UINT16 index)
{
    BYTE buffer[8];
    Int16ArrayParam* param;
    INT16 data;
    
    switch(oid)
    {
        case AUDIO_VOL_HEADER_OID: {
            param=&audioVolumeHeaderParam;
        } break;
        case AUDIO_VOL_DATA_OID: {
            param=&audioVolumeDataParam;
        } break;
        case AUDIO_VOL_MIXL_OID: {
            param=&audioVolumeMixLeftParam;
        } break;
        case AUDIO_VOL_MIXR_OID: {
            param=&audioVolumeMixRightParam;
        } break;
        //LEVELER
        //Logo
        
        
        default: {
            printf("SetInt16ArrayElementValue not implement yet\r\n");
            return;
        }break;
    }
    data=param->value[index];
    buffer[0]=OGP_OK;
    writeUint16(oid,&buffer[1]);
    writeUint16(index,&buffer[3]);
    buffer[5]=2;
    writeUint16(data,&buffer[6]);
    OGP_SendData(dest,OGP_GET_ARRAY_ELEMENT_RESPONE,buffer,8);
}
//----------------------------------------------------------------------------------------
void OGP_SendInt16ArrParamValue(unsigned char dest,Int16ArrayParam* param)
{
    BYTE buffer[OGP_MTU];
    UINT16 oid,i,size;
    INT16 * pdata;

    oid  = param->oid;
    size = 2*param->datasize;
    // fill in the response message
    buffer[0]  = OGP_OK;
    buffer[1]  = (BYTE)((oid>>8)&0xFF);
    buffer[2]  = (BYTE)(oid&0xFF);			//oid
    buffer[3]  = size;                       //data length
    for(i=0;i<param->datasize;i++){
       pdata = &param->value[i];
       buffer[4+i*2]  = (BYTE)((*pdata>>8)&0xFF);     //data
       buffer[5+i*2]  = (BYTE)(*pdata&0xFF);          //data
    }
    #ifdef DEBUG	
    printf("GetParam oid 0x%04X: ",oid);
    for(i=0;i<param->datasize;i++)
       printf("0x%04X ",param->value[i]);
    printf("\r\n");
    #endif
    OGP_SendData(dest,OGP_GET_PARAM_RESPONE,buffer,size+4);
}  
//----------------------------------------------------------------------------------------
void OGP_SendStrArrParamValue(unsigned char dest,StrArrayParam* param)
{
    BYTE buffer[OGP_MTU];
    UINT16 oid,i;
    char * pdata;
    oid  = param->oid;
    INT16 dlen,len;
    
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);       //oid
    dlen = 0;
    for(i=0;i<param->datasize;i++){
       pdata = param->value[i];
       len = strlen(pdata) + 1;
       strcpy(&buffer[4],pdata);
       dlen += len;
    }
    buffer[3]  = dlen;                       //data length

    #ifdef DEBUG	
    for(i=0;i<(dlen+4);i++)
        printf("%02X ",buffer[i]);
    printf("\r\n");
    #endif
    OGP_SendData(dest,OGP_GET_PARAM_RESPONE,buffer,dlen+4);
}  
//----------------------------------------------------------------------------------------
void OGP_SendInt16ArrParamDesc(unsigned char dest, Int16ArrayParam* param)
{
    int i;
    char buffer[OGP_MTU];
    BYTE clen;
    // length of the name (including terminator)
    BYTE namelen = strlen(param->name) + 1;
    // location where the constraint data is written
    BYTE cstart = 13 + namelen;
    // write the constraint data there and store its length
    clen = encodeIntArrConstraint(param, &buffer[cstart]);
    // total reponse length
    BYTE resplen = cstart + clen;
    UINT16 oid;

    oid  = param->oid;
    #ifdef DEBUG	
    printf("Clen=%d\r\n",clen);
    #endif
    // fill in the response message
    buffer[0]  = OGP_OK;
    buffer[1]  = (BYTE)((oid>>8)&0xFF);
    buffer[2]  = (BYTE)(oid&0xFF); 			//oid
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = INT16_ARRAY_TYPE;           // type
    buffer[6]  = 2*param->datasize;          // data size
    buffer[7]  = param->access;              // access
    buffer[8]  = PRECISION_NONE;             // precision
    buffer[9]  = param->widget;              // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);       	// name
    buffer[11 + namelen] = param->ctype;     // ctype
    buffer[12 + namelen] = clen;             // clen
    #ifdef DEBUG2	
    printf("String Descriptor lenght: %d\r\n",buffer[3]);
    for(i=0;i<resplen;i++)
        printf("D%d: %02x\r\n",i,buffer[i]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SendStrArrParamDesc(unsigned char dest, StrArrayParam* param)
{
    int i;
    char buffer[OGP_MTU];
    BYTE clen=0;
    // length of the name (including terminator)
    BYTE namelen = strlen(param->name) + 1;
    // location where the constraint data is written
    BYTE cstart = 13 + namelen;
    // write the constraint data there and store its length
//    clen = encodeIntArrConstraint(param, &buffer[cstart]);
    // total reponse length
    BYTE resplen = cstart + clen;
    UINT16 oid;

    oid  = param->oid;
    #ifdef DEBUG	
    printf("Clen=%d\r\n",clen);
    #endif
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid, &buffer[1]);           //oid
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = STRING_ARRAY_TYPE;           // type
    buffer[6]  = 255;          // data size
    buffer[7]  = param->access;              // access
    buffer[8]  = 64;             // precision
    buffer[9]  = param->widget;              // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);       	// name
    buffer[11 + namelen] = 0;     // ctype
    buffer[12 + namelen] = 0;             // clen
    #ifdef DEBUG2	
    printf("String Descriptor lenght: %d\r\n",buffer[3]);
    for(i=0;i<resplen;i++)
        printf("D%d: %02x\r\n",i,buffer[i]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SetArrayElementRespone(OgpShortFrame * pshort)
{
    BYTE buffer[8];
    BYTE dest=pshort->header.source;
    UINT16 oid=readUint16(&pshort->data[1]);
    UINT16 index = readUint16(&pshort->data[3]);
    INT16 data;
    switch(oid)
    {
        case AUDIO_VOL_DATA_OID: {
            data=readUint16(&pshort->data[6]);
            //printf("Volume=0x%04X\r\n",data);
            audioVolumeDataParam.value[index]=data;
            buffer[0]=OGP_OK;
            writeUint16(oid,&buffer[1]);
            writeUint16(index,&buffer[3]);
            buffer[5]=2;
            writeUint16(data,&buffer[6]);
            OGP_SendData(dest,OGP_SET_ARRAY_ELEMENT_RESPONE,buffer,8);
            DoSetAudioVolume(index,data);
        } break;
        case AUDIO_VOL_MIXL_OID: {
            data=readUint16(&pshort->data[6]);
            audioVolumeMixLeftParam.value[index]=data;
            buffer[0]=OGP_OK;
            writeUint16(oid,&buffer[1]);
            writeUint16(index,&buffer[3]);
            buffer[5]=2;
            writeUint16(data,&buffer[6]);
            OGP_SendData(dest,OGP_SET_ARRAY_ELEMENT_RESPONE,buffer,8);
            DoSetAudioMixer(index,data);
        } break;
        case AUDIO_VOL_MIXR_OID: {
            data=readUint16(&pshort->data[6]);
            audioVolumeMixRightParam.value[index]=data;
            buffer[0]=OGP_OK;
            writeUint16(oid,&buffer[1]);
            writeUint16(index,&buffer[3]);
            buffer[5]=2;
            writeUint16(data,&buffer[6]);
            OGP_SendData(dest,OGP_SET_ARRAY_ELEMENT_RESPONE,buffer,8);
            DoSetAudioMixer2(index,data);
        } break;
//Leverler

        default: {
            printf("SetArrayElement not implement yet\r\n");
        }break;
    }
}
//----------------------------------------------------------------------------------------
void OGP_StartProcessLongData(OgpLongFrame * asData)
{
    UINT16 oid;
    unsigned char dlen, dest;
    int i;
    //Examine the checksum
    if(getMessageCrc(&asData->data[0],asData->length,0)==asData->msgCrc){
        printf("Checksum correct\r\n");
        switch(asData->header.msgType)
        {
            case OGP_SET_PARAM: {
                oid = readUint16(&asData->data[1]);
                dlen = asData->data[3];
                switch (oid)
                {
                    case ADMIN_UNAME_OID : {
                        strcpy(adminUserNameParam.value, &asData->data[4]);
                        OGP_SetStringParamRespone(dest,oid,adminUserNameParam.value,dlen);
#ifdef DEBUG
                        printf("UserName: %s\r\n",adminUserNameParam.value);
#endif                        
                    }break;
                    case ADMIN_PASSWD_OID : {
                        strcpy(adminUserPasswdParam.value, &asData->data[4]);
                        OGP_SetStringParamRespone(dest,oid,adminUserPasswdParam.value,dlen);
#ifdef DEBUG
                        printf("UserPass: %s\r\n",adminUserPasswdParam.value);
#endif                        
                    }break;
                    case ADMIN_NEWLIC_OID : {
                        strcpy(adminLicNewParam.value, &asData->data[4]);
                        OGP_SetStringParamRespone(dest,oid,adminLicNewParam.value,dlen);
#ifdef DEBUG
                        printf("New license: %s\r\n",adminLicNewParam.value);
#endif                        
                    }break;
                }
                
            }break;
            default: {
                printf("LongData has not implement yet\r\n");
            }break;
        }
    }
    else{
        printf("Checksum failed\r\n");
    }
}
//----------------------------------------------------------------------------------------
void OGP_SendInt32ParamValue(unsigned char dest, Int32Param* param)
{
   BYTE buffer[8];
   UINT16 oid;
   UINT32 data;
   
   oid  = param->oid;
   data = param->value;
   // fill in the response message
   buffer[0]  = OGP_OK;
   writeUint16(oid,&buffer[1]);
   buffer[3]  = 4;                			//data length
   writeUint32(data,&buffer[4]);
#ifdef DEBUG	
   printf("GetParam oid 0x%04X: 0x%04X\r\n",oid,data);
#endif
   OGP_SendData(dest,OGP_GET_PARAM_RESPONE,buffer,8);
}  
//----------------------------------------------------------------------------------------
void OGP_SendInt32ParamDesc(unsigned char dest, Int32Param* param)
{
    int i;
    char buffer[OGP_MTU];
    BYTE clen;
    // length of the name (including terminator)
    BYTE namelen = strlen(param->name) + 1;
    // location where the constraint data is written
    BYTE cstart = 13 + namelen;
    // write the constraint data there and store its length
    clen = encodeInt32Constraint(param, &buffer[cstart]);
    // total reponse length
    BYTE resplen = cstart + clen;
    UINT16 oid;

    oid  = param->oid;
    #ifdef DEBUG	
    printf("Clen=%d\r\n",clen);
    #endif
    // fill in the response message
    buffer[0]  = OGP_OK;
    writeUint16(oid,&buffer[1]);
    buffer[3]  = resplen - 4;                // descriptor length
    buffer[4]  = 2;                          // version 2
    buffer[5]  = INT32_PARAM_TYPE;           // type
    buffer[6]  = INT32_PARAM_SIZE;           // data size
    buffer[7]  = param->access;              // access
    buffer[8]  = PRECISION_NONE;             // precision
    buffer[9]  = param->widget;              // widget
    buffer[10] = namelen;                    // name length
    strcpy(&buffer[11], param->name);       	// name
    buffer[11 + namelen] = param->ctype;     // ctype
    buffer[12 + namelen] = clen;             // clen
    #ifdef DEBUG2	
    printf("String Descriptor lenght: %d\r\n",buffer[3]);
    for(i=0;i<resplen;i++)
        printf("D%d: %02x\r\n",i,buffer[i]);
    #endif
    OGP_SendData(dest,OGP_GET_DESCRIPTOR_RESPONE,buffer,resplen);
}  
//----------------------------------------------------------------------------------------
void OGP_SetInt32ParamRespone(unsigned char dest, Int32Param* param)
{
   BYTE buffer[8];
   UINT16 oid;
   INT32 data;
   
   oid  = param->oid;
   data = param->value;
   // fill in the response message
   buffer[0]  = OGP_OK;
   writeUint16(oid,&buffer[1]);
   buffer[3]  = 4;                			//data length
   writeUint32(data,&buffer[4]);
#ifdef DEBUG	
   printf("SetParam oid 0x%04X: 0x%04X\r\n",oid,data);
#endif
   OGP_SendData(dest,OGP_SET_PARAM_RESPONE,buffer,8);
}  
//----------------------------------------------------------------------------------------
void ObjectEnable(ObjectID oid, unsigned char val)
{
    BYTE buffer[4];

    // fill in the response message
    buffer[0]  = OGP_PARAM_CHANGED;
    writeUint16(oid,&buffer[1]);
    buffer[3]  = val;                			//0=disable, 1=enable, 2=changed
    #ifdef DEBUG	
    printf("ParamChanged oid 0x%04X: %d\r\n",oid,val);
    #endif
    OGP_SendData(OGP_ADDR_TRAP,OGP_TRAP,buffer,4);
}
//----------------------------------------------------------------------------------------
void GroupEnable(ObjectID * objs, int size, unsigned char val)
{
    int i;
    for(i=0;i<size;i++)
        ObjectEnable(objs[i],val);
}
//----------------------------------------------------------------------------------------
void ChangeObjectDesc(ObjectID oid, char enable)
{
    switch(oid)
    {
        //FrameSync
        case FRAMESYNC_ENABLE_OID: {
            frameSyncEnableCheckboxParam.access = enable;
        }break;
        case ON_LOSS_INPUT_SIG_OID: {
            onLossInputSignalParam.access = enable;
        }break;
        case ON_LOSS_FRAMESYNC_OID: {
            onLossFrameSyncColorParam.access = enable;
        }break;
        //Key
        case KEY_ENABLE_OID: {
            keyEnableCheckboxParam.access = enable;
        }break;
        
        //Logo
        case LOGO_CURIDX_OID: {
            logoCurIdxParam.access = enable;
        }break;
        case LOGO_CURPOSX_OID: {
            logoCurPosXParam.access = enable;
        }break;
        case LOGO_CURPOSY_OID: {
            logoCurPosYParam.access = enable;
        }break;
        case LOGO_ACTIVATE_OID: {
            logoActivateParam.access = enable;
        }break;
        //Mixer
        case AUDIO_VOL_DATA_OID: {
            audioVolumeDataParam.access = enable;
        }break;
        case AUDIO_VOL_MIXL_OID: {
            audioVolumeMixLeftParam.access = enable;
        }break;
        case AUDIO_VOL_MIXR_OID: {
            audioVolumeMixRightParam.access = enable;
        }break;
        //Leveler
        case LEVEL_BYPASS_OID: {
            levelerBypassCheckboxParam.access = enable;
        }break;
        case LEVEL_MODE_OID: {
            leverlerModeParam.access = enable;
        }break;
        case LEVEL_OPTHR_OID: {
            lvOpThresholdParam.access = enable;
        }break;
        case LEVEL_GATETHR_OID: {
            lvGateThresholdParam.access = enable;
        }break;
        case LEVEL_TRRANGE_OID: {
            lvTransientRangeParam.access = enable;
        }break;
        case LEVEL_TRBYPASS_OID: {
            lvTransientBypassParam.access = enable;
        }break;
        case LEVEL_TRTIME_OID: {
            lvTransientTimeParam.access = enable;
        }break;
        case LEVEL_AGCRANGE_OID: {
            lvAgcRangeParam.access = enable;
        }break;
        case LEVEL_AGCBPRANGE_OID: {
            lvAgcBypassRangeParam.access = enable;
        }break;
        case LEVEL_AGCTIME_OID: {
            lvAgcTimeParam.access = enable;
        }break;
        case LEVEL_AGCRMSTIME_OID: {
            lvAgcRmsTimeParam.access = enable;
        }break;
        //Audio Delay
        case DL_BYPASS_OID: {
            dlBypassCheckboxParam.access = enable;
        }break;
        
    }
}
//----------------------------------------------------------------------------------------
void ChangeGroupDesc(ObjectID * objs, int size, unsigned char val)
{
    int i;
    for(i=0;i<size;i++)
        ChangeObjectDesc(objs[i],val);
}
//----------------------------------------------------------------------------------------
void OGP_ReportStringParam(StringParam* param)
{
BYTE buffer[OGP_MTU];
UINT16 oid,size;
char * data;
	oid=param->oid;
	data=param->value;
    size = strlen(data) + 1;
	buffer[0]=0x00;
    writeUint16(oid,&buffer[1]);
	buffer[3]=size;	//data size
    strcpy(&buffer[4],data);
#ifdef DEBUG	
	printf("Report param for oid: 0x%04X\r\n",oid);
#endif
	OGP_SendData(OGP_ADDR_TRAP,OGP_REPORT_PARAM,buffer,size + 4);
}
//----------------------------------------------------------------------------------------
//extern variables for checking
extern struct leveler_item leveler_param;
extern struct comp_item comp_param;
extern struct audio_mixer audio_mixer_param;
extern struct key_mixer   key_mixer_param;
extern struct reference reference_param;
extern struct frame_sync frame_sync_param;
extern unsigned short key_ref_status; // store external key, reference status update to FPGA

extern int no_logo;
extern int active_logo;
extern int live;
extern int normal;
extern int volume1;
extern int volume2;
extern int volume3;
extern int pan;
extern int volume_master;
extern int video_mode;
extern struct logo_item logo_list[];
extern int v_page;
extern struct  func_item function;
/*And others shared params
 extern double rms_ch12_in;
extern double peak_ch12_in;
extern double gain_ch12_out;

extern double rms_ch12_out;
extern double peak_ch12_out;
extern double peak_com_ch12_in;
extern double peak_com_ch12_out;

 */
//audio_mixer_param.pan:                  Ch3L|Ch3R|Ch2L|Ch2R|Ch1L|Ch1R
//CH1 left: 0x02
//CH1 right: 0x01
//Ch2 right: 
//----------------------------------------------------------------------------------------
void CheckVariablesChanged(void)
{
    if(setModeAlready == FALSE){
        EnableCtrlPanelMode(ctrlPanelIsAvailable);
        setModeAlready = TRUE;
    }
    if(leveler_param.bypass != levelerBypassCheckboxParam.value){   //Bypass changed
        levelerBypassCheckboxParam.value = leveler_param.bypass;
        OGP_ReportInt16Param(&levelerBypassCheckboxParam);
    }
    
    //Audio Mixer audio_mixer_param
//    extern Int16ArrayParam audioVolumeDataParam;
    short i,val;
    for(i=0;i<4;i++)
    {
        val = (short)round(audio_mixer_param.volume[i]);
        if(val != audioVolumeDataParam.value[i])
        {
            audioVolumeDataParam.value[i] = val;
            OGP_ReportInt16ArrayParam(&audioVolumeDataParam);
        }
    }
    //Int16ArrayParam audioVolumeMixRightParam
    //Int16ArrayParam audioVolumeMixLeftParam
    val = (short)(audio_mixer_param.pan & 0x01);    //Mixed Ch1Right
    if((val != 0) && (audioVolumeMixRightParam.value[0] == 0))
    {
        audioVolumeMixRightParam.value[0] = 1;
        OGP_ReportInt16ArrayParam(&audioVolumeMixRightParam);
    }
    else if((val == 0) && (audioVolumeMixRightParam.value[0] != 0))
    {
        audioVolumeMixRightParam.value[0] = 0;
        OGP_ReportInt16ArrayParam(&audioVolumeMixRightParam);
    }
    
    val = (short)(audio_mixer_param.pan & 0x02);    //Mixed Ch1Left
    if((val != 0) && (audioVolumeMixLeftParam.value[0] == 0))
    {
        audioVolumeMixLeftParam.value[0] = 1;
        OGP_ReportInt16ArrayParam(&audioVolumeMixLeftParam);
    }
    else if((val == 0) && (audioVolumeMixLeftParam.value[0] != 0))
    {
        audioVolumeMixLeftParam.value[0] = 0;
        OGP_ReportInt16ArrayParam(&audioVolumeMixLeftParam);
    }

    val = (short)(audio_mixer_param.pan & 0x04);    //Mixed Ch2Right
    if((val != 0) && (audioVolumeMixRightParam.value[1] == 0))
    {
        audioVolumeMixRightParam.value[1] = 1;
        OGP_ReportInt16ArrayParam(&audioVolumeMixRightParam);
    }
    else if((val == 0) && (audioVolumeMixRightParam.value[1] != 0))
    {
        audioVolumeMixRightParam.value[1] = 0;
        OGP_ReportInt16ArrayParam(&audioVolumeMixRightParam);
    }
    
    val = (short)(audio_mixer_param.pan & 0x08);    //Mixed Ch2Left
    if((val != 0) && (audioVolumeMixLeftParam.value[1] == 0))
    {
        audioVolumeMixLeftParam.value[1] = 1;
        OGP_ReportInt16ArrayParam(&audioVolumeMixLeftParam);
    }
    else if((val == 0) && (audioVolumeMixLeftParam.value[1] != 0))
    {
        audioVolumeMixLeftParam.value[1] = 0;
        OGP_ReportInt16ArrayParam(&audioVolumeMixLeftParam);
    }
    
    val = (short)(audio_mixer_param.pan & 0x10);    //Mixed Ch3Right
    if((val != 0) && (audioVolumeMixRightParam.value[2] == 0))
    {
        audioVolumeMixRightParam.value[2] = 1;
        OGP_ReportInt16ArrayParam(&audioVolumeMixRightParam);
    }
    else if((val == 0) && (audioVolumeMixRightParam.value[2] != 0))
    {
        audioVolumeMixRightParam.value[2] = 0;
        OGP_ReportInt16ArrayParam(&audioVolumeMixRightParam);
    }
    
    val = (short)(audio_mixer_param.pan & 0x20);    //Mixed Ch3Left
    if((val != 0) && (audioVolumeMixLeftParam.value[2] == 0))
    {
        audioVolumeMixLeftParam.value[2] = 1;
        OGP_ReportInt16ArrayParam(&audioVolumeMixLeftParam);
    }
    else if((val == 0) && (audioVolumeMixLeftParam.value[2] != 0))
    {
        audioVolumeMixLeftParam.value[2] = 0;
        OGP_ReportInt16ArrayParam(&audioVolumeMixLeftParam);
    }
    //Not implement
    //reference_param
    if(reference_param.mode != referenceParam.value){ //Reference changed
        referenceParam.value = reference_param.mode;
        
        OGP_ReportInt16Param(&referenceParam);
    }
    //frame_sync_param
    if(frame_sync_param.color != onLossFrameSyncColorParam.value){ //OnlossSignalColorChanged
        onLossFrameSyncColorParam.value = frame_sync_param.color;
        OGP_ReportInt16Param(&onLossFrameSyncColorParam);
    }
    if(frame_sync_param.enable){    //FrameSyncEnable changed
        
    }
    if(frame_sync_param.on_loss != onLossInputSignalParam.value){ //OnlossSignal Changed
        onLossInputSignalParam.value = frame_sync_param.on_loss;
        OGP_ReportInt16Param(&onLossInputSignalParam);
    }
    //no_logo,active_logo
    if(no_logo != logoTotalParam.value) {   //total of logo changed
        logoTotalParam.value = no_logo;
        OGP_ReportInt16Param(&logoTotalParam);
    }
    if(active_logo != logoActIdxParam.value){//Active logo changed
        OGP_UpdataActiveLogo(no_logo, active_logo, logo_list[active_logo].file_name);
    }
    //video mode
    if(video_mode != videoFormatParam.value){   //Input format changed
        videoFormatParam.value = video_mode;
        OGP_ReportInt16Param(&videoFormatParam);
    }
    //Check Leveler and Audio delay
    
    if(alc_param.mode != leverlerModeParam.value) {
        leverlerModeParam.value = (short)(alc_param.mode);        
        OGP_ReportInt16Param(&leverlerModeParam);    
        //LoadDefaultValues(alc_param.mode);
        DoSetLeverlerMode(alc_param.mode);
        
        
    }
    if(alc_param.enabled == levelerBypassCheckboxParam.value) {
        levelerBypassCheckboxParam.value = (short)(alc_param.enabled ^ 1);
        OGP_ReportInt16Param(&levelerBypassCheckboxParam);
    }
    if(alc_param.op_threshold != lvOpThresholdParam.value){
        lvOpThresholdParam.value = (float)alc_param.op_threshold;
        OGP_ReportFloatParam(&lvOpThresholdParam);
    }
    if(alc_param.gate_threshold != lvGateThresholdParam.value){
        lvGateThresholdParam.value = (float)alc_param.gate_threshold;
        OGP_ReportFloatParam(&lvGateThresholdParam);
    }
    if(alc_param.transient_range != lvTransientRangeParam.value){
        lvTransientRangeParam.value = (float) alc_param.transient_range;
        OGP_ReportFloatParam(&lvTransientRangeParam);
    }
    if(alc_param.transient_bypass_range != lvTransientBypassParam.value){
        lvTransientBypassParam.value = (float)alc_param.transient_bypass_range;
        OGP_ReportFloatParam(&lvTransientBypassParam);
    }
    if(alc_param.agc_range != lvAgcRangeParam.value){
        lvAgcRangeParam.value = (float)alc_param.agc_range;
        OGP_ReportFloatParam(&lvAgcRangeParam);
    }
    if(alc_param.agc_bypass_range != lvAgcBypassRangeParam.value){
        lvAgcBypassRangeParam.value = (float)alc_param.agc_bypass_range;
        OGP_ReportFloatParam(&lvAgcBypassRangeParam);
    }
    if(alc_param.transient_time != (double)lvTransientTimeParam.value){
        lvTransientTimeParam.value = (float)alc_param.transient_time;
        OGP_ReportFloatParam(&lvTransientTimeParam);
    }
    if(alc_param.agc_time != lvAgcTimeParam.value){
        lvAgcTimeParam.value = (short)alc_param.agc_time;
        OGP_ReportInt16Param(&lvAgcTimeParam);
    }
    if(alc_param.agc_rms_time != lvAgcRmsTimeParam.value){
        lvAgcRmsTimeParam.value = (short)alc_param.agc_rms_time;
        OGP_ReportInt16Param(&lvAgcRmsTimeParam);
    }
    if((alc_param.enabled == 0)||(leverlerModeParam.value == 3))
        GroupEnable(&audioLevelGroup[1],(N_AUDLEVEL_OBJS - 1),alc_param.enabled);
    else
        GroupEnable(&audioLevelGroup[1],1,alc_param.enabled);
    if(audio_delay_param.enabled == dlBypassCheckboxParam.value){
        dlBypassCheckboxParam.value = (short)(audio_delay_param.enabled ^ 1);
        OGP_ReportInt16Param(&dlBypassCheckboxParam);
    }
    if(audio_delay_param.delay != (unsigned long)dlTimeParam.value){
        dlTimeParam.value = (int)audio_delay_param.delay;
        OGP_ReportInt32Param(&dlTimeParam);
    }
    GroupEnable(&audioDelayGroup[1],(N_AUDDELAY_OBJS - 1),audio_delay_param.enabled);
}
//----------------------------------------------------------------------------------------
void ReportGroupParamsChanged(ObjectID * grp, int size)
{
    int i;
    for(i=0;i<size;i++){
        switch (grp[i]){
            case LEVEL_OPTHR_OID: {
                OGP_ReportFloatParam(&lvOpThresholdParam);
            }break;
            case LEVEL_GATETHR_OID:{
                OGP_ReportFloatParam(&lvGateThresholdParam);
            }break;
            case LEVEL_TRRANGE_OID:{
                OGP_ReportFloatParam(&lvTransientRangeParam);
            }break;
            case LEVEL_TRBYPASS_OID:{
                OGP_ReportFloatParam(&lvTransientBypassParam);
            }break;
            case LEVEL_AGCRANGE_OID:{
                OGP_ReportFloatParam(&lvAgcRangeParam);
            }break;
            case LEVEL_AGCBPRANGE_OID:{
                OGP_ReportFloatParam(&lvAgcBypassRangeParam);
            }break;
                
            case LEVEL_TRTIME_OID:{
                OGP_ReportFloatParam(&lvTransientTimeParam);
            }break;
            case LEVEL_AGCTIME_OID:{
                OGP_ReportInt16Param(&lvAgcTimeParam);
            }break;
            case LEVEL_AGCRMSTIME_OID:{
                OGP_ReportInt16Param(&lvAgcRmsTimeParam);
            }break;
                
        }
    }
}
//----------------------------------------------------------------------------------------
void LoadDefaultValues(short mode)
{
    lvOpThresholdParam.value = (float)lvDefaultModes[mode].op_threshold;
    lvGateThresholdParam.value = (float)lvDefaultModes[mode].gate_threshold;
    lvTransientRangeParam.value = (float)lvDefaultModes[mode].transient_range;
    lvTransientBypassParam.value = (float)lvDefaultModes[mode].transient_bypass_range;
    lvAgcRangeParam.value = (float)lvDefaultModes[mode].agc_range;
    lvAgcBypassRangeParam.value = (float)lvDefaultModes[mode].agc_bypass_range;
    
    lvTransientTimeParam.value = (float)lvDefaultModes[mode].transient_time;
    lvAgcTimeParam.value = (short)lvDefaultModes[mode].agc_time;
    lvAgcRmsTimeParam.value = (short)lvDefaultModes[mode].agc_rms_time;
    
    alc_param.agc_bypass_range = lvDefaultModes[mode].agc_bypass_range;
    alc_param.agc_range = lvDefaultModes[mode].agc_range;
    alc_param.agc_rms_time = lvDefaultModes[mode].agc_rms_time;
    alc_param.agc_time = lvDefaultModes[mode].agc_time;
    alc_param.gate_threshold = lvDefaultModes[mode].gate_threshold;
    alc_param.op_threshold = lvDefaultModes[mode].op_threshold;
    alc_param.transient_bypass_range = lvDefaultModes[mode].transient_bypass_range;
    alc_param.transient_range = lvDefaultModes[mode].transient_range;
    alc_param.transient_time = lvDefaultModes[mode].transient_time;
}

void UpdateUserDefineValue(void)
{
    alc_param.agc_bypass_range = lvAgcBypassRangeParam.value;
    alc_param.agc_range = lvAgcRangeParam.value;
    alc_param.agc_rms_time = lvAgcRmsTimeParam.value;
    alc_param.agc_time = lvAgcTimeParam.value;
    alc_param.gate_threshold = lvGateThresholdParam.value;
    alc_param.op_threshold = lvOpThresholdParam.value;
    alc_param.transient_bypass_range = lvTransientBypassParam.value;
    alc_param.transient_range = lvTransientRangeParam.value;
    alc_param.transient_time = lvTransientTimeParam.value;
}