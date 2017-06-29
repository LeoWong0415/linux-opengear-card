#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "control.h"
#include "config.h"
#include "logo.h"
#include "ogp.h"

extern unsigned char audio_save_context;

unsigned int volume_lookup[] = {
1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,
7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,
9,9,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,13,13,13,13,
13,13,13,14,14,14,14,14,14,15,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,
19,19,19,19,19,20,20,20,20,21,21,21,21,22,22,22,22,23,23,23,23,24,24,24,24,25,25,25,26,26,
26,27,27,27,27,28,28,28,29,29,29,30,30,30,31,31,32,32,32,33,33,33,34,34,35,35,35,36,36,37,
37,37,38,38,39,39,40,40,41,41,42,42,43,43,43,44,45,45,46,46,47,47,48,48,49,49,50,51,51,52,
52,53,54,54,55,55,56,57,57,58,59,59,60,61,61,62,63,64,64,65,66,67,67,68,69,70,71,71,72,73,
74,75,76,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,100,101,102,103,
104,106,107,108,109,111,112,113,114,116,117,118,120,121,123,124,125,127,128,130,131,133,134,136,138,139,141,142,144,146,
147,149,151,153,154,156,158,160,162,163,165,167,169,171,173,175,177,179,181,183,186,188,190,192,194,197,199,201,203,206,
208,211,213,215,218,220,223,226,228,231,234,236,239,242,245,247,250,253,256,259,262,265,268,271,274,278,281,284,287,291,
294,297,301,304,308,311,315,319,322,326,330,334,338,341,345,349,354,358,362,366,370,374,379,383,388,392,397,401,406,411,
415,420,425,430,435,440,445,450,455,461,466,471,477,482,488,494,499,505,511,517,523,529,535,541,548,554,560,567,573,580,
587,593,600,607,614,621,629,636,643,651,658,666,674,681,689,697,705,713,722,730,739,747,756,765,773,782,791,801,810,819,
829,838,848,858,868,878,888,898,909,919,930,941,951,962,974,985,996,1008,1020,1031,1043,1055,1068,1080,1092,1105,1118,1131,1144,1157,
1171,1184,1198,1212,1226,1240,1254,1269,1283,1298,1313,1329,1344,1360,1375,1391,1407,1424,1440,1457,1474,1491,1508,1525,1543,1561,1579,1597,1616,1635,
1653,1673,1692,1712,1731,1751,1772,1792,1813,1834,1855,1877,1898,1920,1943,1965,1988,2011,2034,2058,2082,2106,2130,2155,2180,2205,2230,2256,2282,2309,
2336,2363,2390,2418,2446,2474,2503,2532,2561,2591,2621,2651,2682,2713,2744,2776,2808,2840,2873,2907,2940,2974,3009,3044,3079,3115,3151,3187,3224,3261,
3299,3337,3376,3415,3455,3495,3535,3576,3617,3659,3702,3744,3788,3832,3876,3921,3966,4012,4059,4106,4153,4201,4250,4299,4349,4399,4450,4502,4554,4607,
4660,4714,4769,4824,4880,4936,4993,5051,5110,5169,5229,5289,5350,5412,5475,5538,5603,5667,5733,5799,5867,5935,6003,6073,6143,6214,6286,6359,6433,6507,
6582,6659,6736,6814,6893,6973,7053,7135,7218,7301,7386,7471,7558,7645,7734,7823,7914,8006,8098,8192,    
8287,8383,8480,8578,8677,8778,8880,8982,9086,9192,9298,9406,9515,9625,9736,9849,9963,10078,10195,10313,10433,10553,10676,10799,10924,11051,11179,11308,11439,11572,
11705,11841,11978,12117,12257,12399,12543,12688,12835,12983,13134,13286,13440,13595,13753,13912,14073,14236,14401,14568,14736,14907,15080,15254,15431,15610,15790,15973,16158,16345,
16534,16726,16920,17116,17314,17514,17717,17922,18130,18340,18552,18767,18984,19204,19426,19651,19879,20109,20342,20577,20816,21057,21301,21547,21797,22049,22304,22563,22824,23088,
23356,23626,23900,24176,24456,24739,25026,25316,25609,25905,26205,26509,26816,27126,27440,27758,28080,28405,28734,29066,29403,29743,30088,30436,30789,31145,31506,31871,32240,32613,
32991,33373,33759,34150,34545,34945,35350,35759,36173,36592,37016,37445,37878,38317,38761,39209,39663,40123,40587,41057,41533,42014,42500,42992,43490,43994,44503,45018,45540,46067,
46600,47140,47686,48238,48797,49362,49933,50511,51096,51688,52287,52892,53504,54124,54751,55385,56026,56675,57331,57995,58666,59346,60033,60728,61431,62143,62862,63590,64327,65071,
65825,66587,67358,68138,68927,69725,70533,71349,72176,73011,73857,74712,75577,76452,77337,78233,79139,80055,80982,81920,82869,83828,84799,85781,86774,87779,88795,89823,90864,91916,
92980,94057,95146,96248,97362,98490,99630,100784,101951,103131,104325,105533,106755,107992,109242,110507,111787,113081,114391,115715,117055,118410,119782,121169,122572,123991,125427,126879,128348,129834,
131338,132859,134397,135953,137528,139120,140731,142361,144009,145677,147364,149070,150796,152542,154309,156095,157903,159731,161581,163452,165345,167259,169196,171155,173137,175142,177170,179221,181297,183396,
185520,187668,189841,192039,194263,196512,198788,201090,203418,205774,208156,210567,213005,215472,217967,220491,223044,225626,228239,230882,233555,236260,238996,241763,244563,247394,250259,253157,256088,259054,
262054,265088,268158,271263,274404,277581,280795,284047,287336,290663,294029,297434,300878,304362,307886,311451,315058,318706,322396,326129,329906,333726,337590,341499,345454,349454,353500,357594,361735,365923,
370160,374447,378783,383169,387606,392094,396634,401227,405873,410573,415327,420136,425001,429922,434901,439936,445031,450184,455397,460670,466004,471400,476859,482381,487966,493617,499333,505115,510964,516880,
522865,528920,535045,541240,547507,553847,560260,566748,573311,579949,586665,593458,600330,607281,614313,621427,628623,635902,643265,650714,658249,665871,673581,681381,689271,697252,705326,713493,721755,730113,
738567,747119,755771,764522,773375,782330,791389,800553,809823,819200,
};

