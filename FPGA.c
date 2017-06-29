
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "FPGA.h"
#include "spicontrol.h"
#include "control.h"

extern struct frame_sync frame_sync_param;
extern struct  func_item function;
extern pthread_mutex_t mutex1;

// fd_spi file handle for SPI driver
// fd file handle for memory mapping driver
void write_hien(int fd_spi,unsigned char data){
    unsigned char my_data;
    my_data=((data>>8)&0xff);    
    write(fd_spi,&my_data,1);
    my_data=(data&0xff);
    write(fd_spi,&my_data,1);
}
unsigned int Send_FPGA_Config(int fd_spi,int fd){
  int fd_config;
  unsigned short data;
  unsigned short read_back[50];
  int no_read;
  int i,j;
  long index;
  
  
  if(function.frame_sync==0) {
     if(function.audio_comp==0)
     {
        fd_config=open("/home/admin/FPGA_I_test.rbf",O_RDONLY);
        printf("\n\rMo file config FPGA cho option Logo insert");
     }
     else
     {
        fd_config=open("/home/admin/FPGA_I_ADELAY.rbf",O_RDONLY);
        printf("\n\rMo file config FPGA cho option Delay Audio");         
     }
  } else {     
     fd_config=open("/home/admin/FPGA_I_FRAME.rbf",O_RDONLY);
     printf("\n\rMo file frame_sync FPGA");
  }      
  
  if(fd_config<0)
  	{
		printf("\nKHong mo duoc file FPGA config");
		return(0);
	}

  // thu thay doi chan nconfig  
 
   ioctl(fd,W_PB,0);   //
  data=(1<<11); //
  write(fd,&data,0);  // nconfig =1;





  usleep(10000);
  
  ConfigSPI(fd_spi);


          
  data=(1<<11); //bit thu 16 la chan reset, set bit on  
  write(fd,&data,0);  // nconfig =1;	
  usleep(1000);
  data=(1<<11);
  write(fd,&data,4); // nconfig =0;  
  usleep(100);
  data=(1<<11); //bit thu 16 la chan reset, set bit on
  write(fd,&data,0);  // nconfig =1;

  
  
  //printf("kiem tra xem nstatus da bang o chua?\n\r");
  while(1)
   {
   read(fd,&data,0);
   usleep(5000);
   if(((data>>15) &1) == 1) /// doi cho den khi nstatus =1
	break;
   }     
  
  
  
  //usleep(5000);   
  j=0;
  while(1)
	{	
      //printf("bom");
        
	   no_read=read(fd_config,read_back,48);           
 	   if(no_read>0)
 	    {
              //do_send_ndata(fd_spi,read_back,no_read/2);              
              write(fd_spi,read_back,no_read);               
            }
	   else
	    break;
	}  
  
  
  read_back[0]=0;
  //do_send_ndata(fd_spi,read_back,1);  
  write(fd_spi,read_back,2);
  read(fd,&data,0);
  close(fd_config);  
  

  if(((data>>14)& 0x1) !=1)     
	return(0);     
  else
	return(1);
  

}
//---------------------------------------------------------------------------------------
unsigned short FPGA_read(int fd,unsigned long adr)
{
    unsigned short data;
    #ifdef LOCK
    pthread_mutex_lock( &mutex1 );
    #endif
    ioctl(fd,0,128);
    read(fd,&data,adr);
    #ifdef LOCK
    pthread_mutex_unlock( &mutex1 );
    #endif
    return(data);
}
//---------------------------------------------------------------------------------------
void FPGA_write(int fd,unsigned long adr,unsigned short data)
{
    #ifdef LOCK
    pthread_mutex_lock( &mutex1 );
    #endif
    ioctl(fd,0,128);    
    write(fd,&data,adr);

    #ifdef LOCK
    pthread_mutex_unlock( &mutex1 );
    #endif
}
//--------------------------------------------------------------------------------------
void FPGA_write_nolock(int fd,unsigned long adr,unsigned short data)
{
    ioctl(fd,0,128);    
    write(fd,&data,adr);
}
//--------------------------------------------------------------------------------------
void Send_Reset(int fd)
{
  unsigned short data;  
  ioctl(fd,W_PA,0);      // chuyen mode ghi vao thanh ghi PIOB set bit	
  // test thu SPI
  //ConfigSPI(fd_spi);
  data=(1<<13);	      //bit thu 29 la chan nconfig, set bit on
  write(fd,&data,4);  //spi_reset =0;  
  usleep(10000);
  data=(1<<13); //bit thu 16 la chan reset, set bit on
  write(fd,&data,0);  // spi_reset =1;
  
  
  ioctl(fd,W_PC_HIGH,0);
  data=(1<<14); // GENLOCK_PIN
  write(fd,&data,4); // set GENLOCK_PIN TO low
  
}
//-----------------------------------------------------------------------------

void Check(int fd)
{
 unsigned short buffer[256];
  //ioctl(fd,1,0x20); //  truy cap dia chi 0x200000    
 ioctl(fd,1,0x20); //  truy cap dia chi 0x100000     
  write(fd,buffer,128);
  printf("chay ok-------------------------\n");
}
