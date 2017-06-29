

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "menus.h"
#include "oids.h"
#include "ogpdef.h"
#include "ogp.h"
#include "types1.h"

//===========================================================================================
//GLOBAL VARIABLES
//-------------------------------------------------------------------------------------------
ObjectID infoItems[4] = { DEVICE_NAME_OID,SUPPLIER_NAME_OID,
                        SOFTWARE_REV_OID, FPGA_VERSION_OID };
ObjectID signalItems[5] = {SDI_SIGNAL_FORMAT_OID, FRAMESYNC_STATUS_OID, KEY_STATE_OID,
    AUDIO_PROC_OID, LOGO_PROC_OID};
MenuPage infoPage[2]  = {
	{ "Card Info", infoItems, 4 },
	{ "Card Status", signalItems, 5}
};

ObjectID setupItems[6] = { 
        FRAMESYNC_ENABLE_OID, ON_LOSS_INPUT_SIG_OID, ON_LOSS_FRAMESYNC_OID,
        VERTICAL_DELAY_OID, HORIZONTAL_DELAY_OID, AUDIO_OFFSET_OID
};
// "Key Control" menu
ObjectID keyItems[13] = { KEY_ENABLE_OID, KEY_CLIP_OID, KEY_GAIN_OID, KEY_MAKE_LINEAR_OID, 
    KEY_INVERT_OID, KEY_TYPE_OID, KEY_ALPHA_OID, KEY_TRANSPARENT_OID, KEY_BOXMASK_OID, 
    KEY_MAX_TOP_OID, KEY_MAX_BOT_OID, KEY_MAX_LEFT_OID, KEY_MAX_RIGHT_OID
};
// "Audio" menu, contain 3 tab: Compressor,Mixer,Leveler
ObjectID audioItems[21] = { 
    //9
    AUDIO_VOL_TAB_OID,AUDIO_CTR_VIA_PANEL,AUDIO_VOL_HEADER_OID, AUDIO_VOL_DATA_OID,
    AUDIO_VOL_MIXL_OID,AUDIO_VOL_MIXR_OID,
    //Levelern 13
    LEVEL_TAB_OID, LEVEL_BYPASS_OID, LEVEL_MODE_OID, LEVEL_OPTHR_OID, LEVEL_GATETHR_OID, LEVEL_TRRANGE_OID,
    LEVEL_TRBYPASS_OID, LEVEL_TRTIME_OID, LEVEL_AGCRANGE_OID,LEVEL_AGCBPRANGE_OID, LEVEL_AGCTIME_OID,
    LEVEL_AGCRMSTIME_OID,
    //Delay subTab
    DL_TAB_OID, DL_BYPASS_OID, DL_TIME_OID
};
//Logos
ObjectID logoItems[10]= {
    LOGO_STATITL_OID, LOGO_TOTAL_OID, LOGO_ACTIDX_OID, LOGO_ACTNAME_OID,LOGO_CURTITL_OID, LOGO_CURIDX_OID, 
    LOGO_CURNAME_OID, LOGO_CURPOSX_OID, LOGO_CURPOSY_OID, LOGO_ACTIVATE_OID
};
//Setup tab
ObjectID setupInputItems[2]= {
    VIDEO_FORMAT_OID, REFERENCE_OID
};
//Admin tab
ObjectID adminItems[13]= {
    ADMIN_TITLE_OID, ADMIN_UNAME_OID, ADMIN_PASSWD_OID, ADMIN_LOGIN_OID, ADMIN_NETTITL_OID,
    ADMIN_NETMAC_OID, ADMIN_NETIP_OID, ADMIN_NETCANCEL_OID, ADMIN_NETAPPLY_OID, ADMIN_LICTITL_OID,
    ADMIN_LICINFO_OID, ADMIN_NEWLIC_OID, ADMIN_LICUPDATE_OID
};
//--------------------------------------------------------------------------------------------
MenuPage setupPage[6] = {  // { "Network Config", setupItems, 5},
                            { "FrameSync", setupItems, 6},
                            { "Key", keyItems, 13},
                            { "Audio", audioItems, 21},
                            { "LogoInsert", logoItems, 10},
                            { "Setup",setupInputItems, 2},
                            { "Administrator", adminItems, 13}
                        };
//
MenuGroup menuGroups[NUM_OF_MENU_GROUP] =
{
   { "Status", infoPage, 2 },
   { "Config", setupPage, 6 }
}; 


