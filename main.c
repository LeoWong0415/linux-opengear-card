/* 
 * File:   main.c
 * Author: sau
 *
 * Created on August 27, 2012, 11:18 AM
 * ------------------------------------------------------------------------------------------
 * Modified on 29/03/2013 (by Sau)
 * - Remove Audio/Compressor Tab
 * - Remove Video delay parameter on Card status tab
 * - Add 4 Function Status on Card status Tab to indicate it is activated or not
 * - Modified all parameters on Audio/Leveler Tab
 * - Add new Audio/Delay Tab
 * USE LEVELER TAB:
 * + All parameters on Leveler tab are map on "struct agc_item alc_param;" on "leveler.c" file.
 *   remember to extern this structure before use it on others files.
 * + "Mode Select" base on "const struct agc_item lvDefaultModes[3]" placed on "parameters.c" file.
 *   Change this default values corresponding each mode.
 * USE DELAY TAB:
 * + Delay time is map on "DELAY_T audio_delay_param = {0,0}; placed on "parameters.c" file.
 * -------------------------------------------------------------------------------------------
 * Modified on 02/02/2013 (by Sau)
 * -Remove Devices Discovery thread
 * -Add new parameter named "Control Panel" checbox in AudioMixer tab, checked while Control Panel available.
 * -Auto detect Control Panel, disable software controls when Control Panel activated
 * -Software Rev. now is 3.06
 * Modified on 17/1/2013
 * - remove Audio reset, real-time signal 10 exception
 * Modified on 14/12/2012
 * -Fix Audio Mixer different to Software HDManager
 * Modified on 10/12/2012 (by Sau)
 * -Fix ActiveLogo on UDP thread
 * -Fix AudioMixerVolume not change when adjust by Control Panel
 * them time_format, de thong bao loai tinh hieu (1 hoac 3 la SD), (0 la HD), (2 la 3G)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "adc.h"
#include "can_lv.h"
#include "ogp.h"
#include "opengear.h"
#include "ogpdef.h"
#include "utilities.h"
#include "ogp_ctrl.h"
//---Include from HIEN-------------------------------------------------------------------
#define HIEN
#ifdef HIEN
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/mman.h>

#include <signal.h>
#include "types.h"
#include "spidev.h"
#include "control.h"
#include "spicontrol.h"
#include "FPGA.h"
#include "leveler.h"
#include "logo.h"
#include "clienthandler.h"
#include "compressor.h"

#define LOGO_FOLDER     //mnt//home//logos//


#define R_PC    4       //read port C
#define R_PB	6	// read port b
#define R_PHY	7
  
//Indicate that there is Control Panel is connected to this card
BOOL ctrlPanelIsAvailable = FALSE;
BOOL setModeAlready = FALSE;    //indicate action state of setting mode for Hardware/Sofware control

#define TICKS_IN_5_SECONDS  1250        /*= 5 seconds x 4ms*/
unsigned int ctrlPanelTimeOut=0;    //Control Panel Timer

  int fd_spi_gs2972; // GS2972
  int fd_spi_gs2971_anpha; // GS2972
  int fd_spi_gs2971_video; // GS2972
  
struct comp_item comp_param;
struct audio_mixer audio_mixer_param;
struct key_mixer   key_mixer_param;
struct reference reference_param;
struct frame_sync frame_sync_param;
extern struct leveler_item leveler_param;
struct audio_delay_item audio_delay_param;
struct auto_mixer auto_mixer_param;
extern struct agc_item alc_param;
unsigned short time_format; // chua thong tin ve dang tin hieu vao la SD, HD hay 3G
// time_format == 1 or 3 , tin hieu vao la SD
// time_format == 0 la tin hieu HD
// time_format == 2 la tin hieu 3G
unsigned short key_ref_status; // store external key, reference status update to FPGA

  
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;  

  
static int fd_spi_gs2971; // file pointer to spidev1.1, control GS2971 background
int fd_spi_gs4911;
static int fd_led;
unsigned short *phy_mem;
  


struct  func_item function;  

int v_page=0;
int fd;
int card_type=1;// Only Logo Insert
// card_type is type of video card
// b3	Set for AFD insert
// b2	Set for Key processing
// b1	Set for Audio Compressor 
// b0:	Set for Logo Insert