extern struct  func_item function;  
extern double rms_ch12_in;
extern double peak_ch12_in;
extern double gain_ch12_out;

extern double rms_ch12_out;
extern double peak_ch12_out;
extern double peak_com_ch12_in;
extern double peak_com_ch12_out;
extern double  comp_gain_ch12;

extern struct sockaddr_in udpserver, echoclient;

/*
extern int leveler_bypass;
extern int leveler_low_threshold;
extern int leveler_background_threshold;
extern int leveler_maximum_gain;
extern long leveler_rms_time;
extern int leveler_attack_time;
extern int leveler_release_time;
*/
extern struct leveler_item leveler_param;
extern struct comp_item comp_param;
extern struct agc_item alc_param;
extern struct audio_mixer audio_mixer_param;
extern struct key_mixer   key_mixer_param;
extern struct reference reference_param;

extern unsigned short key_ref_status; // store external key, reference status update to FPGA
extern struct frame_sync frame_sync_param;
extern struct audio_delay_item audio_delay_param;
extern struct auto_mixer auto_mixer_param;
/***************************************
 * Report_Key_Mixer -- report key mixer parameter
 * 
 * Parameters
 *      sock: network socket handle
 *      *cmd_index: string value to locate parameter index
 *              '0':    enable and disable key mixer
 * Returns
 *      Send back to socket network string
 *              if Ok:
 *                      "200 AUDIO_MIXER "+index+" "+ value
 *              if not Ok:
 *                      "500 error\n"
 * 
 ******************************************************/
void Report_Reference(int sock,char *cmd_index){
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){    
            strcpy(buffer,"200 REFERENCE ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // ken enable information
                    sprintf(s,"%d",reference_param.mode);
                    break;
            }
            
            strcat(buffer,s);
            strcat(buffer,"\n");
            send(sock,buffer,strlen(buffer),0); // send it back
    }else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);                
    }  
    
}

/***************************************
 * Report_Key_Mixer -- report key mixer parameter
 * 
 * Parameters
 *      sock: network socket handle
 *      *cmd_index: string value to locate parameter index
 *              '0':    enable and disable key mixer
 * Returns
 *      Send back to socket network string
 *              if Ok:
 *                      "200 AUDIO_MIXER "+index+" "+ value
 *              if not Ok:
 *                      "500 error\n"
 * 
 ******************************************************/
void Report_Frame_Sync(int sock,char *cmd_index){
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){    
            strcpy(buffer,"200 FRAME_SYNC ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // frame enable information
                    sprintf(s,"%d",frame_sync_param.enable);
                    break;
                case 1: // 
                    sprintf(s,"%d",frame_sync_param.on_loss);
                    break;                    
                case 2: // 
                    sprintf(s,"%d",frame_sync_param.color);
                    break;                                        
            }
            
            strcat(buffer,s);
            strcat(buffer,"\n");
            send(sock,buffer,strlen(buffer),0); // send it back
    }else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);                
    }  
    
}
// 
void Set_Key_Mixer_Startup(int fd){
    unsigned short data;
    data=(reference_param.mode&0x01);
    data=data|((key_mixer_param.enable&0x1)<<1);
    key_ref_status=data;
    FPGA_write(fd,33,data); 
}
void Setup_Reference(int sock,char *cmd_index,char *cmd_param,int fd)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    unsigned short data;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // setup low threshold
                    reference_param.mode = param; // 0 videoin, 1 = external
                    break;
            }
            
            SetReference(reference_param.mode);

            strcpy(buffer,"200 REFERENCE ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}

void Setup_Frame_Sync(int sock,char *cmd_index,char *cmd_param,int fd)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    unsigned short data;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // set frame enable
                    frame_sync_param.enable = param;
    
    
                    break;
                case 1:    
                    frame_sync_param.on_loss = param;   
                                        
    
                    break;
                case 2:
                    frame_sync_param.color = param;
                    
                    break;
            }
                              
            
                FPGA_write(fd,21,frame_sync_param.enable);       
                FPGA_write(fd,19,frame_sync_param.on_loss);
                FPGA_write(fd,20,frame_sync_param.color);
            
                SaveAudioParam();
                strcpy(buffer,"200 FRAME_SYNC ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}

/***************************************
 * Report_Key_Mixer -- report key mixer parameter
 * 
 * Parameters
 *      sock: network socket handle
 *      *cmd_index: string value to locate parameter index
 *              '0':    enable and disable key mixer
 * Returns
 *      Send back to socket network string
 *              if Ok:
 *                      "200 AUDIO_MIXER "+index+" "+ value
 *              if not Ok:
 *                      "500 error\n"
 * 
 ******************************************************/
void Report_Key_Mixer(int sock,char *cmd_index){
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){    
            strcpy(buffer,"200 KEYMIXER ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // ken enable information
                    sprintf(s,"%d",key_mixer_param.enable);
                    break;
            }
            
            strcat(buffer,s);
            strcat(buffer,"\n");
            send(sock,buffer,strlen(buffer),0); // send it back
    }else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);                
    }  
    
}

