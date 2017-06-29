/* 
 * File:   menus.h
 * Author: sau
 *
 * Created on August 29, 2012, 10:43 AM
 */

#ifndef MENUS_H
#define	MENUS_H

#define NUM_OF_MENU_GROUP	2

void OGP_GetMenuSetNameRespone(unsigned char dest,unsigned char group);
void OGP_GetMenuCountRespone(unsigned char dest,unsigned char group);
void OGP_GetMenuNameRespone(unsigned char dest,unsigned char group,unsigned char menu);
void OGP_GetMenuOidsRespone(unsigned char dest,unsigned char group,unsigned char menu);
void OGP_GetMenuStateRespone(unsigned char dest,unsigned char group,unsigned char menu);

#endif	/* MENUS_H */

