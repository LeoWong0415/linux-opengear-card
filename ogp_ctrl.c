#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oids.h"
#include "ogp_ctrl.h"
#include "parameters.h"
#include "utilities.h"

//hien
#include "control.h"
#include "logo.h"
//========================================================================================
//GLOBAL VARIABLES

ObjectID admGroup[N_ADMIN_OBJS]= {
    ADMIN_NETIP_OID, ADMIN_NETCANCEL_OID, ADMIN_NETAPPLY_OID, ADMIN_NEWLIC_OID, ADMIN_LICUPDATE_OID
};
ObjectID frameSyncGroup[N_FRAMESYNC_OBJS]= {
    FRAMESYNC_ENABLE_OID, ON_LOSS_INPUT_SIG_OID, ON_LOSS_FRAMESYNC_OID
};
ObjectID keyGroup[N_KEY_OBJS]= {
    KEY_ENABLE_OID
};
ObjectID audioMixerGroup[N_AUDMIX_OBJS]= {
    AUDIO_VOL_DATA_OID,AUDIO_VOL_MIXL_OID,AUDIO_VOL_MIXR_OID
};
ObjectID audioLevelGroup[N_AUDLEVEL_OBJS]= {
    LEVEL_BYPASS_OID, LEVEL_MODE_OID, LEVEL_OPTHR_OID, LEVEL_GATETHR_OID, LEVEL_TRRANGE_OID,
    LEVEL_TRBYPASS_OID, LEVEL_TRTIME_OID, LEVEL_AGCRANGE_OID, LEVEL_AGCBPRANGE_OID, 
    LEVEL_AGCTIME_OID, LEVEL_AGCRMSTIME_OID
};
ObjectID audioDelayGroup[N_AUDDELAY_OBJS]={
    DL_BYPASS_OID, DL_TIME_OID
};
ObjectID logoGroup[N_LOGO_OBJS]= {
    LOGO_CURIDX_OID, LOGO_CURPOSX_OID, LOGO_CURPOSY_OID, LOGO_ACTIVATE_OID
};

unsigned char audio_save_context=0;
//extern variables
extern StringParam adminUserNameParam;
extern StringParam adminUserPasswdParam;
extern Int16Param adminLoginButtonParam;
extern unsigned int adminOldIpAddr;
extern Int32Param adminIpAddrParam;
extern StringParam adminLicNewParam;
//leveler
extern Int16Param levelerBypassCheckboxParam;
extern Int16Param leverlerModeParam;
//mixer
extern Int16ArrayParam audioVolumeDataParam;
extern Int16ArrayParam audioVolumeMixLeftParam;
extern Int16ArrayParam audioVolumeMixRightParam;
//reference
extern Int16Param videoFormatParam;
//reference
extern Int16Param referenceParam;
//logo
extern Int16Param logoTotalParam;
extern Int16Param logoActIdxParam;
extern StringParam logoActNameParam;
extern Int16Param logoCurIdxParam;
extern StringParam logoCurNameParam;
extern Int16Param logoCurPosXParam;
extern Int16Param logoCurPosYParam;
extern Int16Param logoActivateParam;


extern Int16Param frameSyncEnableCheckboxParam;
extern Int16Param onLossInputSignalParam;
extern Int16Param onLossFrameSyncColorParam;
extern Int16Param ctrlPanelCheckboxParam;
    
extern Int16Param frameSyncStatusParam;
extern Int16Param keyStatusParam;
extern Int16Param audioStatusParam;
extern Int16Param logoStatusParam;

extern struct leveler_item leveler_param;
extern struct comp_item comp_param;
extern struct audio_mixer audio_mixer_param;
extern struct key_mixer   key_mixer_param;
extern struct reference reference_param;
extern struct frame_sync frame_sync_param;

extern int fd;


extern int video_mode;
extern int no_logo;
extern int active_logo;
extern struct logo_item logo_list[200];

