#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "leveler.h"
#include "control.h"
#include "FPGA.h"

//Global variables
double rms_ch12_in;
double peak_ch12_in;

double rms_ch12_out;
double peak_ch12_out;

double gain_ch12_out;

double peak_com_ch12_in;
double  comp_gain_ch12;
double peak_com_ch12_out;
struct leveler_item leveler_param;
double *peak_buffer_levelerout;
long n_size_levelerout;
long head_levelerout,tail_levelerout;
struct agc_item alc_param;
extern struct auto_mixer auto_mixer_param;
//DELAY_T audio_delay_param = {0,0}; //{disabled, 0ms}

//extern
extern int fd;
extern int halt_prog;
extern struct comp_item comp_param;
//======================================================================================







double *peak_buffer;
long n_size;
long head,tail;
void init_fifo(long nsize)
{
    peak_buffer=(double*)malloc(nsize*sizeof(double));
    n_size=nsize;
    head=0;
    tail=0;
}
void release_fifo()
{
    if(n_size!=0)
     free(peak_buffer);
}
void push(double data){
    if(n_size>0)
    {
        peak_buffer[head]=data;
        if(head!=(n_size-1)) head++;
        else head=0;
    }
}
double long pop()
{    
    double data;
    if(n_size>0)
    {
    data=peak_buffer[tail];
    if(tail!=(n_size-1)) tail++;
    else tail=0;
    return(data);
    }
}


//#define DEBUG_AUDIO
//#define DEBUG_AUDIO_AGC


long            agc_number_sample;  // do sau cua fifo
long            agc_current_sample; // so mau hien tai co trong fifo
double          agc_sum_rms;

