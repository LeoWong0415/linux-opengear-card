/* 
 * File:   ogp_ctrl.h
 * Author: sau
 *
 * Created on August 29, 2012, 11:21 AM
 */

#ifndef OGP_CTRL_H
#define	OGP_CTRL_H
#include "types1.h"


#define N_ADMIN_OBJS    5
#define N_FRAMESYNC_OBJS    3
#define N_KEY_OBJS    1
#define N_AUDMIX_OBJS    3
#define N_AUDCOMP_OBJS    6
#define N_AUDLEVEL_OBJS    11
#define N_AUDDELAY_OBJS     2
#define N_LOGO_OBJS    4

#define N_REPORT_ITEMS 3


#include "logo.h"

void EnableCtrlPanelMode(BOOL available);
void DoSetReference(int mode);
void DoSetVerticalDelay(int value);
void DoSetHorizontalDelay(float value);
void DoSetAudioOffset(float value);
void DoSetOnLossInputSignalOption(int option);
void DoSetOnLossFrameSyncOption(int option);
void DoSetOnLossFrameSyncOption(int option);
void DoSetInputVideoFormat(short data);

void DoSetKeyEnable(short enable);
void DoSetKeyClip(short data);
void DoSetKeyGain(short data);
void DoSetKeyMakeLine(short data);
void DoSetKeyInvert(short data);
void DoSetKeyType(short data);
void DoSetKeyAlphaType(short data);
void DoSetKeyTransparent(short data);
void DoSetKeyBoxMask(short data);
void DoSetKeyBoxMaskTop(short data);
void DoSetKeyBoxMaskBot(short data);
void DoSetKeyBoxMaskLeft(short data);
void DoSetKeyBoxMaskRight(short data);

void DoSetAudioVolume(INT16 index,INT16 data);
void DoSetAudioVolume2(INT16 index,INT16 data);
void DoSetAudioMixer(INT16 index,INT16 data);
void DoSetAudioMixer2(INT16 index,INT16 data);

void DoSetLevelerBypass(INT16 bypass);
void DoSetLeverlerMode(short data);

void DoSetDelayBypass(INT16 bypass);
void DoSetDelayTime(INT32 delay);

void DoSetLogoIndex(INT16 logoidx);
void DoSetLogoPosX(INT16 logoPosX);
void DoSetLogoPosY(INT16 logoPosY);
void DoSetLogoActivate(INT16 stat);

void DoSetAdminLogin(INT16 logged);
void DoSetCancelNetConfig(void);
int DoSetApplyNetConfig(void);
void DoSetNewLicenseUpdate(void);

void LoadAudioComponents(void);
void LoadVideoSetupComponents(void);

void OGP_UpdataActiveLogo(int n_logo, int active_logo,char *name);
void OGP_LoadLogoInfor(int index,struct logo_item * list);
void DoSetFrameSyncEnable(short enable);

void OGP_SetupByLicense(unsigned char license);

#endif	/* OGP_CTRL_H */

