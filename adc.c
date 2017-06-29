/* 
 * File:   main.c
 * Author: sau
 *
 * Created on August 27, 2012, 10:06 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "adc.h"
/*
 * 
 */
#define DEBUG_LEVEL0
#define ADC_READ_CHANNEL_0  1
//---------------------------------------------------------
unsigned char readMyId()
{
    int dev;
    unsigned int adc_value,sum,i;
    unsigned char id;
    
    //open driver
    dev = open("/dev/adcdev", O_RDONLY);
    if (dev < 0) {
        printf("Cannot open ADC driver\r\n");
        return (0);
    }

	sum=0;
	for(i=0;i<10;i++){
        ioctl(dev, ADC_READ_CHANNEL_0, &adc_value);
		sum+=adc_value;
	}
	adc_value=sum/10;
    if(adc_value > 16) adc_value -= 16;
    if(adc_value > 1007) id = 1;
    else if(adc_value > 975) id = 2;
    else if(adc_value > 943) id = 3;
    else if(adc_value > 911) id = 4;
    else if(adc_value > 879) id = 5;
    else if(adc_value > 847) id = 6;
    else if(adc_value > 815) id = 7;
    else if(adc_value > 783) id = 8;
    else if(adc_value > 751) id = 9;
    else if(adc_value > 719) id = 10;
    else if(adc_value > 678) id = 11;
    else if(adc_value > 656) id = 12;
    else if(adc_value > 624) id = 13;
    else if(adc_value > 592) id = 14;
    else if(adc_value > 560) id = 15;
    else if(adc_value > 528) id = 16;
    else if(adc_value > 496) id = 17;
    else if(adc_value > 464) id = 18;
    else if(adc_value > 432) id = 19;
    else if(adc_value > 400) id = 20;
    else if(adc_value > 368) id = 21;
    else if(adc_value > 336) id = 22;
    else if(adc_value > 304) id = 23;
    else if(adc_value > 272) id = 24;
    else if(adc_value > 240) id = 25;
    else if(adc_value > 208) id = 26;
    else if(adc_value > 176) id = 27;
    else if(adc_value > 144) id = 28;
    else if(adc_value > 112) id = 29;
    else if(adc_value > 80) id = 30;
    else if(adc_value > 48) id = 31;
    else id = 32;
//	id=(unsigned char)(32-((adc_value+16)/31)+1);
	printf("My ID: %02d <=> 0x%02X\r\n",id,(id+0x10));
	return id + 0x10;
}