// agc_gain la muc gain hien tai cua agc
// sum_sample la trung binh mau trong 100 ms cua 2 mau kenh  tieng 1 2
void agc_process(double sum_sample,double* agc_gain)
{

double          agc_target_gain;        // muc gain mong muon cua agc
double          agc_gain_step;          // muc tang theo thoi gian, tham so nay dat theo AGC time;
//int             agc_time;               // tu 0 den 2 gio, tinh bang giay, 0 -> 7200
//int             agc_rms_time;           // thoi gian tinh rms, tu 0 -> 3600 giay

double          agc_range;
double          agc_bypass_range;
double          op_threshold;
double          gate_threshold;
//////////////////////////////////////////////////

double          agc_rms_db=0;
double          temp_double;
double          rms_in12_db; // rms bang decibel trong 100 ms




agc_range=alc_param.agc_range;
agc_bypass_range = alc_param.agc_bypass_range;
op_threshold = alc_param.op_threshold;
gate_threshold = alc_param.gate_threshold;


if((alc_param.agc_rms_time<=0) && (alc_param.agc_rms_time>=3600))
    alc_param.agc_rms_time=40;

if((alc_param.agc_time<=0) && (alc_param.agc_time>=7200))
    alc_param.agc_time=40;

agc_gain_step = (double)(alc_param.agc_range / (double)(alc_param.agc_time*10));


temp_double =(double)(sum_sample/8388607.0);        
if(temp_double>0.0)        
  rms_in12_db = 20*log10(temp_double);   

#ifdef DEBUG_AUDIO_AGC
printf("\nCH12=%f",rms_in12_db);
#endif

agc_target_gain=0;

if(agc_number_sample == ((long)(alc_param.agc_rms_time)*10))      // neu khong co su thay doi agc_rms_time
{
    
 if(rms_in12_db>gate_threshold) // neu lon hon nguong gate thi moi xu ly
   {    
    if(agc_current_sample< agc_number_sample)
    {
        push_fifo_agc((double)(sum_sample*sum_sample));
        agc_sum_rms=agc_sum_rms+(double)(sum_sample*sum_sample);
        agc_current_sample++;        
    }
    else
    {
#ifdef DEBUG_AUDIO_AGC
        printf("\nBat dau tinh.........................");
#endif
        agc_sum_rms=agc_sum_rms - pop_fifo_agc();
        agc_sum_rms=agc_sum_rms + (sum_sample*sum_sample);
        push_fifo_agc(sum_sample*sum_sample);        
 //       printf("\nAGC_SUM_RMS=%f",agc_sum_rms);
        temp_double = sqrt((agc_sum_rms/((double)agc_number_sample)))/8388607.0;
        if(temp_double>0)
                agc_rms_db = 20*log10(temp_double);// tinh ra decibel
        
        
        
        if(agc_rms_db>gate_threshold) // neu lon hon nguong gate thi moi xu ly
        {
            if(agc_rms_db>=op_threshold)
            {
                if((agc_rms_db-op_threshold)>agc_bypass_range)
                {
                    if((agc_rms_db-op_threshold)>agc_range)
                    {
                        // can giam di bang khoang range
                        agc_target_gain = -agc_range;
                    }
                    else
                    {
                        agc_target_gain = op_threshold -agc_rms_db;
                    }
                   #ifdef DEBUG_AUDIO_AGC                    
                   printf("\nAGC giam gain=%f",agc_target_gain);
                   #endif

                }
                else
                {
                    agc_target_gain = 0; // reset gain
                    #ifdef DEBUG_AUDIO_AGC
                   printf("\nAGC reset gain");
                   #endif
                }

            }
            else
            {
                if((op_threshold-agc_rms_db)>agc_bypass_range)
                {
                    if((op_threshold-agc_rms_db)>agc_range)
                    {
                        // tang len bang range
                        agc_target_gain = agc_range;                         
                    }
                    else
                    {
                        agc_target_gain = op_threshold- agc_rms_db;
                    }
                    #ifdef DEBUG_AUDIO_AGC
                    printf("\nAGC tang gain=%f",agc_target_gain);
                    #endif

                }
                else
                {
                    agc_target_gain =0; // reset gain
                    #ifdef DEBUG_AUDIO_AGC
                    printf("\nAGC reset gain");
                   #endif
                }
            }
        }
        else
        {
                    agc_target_gain =0; // reset gain
                    #ifdef DEBUG_AUDIO_AGC
                    printf("\nAGC reset gain do duoi nguong gate");
                    #endif
        } 
            


        if(fabs(*agc_gain-agc_target_gain)>agc_gain_step)
        {
            if(*agc_gain>agc_target_gain)
                *agc_gain = *agc_gain - agc_gain_step;
            else
                *agc_gain = *agc_gain + agc_gain_step;
        }
    #ifdef DEBUG_AUDIO_AGC


    #endif        
        // doan xu ly gain               
    }
    
  } 
    
    
}
else    // neu co su thay doi agc_rms_time thi phai khoi tao lai fifo agc
{
    agc_number_sample = (long)(((long)(alc_param.agc_rms_time))*10);
    release_fifo_agc();
    init_fifo_agc(agc_number_sample); // khoi tao fifo voi so mau bang agc_rms_time*10 vi moi mau cach nhau 100 ms
    agc_sum_rms=0;        
    agc_current_sample=0;    
    agc_target_gain =0; // reset gain
    *agc_gain=0;
    // thay doi gain ve o    
    #ifdef DEBUG_AUDIO_AGC
    printf("\ncan khoi tao lai.....................");           
    #endif
    
}




#ifdef DEBUG_AUDIO_AGC
printf("\nagc_gain_step = %f",agc_gain_step);
printf("\nAGC rms in decibel =%f",agc_rms_db);
printf("\nagc_number_sample=%d",agc_number_sample);
printf("\nAGC_gain=%f",*agc_gain);

#endif
    
}

