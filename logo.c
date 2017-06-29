#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <net/route.h>
//#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "logo.h"
#include "FPGA.h"
#include "control.h"
#include "ogp.h"

//Global variables
int no_logo;
int active_logo;
int live;
int normal;
int volume1;
int volume2;
int volume3;
int pan;
int volume_master;
int video_mode;
struct logo_item logo_list[200];
char full_path[100];
unsigned short width=126;
unsigned short height=70;

//extern
extern int card_type;
extern struct sockaddr_in udpserver;
extern struct sockaddr_in echoclient;
extern int v_page;
extern int fd;
extern struct  func_item function;
extern int fd_spi_gs4911;
extern pthread_mutex_t mutex1;

extern struct key_mixer   key_mixer_param;
extern struct reference reference_param;
extern unsigned short key_ref_status; // store external key, reference status update to FPGA

//logo
extern Int16Param logoTotalParam;
extern Int16Param logoActIdxParam;
extern StringParam logoActNameParam;
extern Int16Param logoCurIdxParam;
extern StringParam logoCurNameParam;
extern Int16Param logoCurPosXParam;
extern Int16Param logoCurPosYParam;


//Functions
// ham Load_Logo_Info nap toan bo thong tin cua file cau hinh logo vao logo_list
void Load_Logo_Infor()
{
FILE *fin;
int no_read;
int temp;
int i;
#ifdef LOCK

pthread_mutex_lock( &mutex1 );
#endif

fin=fopen("//mnt//home//admin//logo_cfg.bin","rb");
if(fin==NULL)
	{
		printf("Khong mo duoc file config");
		no_logo=0;
                goto exit;
	}
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{	
	no_logo=temp; // doc duoc so luong logo
	}
else
	{
	no_logo=0;
	printf("Loi:%d",no_read);
	//return; // loi file chua co 2 byte so luong logo go o dau
        goto exit;
	}
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	active_logo=temp;
	}
else
	{
	no_logo=0;
	printf("Loi:%d",no_read);
        goto exit;

	}
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	live=temp;
	}
else
	{
	live=0;
	printf("Loi:%d",no_read);
	//return; // loi file chua co 2 byte so luong logo go o dau
        goto exit;

	}
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	normal=temp;
	}
else
	{
	normal=0;
	printf("Loi:%d",no_read);
	//return; // loi file chua co 2 byte so luong logo go o dau
        goto exit;

	}


// doc volume1
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	volume1=temp;
	}
else
	{
	volume1=0;
	printf("Loi:%d",no_read);
        goto exit;
	}
// doc volume2
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	volume2=temp;
	}
else
	{
	volume2=0;
	printf("Loi:%d",no_read);
	//return; // loi file chua co 2 byte so luong logo go o dau
        goto exit;
	}	
// doc volume3
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	volume3=temp;
	}
else
	{
	volume3=0;
	printf("Loi:%d",no_read);
        goto exit;
	}
// doc volume_Master
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	volume_master=temp;
	}
else
	{
	volume_master=0;
	printf("Loi:%d",no_read);
        goto exit;
	//return; // loi file chua co 2 byte so luong logo go o dau
	}		
// doc pan
no_read=fread(&temp,sizeof(int),1,fin);
if(no_read==1)
	{
	pan=temp;
	}
else
	{
	pan=0;
	printf("Loi:%d",no_read);
	//return; // loi file chua co 2 byte so luong logo go o dau
        goto exit;
	}	

// doc video mode
no_read = fread(&temp,sizeof(int),1,fin);
if(no_read==1)
{
video_mode=temp;	
}
else
{
  video_mode=0;
  printf("Loi:%d",no_read);
  goto exit;
}
	
	


for(i=0;i<no_logo;++i)
	{
		no_read=fread(&logo_list[i],sizeof(struct logo_item),1,fin);
		if(no_read==0)
			break;

	}
printf("\nMo xong file config");

fclose(fin);

exit:

#ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif
i=0;
}
//-----------------------------------------------------------------------------------
// ham nay update logo_list vao file cau hinh logo
void Update_Logo_Infor()
{
FILE *fout;
int i;
//fd_config=open("/home/app9260/logo_cfg.bin",O_CREAT|ORDWR);
#ifdef LOCK
pthread_mutex_lock( &mutex1 );
#endif
fout=fopen("//mnt//home//admin//logo_cfg.bin","wb");
if(fout==NULL)
	{
		printf("\nKhong mo duoc file");
		no_logo=0;
		return;
	}	
fwrite(&no_logo,sizeof(int),1,fout);
fwrite(&active_logo,sizeof(int),1,fout);
fwrite(&live,sizeof(int),1,fout);
fwrite(&normal,sizeof(int),1,fout);
fwrite(&volume1,sizeof(int),1,fout);
fwrite(&volume2,sizeof(int),1,fout);
fwrite(&volume3,sizeof(int),1,fout);
fwrite(&volume_master,sizeof(int),1,fout);
fwrite(&pan,sizeof(int),1,fout);
fwrite(&video_mode,sizeof(int),1,fout); // ghi video mode
for(i=0;i<no_logo;++i)
{
fwrite(&logo_list[i],sizeof(struct logo_item),1,fout);
}
printf("ghi xong cau hinh file");

fclose(fout);

#ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif
//fsync(2);
}
//--------------------------------------------------------------------------------------
void Send_Logo_Status(int sock)
{
int i;
int j;
int index;
char buff[2000];
char temp[200];
strcpy(buff,"");
/*sprintf(temp,"%d",no_logo);
strcpy(buff,temp);*/
index=0;
sprintf(temp,"%d",no_logo);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",active_logo);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}


sprintf(temp,",%d",live);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}


sprintf(temp,",%d",normal);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}


sprintf(temp,",%d",volume1);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",volume2);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",volume3);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",volume_master);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",pan);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",video_mode);
for(j=0;j<strlen(temp);++j)
{
  buff[index]=temp[j];
  index++;
}
sprintf(temp,",%d",card_type); // card type information
for(j=0;j<strlen(temp);++j)
{
  buff[index]=temp[j];
  index++;
}

for(i=0;i<no_logo;++i)
{
	sprintf(temp,",%d",logo_list[i].x);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
	sprintf(temp,",%d",logo_list[i].y);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
	sprintf(temp,",%d",logo_list[i].width);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
	sprintf(temp,",%d",logo_list[i].height);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
		
	sprintf(temp,",%d,",logo_list[i].setting);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}	
	for(j=0;j<strlen(logo_list[i].file_name);++j)
		{
		buff[index]=logo_list[i].file_name[j];
		index++;		
		}






/*	

	sprintf(temp,",%d",logo_list[i].x);
	strcat(buff,temp);
	sprintf(temp,",%d",logo_list[i].y);
	strcat(buff,temp);
	sprintf(temp,",%d",logo_list[i].width);
	strcat(buff,temp);
	sprintf(temp,",%d,",logo_list[i].height);
	strcat(buff,temp);
	strcat(buff,logo_list[i].file_name);
*/
}
//strcat(buff,"\n");
buff[index]='\n';
index++;
send(sock,buff,index,0);
printf("\nStatus string:%s",buff);
}
//------------------------------------------------------------------------------------------
void Send_Logo_Status_UDP(int sock)
{
int i;
int j;
int index;
char buff[2000];
char temp[200];
strcpy(buff,"");
/*sprintf(temp,"%d",no_logo);
strcpy(buff,temp);*/
index=0;
sprintf(temp,"%d",no_logo);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",active_logo);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}


sprintf(temp,",%d",live);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}


sprintf(temp,",%d",normal);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}


sprintf(temp,",%d",volume1);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",volume2);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",volume3);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",volume_master);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",pan);
for(j=0;j<strlen(temp);++j)
{
 buff[index]=temp[j];
 index++;
}

sprintf(temp,",%d",video_mode);
for(j=0;j<strlen(temp);++j)
{
  buff[index]=temp[j];
  index++;
}
sprintf(temp,",%d",card_type); // card type information
for(j=0;j<strlen(temp);++j)
{
  buff[index]=temp[j];
  index++;
}

