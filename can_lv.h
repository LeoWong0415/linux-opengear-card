/* 
 * File:   can_lv.h
 * Author: sau
 * Decriptions: Process low-level CAN interface
 * Created on August 27, 2012, 1:02 PM
 */

#ifndef CAN_LV_H
#define	CAN_LV_H
#include <linux/can.h>
//#include <sys/types.h>
#include "ogp.h"

//int add_filter(u_int32_t id, u_int32_t mask);
int OgpInit(OgpFilter * filt, int nfil);
int CanRead(struct can_frame * fr);
void CanToOgp(struct can_frame * can, OgpShortFrame * ogp);
void OgpToCan(OgpShortFrame * ogp, struct can_frame * can);
void OgpAddFilter(OgpFilter * ogp_fil);
int OgpWrite(OgpShortFrame * fr);
int OgpRead(OgpShortFrame * fr);

#endif	/* CAN_LV_H */