int udpsock;
struct sockaddr_in udpserver, echoclient;


//OPENGEAR
extern ObjectID frameSyncGroup[];
extern ObjectID keyGroup[];
extern ObjectID audioMixerGroup[];
extern ObjectID audioCompGroup[];
extern ObjectID audioLevelGroup[];
extern ObjectID logoGroup[];

//logos
extern int video_mode;
extern int no_logo;
extern struct logo_item logo_list[];
extern int active_logo;

extern Int16Param logoTotalParam;
extern Int16Param logoActIdxParam;
extern StringParam logoActNameParam;
extern Int16Param logoCurIdxParam;
extern StringParam logoCurNameParam;
extern Int16Param logoCurPosXParam;
extern Int16Param logoCurPosYParam;
extern Int16Param logoActivateParam;

//--------------------------------------------
//==========================================================================
//MAIN

void Change_Ref(int fd_spi,int mode){
    if(mode==0){ // 1080i
//        printf("\\n---chuyen sang 1080i");
    SPI_write(fd_spi,0x4d,27);
    SPI_write(fd_spi,0x4c,0x2);
    SPI_write(fd_spi,0x43,0x2);
    SPI_write(fd_spi,0x45,0x4);    
    SPI_write(fd_spi,0x47,0x6);    
#ifdef DEBUG
    //printf("--------------------\n\r");
    //printf("Adr= 0x4d=%x\n\r",SPI_read(fd_spi,0x4d));
    //printf("Adr= 0x4c=%x\n\r",SPI_read(fd_spi,0x4c));
    //printf("Adr= 0x43=%x\n\r",SPI_read(fd_spi,0x43));
    //printf("Adr= 0x45=%x\n\r",SPI_read(fd_spi,0x45));
    //printf("Adr= 0x47=%x\n\r",SPI_read(fd_spi,0x47));    
#endif    
    }
    if(mode==1){ // 576i
#ifdef DEBUG        
        //printf("\\n---chuyen sang 576i");
#endif        
        SPI_write(fd_spi,0x4d,4);
        SPI_write(fd_spi,0x4c,0x2);
    SPI_write(fd_spi,0x43,0x2);
    SPI_write(fd_spi,0x45,0x4);    SPI_write(fd_spi,0x47,0x6);

    SPI_write(fd_spi,0x47,0x6);    SPI_write(fd_spi,0x47,0x6);                
    
    
    SPI_write(fd_spi,0x4c,0x2);
    SPI_write(fd_spi,0x43,0x2);
    SPI_write(fd_spi,0x45,0x4);    SPI_write(fd_spi,0x47,0x6);

    SPI_write(fd_spi,0x47,0x6);    SPI_write(fd_spi,0x47,0x6);        
    }
}