void transient_process(double sum_sample,double* transient_gain)
{
int           number_sample;        
int           n_sample;        
double        sum_peak;
// transient_bypass_range;
double        op_threshold;             // muc am thanh mong muon 
double        transient_range;          // dai am thanh se tang hoac giam
double        transient_bypass_range;       // dai am thanh khong xu ly, neu muc do am thanh nam trong khoang nay thi khong xu ly
double        transient_target_gain;            // muc gain ma bo transient tinh duoc
//double        transient_gain;                   /// gia tri gain cua transient hien tai
double        peak_hold_in12_db; // gia tri muc nang luong tinh theo decibel cua kenh 1 va 2 trong 100 ms
double        transient_gain_step; // muc do tang hoac giam gain o moi 100 ms, tham so nay quyet dinh toc do tang giam nhanh hay cham
double        gate_threshold;           // duoc muc nay  khong xu ly
double        transient_time;           // thoi gian tang giam gain db/s duoc tinh bang giay
double        temp_double;
double        db_tmp;
double        rms;

double        db_ch1;
/*
 * Vay transient_processing co nhung tham so sau
 * + op_threshold: tinh theo decibel tu 0 den -40 decibel
 * + transient_range: tinh theo decibel tu 0 den 30 decibel
 * + transient_bypass_range: tinh theo decibel tu 0 den 30 decibel
 * + transient_gain_step: tinh theo decibel tu 0 den 4 decibel
 * 
 * Vi du hien tai
 *   op_threshold=-18
 *   transient_range = 6
 *   transient_bypass_range = 6
 *   transient_gain_step = 0.2
 *   gate_threshold = -50
 */    


gate_threshold = alc_param.gate_threshold;
op_threshold = alc_param.op_threshold;
transient_range= alc_param.transient_range;
transient_bypass_range = alc_param.transient_bypass_range;
transient_time = alc_param.transient_time; 


transient_gain_step= (double)(transient_time / 10.0);

//op_threshold = -30.0tran
                
number_sample=10; // tao 1 fifo co do sau 10 mau
release_fifo();
init_fifo(number_sample);
n_sample=0;
sum_peak=0;
transient_target_gain=0;


        // tinh duoc trung binh mau theo decibel
temp_double =(double)(sum_sample/8388607.0);        
if(temp_double>0.0)        
  peak_hold_in12_db = 20*log10(temp_double);                    
#ifdef DEBUG_AUDIO
printf("\nCH12=%f",peak_hold_in12_db);
#endif

rms_ch12_in = peak_hold_in12_db;            

/*
// peak_hold_in12_db chua trung binh mau theo decibel        
// tinh trung binh mau trong number_sample, binh thuong de la 10 mau tuong duong 100 ms.
if(n_sample<number_sample)
{
        db_tmp=sum_sample;                
        push(db_tmp*db_tmp);
        sum_peak=sum_peak+(double)(db_tmp*db_tmp);
        n_sample++;
}
else
{        
        db_tmp=sum_sample;                
        sum_peak=sum_peak-pop();
        sum_peak=sum_peak+(double)(db_tmp*db_tmp);
        push(db_tmp*db_tmp);
        rms=sqrt(sum_peak/number_sample);

        temp_double=(double)(rms/8388607.0);
        if(temp_double>0)
                db_ch1 = 20*log10(temp_double);               
        rms_ch12_in = db_ch1;            
        #ifdef DEBUG_AUDIO
        printf("\nRMS CH12=%f",rms_ch12_in);
        #endif
}
*/

// qua day tinh duoc rms_ch12_in chinh la trung binh muc nang luong trong number_sample=10, khoang 1 giay

if(peak_hold_in12_db>gate_threshold)
{
if(peak_hold_in12_db > op_threshold)
{
    if((peak_hold_in12_db-op_threshold)>=transient_bypass_range)
    {
        if((peak_hold_in12_db-op_threshold)>transient_range)
        {
                #ifdef DEBUG_AUDIO
                printf("\nCan giam %f DB",transient_range);
                #endif
                transient_target_gain = -transient_range;                        
        }
        else
        {
                #ifdef DEBUG_AUDIO
                printf("\nCan giam=%f",op_threshold-peak_hold_in12_db);
                #endif
                transient_target_gain=op_threshold-peak_hold_in12_db;                        
        }
    }
    else
    {
        #ifdef DEBUG_AUDIO
        printf("\nReset gain");
        #endif
        transient_target_gain=0;                
    }


}
else
{
    if((op_threshold-peak_hold_in12_db)>=transient_bypass_range)
    {
        if((op_threshold-peak_hold_in12_db)>transient_range)
        {
                #ifdef DEBUG_AUDIO
                printf("\nCan tang %f DB",transient_range);                        
                #endif  
                transient_target_gain = transient_range;
        }
        else
        {
                #ifdef DEBUG_AUDIO
                printf("\nCan tang=%f",op_threshold-peak_hold_in12_db);                        
                #endif
                transient_target_gain = op_threshold-peak_hold_in12_db;
        }
    }
    else
    {
        #ifdef DEBUG_AUDIO
        printf("\nReset gain");
        #endif                
        transient_target_gain = 0;
    }
 }
}
else
{
    transient_target_gain=0;
}


if(fabs((*transient_gain-transient_target_gain))>transient_gain_step)
{
    
        if((*transient_gain)>transient_target_gain)
                *transient_gain = (*transient_gain - transient_gain_step);
        else
                *transient_gain = (*transient_gain + transient_gain_step);
     
}


#ifdef DEBUG_AUDIO
printf("\nCurrent transient gain=%f",*transient_gain);
printf("\nCH12=%f",peak_hold_in12_db);
printf("\ntransient_gain_step=%f",transient_gain_step);
#endif                                       

}

