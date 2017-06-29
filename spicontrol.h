/* 
 * File:   spicontrol.h
 * Author: vohien
 *
 * Created on April 26, 2012, 6:03 PM
 */



#ifndef SPICONTROL_H
#define	SPICONTROL_H

#include "types.h"


#define WS 0  // write single byte
#define WB 1  // write block 
#define WR 3  // write to PIO register
#define R_PC    4       //read port C
#define W_PC    5       //WRITE PORTC
#define R_PB	6
#define R_PHY	7
#define W_PB	8
#define W_PA	9
#define CDPA	10
#define W_PC_HIGH    11       //WRITE PORTC HIGH WORD

static void pabort(const char *s);
void ConfigSPI(int fd);
void do_send_data(int fd,__u16 data);
void do_send_ndata(int fd,__u32 *data,int len);
unsigned short SPI_write(int fd,unsigned short adr,unsigned short value);
unsigned short SPI_read(int fd,unsigned short adr);

#endif	/* SPICONTROL_H */