for(i=0;i<no_logo;++i)
{
	sprintf(temp,",%d",logo_list[i].x);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
	sprintf(temp,",%d",logo_list[i].y);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
	sprintf(temp,",%d",logo_list[i].width);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
	sprintf(temp,",%d",logo_list[i].height);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}
		
	sprintf(temp,",%d,",logo_list[i].setting);
	for(j=0;j<strlen(temp);++j)
		{
		buff[index]=temp[j];
		index++;		
		}	
	for(j=0;j<strlen(logo_list[i].file_name);++j)
		{
		buff[index]=logo_list[i].file_name[j];
		index++;		
		}

}
	buff[index]='\n';
	index++;
	sendto(sock, buff, index, 0,(struct sockaddr *) &echoclient,sizeof(echoclient));
#ifdef TEST
	printf("\nStatus string:%s",buff);
#endif	
}
//------------------------------------------------------------------------------------
void SetLogoStartup(void)
{
// hien thi logo khi moi bat dien
if((no_logo>0) && (active_logo<no_logo))
{
	v_page=0;
	SetPage(fd,v_page);	
	v_page=1;
	SetPage(fd,v_page);
	v_page=0;
	SetPage(fd,v_page);		
									strcpy(full_path,"//mnt//home//logos//");
									strcat(full_path,logo_list[active_logo].file_name);
									Display_Logo(fd,full_path,&width,&height);
									//printf("\nWidth=%d",width);
									//printf("\nHeight=%d",height);
									//printf("\nFullPath=%s",full_path);							
									SetUpKeyPlane(fd,(logo_list[active_logo].x/2)*2,(logo_list[active_logo].y/2),width,height);
									if(v_page==0)
										{
										v_page=1;
										SetPage(fd,v_page);
										}
									else
										{
										v_page=0;
										SetPage(fd,v_page);	
										}
									//printf("\n\rActive logo:%d",active_logo);
										
                                    OGP_UpdataActiveLogo(no_logo, active_logo,logo_list[active_logo].file_name);
                                    if(logoCurIdxParam.value < no_logo)
                                        OGP_LoadLogoInfor(logoCurIdxParam.value,logo_list);
}
    
}
//--------------------------------------------------------------------------------------

