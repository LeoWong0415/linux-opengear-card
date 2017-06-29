/* 
 * File:   can_lv.c
 * Author: sau
 * Decriptions: Process low-level CAN interface
 * Created on August 27, 2012, 1:02 PM
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

#include "can_lv.h"
#include "ogp.h"
//Defines


//Global variables
struct can_filter *filter = NULL;
int filter_count = 0;
int can_sock = -1;

//Functions
//==================================================================================
int add_filter(u_int32_t id, u_int32_t mask)
{
	filter = realloc(filter, sizeof(struct can_filter) * (filter_count + 1));
	if(!filter)
		return -1;

	filter[filter_count].can_id = id;
	filter[filter_count].can_mask = mask;
	filter_count++;

	printf("id: 0x%08x mask: 0x%08x\n",id,mask);
	return 0;
}
//----------------------------------------------------------------------------------
//Try to open Socket-Can, return 0 if success. 
//Global variable used to access Socket can
//struct can_filter* filt is set via add_filter()
//nfil: number of filter want to use
//Note: if do not want to use filter, call CanInit(null,0);
int OgpInit(OgpFilter * filt, int nfil)
{
	struct can_frame frame;
	struct ifreq ifr;
	struct sockaddr_can addr;
	int loopcount = 1, infinite = 0;
	int ret, dlc = 0;
    char* interface = "can0";
    int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
    int nbytes, i;
	uint32_t id, mask;
	int n = 0, err;
    
	if ((can_sock = socket(family, type, proto)) < 0) {
		perror("CAN socket");
		return 1;
	}

	addr.can_family = family;
	strncpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));
	if (ioctl(can_sock, SIOCGIFINDEX, &ifr)) {
		perror("CAN-sock ioctl");
		return 1;
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(can_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("CAN-sock bind");
		return 1;
	}

	if (nfil > 0) {
        for(i=0;i<nfil;i++)
            OgpAddFilter(&filt[i]);
		if (setsockopt(can_sock, SOL_CAN_RAW, CAN_RAW_FILTER, filter,
			       filter_count * sizeof(struct can_filter)) != 0) {
			perror("CAN-sock set option");
			exit(1);
		}
	}
    
    return 0;
}
//----------------------------------------------------------------------------------
//Read CANbus, return number of bytes read
//Use global variable can_sock
//Read data transfer to can_fram * fr
int CanRead(struct can_frame * fr)
{
    return read(can_sock, fr, sizeof(struct can_frame));
}
//----------------------------------------------------------------------------------
// P PVVS SSSS SDDD DDDM MMMM MMMF FFFF
#define FLAG_MASK   0x0000001F
#define MESS_MASK   0x00001FE0
#define DEST_MASK   0x0007E000
#define SRC_MASK    0x01F80000
#define VER_MASK    0x06000000
#define PRIO_MASK   0x18000000

#define FLAG_POS    0
#define MESS_POS    5
#define DEST_POS    13
#define SRC_POS     19
#define VER_POS     25
#define PRIO_POS    27
//#define DEBUG       1
//Convert Can frame to OpenGear short frame
void CanToOgp(struct can_frame * can, OgpShortFrame * ogp)
{
    unsigned int id = can->can_id;
    int i;
    //Convert CAN ID to OpenGear Header
    //can_id[32bit] = [1bit CAN_EFF][1bit CAN_RTR][1bit ERR_FRAME][EID{28:0}]
    //OgpHeader= PPVV SSSS SSD | D DDDD MMMM MMMM FFFF Fzzz
    ogp->header.value = id;
    ogp->header.flags = (unsigned char)(id & FLAG_MASK);
    ogp->header.msgType = (unsigned char)((id & MESS_MASK) >> MESS_POS);
    ogp->header.dest = (unsigned char)((id & DEST_MASK) >> DEST_POS);
    ogp->header.source = (unsigned char)((id & SRC_MASK) >> SRC_POS);
    ogp->header.version = (unsigned char)((id & VER_MASK) >> VER_POS);
    ogp->header.priority = (unsigned char)((id & PRIO_MASK) >> PRIO_POS);
    ogp->length = can->can_dlc;
    memcpy(ogp->data,can->data,can->can_dlc);
//#ifdef DEBUG
//    printf("Value: %08X\r\n",ogp->header.value);
//    printf("Sour:  %02X\r\n",ogp->header.source);
//    printf("Dest:  %02X\r\n",ogp->header.dest);
//    printf("Mess:  %02X\r\n",ogp->header.msgType);
//    printf("Flags: %02X\r\n",ogp->header.flags);
//    for(i=0;i<ogp->length;i++)
//        printf("%02X ",ogp->data[i]);
//    printf("\r\n");
//#endif
}
//----------------------------------------------------------------------------------
//Convert OpenGear short frame to CAN frame
void OgpToCan(OgpShortFrame * ogp, struct can_frame * can)
{
/*
    printf("Flags: %02X\r\n",ogp->header.flags);
    printf("Mess:  %02X\r\n",ogp->header.msgType);
    printf("Dest:  %02X\r\n",ogp->header.dest);
    printf("Sour:  %02X\r\n",ogp->header.source);
*/
    can->can_dlc = ogp->length;
    can->can_id = ((unsigned int) ogp->header.flags |
            ((unsigned int) ogp->header.msgType << MESS_POS) |
            ((unsigned int) ogp->header.dest << DEST_POS) |
            ((unsigned int) ogp->header.source << SRC_POS) |
            ((unsigned int) ogp->header.version << VER_POS) |
            ((unsigned int) ogp->header.priority << PRIO_POS) |
            CAN_EFF_FLAG);
    memcpy(can->data, ogp->data,ogp->length);
/*
    printf("can_id: 0x%08X\r\n",can->can_id);
    printf("can_dlc: %d\r\n",can->can_dlc);
*/
}
//----------------------------------------------------------------------------------
void OgpAddFilter(OgpFilter * ogp_fil)
{
    unsigned int mask = 0x1FFE000;
    unsigned int id = (((unsigned int)ogp_fil->dest << DEST_POS) |
        ((unsigned int)ogp_fil->source << SRC_POS));
    add_filter(id,mask);
}
//----------------------------------------------------------------------------------
//Write a struct Can_frame to Canbus, return -1 if fail
//Use global variable can_sock opened
//You should modify constants MAX_RETRY and WAIT_TIME for optimized performance
#define MAX_RETRY   3
#define WAIT_TIME   100
int CanWrite(struct can_frame* can_fr)
{
    int ret,count=0;
    do{
        ret = write(can_sock, can_fr, sizeof(struct can_frame));
        if(ret == -1){
            usleep(WAIT_TIME);
        }
    }
    while((ret == -1) && (++count < MAX_RETRY));
    return ret;
}
//----------------------------------------------------------------------------------
int OgpWrite(OgpShortFrame * fr)
{
    int ret;
    struct can_frame can;
    OgpToCan(fr,&can);
    ret = CanWrite(&can);
    if(ret == -1)
        printf("Ogp write failed\r\n");
    return ret;
}
//----------------------------------------------------------------------------------
int OgpRead(OgpShortFrame * fr)
{
    int ret;
    struct can_frame can;
    ret = CanRead(&can);
    if(ret < 0){
        printf("Ogp read failed\r\n");
        return ret;
    }
    CanToOgp(&can,fr);
    return ret;
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
