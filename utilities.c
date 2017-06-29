#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/ioctl.h>
#include <net/if.h> 
#include <unistd.h>
#include <netinet/in.h>

#include "utilities.h"

int getMacAddr(const char *ifname, unsigned char *mac)
{
    struct ifreq ifr;
    int fd;
    int rv;                     // return value - error value from df or ioctl call

    /* determine the local MAC address */
    strcpy(ifr.ifr_name, ifname);
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (fd < 0)
        rv = fd;
    else {
        rv = ioctl(fd, SIOCGIFHWADDR, &ifr);
        if (rv >= 0)            /* worked okay */
            memcpy(mac, ifr.ifr_hwaddr.sa_data, IFHWADDRLEN);
    }

    return rv;
}
//-------------------------------------------------------------------------------------
int print_addresses(void)
{
  int s;
  struct ifconf ifconf;
  struct ifreq ifr[50];
  int ifs;
  int i;

  s = socket(AF_INET/*domain*/, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    return 0;
  }

  ifconf.ifc_buf = (char *) ifr;
  ifconf.ifc_len = sizeof ifr;

  if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) {
    perror("ioctl");
    return 0;
  }

  ifs = ifconf.ifc_len / sizeof(ifr[0]);
  printf("interfaces = %d:\n", ifs);
  for (i = 0; i < ifs; i++) {
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;

    if (!inet_ntop(AF_INET/*domain*/, &s_in->sin_addr, ip, sizeof(ip))) {
      perror("inet_ntop");
      return 0;
    }

    printf("%s - %s\n", ifr[i].ifr_name, ip);
  }

  close(s);

  return 1;
}
//-------------------------------------------------------------------------------------
int getIpAddr(const char * interface, unsigned int* ip)
{
    int s;
    struct ifreq ifr;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
    perror("socket");
    return 0;
    }

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);

    if(ioctl(s, SIOCGIFADDR, &ifr)<0){
        perror("ioctrl");
        return 0;
    }
    close(s);

    /* display result */
    *ip = htol(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
//    printf("Ip (int): %08X\r\n",*ip);
//    printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    return 1;
}
//-------------------------------------------------------------------------------------
unsigned int htol(unsigned int val)
{
    unsigned int ret;
    ret = ((val >> 24)&0xFF) |
          ((val >> 8)&0xFF00) |
          ((val << 8)&0xFF0000) |
          ((val << 24)&0xFF000000);
    return ret;
}
//-------------------------------------------------------------------------------------
void itob(unsigned int val, unsigned char * buf)
{
    int i;
    unsigned char * pc = (unsigned char *)&val;
    for(i=0;i<4;i++)
        buf[3-i]= *pc++;
}
//-------------------------------------------------------------------------------------
long getTotalSystemMemory()
{
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}  
//-------------------------------------------------------------------------------------
void SendLog(char *s)
{
time_t t = time(NULL);	
long length;
char buffer[500];
FILE *fp;
struct tm tm = *localtime(&t);
sprintf(buffer,"%d-%d-%d %d:%d:%d ==> ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
fp=fopen("/home/admin/error.log","a");
if(fp==NULL)
{
  printf("Error on open log file\n");
  return;
}
fseek(fp,0,SEEK_END);
length=ftell(fp);
strcat(buffer,s);
if(length<10485760) // make sure that log file below 10 MB)
	fprintf(fp,"%s\n",buffer);
fclose(fp);
}
//-------------------------------------------------------------------------------------
void Crypt(char* key, char* in, char* out, int len)
{
    int i;
    for(i=0;i<len;i++){
        if(key[i])
            out[i]=key[i]^in[i];
        else
            out[i]=0xAA^in[i];
    }
}
void GetOptionFromString(char* in, char *buf, int len)
{
    char temp[2];
    int i,j=0;
    for(i=0;i<len;i++){
        if((in[i*2]>=0x30)&&(in[i*2]<=0x39))
            temp[0]=in[i*2]-0x30;
        else if((in[i*2]>=0x41)&&(in[i*2]<=0x46))
            temp[0]=in[i*2]-55;
        else if((in[i*2]>=97)&&(in[i*2]<=102))
            temp[0]=in[i*2]-87;
        
        if((in[i*2+1]>=0x30)&&(in[i*2+1]<=0x39))
            temp[1]=in[i*2+1]-0x30;
        else if((in[i*2+1]>=0x41)&&(in[i*2+1]<=0x46))
            temp[1]=in[i*2+1]-55;
        else if((in[i*2+1]>=97)&&(in[i*2+1]<=102))
            temp[1]=in[i*2+1]-87;

        buf[i]=(temp[0]<<4)|(temp[1]);
    }
}
int LoadLicense(char* buf)
{
    FILE* fp;
    char rtxt[100];
    int nreadbyte=0,i=0;
		if ((fp = (FILE *)(fopen( "license.lic","r+"))) == NULL)
		{
			printf("Cannot open license file to read !\r\n");
            return -1;
		}
		else{
			fgets(rtxt,100,fp);
			nreadbyte = strlen(rtxt);
			fclose(fp);
		}
        printf("Read %d bytes from license file\r\n",nreadbyte);
        GetOptionFromString(rtxt,buf,6);
        return 1;
}
unsigned char CheckLicense(char* key)
{
    unsigned char retval=0;
    char buf[7];
    char card_option[7];
    
    if(LoadLicense(buf)<0) return 0;
    card_option[6]=0;
    Crypt(key,buf,card_option,6);
    printf("Card Options: %s\r\n",card_option);
    if(card_option[0]=='F') retval |= FRAME_SYNC;
    else if(card_option[0]!='_') return 0;
    if(card_option[1]=='L') retval |= LOGOINSERT;
    else if(card_option[1]!='_') return 0;
    if(card_option[2]=='K') retval |= KEY;
    else if(card_option[2]!='_') return 0;
    if(card_option[3]=='M') retval |= AUDIOMIXER;
    else if(card_option[3]!='_') return 0;
    if(card_option[4]=='C') retval |= AUDIOCOMP;
    else if(card_option[4]!='_') return 0;
    if(card_option[5]=='L') retval |= AUDIOLEVEL;
    else if(card_option[5]!='_') return 0;
    
    return retval;
}
//void UpdateLicense(char* lic, int size)
void UpdateLicense(char* lic)
{
FILE* fp;
int i;
	if ((fp = (FILE *)(fopen( "/mnt/home/admin/license.lic","w+"))) == NULL)
	{
		printf("Cannot open text file to write !\r\n");
	}
	else{
		printf("Update License...\r\n");
/*
        for(i=0;i<size;i++)
            fprintf(fp,"%02X",lic[i]);
*/
        fprintf(fp,"%s",lic);
		printf("Finished!\r\n");
        sleep(5);
		fclose(fp);
	}    
sleep(5);
}
//-------------------------------------------------------------------------------------
#ifdef TEST
void TestCrypt()
{
    char key[6]={0x00,0x50,0x56,0x0A,0x00,0x12};
    char in[6]={0x46,0x31,0x32,0x33,0x34,0x35};
    char out[6];
    char dec[6];
    Crypt(key,in,out,6);
    int i;
    printf("out: ");
    for(i=0;i<6;i++)
        printf("0x%02X ",out[i]);
    printf("\r\n");
    UpdateLicense(out,6);
    Crypt(key,out,dec,6);
    printf("dec: ");
    for(i=0;i<6;i++)
        printf("0x%02X ",dec[i]);
    printf("\r\n");
    while(1);
}
#endif