void Setup_Key_Mixer(int sock,char *cmd_index,char *cmd_param,int fd)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    unsigned  short data;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // setup low threshold
                    // key_mixer_param.enable = 1 : disable key
                    // key_mixer_param.enable = 0 : enable key
                    key_mixer_param.enable = param;
                    break;
            }
            
            SetKeyEnable(key_mixer_param.enable);
            
                strcpy(buffer,"200 KEYMIXER ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}

/***************************************
 * Report_Audio_Mixer -- report audio mixer level and pan status
 * 
 * Parameters
 *      sock: network socket handle
 *      *cmd_index: string value to locate parameter index
 *              '0':    Channel1
 *              '1':    Channel2
 *              '2':    Channel3
 *              '3':   Mater volume for channel1 and 2 output
 *              '4':    Pan control
 * Returns
 *      Send back to socket network string
 *              if Ok:
 *                      "200 AUDIO_MIXER "+index+" "+ value
 *              if not Ok:
 *                      "500 error\n"
 * 
 */
void Report_Audio_Mixer(int sock,char *cmd_index){
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){    
            strcpy(buffer,"200 AUDIO_MIXER ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // volume ch1                                        
                case 1: // volume ch2
                case 2: // volume ch3
                case 3: // volume ch4
                case 4: // volume master mix1
                case 5: // volume ch1                                        
                case 6: // volume ch2
                case 7: // volume ch3
                case 8: // volume ch4
                case 9: // volume master mix2                    
                    sprintf(s,"%f",audio_mixer_param.volume[index]);
                    break;
                case 10: // pan information
                    sprintf(s,"%d",audio_mixer_param.pan);
                    break;
                case 11: // pan information
                    sprintf(s,"%d",audio_mixer_param.pan_mix2);
                    break;                    
            }
            
            strcat(buffer,s);
            strcat(buffer,"\n");
            send(sock,buffer,strlen(buffer),0); // send it back
    }else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);                
    }  
    
}
void Report_Audio_MixerUDP(int sock,char *cmd_index){
    char buffer[32];
    char *endptr;
    int index;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){    
            switch(index){
                case 0: // volume ch1                                        
                case 1: // volume ch2
                case 2: // volume ch3
                case 3: // volume ch4
                    sprintf(buffer,"200 AUDIO_MIXER %d %-2.2f\n",index,audio_mixer_param.volume[index]);
                    break;
                case 4: // pan information
                    sprintf(buffer,"200 AUDIO_MIXER %d %d\n",index,audio_mixer_param.pan);
                    break;
            }
            sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
    }else {
        strcpy(buffer,"NACK\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
    }  
    
}

/***************************************
 * Change_Audio_Mixer -- Change audio mixer level and pan
 * 
 * Parameters
 *      sock: network socket handle
 *      *cmd_index: string value to locate parameter index
 *              '0':    Channel1
 *              '1':    Channel2
 *              '2':    Channel3
 *              '3':   Mater volume for channel1 and 2 output
 *              '4':    Pan control
 *      *cmd_param: string value for parameter index
 *              if cmd_index = 0 to 3, cmd_param is from -80 to 40 db
 *              if cmd_index =4, cmd_param is pan information
 * Returns
 *      Send back to socket network string
 *              if Ok:
 *                      "200 AUDIO_MIXER "+index+" "+ value
 *              if not Ok:
 *                      "500 error\n"
 * 
 */
// hien sua doan nay
void Change_Audio_Mixer(int sock,char *cmd_index,char *cmd_param,int fd)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
//    int         index_volume;
    double param;
    unsigned short data;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                        
        param=strtod(cmd_param,&endptr);             
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // volume ch1                               
                case 1: // volume ch2                    
                case 2: // volume_ch3   
                case 3: // volume_ch4 cho mixer 1              
                case 4://  master volume mixer 1
                case 5: // volume ch1                               
                case 6: // volume ch2                    
                case 7: // volume_ch3   
                case 8: // volume_ch4 cho mixer 2                                  
                case 9://  master volume mixer 2    
                    if(param<=-80)
                        param=-79.9;
                    if(param>=40)                        
                        param=39.9;                       
                    audio_mixer_param.volume[index]=param;   
