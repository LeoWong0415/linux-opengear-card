/* 
 * File:   leveler.h
 * Author: sau
 *
 * Created on September 5, 2012, 10:09 AM
 */

#ifndef LEVELER_H
#define	LEVELER_H

void * thread_audio_leveler();
void LevelerInit(void);
void init_fifo_agc(long nsize);
void release_fifo_agc();
void push_fifo_agc(double data);
double long pop_fifo_agc();
void agc_process(double sum_sample,double* agc_gain);
void transient_process(double sum_sample,double* transient_gain);
void Change_Gain_Full_CH12(int fd,double gain);

#endif	/* LEVELER_H */