//===========================================================================================
//FUNCTIONS
//-------------------------------------------------------------------------------------------
void OGP_GetMenuSetNameRespone(unsigned char dest,unsigned char group)
{
BYTE buffer[64];
int dlen;
const char* name;
    if (group < NUM_OF_MENU_GROUP)
    {
         name = menuGroups[group].name;
         dlen = strlen(name) + 1;
         buffer[0] = OGP_OK;
         buffer[1] = group;
         buffer[2]=0;	//spare
         buffer[3] = dlen;
         strcpy(&buffer[4], name);
#ifdef DEBUG	
         printf("Reply to GetMenuSetName\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENUSET_NAME_RESPONE,buffer,(dlen+4));
    }
      // otherwise flag it as not found
    else
    {
         buffer[0] = OGP_NOTFOUND;
         buffer[1] = group;
         buffer[2]=0;	//spare
#ifdef DEBUG	
         printf("GetMenuSetName Group not found\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENUSET_NAME_RESPONE,buffer,3);
    }
}
//-------------------------------------------------------------------------------------------
void OGP_GetMenuCountRespone(unsigned char dest,unsigned char group)
{
BYTE buffer[3];
    if (group < NUM_OF_MENU_GROUP)
    {
         buffer[0] = OGP_OK;
         buffer[1] = group;
         buffer[2]=menuGroups[group].numMenus;
#ifdef DEBUG	
         printf("Reply to GetMenuCount\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_COUNT_RESPONE,buffer,3);
    }
      // otherwise flag it as not found
    else
    {
         buffer[0] = OGP_NOTFOUND;
         buffer[1] = group;
         buffer[2]=0;	//spare
#ifdef DEBUG	
         printf("GetMenuCount for Group not found\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_COUNT_RESPONE,buffer,2);
    }
}
//-------------------------------------------------------------------------------------------
void OGP_GetMenuNameRespone(unsigned char dest,unsigned char group,unsigned char menu)
{
BYTE buffer[64];
int dlen;
const char* name;
    if ((group < NUM_OF_MENU_GROUP)&&(menu < menuGroups[group].numMenus))
    {
         name = menuGroups[group].menus[menu].name;
         dlen = strlen(name) + 1;
         buffer[0] = OGP_OK;
         buffer[1] = group;
         buffer[2] = menu;
         buffer[3] = dlen;
         strcpy(&buffer[4], name);
#ifdef DEBUG	
         printf("Reply to GetMenuName\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_NAME_RESPONE,buffer,(dlen+4));
    }
      // otherwise flag it as not found
    else
    {
         buffer[0] = OGP_NOTFOUND;
         buffer[1] = group;
         buffer[2]=menu;	//spare
#ifdef DEBUG	
         printf("GetMenuName for Group not found\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_NAME_RESPONE,buffer,3);
    }
}
//-------------------------------------------------------------------------------------------
void OGP_GetMenuOidsRespone(unsigned char dest,unsigned char group,unsigned char menu)
{
BYTE buffer[OGP_MTU];
int i;
BYTE numItems;
UINT16 oid;
MenuPage *page;
    if ((group < NUM_OF_MENU_GROUP)&&(menu < menuGroups[group].numMenus))
    {
         page = &menuGroups[group].menus[menu];
         numItems = page->numItems;
         buffer[0] = OGP_OK;
         buffer[1] = group;
         buffer[2] = menu;
         buffer[3] = numItems;
		
		 for(i=0;i<numItems;i++){
			 oid = page->items[i];
             writeUint16(oid, &buffer[i*2 + 4]);
		 }
#ifdef DEBUG	
         printf("Reply to GetMenuOids\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_OIDS_RESPONE,buffer,(numItems*2+4));
    }
      // otherwise flag it as not found
    else
    {
         buffer[0] = OGP_NOTFOUND;
         buffer[1] = group;
         buffer[2]= menu;	//spare
#ifdef DEBUG	
         printf("GetMenuOids for Group not found\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_OIDS_RESPONE,buffer,3);
    }
}
//-------------------------------------------------------------------------------------------
void OGP_GetMenuStateRespone(unsigned char dest,unsigned char group,unsigned char menu)
{
BYTE buffer[4];
    if ((group < NUM_OF_MENU_GROUP)&&(menu < menuGroups[group].numMenus))
    {
         buffer[0] = OGP_OK;
         buffer[1] = group;
         buffer[2] = menu;
         buffer[3] = OGP_MENU_NORMAL;
#ifdef DEBUG	
         printf("Reply to GetMenuState\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_STATE_RESPONE,buffer,4);
    }
      // otherwise flag it as not found
    else
    {
         buffer[0] = OGP_NOTFOUND;
         buffer[1] = group;
         buffer[2]= menu;	//spare
#ifdef DEBUG	
         printf("GetMenuState for Group not found\r\n");
#endif
         OGP_SendData(dest,OGP_GET_MENU_STATE_RESPONE,buffer,3);
    }
}
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