//                    audioVolumeDataParam.value[index]=round(param);
                    break;
                case 10: // pan
                    audio_mixer_param.pan = (unsigned int)param;
                    break;     
                case 11: // pan mixer2
                    audio_mixer_param.pan_mix2 = (unsigned int)param;
                    break;                         

            }
            
            ChangeAudioMixer(index,fd);
            audio_save_context = 1;
           
            //printf("\nVolume1=%f",audio_mixer_param.volume[0]);
                
                //SaveAudioParam();
            
                strcpy(buffer,"200 AUDIO_MIXER ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                if(index!=4)
                        sprintf(s,"%f",param);
                else
                    sprintf(s,"%d",(int)param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Change_Audio_MixerUDP(int sock,char *cmd_index,char *cmd_param,int fd)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    int         index_volume;
    double param;
    unsigned short data;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                        
        param=strtod(cmd_param,&endptr);             
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // volume ch1                               
                case 1: // volume ch2                    
                case 2: // volume_ch3                    
                case 3://  master volume
                    if(param<=-80)
                        param=-79.9;
                    if(param>=40)                        
                        param=39.9;                       
                    audio_mixer_param.volume[index]=param;                    
                    break;
                case 4: // pan
                    audio_mixer_param.pan = (unsigned int)param;
                    break;                    
            }
            
            switch(index){
                case 0:
                    index_volume=(int)((audio_mixer_param.volume[0]+79.9)*10);
#ifdef TEST                    
					printf("\n--------index_volume=%d",index_volume);
#endif           
					//ioctl(fd,0,128);
                    data=(volume_lookup[index_volume]&0xffff);							
                    //write(fd,&data,7); // write low volume1
                    FPGA_write(fd,7,data);   
                    data=((volume_lookup[index_volume]>>16)&0xffff);							
                    FPGA_write(fd,8,data);   
                    //write(fd,&data,8); // write low volume1
                    break;
                case 1:
                    index_volume=(int)((audio_mixer_param.volume[1]+79.9)*10);
                    //ioctl(fd,0,128);
                    data=(volume_lookup[index_volume]&0xffff);							
                    FPGA_write(fd,9,data);   
                    //write(fd,&data,9); // write low volume1
                    data=((volume_lookup[index_volume]>>16)&0xffff);							
                    FPGA_write(fd,10,data);   
                    //write(fd,&data,10); // write low volume1                    
                    break;
                case 2:
                    index_volume=(int)((audio_mixer_param.volume[2]+79.9)*10);
                    //ioctl(fd,0,128);
                    data=(volume_lookup[index_volume]&0xffff);							
                    FPGA_write(fd,11,data);   
                    //write(fd,&data,11); // write low volume1
                    data=((volume_lookup[index_volume]>>16)&0xffff);							
                    FPGA_write(fd,12,data);   
                    //write(fd,&data,12); // write low volume1                    
                    break;
                case 3:
                    index_volume=(int)((audio_mixer_param.volume[3]+79.9)*10);
                    //ioctl(fd,0,128);
                    data=(volume_lookup[index_volume]&0xffff);							
                    FPGA_write(fd,13,data);   
                    //write(fd,&data,13); // write low volume1
                    data=((volume_lookup[index_volume]>>16)&0xffff);							
                    FPGA_write(fd,14,data);   
                    //write(fd,&data,14); // write low volume1                    
                    break;
                case 4:                    
                    //ioctl(fd,0,128);                    
                    data=((audio_mixer_param.pan)&0xffff);							
                    FPGA_write(fd,15,data);   
                    //write(fd,&data,15); // write low volume1                                        
                    break;
                 
            }
            
            
                printf("\nVolume1=%f",audio_mixer_param.volume[index]);
                audio_save_context = 1;
                //SaveAudioParam();
                strcpy(buffer,"200 AUDIO_MIXER ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                if(index!=4)
                        sprintf(s,"%f",param);
                else
                    sprintf(s,"%d",(int)param);
                strcat(buffer,s);
                strcat(buffer,"\n");
				sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
            
        }else{
                strcpy(buffer,"500 error\n");
				sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));     
    }     
}