void Change_Ref_Frame(int fd_spi,int mode){
    unsigned short vdelay;
    unsigned short hdelay;    
    FPGA_write_nolock(fd,18,mode); // hd , sd    
    if(mode==0){ // 1080i
#ifdef DEBUG        
        //printf("\\n---chuyen sang 1080i");
#endif        
        SPI_write(fd_spi,0x4d,27);
        SPI_write(fd_spi,0x4c,0x2);
        SPI_write(fd_spi,0x43,0x2);
        SPI_write(fd_spi,0x45,0x4);    
        SPI_write(fd_spi,0x47,0x6);    
    
        vdelay=1125-2;
        hdelay=2640-212;
        SPI_write(fd_spi,0x1C,vdelay); // vertical delay
        SPI_write(fd_spi,0x1B,hdelay); // horizonal delay                
#ifdef DEBUG        
    //printf("--------------------\n\r");
    //printf("Adr= 0x4d=%x\n\r",SPI_read(fd_spi,0x4d));
    //printf("Adr= 0x4c=%x\n\r",SPI_read(fd_spi,0x4c));
    //printf("Adr= 0x43=%x\n\r",SPI_read(fd_spi,0x43));
    //printf("Adr= 0x45=%x\n\r",SPI_read(fd_spi,0x45));
    //printf("Adr= 0x47=%x\n\r",SPI_read(fd_spi,0x47));    
#endif    
    }
    if(mode==1){ // 576i
#ifdef DEBUG
        //printf("\\n---chuyen sang 576i");
#endif
        SPI_write(fd_spi,0x4d,4);
        SPI_write(fd_spi,0x4c,0x2);
        SPI_write(fd_spi,0x43,0x1);
        SPI_write(fd_spi,0x45,0x3);  
        SPI_write(fd_spi,0x47,0x5);  
        
                        vdelay=0;
                
                // 0: 28.03
                // 100: 31.741 us
                // 1440-40 : 15.82
                // .. 140: 12.1
                // 240 : 8.44
                // 440 : 1uS
                // 550 : 1.185 advance
                // 450: 0.7 uS
                // 460:L 0.259
                // 465 : 0.148
                hdelay=1440-469;
                                SPI_write(fd_spi,0x1C,vdelay); // vertical delay
                                                SPI_write(fd_spi,0x1B,hdelay); // horizonal delay                

    }
}
// khoi tao GS4911
void Init_GS4911(int fd_spi){

    /*
    while(1){
    SPI_write(fd_spi,0x4d,27);
    
    printf("Readback=%x\n\r",SPI_read(fd_spi,0x4d));
    //     printf("Readback=%x\n\r",SPI_read(fd_spi,0x48));
    usleep(1000000);        
    }*/
    usleep(10000);
    SPI_write(fd_spi,0x4d,27);
    SPI_write(fd_spi,0x4c,0x2);
    SPI_write(fd_spi,0x43,0x2);
    SPI_write(fd_spi,0x45,0x4);   
    SPI_write(fd_spi,0x47,0x6);   

#ifdef DEBUG    
    //printf("Adr= 0x4d=%x\n\r",SPI_read(fd_spi,0x4d));
    //printf("Adr= 0x4c=%x\n\r",SPI_read(fd_spi,0x4c));
    //printf("Adr= 0x43=%x\n\r",SPI_read(fd_spi,0x43));
    //printf("Adr= 0x45=%x\n\r",SPI_read(fd_spi,0x45));
    //printf("Adr= 0x47=%x\n\r",SPI_read(fd_spi,0x47));    
#endif    
}

void InitBoard(int fd,int fd_spi)
{
  

}
//#define TEST

#ifndef TEST

void test(){
    int fd_mem;
    unsigned short *myptr;
fd_mem=open("/dev/mem",O_RDWR);
if(fd_mem<0)
  {
printf("\n------------Error on open memory pointer");
}
myptr=mmap(NULL,0x2000000,PROT_READ|PROT_WRITE,MAP_SHARED,fd_mem,0x10000000);    
myptr[0]=0x0001;
close(fd_mem);
printf(".");
usleep(100000);
}

#endif


void Die(char *mess) { perror(mess); exit(1); }

//#include <stdio.h>
//#include <stdlib.h>

//ham xu ly chuoi de lay vi tri danh sanh va setting cho logo
// SETT pos,setting




