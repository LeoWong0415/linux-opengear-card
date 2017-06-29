/* 
 * File:   control.h
 * Author: vohien
 *
 * Created on March 16, 2012, 11:36 AM
 */

#ifndef CONTROL_H
#define CONTROL_H


//clock_t start, stop;
//assert((start = clock())!=-1);
/* Stop timer */
//stop = clock();
//t = (double) (stop-start)/CLOCKS_PER_SEC;

#define ANPHA_LED       4
#define VIDEO_LED       3
#define BKGD_LED        2
#define REF_LED         1
#define CAN_LED         5

struct  func_item {
    unsigned char logo_insert;
    unsigned char external_key;
    unsigned char frame_sync;
    unsigned char audio_leveler;
    unsigned char audio_comp;
    unsigned char audio_delay;
};  

struct agc_item{
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

struct auto_mixer{
    double      check_threshold;
    int         is_copy12;
    double      ch34_level;
    int         is_auto;
};
struct audio_delay_item{
    int enabled;                // cho phep hoac khong cho phep delay audio, 0=khong dung, 1 = co dung audio_delay
    unsigned long delay; // tu 0 - >40000 milisecond
};
typedef struct audio_delay_item DELAY_T;

struct leveler_item{         
int leveler_low_threshold;
int leveler_background_threshold;
int leveler_maximum_gain;
long leveler_rms_time;
int leveler_attack_time;
int leveler_release_time;

int bypass;
int input_gain;
};
struct audio_mixer{
    double         volume[10];      // ch1,ch2,ch3,master volume
    unsigned int        pan;    // pan information    
    unsigned int        pan_mix2;    // pan information    
};

struct frame_sync{
    unsigned char mode;
    unsigned char enable;
    unsigned char color;
    unsigned char on_loss;
};

struct  reference{
    unsigned char mode;
};
struct key_mixer{
    unsigned char enable;
};
struct comp_item{
    int bypass;
    int threshold;
    int ratio;
    int attack_time;
    int release_time;
    int input_gain;
};

void parse_string(char *strin,char (*str_array)[200],int *length);    
void Send_Function(int sock,char *cmd_arg);    
void Send_LeverMeterIn(int sock,char *cmd_arg);
void Send_LeverGain(int sock,char *cmd_arg);
void Setup_Leveler(int sock,char *cmd_index,char *cmd_param);
void Report_Setup_Leveler(int sock,char *cmd_index);
void Send_LeverMeterOut(int sock,char *cmd_arg);
void print_hex_string(char *strin,int len);
void Setup_Audio_Mixer_Startup(int fd);
void Report_Reference(int sock,char *cmd_index);
void Setup_Reference(int sock,char *cmd_index,char *cmd_param,int fd);
void Report_Key_Mixer(int sock,char *cmd_index);
void Setup_Key_Mixer(int sock,char *cmd_index,char *cmd_param,int fd);
void Set_Key_Mixer_Startup(int fd);
void Report_Frame_Sync(int sock,char *cmd_index);
void Setup_Frame_Sync(int sock,char *cmd_index,char *cmd_param,int fd);
void Setup_AGC(int sock,char *cmd_index,char *cmd_param);
void Report_ALC(int sock,char *cmd_index);
void ChangeAudioMixer(int index,int fd);
void Report_Audio_Delay(int sock,char *cmd_index);
void Setup_Audio_Delay(int sock,char *cmd_index,char *cmd_param,int fd);
void Setup_Auto_Mixer(int sock,char *cmd_index,char *cmd_param);
void Report_Auto_Mixer(int sock,char *cmd_index);
#endif	/* CONTROL_H */