void Setup_Audio_Mixer_Startup(int fd){
    
int index_volume;    
unsigned short data;
int i;

for(i=0;i<12;++i)
    ChangeAudioMixer(i,fd);

/*

ioctl(fd,0,128);

index_volume=(int)((audio_mixer_param.volume[0]+79.9)*10);        
data=(volume_lookup[index_volume]&0xffff);							
write(fd,&data,7); // write low volume1
data=((volume_lookup[index_volume]>>16)&0xffff);							
write(fd,&data,8); // write low volume1

index_volume=(int)((audio_mixer_param.volume[1]+79.9)*10);        
data=(volume_lookup[index_volume]&0xffff);							
write(fd,&data,9); // write low volume1
data=((volume_lookup[index_volume]>>16)&0xffff);							
write(fd,&data,10); // write low volume1

index_volume=(int)((audio_mixer_param.volume[2]+79.9)*10);        
data=(volume_lookup[index_volume]&0xffff);							
write(fd,&data,11); // write low volume1
data=((volume_lookup[index_volume]>>16)&0xffff);							
write(fd,&data,12); // write low volume1


index_volume=(int)((audio_mixer_param.volume[3]+79.9)*10);        
data=(volume_lookup[index_volume]&0xffff);							
write(fd,&data,13); // write low volume1
data=((volume_lookup[index_volume]>>16)&0xffff);							
write(fd,&data,14); // write low volume1


data=((audio_mixer_param.pan)&0xffff);							
write(fd,&data,15); // write low volume1   
*/


}
void Report_Setup_Leveler(int sock,char *cmd_index)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                
            strcpy(buffer,"200 SETUPLEVELER ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // setup low threshold                    
                    sprintf(s,"%d",leveler_param.leveler_low_threshold);
                    break;
                case 1: // setup background threshold;
                    sprintf(s,"%d",leveler_param.leveler_background_threshold);
                    break;
                case 2: // setup maximum gain
                    sprintf(s,"%d",leveler_param.leveler_maximum_gain);
                    break;
                case 3:// setup rms time
                    sprintf(s,"%ld",leveler_param.leveler_rms_time);
                    break;
                case 4: // setup attack time
                    sprintf(s,"%d",leveler_param.leveler_attack_time);
                    break;
                case 5: // setup release time
                    sprintf(s,"%d",leveler_param.leveler_release_time);
                    break;
                case 6: // setup bypass
                    sprintf(s,"%d",leveler_param.bypass);
                    break;
                case 7:
                    sprintf(s,"%d",leveler_param.input_gain);
                    break;
            }
            
                
                
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Setup_Audio_Delay(int sock,char *cmd_index,char *cmd_param,int fd)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    unsigned short data;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // set frame enable
                    audio_delay_param.enabled = param;        
                    break;
                case 1:    
                    audio_delay_param.delay = param;                                               
                    break;
            }
                              
                
                FPGA_write(fd,37,audio_delay_param.enabled);       // bypass =1 cho phep chay delay, =0 bo qua delay
                Change_Delay_Audio(fd,audio_delay_param.delay); // do tre tinh bang milisecond

            
                SaveAudioParam();
                strcpy(buffer,"200 AUDIO_DELAY ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Report_Audio_Delay(int sock,char *cmd_index){
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){    
            strcpy(buffer,"200 AUDIO_DELAY ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // frame enable information
                    sprintf(s,"%d",audio_delay_param.enabled);
                    break;
                case 1: // 
                    sprintf(s,"%d",audio_delay_param.delay);
                    break;                    
            }
            
            strcat(buffer,s);
            strcat(buffer,"\n");
            send(sock,buffer,strlen(buffer),0); // send it back
    }else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);                
    }  
    
}

void Report_Auto_Mixer(int sock,char *cmd_index)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                
            strcpy(buffer,"200 SETUPAUTOMIXER ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // operation level
                    sprintf(s,"%f",auto_mixer_param.check_threshold);
                    break;
                case 1: // 
                    sprintf(s,"%d",auto_mixer_param.is_auto);
                    break;
                case 2: 
                    sprintf(s,"%f",auto_mixer_param.ch34_level);
                    break;
                case 3: 
                    sprintf(s,"%d",auto_mixer_param.is_copy12);
                    break;                                        
            }
            
                
                
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}

void Report_ALC(int sock,char *cmd_index)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                
            strcpy(buffer,"200 SETUPAGC ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // operation level
                    sprintf(s,"%f",alc_param.op_threshold);
                    break;
                case 1: // 
                    sprintf(s,"%f",alc_param.gate_threshold);
                    break;
                case 2: 
                    sprintf(s,"%f",alc_param.transient_range);
                    break;
                case 3:// 
                    sprintf(s,"%f",alc_param.agc_bypass_range);
                    break;
                case 4: 
                    sprintf(s,"%f",alc_param.transient_time);
                    break;
                case 5: 
                    sprintf(s,"%f",alc_param.agc_range);
                    break;
                case 6: // 
                    sprintf(s,"%f",alc_param.agc_bypass_range);
                    break;
                case 7:
                    sprintf(s,"%d",alc_param.agc_time);
                    break;
                case 8:
                    sprintf(s,"%d",alc_param.agc_rms_time);
                    break;
                case 9:
                    sprintf(s,"%d",alc_param.enabled);
                    break;
                case 10:
                    sprintf(s,"%d",alc_param.mode);
                    break;
            }
            
                
                
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Report_Setup_LevelerUDP(int sock,char *cmd_index)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                
            strcpy(buffer,"200 SETUPLEVELER ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // setup low threshold                    
                    sprintf(s,"%d",leveler_param.leveler_low_threshold);
                    break;
                case 1: // setup background threshold;
                    sprintf(s,"%d",leveler_param.leveler_background_threshold);
                    break;
                case 2: // setup maximum gain
                    sprintf(s,"%d",leveler_param.leveler_maximum_gain);
                    break;
                case 3:// setup rms time
                    sprintf(s,"%ld",leveler_param.leveler_rms_time);
                    break;
                case 4: // setup attack time
                    sprintf(s,"%d",leveler_param.leveler_attack_time);
                    break;
                case 5: // setup release time
                    sprintf(s,"%d",leveler_param.leveler_release_time);
                    break;
                case 6: // setup bypass
                    sprintf(s,"%d",leveler_param.bypass);
                    break;
            }
            
                
                
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 
                
        
    } else {
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 
    }     
}