void * thread_audio_leveler(){
  
double  transient_gain; // gain tinh duoc khi qua bo xu ly nhanh transient    
double  agc_gain;
double  system_gain;



unsigned long long peak_hold_in12; // gia tri mau trung binh cho kenh 1 va 2 o 100 ms

unsigned long long peak_hold12;

double db_ch12_levelerout;
int number_sample_levelerout;
double rms_levelerout,sum_peak_levelerout;


int time;
int ch34_status;  // ==0, kenh 34 lay goc, ==1 kenh 34 copy tu kenh 12


int audio_state;    
int level_state;
int bypass;

double threshold_ch1;
double  threshold_noise;
double maximum_gain;
unsigned char c12_copy;

int current_gain;

int counter;
int cnt_time_out;
int counter_comp;
int attack_counter;
int release_counter;
int state_levelerout;

//fifo ch12_fifo;

struct timeval tv,tv_new;
unsigned int interval;
double rms_ch12;

double sum_sample;
double temp_double;
double peak_in_decibel;
bypass=1;

FPGA_write(fd,19,24067);
FPGA_write(fd,20,0); // setup number of time to get pick


//FPGA_write(fd,40,1);// copy 1,2 -> 3,4


audio_state=0;



threshold_noise = -40; // - 40 db for noise
threshold_ch1=-28; // -28 db
maximum_gain = 12; // 8 db
level_state=0;
current_gain =0;        
Change_GainCH12(fd,current_gain);
gain_ch12_out = (double)(current_gain/10);
counter=0; // reset counter

state_levelerout=0;
rms_levelerout=0;
sum_peak_levelerout=0;
number_sample_levelerout=0;














//FPGA_write(fd,33,1); // su dung leveler

//FPGA_write(fd,33,1); // bo qua compressor



counter = 0;



FPGA_write(fd,32,1); // reset
FPGA_write(fd,32,0); // reset



Change_Gain_Full_CH12(fd,0);                





//////////////////////////////
//***********************************dskjfdskj
// nho bo cai nay
/*
alc_param.gate_threshold=-50;
alc_param.op_threshold = -18;
alc_param.agc_range = 10; // tang giam 10 dB
alc_param.agc_bypass_range = 3; // trong khoang 3 db so voi op_threshold
alc_param.agc_time = 40; // agc_time 40 giay
alc_param.agc_rms_time = 40;// 40 giay
*/



/*

alc_param.transient_range=6; // tang giam 4 dB
alc_param.transient_bypass_range = 6;
alc_param.transient_time = 2; 
*/

////
//////////////////////////////////
if(alc_param.agc_rms_time<=0)
    alc_param.agc_rms_time=1;

agc_number_sample = (long)(((long)(alc_param.agc_rms_time))*10);
agc_current_sample=0;
release_fifo_agc();
init_fifo_agc(agc_number_sample); // khoi tao fifo voi so mau bang agc_rms_time*10 vi moi mau cach nhau 100 ms
agc_sum_rms=100000;        

//
transient_gain=0;
agc_gain =0;
bypass=0;
FPGA_write(fd,27,0);

cnt_time_out=0;
time=0;

FPGA_write(fd,19,4800);
FPGA_write(fd,20,0); // setup number of time to get pick

auto_mixer_param.is_copy12=0; // khong can copy tu ch1, ch2
FPGA_write(fd,40,0);// 3,4 mac dinh
ch34_status = 0; // 34 goc
while(1)
{
    
    if(bypass!=alc_param.enabled)
    {
        bypass=alc_param.enabled;
        if(bypass==0)  // bo qua leveler gain
            FPGA_write(fd,27,0);
        else
            FPGA_write(fd,27,1);            
    }
    
    if(counter==0)
    {
       // time++;
       // printf("\nTime=%d",time);
       cnt_time_out++;
       if(cnt_time_out==20)
       {
           cnt_time_out=0;
           FPGA_write(fd,32,1); // reset
           usleep(20);
           FPGA_write(fd,32,0); // reset           
           
       }        
    }
    
    if((counter==0)) // xuat hien o 100 ms
    {

#ifdef DEBUG_AUDIO       
        printf("\nalc_param.transient_range=%f",alc_param.transient_range);
        printf("\nalc_param.transient_bypass_range=%f",alc_param.transient_bypass_range);
        printf("\nalc_param.transient_time=%f",alc_param.transient_time);
        printf("\nalc_param.enabled=%d",alc_param.enabled);
#endif        

#ifdef DEBUG_AUDIO_AGC
        printf("\nalc_param.agc_range=%f",alc_param.agc_range);
        printf("\nalc_param.agc_rms_time=%d",alc_param.agc_rms_time);
        printf("\nalc_param.agc_time%d",alc_param.agc_time);
        printf("\nalc_param.agc_bypass_range=%f",alc_param.agc_bypass_range);
#endif        
        // Lay tong cac mau trong 100 ms, tinh duoc la tong binh phuong cac mau chi cho 4096 mau
        //peak_hold12
        peak_hold12=(unsigned long long)FPGA_read(fd,25);
        peak_hold12=peak_hold12 | (unsigned long long)(FPGA_read(fd,26))<<16;        

        if(peak_hold12==0)
            peak_hold12=1;
               
        
        temp_double =(double)(peak_hold12/8388607.0);        
        if(temp_double>0.0)        
            peak_in_decibel = 20*log10(temp_double);                            
        
        auto_mixer_param.ch34_level = peak_in_decibel;
        

        if(auto_mixer_param.is_auto==1)
        {
            if(auto_mixer_param.ch34_level<auto_mixer_param.check_threshold)
            {
                // neu muc tin hieu kenh 3,4 be hon nguong thi copy tu kenh 1,2
                if(auto_mixer_param.is_copy12==0)
                {
                    auto_mixer_param.is_copy12=1;
                    FPGA_write(fd,40,1);// copy 1,2 -> 3,4
                    ch34_status=1;
                }            

            }
            else
            {
                if(auto_mixer_param.is_copy12==1)
                {
                    // tra tro ve
                    auto_mixer_param.is_copy12=0; // khong can copy 12 -> 34
                    FPGA_write(fd,40,0);// mac dinh
                    ch34_status=0;
                }
            }
        }
        else
        {
            if(auto_mixer_param.is_copy12==0)
            {
                if(ch34_status==1)// neu dang copy tu 12
                {
                    FPGA_write(fd,40,0);// chuyen ve 34 lay tu goc
                    ch34_status=0;
                }
            }
            else
            {
                if(ch34_status==0)// neu dang lay tu goc
                {
                    FPGA_write(fd,40,1);// chuyen ve lay tu 1,2
                    ch34_status=1;
                }                
            }
        }
        /*
               
        printf("\nCH34=%f",auto_mixer_param.ch34_level);
        printf("\nThreshold=%f",auto_mixer_param.check_threshold);
        if(auto_mixer_param.is_copy12==0)            
                printf("\nKenh 34 goc");
        else
            printf("\nKenh 34 copy tu 12");
        printf("\nAuto=%d",auto_mixer_param.is_auto);
        */
        if((cnt_time_out!=0))
        {
        peak_hold_in12=(unsigned long long)FPGA_read(fd,21);
        peak_hold_in12=peak_hold_in12 | (unsigned long long)(FPGA_read(fd,22))<<16;
        peak_hold_in12=peak_hold_in12 | (unsigned long long)(FPGA_read(fd,23))<<32;
        }
        //printf("\npeak_hold_in12=%llx",peak_hold_in12);
                
        rms_ch12 = sqrt(peak_hold_in12); // lay duoc gia tri trung binh mau
        if(rms_ch12==0)
            rms_ch12=1;        
        
        // xu ly transient gain
        transient_process(rms_ch12,&transient_gain);                
        // xu ly agc        
        agc_process(rms_ch12,&agc_gain);
        
        system_gain = transient_gain+agc_gain;
        
        if(alc_param.enabled==1)
                gain_ch12_out = system_gain;
        else
            gain_ch12_out = 0;
        
#ifdef DEBUG_AUDIO
        printf("\nTransient gain=%f",transient_gain);
        printf("\nAGC gain=%f",agc_gain);
        printf("\nSystem gain=%f",system_gain);
#endif        
        Change_Gain_Full_CH12(fd,system_gain);


        
        
        
    }    
    if(halt_prog)       // neu chuong trinh chinh dung thi dung thread luon
        break;
    
    if(counter>=10)
        counter=0;
    else
       counter++;                      
    usleep(10000); // sleep for 10 ms    
}

}


