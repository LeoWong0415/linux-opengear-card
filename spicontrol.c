
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <sys/types.h>

#include "spicontrol.h"
#include "types.h"
#include "spidev.h"
#include <sys/ioctl.h>
#include <sys/types.h>
///-----------------------------------------------

static __u8 mode = 0;
static __u8 bits = 8;
static __u32 speed = 800000*10;  /* 400 Khz */
//static __u32 speed = 800000*4;  /* 400 Khz */
static __u16 delay=1;
static __u8 lsb =1;
static void pabort(const char *s)
{
	perror(s);
	abort();
}

void ConfigSPI(int fd){
	int ret=0;
	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");


	/*
	 * bits per word
	 */     
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);  
	if (ret == -1)
		pabort("can't set bits per word");


	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

  /*
        ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb);
	if (ret == -1)
		pabort("can't set lsb");
*/
//SPI_IOC_WR_LSB_FIRST

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

}
///------------------------------------------------
// send data to SPI
//Function send message to slave
void do_send_data(int fd,__u16 data)
{
	struct spi_ioc_transfer	xfer[2];
	int			status;
	__u16 tx[10];
	__u16 rx[10];

	memset(xfer, 0, sizeof xfer);
	tx[0]=data;

	//rx[0] = 0xaa;
	xfer[0].tx_buf = (__u32)(tx);
	xfer[0].len = 2;
        xfer[0].delay_usecs=delay;

	xfer[1].rx_buf = (__u32)rx;
	xfer[1].len = 2;
        xfer[1].delay_usecs=delay;

	status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
	if (status < 0) {
		perror("SPI_IOC_MESSAGE");
		return;
	}
	//printf("\nSent");
}

///------------------------------------------------
// send data to SPI
//Function send message to slave
void do_send_ndata(int fd,__u32 *data,int len)
{
	struct spi_ioc_transfer	xfer[2];
	int			status;
	__u16 tx[32];
	__u16 rx[32];

	memset(xfer, 0, sizeof xfer);

	xfer[0].tx_buf = (__u32)data;
	xfer[0].len = 2*len;

	xfer[1].rx_buf = (__u32)rx;
	xfer[1].len = 0*len;

	status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
	if (status < 0) {
		perror("SPI_IOC_MESSAGE");
		return;
	}
	//printf("\nSent");
}

/***********************************************************************
* SPI_write -- write data from spi devices
*
* Parameters 
*	fd	-- file handle to spi controller
*	adr	-- address register of spi device
*	value	-- value data we need put to it's address
* Returns
*	none-zero when error occur
************************************************************************/
unsigned short SPI_write(int fd,unsigned short adr,unsigned short value)
{
    unsigned short data[2];
    unsigned short temp;
    struct spi_ioc_transfer	xfer;
    int			status;
    __u16 tx[32];
    __u16 rx[32];
    /*temp=adr;
    temp
    temp=temp&0x7fff;*/
    
    temp = 0;
    temp |= ((adr>>8) & 0xf);
    temp |= ((adr&0xff)<<8);
    
    
    data[0]=temp;
    
    //printf("adr=%x",data[0]);
    temp=0; 
    temp |= ((value>>8) &0xff);
    temp |= ((value&0xff)<<8);
    
    data[1]=temp;
    //printf("value=%x\n\r",data[1]);
    
       
    ;
    memset(&xfer, 0, sizeof xfer);
    xfer.tx_buf = (__u32)data;
    xfer.len = 4;
    xfer.rx_buf = (__u32)rx;	      	 		
    status = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (status < 0) {
	  perror("SPI_IOC_MESSAGE");
	  return(-1);    
    }    
    
}


/***********************************************************************
* SPI_read -- read data from spi devices
*
* Parameters 
*	fd	-- file handle to spi controller
*	adr	-- address register of spi device
* Returns
*	value from address register
************************************************************************/
unsigned short SPI_read(int fd,unsigned short adr)
{
    unsigned short data[2];    
    unsigned short temp;
    struct spi_ioc_transfer	xfer;
    int			status;
    __u16 tx[32];
    __u16 rx[32];
    temp=adr;
    temp=temp|0x8000;
    //printf("Adr=%x\n\r",adr);
    
    temp = 0;
    temp |= (((adr>>8) & 0xf) | 0x80);    
    temp |= ((adr&0xff)<<8);    
    
    data[0]=temp;
    //printf("Read adr=%x\n\r",data[0]);    
        
    data[1]=0x0000;
    
   
   
            
    memset(&xfer, 0, sizeof xfer);
    xfer.tx_buf = (__u32)data;
    xfer.len = 4;
    xfer.rx_buf = (__u32)rx;	      	 		
    status = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (status < 0) {
	  perror("SPI_IOC_MESSAGE");
	  return(-1);    
    }    	      
//    printf("rx[1]=%x\n\r",rx[1]);
    temp=0;
    temp |= ((rx[1]>>8) & 0xff);
    temp |= (((rx[1]>>0) & 0xff)<<8);
//    printf("temp=%x\n\r",temp);
    return(temp);
    
}