int halt_prog;
void* ErrorHandle(void* arg) {	
/*
unsigned short ERROR_STAT_1;
unsigned short ERROR_STAT_2;
unsigned short EDH_FLAG_IN;
unsigned short EDH_FLAG_OUT;
unsigned short AUD_DET2;
unsigned short CSUM_ERR_DET;
char s[100];
*/
unsigned short led_on,led_off,data;


	while(1) {
    
            
            
            time_format = ((SPI_read(fd_spi_gs2971,0x22)>>14)&0x3);            
            // lay bit [15:14] cuar thanh ghi 0x22
            // time_format ==1 or 3 : SD 
            // time_format ==0 HD
            // time_format ==2 SD            
            led_on=0;
            led_off=0;
            //printf("REf=%x",SPI_read(fd_spi_gs4911,0x15));
            //printf("\nAUDSTATUS=%08x",SPI_read(fd_spi_gs2971,0x201));
            if(SPI_read(fd_spi_gs4911,0x15)&(1<<5))
                led_on |= (1<<REF_LED);
            else
                led_off|=(1<<REF_LED);
            if((SPI_read(fd_spi_gs2971,0x22) & (1<<12)))
                    led_on |= (1<<BKGD_LED);
            else
                    led_off|= (1<<BKGD_LED);
            
            if((SPI_read(fd_spi_gs2971_video,0x22) & (1<<12)))
                    led_on |= (1<<VIDEO_LED);
            else
                    led_off|= (1<<VIDEO_LED);
            
            if(fd_spi_gs2971_anpha>0){
            if((SPI_read(fd_spi_gs2971_anpha,0x22) & (1<<12)))
                    led_on |= (1<<ANPHA_LED);
            else
                    led_off |= (1<<ANPHA_LED);
            }
            
//            printf("Word per active line=%d \n\r",SPI_read(fd_spi_gs2971,0x1f));
            
            // if reference is external, key enable
            //printf("Reference mode =%x, mixer.eanble=%x\n\r",reference_param.mode,key_mixer_param.enable);
           
            if(function.frame_sync==0){
                if((reference_param.mode==1) && (key_mixer_param.enable==0)) {
                    if((led_on &((1<<ANPHA_LED)|(1<<VIDEO_LED))) != ((1<<ANPHA_LED)|(1<<VIDEO_LED))) { // lost anpha or video
                        if((key_ref_status & (1<<1))==0) {
                            // turn off external key
                           // printf("--------------------____Tat key\n\r");


                                data=(reference_param.mode&0x01);
                                data |= (1<<1); 
                                key_ref_status = data;
                                //printf("key_ref_status tat key=%x\n\r",key_ref_status);
                                FPGA_write(fd,33,data); 
                        } // end                         

                    }

                     if((led_on &((1<<ANPHA_LED)|(1<<VIDEO_LED))) == ((1<<ANPHA_LED)|(1<<VIDEO_LED)))   { // it is ok

                        if((key_ref_status & (1<<1))!=0) {
                            //printf("--------------------____Bat key\n\r");
                            data=(reference_param.mode&0x01);
                            data=data|((key_mixer_param.enable&0x1)<<1);
                            key_ref_status=data;
                            FPGA_write(fd,33,data);                     
                        }
                   }
                }
            }
            
                
                data =led_on;
                write(fd_led,&data,0);
                data=led_off;
                write(fd_led,&data,4);
                
                //printf("HD_3GPhase =%08x\n\r",SPI_read(fd_spi_gs2971,0x209));

		usleep(500000);
                if(halt_prog==1)
                    break;
                    
	}
}




void Setup_Function(unsigned char license)
{    
    if(license & FRAME_SYNC){
        function.logo_insert=0;
        function.external_key=0;
        function.frame_sync=1;
        function.audio_leveler=0;
        function.audio_comp=0;       
    }
    else{
        function.frame_sync = 0;
        if(license & LOGOINSERT) function.logo_insert=3; // 3 chu ko phai la 1 de tuong thich nguoc voi card cu
        else function.logo_insert=0;
        if(license & KEY) function.external_key=1;
        else function.external_key=0;
        if(license & AUDIOMIXER){}
        if(license & AUDIOCOMP) function.audio_comp=1;
        else function.audio_comp=0;
        if(license & AUDIOLEVEL) function.audio_leveler=1;
        else function.audio_leveler=0;
    }
}

void DieWithError(char *errorMessage)
{
    printf("doan code cua cu Minh");
perror(errorMessage);
exit(1);
}

#define SIG_TEST 44 

long locked_timeout;

//void receiveSignal(int n,siginfo_t *info,void *unused) {
void receiveSignal(int signum) {
    //printf("received value %i\n",info->si_int);
    //printf("received value ---****** signum = %d\n",signum);
    //printf(" Locked signal's changed!\n");   
    locked_timeout = 0;
    //SPI_write(fd_spi_gs2971,0x05E,0x004); // reset audio timming generator    
    //SPI_write(fd_spi_gs2971,0x05E,0x000); // release it
  //  SPI_write(fd_spi_gs2971,0x05E,0x004); // reset audio timming generator    
//    usleep(10);
//    SPI_write(fd_spi_gs2971,0x05E,0x000); // release it
    return;
}



#endif //of HIEN
//---EndOfInclude from HIEN-------------------------------------------------------------------

//Global variables
unsigned char my_id;
const char buf[]="HD-FX1 Universal Video Processing -version 2.2.2";
char my_mac[6];
char my_ip[4];