int Display_Logo(int fd,char* logoname,unsigned short *width,unsigned short *height)
{       


int fd_logo;
unsigned short *logo;
long off_set;
unsigned short buffer[256];
long max;
long i;
long no_read;
//printf("\nLogoname=%s",logoname);

fd_logo=open(logoname,O_RDONLY);
//fd_logo=open("/home/logos/Fire0089.hdf",O_RDONLY);
if(fd_logo<0)
	{
		printf("\nKhong mo duoc file logo");
		return(0);
	}

 
read(fd_logo,buffer,4);
*width=buffer[0];
*height=buffer[1];

logo=malloc((*width)*((*height)+1)*3+512);


 off_set=0;
 while(1)
  {
     no_read=read(fd_logo,&logo[off_set],1024);
     off_set=off_set+no_read/2;
     if(no_read<=0)
	break;	
	
  }


ioctl(fd,1,0);
max=(((*width)*(*height)*3)/2); // total word of logo
off_set=0;
while(1)
{
if((off_set % 0x10000)==0)
 {
 ioctl(fd,1,off_set/0x10000);
 }
if((off_set+64)>max)
  {	
	for(i=0;i<max-off_set;++i)
	   buffer[i]=logo[off_set+i];
	write(fd,buffer,off_set % 0x10000);
	break;
  }
else
  {	
	write(fd,&logo[off_set],off_set % 0x10000);	
	off_set=off_set+64;	
  }

} 
free(logo);
close(fd_logo);
}
//-------------------------------------------------------------------------------------
void TurnOnRelayBypass(int fd)
{
int data;
//    data=(1<<6);    // relay bypass 
    data=0;
    data = (1<<(ANPHA_LED)) | (1<< (VIDEO_LED)) | (1<<(BKGD_LED)) | (1<<(REF_LED)) | (1<<6);
    write(fd,&data,4);
}
void TurnOffRelayBypass(int fd)
{
int data;
data =0;
    data=(1<<6);    // relay bypass         
    write(fd,&data,0); // thanh ghi bat
}
//-------------------------------------------------------------------------------------
void SetUpKeyPlane(int fd,int x,int y,int width,int height)
{
unsigned short data;
ioctl(fd,0,128);
x+=2;
data=x;
write(fd,&data,2); // write start x pos 0x200002
y+=2;
data=y;
write(fd,&data,3); // write start y pos
data=x+width;
write(fd,&data,4); // write width 
if((height%2)==0)
  data=y+ (height/2);
else
  data=y+((height+1)/2);
write(fd,&data,5); // write height
}
//-------------------------------------------------------------------------------------
void SetPage(int fd,unsigned short page)
{
unsigned short data;
ioctl(fd,0,128);
data=page;
write(fd,&data,1); // page 0x200001
}
//------------------------------------------------------------------------------------
void ActivateLogo(int logoidx)
{
#ifdef LOCK
pthread_mutex_lock( &mutex1 );
#endif                                                            
char full_path[128];
    strcpy(full_path,"//mnt//home//logos//");
    strcat(full_path,logo_list[logoidx].file_name);
    Display_Logo(fd,full_path,&width,&height);
    //printf("\nWidth=%d",width);
    //printf("\nHeight=%d",height);
    //printf("\nFullPath=s",full_path);							
    SetUpKeyPlane(fd,(logo_list[logoidx].x/2)*2,(logo_list[logoidx].y/2),width,height);

    if(v_page==0)
    {
        v_page=1;
        SetPage(fd,v_page);
    }
    else
    {
        v_page=0;
        SetPage(fd,v_page);	
    }

#ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif                          									
    
}
//----------------------------------------------------------------------------------------
void DeActivateLogo(void)
{
//    printf("\nKo hien thi logo");
    SetUpKeyPlane(fd,2,2,0,0);  
    if(v_page==0)
    {
        v_page=1;
        SetPage(fd,v_page);
    }
    else
    {
        v_page=0;
        SetPage(fd,v_page);	
    }
    active_logo=no_logo+1;
}
//----------------------------------------------------------------------------------------
void ChangeVideoInputMode(int mode)
{
    unsigned short data;
    
    if(mode==0)	// 1080 i
    {
        data=0;	
    #ifdef SDONLY                                    
        data=1; //option
    #endif                                    

        if(function.frame_sync==0){
            FPGA_write(fd,18,data);   
            Change_Ref(fd_spi_gs4911,0);
        } else {
            Change_Ref_Frame(fd_spi_gs4911,0);
        }
    }
    if(mode==1)	// 576 i
    {
        if(function.frame_sync==0){
            data=1;							
            FPGA_write(fd,18,data);   
            Change_Ref(fd_spi_gs4911,1);
    } else 
    {
        Change_Ref_Frame(fd_spi_gs4911,1);
    }                                        
    }
    video_mode=mode;
}
//----------------------------------------------------------------------------------------
void SetReference(int mode)
{
    unsigned short data;
            
    if(mode==0) { //video in
        data=0;                    
        data=(mode&0x01);
        data=data|((key_mixer_param.enable&0x1)<<1);                    
        key_ref_status=data;
        FPGA_write(fd,33,data);  
    }
    else if(mode==1) { //external
        data=1;
        data=data|((key_mixer_param.enable&0x1)<<1);
        key_ref_status=data;
        FPGA_write(fd,33,data);  
    }     
    reference_param.mode = mode;
    SaveAudioParam();
}
//------------------------------------------------------------------------------------
void SetKeyEnable(int enable)
{
    unsigned short data;
    data=(reference_param.mode&0x01);
    data=data|((enable&0x1)<<1);
    key_ref_status=data;
    FPGA_write(fd,33,data);                  
    key_mixer_param.enable=enable;
    
    SaveAudioParam();
}