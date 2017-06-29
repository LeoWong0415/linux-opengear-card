/* 
 * File:   utilities.h
 * Author: sau
 *
 * Created on September 4, 2012, 12:07 PM
 */

#ifndef UTILITIES_H
#define	UTILITIES_H

#define FRAME_SYNC  0x1     //bit 0
#define LOGOINSERT  0x2     //bit 1
#define KEY         0x4     //bit 2
#define AUDIOMIXER  0x8     //BIT3
#define AUDIOCOMP   0x10    //BIT4
#define AUDIOLEVEL  0x20    //BIT5


int getMacAddr(const char *ifname, unsigned char *mac);
int print_addresses(void);
int getIpAddr(const char * interface, unsigned int* ip);
unsigned int htol(unsigned int val);
void itob(unsigned int val, unsigned char * buf);
long getTotalSystemMemory();
void SendLog(char *s);
void Crypt(char* key, char* in, char* out, int len);
void GetOptionFromString(char* in, char *buf,int len);
int LoadLicense(char* buf);
unsigned char CheckLicense(char* key);
void UpdateLicense(char* lic);
//void TestCrypt();
#endif	/* UTILITIES_H */