extern ReportObject reportItems[];
extern StringParam adminNetMacParam;
extern Int32Param adminIpAddrParam;

/*
 * 
 */
//Opengear respone message thread
void * ResponeHandlerThread(void* arg)
{
    while(1)
    {
        OGP_Process();
        usleep(10);
    }
}
//Opengear report thread
void * OgpReportHandlerThread(void* arg)
{
    //Wait for initionlization finished
    sleep(3);
    while(1)
    {
        OGP_CyclicReportParams(reportItems,N_REPORT_ITEMS);
        usleep(10);
    }
}
void signal_handler_SIGFPE(int signum)
{
   // printf("sigfpe caught:%d\n",signum);
}
int main(int argc, char** argv) {
    pthread_t respone_t, ogpReport_t;

    OgpFilter ogpfil[2];
    int i;
    unsigned int ipaddr;
    
    
#ifdef HIEN
    int pid;   
    int audio_state;    
    int level_state;
    double rms,sum_peak,db_tmp;
    double threshold_ch1;
    double maximum_gain;
    int current_gain;
    int n_sample;
    long adr;

    unsigned short j;

    unsigned short data;
    int number_sample;
    struct sigaction sig;


    
    
    
   
#endif //of HIEN    
 /*
      pid = getpid();
  ioctl(fd,20,pid);    // setup PID to map_dev module      
  usleep(200);
  sigemptyset(&sig.sa_mask);
  sig.sa_flags = SA_SIGINFO;
  sig.sa_handler= receiveSignal;
  
  sigaction(SIG_TEST,&sig,NULL);    */    
    
    
    //get MAC addr
    if(getMacAddr("eth0",my_mac)==0)
    {
        printf("MacAddr: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                my_mac[0],my_mac[1],my_mac[2],my_mac[3],my_mac[4],my_mac[5]);
        sprintf(adminNetMacParam.value, "%02X:%02X:%02X:%02X:%02X:%02X",
                my_mac[0],my_mac[1],my_mac[2],my_mac[3],my_mac[4],my_mac[5]);
    }
    else{
        printf("Get mac failed\r\n");
//        while(1);
    }
    //print_addresses();
    getIpAddr("eth0",&ipaddr);
    itob(ipaddr,my_ip);
    adminIpAddrParam.value = ipaddr;
    
    unsigned char chk;

    chk = CheckLicense(my_mac);
    OGP_SetupByLicense(chk);
    my_id = readMyId();
    ogpfil[0].dest = my_id;
    ogpfil[0].source = 0x00;
    ogpfil[1].dest = 0x3f;
    ogpfil[1].source = 0x00;
    //Test Can
    OgpInit(ogpfil,2);

    if(pthread_create(&respone_t,NULL,ResponeHandlerThread,NULL)!=0){
		printf("error: cannot create respone thread");
    }
	else
		pthread_detach(respone_t);
    
    OGP_SendData(OGP_ADDR_PRINT,OGP_PRINT,(BYTE*)buf,strlen(buf)+1);
    usleep(100000);
    OGP_SendRestartState();
    if(pthread_create(&ogpReport_t,NULL,OgpReportHandlerThread,NULL)!=0){
		printf("error: cannot create report thread");
    }
	else
		pthread_detach(ogpReport_t);
    
#ifdef HIEN
    // Open memory device
    fd_led=open("/dev/led_drv",O_RDWR);
    if(fd_led<0) {
       printf("Error on open memory pointer\n\r");
       return(0);
    }
  
    fd = open("/dev/map_dev", O_RDWR);
    if(fd<0)
    {
    printf("\nKhong mo duoc driver");
    return(0);
    }

    fd_spi_gs2971=open("/dev/spidev1.0",O_RDWR);
    if(fd_spi_gs2971<0)
    {
        printf("\nKhong mo duoc driver SPI");
        close(fd_spi_gs2971);
        return(0);
    }
  
    fd_spi_gs2972=open("/dev/spidev1.1",O_RDWR);
    if(fd_spi_gs2972<0)
    {
        printf("\nKhong mo duoc driver SPIDEV1.2");
        close(fd_spi_gs2972);
        return(0);
    }

    fd_spi_gs4911=open("/dev/spidev1.2",O_RDWR);
    if(fd_spi_gs4911<0)
    {
        printf("\nKhong mo duoc driver spi gs4911");
        close(fd_spi_gs4911);
        return(0);
    }
  
    fd_spi_gs2971_video=open("/dev/spidev1.3",O_RDWR);
    if(fd_spi_gs2971_video<0)
    {
        printf("\nKhong mo duoc driver spi gs4911");
        close(fd_spi_gs2971_video);
        return(0);
    }
    fd_spi_gs2971_anpha=open("/dev/spidev1.4",O_RDWR);
    if(fd_spi_gs2971_anpha<0)
    {
        printf("\nKhong mo duoc driver spi gs4911");
        //close(fd_spi_gs2971_anpha);
        //return(0);
    }
  
    ConfigSPI(fd_spi_gs2971_video);
    if(fd_spi_gs2971_anpha>0)
        ConfigSPI(fd_spi_gs2971_anpha);
    ConfigSPI(fd_spi_gs4911);
    ConfigSPI(fd_spi_gs2971);
    ConfigSPI(fd_spi_gs2972);
    j=0;
    i=1;
    printf("\nXuat du lieu");

    //  width=566;
    //  height=328;
    // ghi thu logo
    adr=0;

    InitBoard(fd,fd_spi_gs2971);

    Setup_Function(chk);

    // initialize audio compressor and leveler
    //FPGA_write(fd,19,24067);
    //FPGA_write(fd,19,48000);
    //FPGA_write(fd,20,0); // setup number of time to get pick
    
           
    Change_Gain_Full_CH12(fd,0);

    LevelerInit();


    
     
    


////////////////////////////////
LoadAudioParam(); //Dlay cac thuoc tinh audio tu file cau hinh
TurnOffRelayBypass(fd_led);

#define FPGA_CONFIG
    
#ifdef FPGA_CONFIG

 ioctl(fd,CDPA,0);
 data=0;
 write(fd,&data,4); //
 
  printf("\nSend FPGA configuration file...");
  if(Send_FPGA_Config(fd_spi_gs2972,fd)==0)
     {
	printf("\nError: Cannot send fpga configuration file!");
     }
  else
     printf("\nConfiguration FPGA: Ok");

  
  Send_Reset(fd); // Reset SDI tranceiver

    if(signal(SIGFPE,signal_handler_SIGFPE)==SIG_ERR)
    {
        printf("could not register signal_handler_SIGFPE\n");
        return;
    }

  
  ioctl(fd,CDPA,0);
 data=1;
  write(fd,&data,4); //

  #endif
    printf("Config fpga xong");
  
    ConfigSPI(fd_spi_gs4911);
    ConfigSPI(fd_spi_gs2971);  
  
printf("Bat dau ghi\n");  

data =(1<<0); // power on led
write(fd_led,&data,0);

printf("Thu xong\n");

Send_Reset(fd); // Reset SDI tranceiver

Init_GS4911(fd_spi_gs4911);

SPI_write(fd_spi_gs2971,0x408,0x00ff); // SD setup 24 bit audio samples
SPI_write(fd_spi_gs2971,0x200,0x00f4); // HD setup 24 bit audio samples

Send_Reset(fd); // Reset SDI tranceiver

// dat gia tri cua gain input khi bat dien
//Change_Leveler_Input12(leveler_param.input_gain*10);

if(key_mixer_param.enable==1)
        Set_Key_Mixer_Startup(fd);

if(function.frame_sync!=0){
    Change_Ref_Frame(fd_spi_gs4911,video_mode);
    FPGA_write(fd,20,frame_sync_param.color);
    FPGA_write(fd,19,frame_sync_param.on_loss);
    FPGA_write(fd,21,frame_sync_param.enable); 
}

  Load_Logo_Infor();
  printf("\nSo luong logo=%d",no_logo);
  for(i=0;i<no_logo;++i)
	{
	printf("\n---Logo thu %d",no_logo);
	printf("\nx=%d",logo_list[i].x);
	printf("\ny=%d",logo_list[i].y);
	printf("\nfilename=%s",logo_list[i].file_name);
	}

Setup_Audio_Mixer_Startup(fd); // setup audio when program start up

if(function.audio_comp==1)
{
    FPGA_write(fd,37,audio_delay_param.enabled);       // bypass =1 cho phep chay delay, =0 bo qua delay
    Change_Delay_Audio(fd,audio_delay_param.delay); // do tre tinh bang milisecond
}
    
if(function.audio_leveler==0) // neu khong co option audio leveler thi bypass no
{
    alc_param.enabled =0;
    FPGA_write(fd,27,0);
}
							
// Setup video mode cho FPGA 1080i hay 576i
if(function.frame_sync==0){
				  if(video_mode==0)	// 1080 i
				  {
				    ioctl(fd,0,128);
				    data=0;	
#ifdef SDONLY
                                    data=1;
#endif                                    
				    write(fd,&data,18); // write video_mode
                                    Change_Ref(fd_spi_gs4911,0);
				  }
				  if(video_mode==1)	// 576 i
				  {
				    ioctl(fd,0,128);
				    data=1;							
				    write(fd,&data,18); // write video_mode
				    Change_Ref(fd_spi_gs4911,1);
				  }							
}

// tre nay de chong bat len logo bi loi
usleep(500000);

SetLogoStartup();
//// ket thuc hien thi logo

// bat relay bypass
TurnOnRelayBypass(fd_led);

//printf("\nSetpage");



halt_prog=0; 
 
#ifdef MINH
    pthread_t thread1;
    int x = pthread_create(&thread1, NULL, threadIP, NULL);
        pthread_detach(thread1);
    //pthread_join( thread1, NULL);
#endif  //of MINH
        
        pthread_t thread_handle_audio;
        if(pthread_create(&thread_handle_audio,NULL,thread_audio_leveler,NULL)!=0)
                printf("\nError on create thread audio processor");
        else
            pthread_detach(thread_handle_audio);
       

	pthread_t thread_handle;
	if(pthread_create(&thread_handle,NULL,ErrorHandle,NULL)!=0)
		printf("\nError on create error thread");
	else
		pthread_detach(thread_handle);


	pthread_t child,udp_child;
    unsigned short port = htons(9999);

        int error;
       if((error=pthread_create(&udp_child,NULL,HandleClientUDP,&port)) !=0){
                printf("\r\nError while create UDP child thread");
        }
        else{
                pthread_detach(udp_child);
        }
  
        
        pthread_t thread_handle_remote;
        if(pthread_create(&thread_handle_remote,NULL,RemoteControl,NULL)!=0)
                printf("\nError on create thread remote control");
        else
            pthread_detach(thread_handle_remote);
        
        

  pid = getpid();
  ioctl(fd,20,pid);    // setup PID to map_dev module      
  usleep(200);
  sigemptyset(&sig.sa_mask);
  sig.sa_flags = SA_SIGINFO;
  sig.sa_handler= receiveSignal;
  
  sigaction(SIG_TEST,&sig,NULL);        
        
  
#endif //of HIEN
    
//Reload initialize Opengear interface
  LoadAudioComponents();
  LoadVideoSetupComponents();
  OGP_UpdataActiveLogo(no_logo, active_logo,logo_list[active_logo].file_name);
  OGP_LoadLogoInfor(logoCurIdxParam.value,logo_list);
    while(halt_prog == 0)
    {
        //Check presentation of Control Panel
        if(++ctrlPanelTimeOut >= TICKS_IN_5_SECONDS){
            if(ctrlPanelIsAvailable){
                ctrlPanelIsAvailable = FALSE;
                setModeAlready = FALSE;
                printf("Control Panel removed!\r\n");
            }
            ctrlPanelTimeOut = 0;
        }
        //end check
#ifdef HIEN
        if(locked_timeout<200)
            locked_timeout++;
        if(locked_timeout==100) { // reset it
            printf("Reset audio timming generator!----------------------------------------\n");
           SPI_write(fd_spi_gs2971,0x05E,0x004); // reset audio timming generator    
           usleep(100);
           SPI_write(fd_spi_gs2971,0x05E,0x000); // release it        
        }                    
        usleep(4000);  //4 miliseconds

#endif //of HIEN        
    }
    printf("\nEnd!");
    close(fd_spi_gs2971);
    close(fd);
    close(fd_led);
    
    return (EXIT_SUCCESS);
}