//------------------------------------------------------------------------------------
void LevelerInit(void)
{
    leveler_param.leveler_rms_time=10000; // dat mac dinh la 10 seconds
    leveler_param.leveler_low_threshold=-30;
    leveler_param.leveler_background_threshold=-40;
    leveler_param.leveler_maximum_gain=12;
    leveler_param.leveler_release_time=100;
    leveler_param.leveler_attack_time=100;
    leveler_param.bypass=0; // su dung leveler
    leveler_param.input_gain = 0;
    
}
//------------------------------------------------------------------------------------
void init_fifo_agc(long nsize)
{
    peak_buffer_levelerout=(double*)malloc(nsize*sizeof(double));
    n_size_levelerout=nsize;
    head_levelerout=0;
    tail_levelerout=0;
}
//------------------------------------------------------------------------------------
void release_fifo_agc()
{
    if(n_size_levelerout!=0)
     free(peak_buffer_levelerout);
}
//------------------------------------------------------------------------------------
void push_fifo_agc(double data)
{
    if(n_size_levelerout>0)
    {
        peak_buffer_levelerout[head_levelerout]=data;
        if(head_levelerout!=(n_size_levelerout-1)) head_levelerout++;
        else head_levelerout=0;
    }
}
//------------------------------------------------------------------------------------
double long pop_fifo_agc()
{    
    double data;
    if(n_size_levelerout>0)
    {
    data=peak_buffer_levelerout[tail_levelerout];
    if(tail_levelerout!=(n_size_levelerout-1)) tail_levelerout++;
    else tail_levelerout=0;
    return(data);
    }
}