void Report_Setup_Comp(int sock,char *cmd_index)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                
            strcpy(buffer,"200 SETUPCOMP ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // setup threshold
                    sprintf(s,"%d",comp_param.threshold);
                    break;
                case 1: // setup ratio
                    sprintf(s,"%d",comp_param.ratio);
                    break;
                case 2: // setup attack time
                    sprintf(s,"%d",comp_param.attack_time);
                    break;
                case 3:// setup release time
                    sprintf(s,"%d",comp_param.release_time);
                    break;
                case 4: // setup bypass
                    sprintf(s,"%d",comp_param.bypass);
                    break;
                case 5:
                    sprintf(s,"%d",comp_param.input_gain);
                    break;
            }
            
                
                
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Report_Setup_CompUDP(int sock,char *cmd_index)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
                
            strcpy(buffer,"200 SETUPCOMP ");
            sprintf(s,"%d",index);
            strcat(buffer,s);
            strcat(buffer," ");
            switch(index){
                case 0: // setup threshold
                    sprintf(s,"%d",comp_param.threshold);
                    break;
                case 1: // setup ratio
                    sprintf(s,"%d",comp_param.ratio);
                    break;
                case 2: // setup attack time
                    sprintf(s,"%d",comp_param.attack_time);
                    break;
                case 3:// setup release time
                    sprintf(s,"%d",comp_param.release_time);
                    break;
                case 4: // setup bypass
                    sprintf(s,"%d",comp_param.bypass);
                    break;
            }
            
                
                
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 
                
        
    } else {
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 
    }     
}

