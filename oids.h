/*******************************************************************************
 * FILENAME:
 *    oids.h
 ******************************************************************************/

#ifndef __OIDS_H
#define __OIDS_H

// Define total of Oids
#define OID_TABLE_SIZE      75//81//85//63/*53*/

// Define parameter Oids
#define DEVICE_NAME_OID   	0x0105
#define SDI_IN_REF_OID          0x0300
#define PHASE_PARAM_OID     0x0301
#define CLOCK_TIME_OID      0x0302
#define INPUT_STATUS_OID	0x0303	
#define SDI_IN_SIGNAL_OID	0x0304
#define SDI_SIGNAL_FORMAT_OID	0x0305
#define EQ_GAIN_SETTING_OID	0x0306

//OID for setup tab
#define REFERENCE_OID           0x0230

//OIDs for logo tab
#define LOGO_TOTAL_OID          0x0310
#define LOGO_ACTIDX_OID         0x0311
#define LOGO_ACTNAME_OID        0x0312
#define LOGO_CURIDX_OID         0x0313
#define LOGO_CURNAME_OID        0x0314
#define LOGO_CURPOSX_OID        0x0315
#define LOGO_CURPOSY_OID        0x0316
#define LOGO_ACTIVATE_OID       0x0317
#define LOGO_APPLY_OID          0x0318
#define LOGO_CANCEL_OID         0x0319
#define LOGO_STATITL_OID        0x031A
#define LOGO_CURTITL_OID        0x031B

//OIDs for key tab
#define KEY_ENABLE_OID          0x032C
#define KEY_CLIP_OID            0x0320
#define KEY_GAIN_OID            0x0321
#define KEY_MAKE_LINEAR_OID     0x0322
#define KEY_INVERT_OID          0x0323
#define KEY_TYPE_OID            0x0324
#define KEY_ALPHA_OID           0x0325
#define KEY_TRANSPARENT_OID     0x0326
#define KEY_BOXMASK_OID         0x0327
#define KEY_MAX_TOP_OID         0x0328
#define KEY_MAX_BOT_OID         0x0329
#define KEY_MAX_LEFT_OID        0x032A
#define KEY_MAX_RIGHT_OID       0x032B
#define KEY_STATE_OID           0x032D

//OIDs for Audio/Volume tab
#define AUDIO_VOL_TAB_OID       0x0330
#define AUDIO_VOL_HEADER_OID    0x0331
#define AUDIO_VOL_DATA_OID      0x0333
//#define AUDIO_VOL_MUTE_OID      0x0334
//#define AUDIO_VOL_MIX_OID       0x0335
#define AUDIO_VOL_MIXL_OID       0x0336
#define AUDIO_VOL_MIXR_OID       0x0337
#define AUDIO_CTR_VIA_PANEL         0x0338

//OIDs for Audio/Compressor
#define VIDEO_FORMAT_OID            0x0359
#define AUDIO_PROC_OID              0x035A
#define LOGO_PROC_OID               0x035B
//#define AUDIO_COMP2_THRESHOLD_OID   0x035C
//#define AUDIO_COMP2_RATIO_OID       0x035D
//OIDs for Audio/Leveler
#define LEVEL_TAB_OID           0x0360
#define LEVEL_BYPASS_OID        0x0361
#define LEVEL_MODE_OID          0x0362
#define LEVEL_OPTHR_OID         0x0363
#define LEVEL_GATETHR_OID       0x0364
#define LEVEL_TRRANGE_OID       0x0365
#define LEVEL_TRBYPASS_OID      0x0366
#define LEVEL_TRTIME_OID        0x0367
#define LEVEL_AGCRANGE_OID      0x0368
#define LEVEL_AGCBPRANGE_OID    0x0369
#define LEVEL_AGCTIME_OID       0x036A
#define LEVEL_AGCRMSTIME_OID    0x036B
//OIDs for Audio Delay
#define DL_TAB_OID              0x0370
#define DL_BYPASS_OID           0x0371
#define DL_TIME_OID             0x0372

//OIDs for FrameSync 
#define FRAMESYNC_STATUS_OID    0x0210
#define FRAMESYNC_ENABLE_OID    0x0211
#define VERTICAL_DELAY_OID      0x0212
#define HORIZONTAL_DELAY_OID    0x0213
#define AUDIO_OFFSET_OID        0x0214
#define ON_LOSS_INPUT_SIG_OID   0x0216
#define ON_LOSS_FRAMESYNC_OID   0x0217
#define FPGA_VERSION_OID        0x0218

//OIDs for Admin/Network tab
#define ADMIN_TITLE_OID         0x0220
#define ADMIN_UNAME_OID         0x0221
#define ADMIN_PASSWD_OID        0x0222
#define ADMIN_LOGIN_OID         0x0223
#define ADMIN_NETTITL_OID       0x0224
#define ADMIN_NETMAC_OID        0x0225
#define ADMIN_NETIP_OID         0x0226
#define ADMIN_NETAPPLY_OID      0x0227
#define ADMIN_NETCANCEL_OID     0x0228
#define ADMIN_LICTITL_OID       0x0229
#define ADMIN_NEWLIC_OID        0x022A
#define ADMIN_LICINFO_OID       0x022B
#define ADMIN_LICUPDATE_OID     0x022C

// Sub oids

// other reserved OIDs
#define SUPPLIER_NAME_OID    0x0102
#define SERIAL_NUMBER_OID    0x0106
#define SOFTWARE_REV_OID     0x010B
#define SMPTE_STATUS_OID     0x0201
#define CARD_MILLIAMP_OID    0x0205
#define EDIT_PERMISSION_OID  0x0601
#define NAME_OVERRIDE_OID    0xFF01
#define UPLOAD_URL_OID       0xFF02
// reserved 0xFF03 - 0xFFFF

#endif
