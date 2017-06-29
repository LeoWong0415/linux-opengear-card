/* 
 * File:   clienthandler.h
 * Author: sau
 *
 * Created on September 5, 2012, 11:04 AM
 */

#ifndef CLIENTHANDLER_H
#define	CLIENTHANDLER_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define MAX_SIZE	0x200000	//(4M Words)

#define UDP_SERVER_PORT		9999

#define MAXPENDING 30    /* Max connection requests */
#define BUFFSIZE 300


#define MAXRECVSTRING 1023
#define DESCRIPTION "HD Logo Inserter"                 //gioi thieu ve client
#define CHECK_SCAN "hdvn"                           //du lieu rieng de kiem tra IP, subnet, MAC
#define CHECK_MESSAGE "hdvn_changeinformation"      //du lieu rieng de doi IP, subnet, gateway
#define CHECK_INTERFACE  "eth0"                     //kiem tra ten cua card mang


void* HandleClient(void* arg);
void* HandleClientUDP(void* arg);
void* RemoteControl(void* arg);
void Get_Command_UDP(int sock,char *cmd);
int Get_Command(int sock,char *cmd);
int Parse_Active(char *str,int *pos);
int Parse_VideoMode(char *str,int *pos);
int Parse_Property(char *str,int *x,int *y,int *pos,int *anpha);
int Parse_Setting(char *str,int *pos,int *setting);
int Parse_Volume(char *str,int *vol1,int *vol2,int *vol3,int *vol_master,int *pan);
int Parse_Position(char *str,int *x,int *y,int *width,int *height,char *logo_name);

void *threadIP();
int setDefGateway(const char * deviceName,const char * defGateway);
int gateWayConfig(char *gateway);
char *getmac(char *iface);
int check_subnet(const char *str);
int check_valid(const char *str);
void get_file(char *ip, char *su, char *gw);

#endif	/* CLIENTHANDLER_H */