void Send_CompGain(int sock,char *cmd_arg)
{
    
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        
                strcpy(buffer,"200 GAINCOMP12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",comp_gain_ch12);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
        
    }
    else {    
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);

    }

}
void Send_CompGainUDP(int sock,char *cmd_arg)
{
    
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        
                strcpy(buffer,"200 GAINCOMP12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",comp_gain_ch12);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 
        
    }
    else {    
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 

    }

}
//SETUPAUTOMIXER
void Setup_Auto_Mixer(int sock,char *cmd_index,char *cmd_param)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    double param;
    index=(int)strtof(cmd_index,&endptr);    
    if(endptr!=cmd_index){
        
        param=strtof(cmd_param,&endptr);        
        if(endptr!=cmd_param){                                
            switch((int)index){
                case 0: 
                    auto_mixer_param.check_threshold = param;
                    break;
                case 1: // setup background threshold;
                    auto_mixer_param.is_auto = (int)param;
                    break;
                case 2:
                    auto_mixer_param.is_copy12=(int)param;
                    break;
            }
            
                SaveAudioParam();
                strcpy(buffer,"200 SETUPAUTOMIXER ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Setup_Leveler(int sock,char *cmd_index,char *cmd_param)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // setup low threshold
                    leveler_param.leveler_low_threshold = param;
                    break;
                case 1: // setup background threshold;
                    leveler_param.leveler_background_threshold=param;
                    break;
                case 2: // setup maximum gain
                    leveler_param.leveler_maximum_gain =param;
                    break;
                case 3:// setup rms time
                    leveler_param.leveler_rms_time = param;
                    break;
                case 4: // setup attack time
                    leveler_param.leveler_attack_time = param;
                    break;
                case 5: // setup release time
                    leveler_param.leveler_release_time = param;
                    break;
                case 6: // setup bypass
                    leveler_param.bypass = param;
                    break;
                case 7:
                    leveler_param.input_gain = param;
                    Change_Leveler_Input12(param*10);
                    break;
            }
            
                SaveAudioParam();
                strcpy(buffer,"200 SETUPLEVELER ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Setup_AGC(int sock,char *cmd_index,char *cmd_param)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    double param;
    index=(int)strtof(cmd_index,&endptr);    
    if(endptr!=cmd_index){
        
        param=strtof(cmd_param,&endptr);        
        if(endptr!=cmd_param){                                
            switch((int)index){
                case 0: 
                    alc_param.op_threshold = param;
                    break;
                case 1: // setup background threshold;
                    alc_param.gate_threshold = param;
                    break;
                case 2: // setup maximum gain
                    alc_param.transient_range = param;
                    break;
                case 3:// setup rms time
                    alc_param.transient_bypass_range = param;
                    break;
                case 4: // setup attack time
                    alc_param.transient_time = param;
                    break;
                case 5: // setup release time
                    alc_param.agc_range =param;
                    break;
                case 6: // setup bypass
                    alc_param.agc_bypass_range = param;
                    break;
                case 7:
                    alc_param.agc_time = (int)param;                    
                    break;
                case 8:
                    alc_param.agc_rms_time=(int)param;
                    break;
                case 9:
                    alc_param.enabled = (int)param;
                    break;
                case 10:
                    alc_param.mode = (int)param;
                    break;
            }
            
                SaveAudioParam();
                strcpy(buffer,"200 SETUPAGC ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Setup_LevelerUDP(int sock,char *cmd_index,char *cmd_param)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // setup low threshold
                    leveler_param.leveler_low_threshold = param;
                    break;
                case 1: // setup background threshold;
                    leveler_param.leveler_background_threshold=param;
                    break;
                case 2: // setup maximum gain
                    leveler_param.leveler_maximum_gain =param;
                    break;
                case 3:// setup rms time
                    leveler_param.leveler_rms_time = param;
                    break;
                case 4: // setup attack time
                    leveler_param.leveler_attack_time = param;
                    break;
                case 5: // setup release time
                    leveler_param.leveler_release_time = param;
                    break;
                case 6: // setup bypass
                    leveler_param.bypass = param;
                    break;
            }
            
                SaveAudioParam();
                strcpy(buffer,"200 SETUPLEVELER ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 

            
        }else{
                strcpy(buffer,"500 error\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));                   
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
    }     
}
void Setup_Comp(int sock,char *cmd_index,char *cmd_param)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // setup threshold
                    comp_param.threshold = param;
                    break;
                case 1: // setup ratio
                    comp_param.ratio=param;
                    break;
                case 2: // setup attack time
                    comp_param.attack_time = param;
                    break;
                case 3:// setup release time
                    comp_param.release_time = param;
                    break;
                case 4: // setup bypass
                    comp_param.bypass=param;
                    break;
                case 5:
                    comp_param.input_gain =param;
                    Change_Comp_Input12(param*10);
                    break;
            }
            
                SaveAudioParam();
                strcpy(buffer,"200 SETUPCOMP ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

            
        }else{
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                    
        }
        
    } else {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);        
    }     
}
void Setup_CompUDP(int sock,char *cmd_index,char *cmd_param)
{

    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    long param;
    index=strtol(cmd_index,&endptr,10);
    if(endptr!=cmd_index){
        
        param=strtol(cmd_param,&endptr,10);        
        if(endptr!=cmd_param){        
            switch(index){
                case 0: // setup threshold
                    comp_param.threshold = param;
                    break;
                case 1: // setup ratio
                    comp_param.ratio=param;
                    break;
                case 2: // setup attack time
                    comp_param.attack_time = param;
                    break;
                case 3:// setup release time
                    comp_param.release_time = param;
                    break;
                case 4: // setup bypass
                    comp_param.bypass=param;
                    break;
            }
            
                SaveAudioParam();
                strcpy(buffer,"200 SETUPCOMP ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%ld",param);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient)); 
			}else{
                strcpy(buffer,"500 error\n");
				sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));    
			}
		} else {
			strcpy(buffer,"500 error\n");
			sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));    
    }     
}

void Send_LeverGain(int sock,char *cmd_arg)
{
    
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        
                strcpy(buffer,"200 GAINLEVELER12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",gain_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
        
    }
    else {    
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);

    }

}
void Send_LeverGainUDP(int sock,char *cmd_arg)
{
    
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        
                strcpy(buffer,"200 GAINLEVELER12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",gain_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
        
    }
    else {    
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

    }

}
//GetMeterAuToMixer
void GetMeterAuToMixer(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 GetMeterAuToMixer ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",auto_mixer_param.ch34_level);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 GetMeterAuToMixer ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",auto_mixer_param.ch34_level);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);

    }
    
}
//GetMeterCompIn
void GetMeterCompIn(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERCOMPIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERCOMPIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);

    }
    
}
void GetMeterCompInUDP(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERCOMPIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERCOMPIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

    }
    
}

//GetMeterCompOut
void GetMeterCompOut(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERCOMPOUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_out+gain_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERCOMPOUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_out+gain_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);

    }
    
}
void GetMeterCompOutUDP(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERCOMPOUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERCOMPOUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_com_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

    }
    
}

void Send_LeverMeterIn(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERLEVELERIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",rms_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERLEVELERIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",rms_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);

    }
    
}
void Send_LeverMeterInUDP(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERLEVELERIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",rms_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERLEVELERIN12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_ch12_in);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

    }
    
}

void Send_LeverMeterOut(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERLEVELEROUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",rms_ch12_in+gain_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERLEVELEROUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",rms_ch12_in+gain_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);

    }
    
}
void Send_LeverMeterOutUDP(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    //printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){
        switch(index){
            case 0: // muon lay RMS
                strcpy(buffer,"200 METERLEVELEROUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",rms_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
                break;
            case 1:// muon lay PEAK
                strcpy(buffer,"200 METERLEVELEROUT12 ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%f",peak_ch12_out);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

                break;
        }
    }
    else {    
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

    }
    
}


