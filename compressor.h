/* 
 * File:   compressor.h
 * Author: sau
 *
 * Created on September 5, 2012, 1:28 PM
 */

#ifndef COMPRESSOR_H
#define	COMPRESSOR_H

void Change_GainCH12(int fd,int gain);
void Comp_GainCH12(int fd,int gain);
void Change_Leveler_Input12(int gain);
void Change_Comp_Input12(int gain);
void Change_Delay(int fd,unsigned long delay);
void Change_Delay_Audio(int fd,long delay);

#endif	/* COMPRESSOR_H */