extern struct agc_item alc_param;
extern DELAY_T audio_delay_param;
//========================================================================================
//FUNCTIONS
//----------------------------------------------------------------------------------------
void EnableCtrlPanelMode(BOOL available)
{
    if(available){
        ChangeGroupDesc(audioMixerGroup,N_AUDMIX_OBJS,0);
        GroupEnable(audioMixerGroup,N_AUDMIX_OBJS,0);
        ctrlPanelCheckboxParam.value = 1;
    }
    else{
        ChangeGroupDesc(audioMixerGroup,N_AUDMIX_OBJS,1);
        GroupEnable(audioMixerGroup,N_AUDMIX_OBJS,1);
        ctrlPanelCheckboxParam.value = 0;
    }
    OGP_ReportInt16Param(&ctrlPanelCheckboxParam);
}
void DoSetReference(int mode)
{
    SetReference(mode);
}
//----------------------------------------------------------------------------------------
void DoSetFrameSyncEnable(short enable)
{    
    frame_sync_param.enable = enable;
    FPGA_write(fd,21,frame_sync_param.enable);       
    audio_save_context=1;
}
//----------------------------------------------------------------------------------------
void DoSetVerticalDelay(int value)
{
    //hien
    //SetLineDelay(fd_spi,fd_map,value);
}
//----------------------------------------------------------------------------------------
void DoSetHorizontalDelay(float value)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetAudioOffset(float value)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetOnLossInputSignalOption(int option)
{
    frame_sync_param.on_loss = option;   
    FPGA_write(fd,19,frame_sync_param.on_loss);
    audio_save_context=1;
}
//----------------------------------------------------------------------------------------
void DoSetOnLossFrameSyncOption(int option)
{
    frame_sync_param.color = option;
    FPGA_write(fd,20,frame_sync_param.color);
    audio_save_context=1;
}
//----------------------------------------------------------------------------------------
void DoSetInputVideoFormat(short data)
{
    ChangeVideoInputMode(data);
    Update_Logo_Infor();
}
//----------------------------------------------------------------------------------------
void DoSetKeyEnable(short enable)
{
    SetKeyEnable(enable);
}
//----------------------------------------------------------------------------------------
void DoSetKeyClip(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyGain(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyMakeLine(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyInvert(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyType(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyAlphaType(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyTransparent(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyBoxMask(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyBoxMaskTop(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyBoxMaskBot(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyBoxMaskLeft(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetKeyBoxMaskRight(short data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetAudioVolume(INT16 index,INT16 data)
{
    if(data <= -80) data = -79;
    else if(data >= 40) data = 39;
    audio_mixer_param.volume[index]=data;                    
    ChangeAudioMixer(index,fd);
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetAudioVolume2(INT16 index,INT16 data)
{
    
}
//----------------------------------------------------------------------------------------
void DoSetAudioMixer(INT16 index,INT16 data)
{
    switch(index)
    {
        case 0: {    //CH1 left
            if(data){
                audio_mixer_param.pan |= 0x2;
            }
            else{
                audio_mixer_param.pan &= 0xFD;
            }
        }break;
        case 1: {    //CH2 Left
            if(data){
                audio_mixer_param.pan |= 0x8;
            }
            else{
                audio_mixer_param.pan &= 0x7;
            }
        }break;
        case 2: {    //CH3 Left
            if(data){
                audio_mixer_param.pan |= 0x20;
            }
            else{
                audio_mixer_param.pan &= 0xDF;
            }
        }break;
    }
    ChangeAudioMixer(10,fd);
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetAudioMixer2(INT16 index,INT16 data)
{
    switch(index)
    {
        case 0: {    //CH1 Right
            if(data){
                audio_mixer_param.pan |= 0x1;
            }
            else{
                audio_mixer_param.pan &= 0xFE;
            }
        }break;
        case 1: {    //CH2 Right
            if(data){
                audio_mixer_param.pan |= 0x4;
            }
            else{
                audio_mixer_param.pan &= 0xFB;
            }
        }break;
        case 2: {    //CH3 Right
            if(data){
                audio_mixer_param.pan |= 0x10;
            }
            else{
                audio_mixer_param.pan &= 0xEF;
            }
        }break;
    }
    ChangeAudioMixer(10,fd);
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void DoSetLevelerBypass(INT16 bypass)
{
    if(bypass){
        GroupEnable(&audioLevelGroup[1],(N_AUDLEVEL_OBJS - 1),0);
        alc_param.enabled = 0;
    }
    else{
        alc_param.enabled = 1;
        if(leverlerModeParam.value == 3){
            GroupEnable(&audioLevelGroup[1],(N_AUDLEVEL_OBJS - 1),1);
        }
        else {
            GroupEnable(&audioLevelGroup[1],1,1);
        }
    }
    leveler_param.bypass = bypass;
//    SaveAudioParam();
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetLeverlerMode(short data)
{
    //Disable access right if necessary
    if(data == 3){  //User define
        GroupEnable(&audioLevelGroup[2],(N_AUDLEVEL_OBJS - 2),1);
        UpdateUserDefineValue();
    }
    else{ //Other modes: TV/TV Live/Movie
        GroupEnable(&audioLevelGroup[2],(N_AUDLEVEL_OBJS - 2),0);
        LoadDefaultValues(data);
        ReportGroupParamsChanged(&audioLevelGroup[2],(N_AUDLEVEL_OBJS-2));
    }
    alc_param.mode= data;
    //Update related parameters, remember to report them to OpenGear
    audio_save_context=1;
}

//----------------------------------------------------------------------------------------
void DoSetLevelerLowThreshold(INT16 lThreshold)
{
    leveler_param.leveler_low_threshold = lThreshold;
//    SaveAudioParam();
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetLevelerBGndThreshold(INT16 gndThreshold)
{
    leveler_param.leveler_background_threshold = gndThreshold;
//    SaveAudioParam();
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetLevelerRmsTime(INT16 rmsTime)
{
    leveler_param.leveler_rms_time = rmsTime;
//    SaveAudioParam();
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetLevelerAttackTime(INT16 attackTime)
{
    leveler_param.leveler_attack_time = attackTime;
//    SaveAudioParam();
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetLevelerReleaseTime(INT16 releaseTime)
{
    leveler_param.leveler_release_time = releaseTime;
//    SaveAudioParam();
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetLevelerMaxGain(INT16 maxGain)
{
    leveler_param.leveler_maximum_gain = maxGain;
//    SaveAudioParam();
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
//LOGO
//----------------------------------------------------------------------------------------
void DoSetLogoIndex(INT16 logoidx)
{
    if(logoidx < no_logo)
        OGP_LoadLogoInfor(logoidx, logo_list);
    else {
        logoCurIdxParam.value = (no_logo -1);
        OGP_ReportInt16Param(&logoCurIdxParam);
    }
}
//----------------------------------------------------------------------------------------
void DoSetDelayTime(INT32 delay)
{
    audio_delay_param.delay = delay;
    Change_Delay_Audio(fd,audio_delay_param.delay); // do tre tinh bang milisecond    
    
    audio_save_context = 1;
}
//----------------------------------------------------------------------------------------
void DoSetLogoPosX(INT16 logoPosX)
{
    int curMaxWidth;
    if(videoFormatParam.value==0){  //HD 1080i format
        curMaxWidth = 1920;
    }
    else{   // SD576i format
        curMaxWidth = 720;
    }
    if(logoPosX >= (curMaxWidth - logo_list[logoCurIdxParam.value].width - 1)){
//        printf("MaxWidth: %d\r\n",curMaxWidth);
//        printf("Logo[%d].x=%d\r\n",logoCurIdxParam.value,logo_list[logoCurIdxParam.value].x);
        logo_list[logoCurIdxParam.value].x = curMaxWidth - logo_list[logoCurIdxParam.value].width - 1;
        logoCurPosXParam.value = logo_list[logoCurIdxParam.value].x;
        OGP_ReportInt16Param(&logoCurPosXParam);
    }
    else {
        logo_list[logoCurIdxParam.value].x = logoPosX;
        Update_Logo_Infor();
        if(logoCurIdxParam.value==active_logo){
            ActivateLogo(active_logo);
        }
    }
}
//----------------------------------------------------------------------------------------
void DoSetLogoPosY(INT16 logoPosY)
{
    int curMaxHeight;
    if(videoFormatParam.value==0){  //HD 1080i format
        curMaxHeight = 1080;
    }
    else{   // SD576i format
        curMaxHeight = 576;
    }
    if(logoPosY >= (curMaxHeight - logo_list[logoCurIdxParam.value].height)){
        logo_list[logoCurIdxParam.value].y = curMaxHeight - logo_list[logoCurIdxParam.value].height - 1;
        logoCurPosYParam.value = logo_list[logoCurIdxParam.value].y;
        OGP_ReportInt16Param(&logoCurPosYParam);
    }
    else {
        logo_list[logoCurIdxParam.value].y = logoPosY;
        Update_Logo_Infor();
        if(logoCurIdxParam.value==active_logo){
            ActivateLogo(active_logo);
        }
    }
}
//----------------------------------------------------------------------------------------
//Int16Param videoFormatParam
void DoSetLogoActivate(INT16 stat)
{
    int curMaxWidth,curMaxHeight;
    if(videoFormatParam.value==0){  //HD 1080i format
        curMaxWidth = 1920;
        curMaxHeight = 1080;
    }
    else{   // SD576i format
        curMaxWidth = 720;
        curMaxHeight = 576;
    }
    if(stat){
        active_logo = logoCurIdxParam.value;
        if(logo_list[active_logo].x >= (curMaxWidth-logo_list[active_logo].width)){ //Check if Logo is out of size
           logo_list[active_logo].x = curMaxWidth-logo_list[active_logo].width - 1;
           logoCurPosXParam.value = logo_list[active_logo].x;
           OGP_ReportInt16Param(&logoCurPosXParam);
        }
        if(logo_list[active_logo].y >= (curMaxHeight-logo_list[active_logo].height)){
            logo_list[active_logo].y = curMaxHeight-logo_list[active_logo].height - 1;
            logoCurPosYParam.value = logo_list[active_logo].y;
            OGP_ReportInt16Param(&logoCurPosYParam);
        }
        ActivateLogo(logoCurIdxParam.value);
        OGP_UpdataActiveLogo(no_logo, active_logo,logo_list[active_logo].file_name);
    }
    else {
        DeActivateLogo();
        OGP_UpdataActiveLogo(no_logo, active_logo,"");
    }
    Update_Logo_Infor();
}

//----------------------------------------------------------------------------------------
void DoSetDelayBypass(INT16 bypass)
{
    if(bypass){
        GroupEnable(&audioDelayGroup[1],(N_AUDDELAY_OBJS - 1),0);
        audio_delay_param.enabled = 0;
    }
    else{
            GroupEnable(&audioDelayGroup[1],(N_AUDDELAY_OBJS - 1),1);
            audio_delay_param.enabled = 1;
    }
    //leveler_param.bypass = bypass;
//    SaveAudioParam();
    FPGA_write(fd,37,audio_delay_param.enabled);       // enable =1 cho phep chay delay, =0 bo qua delay    
    audio_save_context = 1;
}



//----------------------------------------------------------------------------------------
void DoSetAdminLogin(INT16 logged)
{
    unsigned char state=1;
    if(logged == 1){
        if((strcmp(adminUserNameParam.value, "admin")) ||
            (strcmp(adminUserPasswdParam.value, "manager"))) {
            state = 0;
            adminLoginButtonParam.value = 0;
            OGP_ReportInt16Param(&adminLoginButtonParam);
        }
    }
    else{
        state = 0;
        strcpy(adminUserNameParam.value, "");
        strcpy(adminUserPasswdParam.value,"");
        strcpy(adminLicNewParam.value,"");
        OGP_ReportStringParam(&adminUserNameParam);
        OGP_ReportStringParam(&adminUserPasswdParam);
        OGP_ReportStringParam(&adminLicNewParam);
    }
    GroupEnable(admGroup,N_ADMIN_OBJS,state);
}
//----------------------------------------------------------------------------------------
void DoSetCancelNetConfig(void)
{
    adminIpAddrParam.value = adminOldIpAddr;
    OGP_ReportInt32Param(&adminIpAddrParam);
}
//----------------------------------------------------------------------------------------
int DoSetApplyNetConfig(void)
{
    int time_count = 10;
    unsigned char newip[4];
    FILE * fp;
    //Create a backup file from /etc/network/interfaces to /mnt/etc/network/interfaces.bak
    system("cp /etc/network/interfaces /mnt/etc/network/interfaces.bak");
    //Create a new interfaces file
	if ((fp = (FILE *)(fopen( "/mnt/etc/network/interfaces","w+"))) == NULL)
	{
		printf("Cannot write to interfaces!\r\n");
		return -1;
	}
	else{
        itob(adminIpAddrParam.value,newip);
		printf("Writing into interfaces file...\r\n");
		fprintf(fp,"# Config Loopback\n");
		fprintf(fp,"auto lo\n");
		fprintf(fp,"iface lo inet loopback\n");
		fprintf(fp,"# Config eth0 for IPv4\n");
		fprintf(fp,"auto eth0\n");
		fprintf(fp,"iface eth0 inet static\n");
		fprintf(fp,"\taddress %d.%d.%d.%d\n",newip[0],newip[1],newip[2],newip[3]);
		fprintf(fp,"\tnetmask 255.255.255.0\n");
		fprintf(fp,"\tnetwork %d.%d.%d.1\n",newip[0],newip[1],newip[2]);
		fprintf(fp,"\tbroadcast %d.%d.%d.255\n",newip[0],newip[1],newip[2]);
		fclose(fp);
        //Wait for write to complete
        while(time_count > 0){
            printf("Wait for writing complete: %d\r\n",time_count);
            sleep(1);
            time_count--;
        }
        system("chmod 755 /mnt/etc/network/interfaces");
        printf("\r\nDone, system reboot now\r\n");
        sleep(1);
        //Done! reboot to take effect
        system("reboot");
	}
    return 1;
}
//----------------------------------------------------------------------------------------
extern char my_mac[6];
unsigned char CheckValidLicense(char* lic)
{
    unsigned char retval=0;
    char buf[7];
    char card_option[7];
    buf[6]=0;
    card_option[6]=0;
    if(strlen(lic)<12) return 0;
    GetOptionFromString(lic,buf,6);
    Crypt(my_mac,buf,card_option,6);
    printf("Card Options: %s\r\n",card_option);
    if(card_option[0]=='F') retval |= FRAME_SYNC;
    else if(card_option[0]!='_') return 0;
    if(card_option[1]=='L') retval |= LOGOINSERT;
    else if(card_option[1]!='_') return 0;
    if(card_option[2]=='K') retval |= KEY;
    else if(card_option[2]!='_') return 0;
    if(card_option[3]=='M') retval |= AUDIOMIXER;
    else if(card_option[3]!='_') return 0;
    if(card_option[4]=='C') retval |= AUDIOCOMP;
    else if(card_option[4]!='_') return 0;
    if(card_option[5]=='L') retval |= AUDIOLEVEL;
    else if(card_option[5]!='_') return 0;
    
    return retval;
}
//----------------------------------------------------------------------------------------
//extern StringParam adminLicNewParam
extern StringParam adminLicInfoParam;
void DoSetNewLicenseUpdate(void)
{
    unsigned char lic;
    lic=CheckValidLicense(adminLicNewParam.value);
    if(lic != 0){
        printf("License accepted. Wait for apply and reboot for a while\r\n");
        strcpy(adminLicInfoParam.value, "License accept. Wait for reboot");
        OGP_ReportStringParam(&adminLicInfoParam);
        UpdateLicense(adminLicNewParam.value);
        system("reboot");
    }
    else {
        printf("License invalid\r\n");
        strcpy(adminLicInfoParam.value, "License invalid");
        OGP_ReportStringParam(&adminLicInfoParam);
    }
}
//----------------------------------------------------------------------------------------
/*
extern struct leveler_item leveler_param;
extern struct comp_item comp_param;
extern struct audio_mixer audio_mixer_param;
extern struct key_mixer   key_mixer_param;
extern struct reference reference_param;
extern struct frame_sync frame_sync_param;
*/
void LoadAudioComponents(void)
{
    
    
    frameSyncEnableCheckboxParam.value = frame_sync_param.enable;
    onLossInputSignalParam.value = frame_sync_param.on_loss;
    onLossFrameSyncColorParam.value = frame_sync_param.color;
    
    OGP_ReportInt16Param(&frameSyncEnableCheckboxParam);
    OGP_ReportInt16Param(&onLossInputSignalParam);
    OGP_ReportInt16Param(&onLossFrameSyncColorParam);
    
    levelerBypassCheckboxParam.value = leveler_param.bypass;
    
    
    audioVolumeDataParam.value[0]=audio_mixer_param.volume[0];
    audioVolumeDataParam.value[1]=audio_mixer_param.volume[1];
    audioVolumeDataParam.value[2]=audio_mixer_param.volume[2];
    audioVolumeDataParam.value[3]=audio_mixer_param.volume[3];
    
    OGP_ReportInt16Param(&levelerBypassCheckboxParam);

    OGP_ReportInt16ArrayParam(&audioVolumeDataParam);
}
//----------------------------------------------------------------------------------------
/*
 extern Int16Param videoFormatParam;
//framesync
extern Int16Param frameSyncEnableParam;

 */
void LoadVideoSetupComponents(void)
{
    videoFormatParam.value = video_mode;
    referenceParam.value = reference_param.mode;
    DoSetInputVideoFormat(video_mode);
    OGP_ReportInt16Param(&videoFormatParam);
    OGP_ReportInt16Param(&referenceParam);
}
//----------------------------------------------------------------------------------------
void OGP_UpdataActiveLogo(int n_logo, int active_logo,char *name)
{
    logoTotalParam.value = n_logo;
    logoActIdxParam.value = active_logo;
    strcpy(logoActNameParam.value,name);
    if(active_logo == logoCurIdxParam.value){
        logoActivateParam.value = 1;
        OGP_ReportInt16Param(&logoActivateParam);
    }
    else {
        logoActivateParam.value = 0;
        OGP_ReportInt16Param(&logoActivateParam);
    }
    OGP_ReportInt16Param(&logoTotalParam);
    OGP_ReportInt16Param(&logoActIdxParam);
    OGP_ReportStringParam(&logoActNameParam);
}
//----------------------------------------------------------------------------------------
void OGP_LoadLogoInfor(int index, struct logo_item * list)
{
    strcpy(logoCurNameParam.value, list[index].file_name);
    logoCurPosXParam.value = list[index].x;
    logoCurPosYParam.value = list[index].y;
    if(index == active_logo){
        logoActivateParam.value = 1;
    }
    else
        logoActivateParam.value = 0;
    OGP_ReportStringParam(&logoCurNameParam);
    OGP_ReportInt16Param(&logoCurPosXParam);
    OGP_ReportInt16Param(&logoCurPosYParam);
    OGP_ReportInt16Param(&logoActivateParam);
}
//------------------------------------------------------------------------------------------
/*
 #define FRAME_SYNC  0x1     //bit 0
#define LOGOINSERT  0x2     //bit 1
#define KEY         0x4     //bit 2
#define AUDIOMIXER  0x8     //BIT3
#define AUDIOCOMP   0x10    //BIT4
#define AUDIOLEVEL  0x20    //BIT5

 */
void OGP_SetupByLicense(unsigned char license)
{
    char frSync_en=0, logo_en=0, key_en=0, comp_en=0, level_en=0;
    char mix_en=0;
    if(license & FRAME_SYNC){
        frSync_en = 1;
        frameSyncStatusParam.value = 1;
        keyStatusParam.value = 2;
        audioStatusParam.value = 2;
        logoStatusParam.value = 2;
    }
    else{
        frameSyncStatusParam.value = 2;
        if(license & LOGOINSERT){ logo_en = 1; logoStatusParam.value = 1;}
        if(license & KEY){ key_en = 1; keyStatusParam.value = 1;}
        if(license & AUDIOCOMP){ comp_en = 1;audioStatusParam.value = 1;}
        if(license & AUDIOLEVEL){ level_en =1; audioStatusParam.value = 1;}
    }

    if(frSync_en){
        ChangeGroupDesc(frameSyncGroup,N_FRAMESYNC_OBJS,1);
        ChangeGroupDesc(keyGroup,N_KEY_OBJS,0);
        ChangeGroupDesc(audioMixerGroup,N_AUDMIX_OBJS,0);
        ChangeGroupDesc(audioLevelGroup,N_AUDLEVEL_OBJS,0);
        ChangeGroupDesc(logoGroup,N_LOGO_OBJS,0);
    }
    else {
        ChangeGroupDesc(frameSyncGroup,N_FRAMESYNC_OBJS,0);
        if(logo_en) ChangeGroupDesc(logoGroup,N_LOGO_OBJS,1);
        else ChangeGroupDesc(logoGroup,N_LOGO_OBJS,0);
        if(key_en)ChangeGroupDesc(keyGroup,N_KEY_OBJS,1);
        else ChangeGroupDesc(keyGroup,N_KEY_OBJS,0);
        if(level_en){
            ChangeGroupDesc(audioMixerGroup,N_AUDMIX_OBJS,1);
            if(leverlerModeParam.value == 3){
                ChangeGroupDesc(audioLevelGroup,N_AUDLEVEL_OBJS,1);
            }
            else{
                ChangeGroupDesc(audioLevelGroup,2,1);
                ChangeGroupDesc(&audioLevelGroup[2],(N_AUDLEVEL_OBJS-2),0);
            } 
            mix_en=1;
        }
        else
            ChangeGroupDesc(audioLevelGroup,N_AUDLEVEL_OBJS,0);
        if(mix_en==0){
            ChangeGroupDesc(audioMixerGroup,N_AUDMIX_OBJS,0);
        }
        
    }
}