void Send_Function(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){        
        switch(index)
        {
            case 0: // ask logo_insert
                printf("den day2");
                strcpy(buffer,"200 FUNCTION ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%d",(int)function.logo_insert);                
                strcat(buffer,s);                
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);
                break;
            case 1:
                strcpy(buffer,"200 FUNCTION ");                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%d",(int)function.external_key);                
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);                
                break;
            case 2:
                strcpy(buffer,"200 FUNCTION ");
                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%d",(int)function.frame_sync);                
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);                
                break;
            case 3:
                strcpy(buffer,"200 FUNCTION ");
                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");                
                sprintf(s,"%d",(int)function.audio_leveler);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);                
                break;
            case 4:
                strcpy(buffer,"200 FUNCTION ");
                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");                
                sprintf(s,"%d",(int)function.audio_comp);
                strcat(buffer,s);
                strcat(buffer,"\n");
                send(sock,buffer,strlen(buffer),0);                
                break;
            default:
                strcpy(buffer,"500 error\n");
                send(sock,buffer,strlen(buffer),0);                
        }
        
    } else
    {
        strcpy(buffer,"500 error\n");
        send(sock,buffer,strlen(buffer),0);
    }
        
}
void Send_FunctionUDP(int sock,char *cmd_arg)
{
    char buffer[200];
    char s[30];
    char *endptr;
    int index;
    printf("den day1");
    index=strtol(cmd_arg,&endptr,10);    
    if(endptr!=cmd_arg){        
        switch(index)
        {
            case 0: // ask logo_insert
                printf("den day2");
                strcpy(buffer,"200 FUNCTION ");
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%d",(int)function.logo_insert);                
                strcat(buffer,s);                
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
                break;
            case 1:
                strcpy(buffer,"200 FUNCTION ");                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%d",(int)function.external_key);                
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
                break;
            case 2:
                strcpy(buffer,"200 FUNCTION ");
                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");
                sprintf(s,"%d",(int)function.frame_sync);                
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
                break;
            case 3:
                strcpy(buffer,"200 FUNCTION ");
                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");                
                sprintf(s,"%d",(int)function.audio_leveler);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
                break;
            case 4:
                strcpy(buffer,"200 FUNCTION ");
                
                sprintf(s,"%d",index);
                strcat(buffer,s);
                strcat(buffer," ");                
                sprintf(s,"%d",(int)function.audio_comp);
                strcat(buffer,s);
                strcat(buffer,"\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
                break;
            default:
                strcpy(buffer,"500 error\n");
                sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
        }
        
    } else
    {
        strcpy(buffer,"500 error\n");
        sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));         
    }
        
}

void print_hex_string(char *strin,int len){
    int i;    
    for(i=0;i<len;++i){
        printf("%02x,",strin[i]);
    }
}
void parse_string(char *strin,char (*str_array)[200],int *length)
{
	char delims[]=" ";
	char *result = NULL;
	int i;        
	(*length) = 0;
	result = strtok(strin,delims);	        
	while(result !=NULL){
		strcpy(str_array[(*length)],result);
		(*length)++;		
		result = strtok(NULL,delims);			
	}
}    
//--------------------------------------------------------------------------------------
void ChangeAudioMixer(int index, int fd)
{
    int index_volume;
    unsigned short data;
 
    switch(index){
        case 0:
            index_volume=(int)((audio_mixer_param.volume[0]+79.9)*10);
            //printf("\n--------index_volume=%d",index_volume);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,7,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,8,data);   
            break;
        case 1:
            index_volume=(int)((audio_mixer_param.volume[1]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,9,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,10,data);   
            break;
        case 2:
            index_volume=(int)((audio_mixer_param.volume[2]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,11,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,12,data);   
            break;
        case 3:
            index_volume=(int)((audio_mixer_param.volume[3]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,59,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,60,data);   
            break;
        case 4:
            index_volume=(int)((audio_mixer_param.volume[4]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,13,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,14,data);   
            break;       
        case 5: // volume 1 mixer 2
            index_volume=(int)((audio_mixer_param.volume[5]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,50,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,51,data);   
            break;              
        case 6:
            index_volume=(int)((audio_mixer_param.volume[6]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,52,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,53,data);   
            break;          
        case 7:
            index_volume=(int)((audio_mixer_param.volume[7]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,54,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,55,data);   
            break;        
        case 8:
            index_volume=(int)((audio_mixer_param.volume[8]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,61,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,62,data);   
            break;          
        case 9:
            index_volume=(int)((audio_mixer_param.volume[9]+79.9)*10);
            data=(volume_lookup[index_volume]&0xffff);							
            FPGA_write(fd,56,data);   
            data=((volume_lookup[index_volume]>>16)&0xffff);							
            FPGA_write(fd,57,data);   
            break;               
        case 10:                    
            data=((audio_mixer_param.pan)&0xffff);							
            FPGA_write(fd,15,data);   
            break;
            
        case 11:                    
            data=((audio_mixer_param.pan_mix2)&0xffff);							
            FPGA_write(fd,58,data);   
            break;            

    }


}