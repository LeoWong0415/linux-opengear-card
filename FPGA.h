/* 
 * File:   FPGA.h
 * Author: vohien
 *
 * Created on April 26, 2012, 7:29 PM
 */

#ifndef FPGA_H
#define	FPGA_H

#define LOCK

void Check(int fd);
void Send_Reset(int fd);
unsigned int Send_FPGA_Config(int fd_spi,int fd);
unsigned short FPGA_read(int fd,unsigned long adr);
void FPGA_write(int fd,unsigned long adr,unsigned short data);
void FPGA_write_nolock(int fd,unsigned long adr,unsigned short data);

#endif	/* FPGA_H */

