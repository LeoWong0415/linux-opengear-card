/* 
 * File:   logo.h
 * Author: sau
 *
 * Created on September 5, 2012, 10:43 AM
 */

#ifndef LOGO_H
#define	LOGO_H

struct logo_item{
	int x;
	int y;
	int width;
	int height;
	int setting;
	char file_name[40];
};


void Load_Logo_Infor();
void Update_Logo_Infor();
void Send_Logo_Status(int sock);
void Send_Logo_Status_UDP(int sock);
void SetLogoStartup(void);
int Display_Logo(int fd,char* logoname,unsigned short *width,unsigned short *height);
void TurnOnRelayBypass(int fd);
void TurnOffRelayBypass(int fd);

void SetUpKeyPlane(int fd,int x,int y,int width,int height);
void SetPage(int fd,unsigned short page);

void ActivateLogo(int logoidx);
void DeActivateLogo(void);
void ChangeVideoInputMode(int mode);
void SetReference(int mode);

#endif	/* LOGO_H */

