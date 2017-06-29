#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <netinet/in.h>
#include <net/route.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>

#include "clienthandler.h"
#include "logo.h"
#include "control.h"
#include "ogp_ctrl.h"

//Global variables

unsigned int clientlen;


//externs
extern BOOL ctrlPanelIsAvailable;      //from main.c; indicate that there is a Control Panel connected
extern unsigned int ctrlPanelTimeOut;   //from main.c; Control Panel Timer
extern BOOL setModeAlready;             //indicate action state of setting mode for Hardware/Sofware control

extern int fd;
extern int halt_prog;
//extern from logo.c
extern int no_logo;
extern int active_logo;
extern int live;
extern int normal;
extern int volume1;
extern int volume2;
extern int volume3;
extern int pan;
extern int volume_master;
extern int video_mode;
extern struct logo_item logo_list[];
extern int v_page;
extern struct  func_item function;
extern int fd_spi_gs4911; //from main.c
extern int udpsock;         //from main.c
extern struct sockaddr_in udpserver,echoclient;

//Functions
void* HandleClient(void* arg) 
{
	char buffer[BUFFSIZE];
        char cmd_buffer[BUFFSIZE];
	int received = -1;
	char cmd[10];	
	char full_path[200];
	int sock=*(int *)arg;
	unsigned short width,height;
	int x,y,pos,anpha;
	int n;
	int isfound;
	int i;
	int i_temp;
        char    cmd_arg[200][200];
        int     cmd_length;

	unsigned short data;
	long time_out;
        
	int setting;
	fprintf(stdout,"welcome");

        
        strcpy(buffer,"220 Hello this is HD LOGO inserter\n");
	send(sock,buffer,strlen(buffer),0);
        
	while(1)
		{
                    

			if(Get_Command(sock,buffer)==0){ // if time out occur
                            goto quit;
                        }
                        strcpy(cmd_buffer,buffer);
			
                        parse_string(cmd_buffer,cmd_arg,&cmd_length);
                        if(cmd_length>0)
                        {
                          //  for(i=0;i<cmd_length;++i)
                            //    printf("\n%s",cmd_arg[i]);
                            if(!strcmp(cmd_arg[0],"SET")){
                                if(cmd_length>=3){
                                    
                                    
                                    if(!strcmp(cmd_arg[1],"KEYMIXER")){
                                        if(cmd_length>=4){
                                            Setup_Key_Mixer(sock,cmd_arg[2],cmd_arg[3],fd);
                                        }
                                    }                                    
                                    if(!strcmp(cmd_arg[1],"REFERENCE")){
                                        if(cmd_length>=4){
                                            Setup_Reference(sock,cmd_arg[2],cmd_arg[3],fd);
                                        }
                                    }                                    
                                    if(!strcmp(cmd_arg[1],"AUDIO_DELAY")){
                                        if(cmd_length>=4){
                                            Setup_Audio_Delay(sock,cmd_arg[2],cmd_arg[3],fd);
                                        }
                                    }                                                                                                            
                                    
                                    if(!strcmp(cmd_arg[1],"FRAME_SYNC")){
                                        if(cmd_length>=4){
                                            Setup_Frame_Sync(sock,cmd_arg[2],cmd_arg[3],fd);
                                        }
                                    }                                    
                                    
                                    
                                    if(!strcmp(cmd_arg[1],"AUDIO_MIXER")){
                                        if(cmd_length>=4){
                                            Change_Audio_Mixer(sock,cmd_arg[2],cmd_arg[3],fd);
                                        }
                                    }
                                    if(!strcmp(cmd_arg[1],"SETUPLEVELER")){
                                        if(cmd_length>=4){
                                            Setup_Leveler(sock,cmd_arg[2],cmd_arg[3]);
                                        }
                                    }
                                    
                                    if(!strcmp(cmd_arg[1],"SETUPAUTOMIXER")){
                                        if(cmd_length>=4){
                                            Setup_Auto_Mixer(sock,cmd_arg[2],cmd_arg[3]);
                                        }
                                    }
                                    
                                    if(!strcmp(cmd_arg[1],"SETUPAGC")){
                                        if(cmd_length>=4){
                                            Setup_AGC(sock,cmd_arg[2],cmd_arg[3]);
                                        }
                                    }                                    
                                    
                                    if(!strcmp(cmd_arg[1],"SETUPCOMP")){
                                        if(cmd_length>=4){
                                            Setup_Comp(sock,cmd_arg[2],cmd_arg[3]);
                                        }
                                    }                                    
                                }
                            }
                            if(!strcmp(cmd_arg[0],"GET")){
                            // co lenh get thi thuc hien lenh get
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"FRAME_SYNC")){
                                        Report_Frame_Sync(sock,cmd_arg[2]);
                                    }
                                }                                                 
                                
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"AUDIO_DELAY")){
                                        Report_Audio_Delay(sock,cmd_arg[2]);
                                    }
                                }                                                                                                                 
                                
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"REFERENCE")){
                                        Report_Reference(sock,cmd_arg[2]);
                                    }
                                }                                                 
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"KEYMIXER")){
                                        Report_Key_Mixer(sock,cmd_arg[2]);
                                    }
                                }                                
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"AUDIO_MIXER")){
                                        Report_Audio_Mixer(sock,cmd_arg[2]);
                                    }
                                }
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"SETUPLEVELER")){                                        
                                            Report_Setup_Leveler(sock,cmd_arg[2]);                                        
                                    }
                                }
                                //SETUPAUTOMIXER
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"SETUPAUTOMIXER")){                                        
                                            Report_Auto_Mixer(sock,cmd_arg[2]);                                        
                                    }
                                }                                                                
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"SETUPAGC")){                                        
                                            Report_ALC(sock,cmd_arg[2]);                                        
                                    }
                                }                                
                                if(cmd_length>=3){
                                    if(!strcmp(cmd_arg[1],"SETUPCOMP")){                                        
                                            Report_Setup_Comp(sock,cmd_arg[2]);                                        
                                    }
                                }

                                if(cmd_length>=2){
                                    if(!strcmp(cmd_arg[1],"FUNCTION")){
                                        if(cmd_length>=3){
                                            Send_Function(sock,cmd_arg[2]);
                                        }
                                    }
                                    
                                    if(!strcmp(cmd_arg[1],"METERCOMPIN12")){
                                       // printf("\nLAY meter");
                                        if(cmd_length>=3){
                                            GetMeterCompIn(sock,cmd_arg[2]);
                                        }
                                        
                                    }
                                     
                                    if(!strcmp(cmd_arg[1],"METERAUTOMIXER")){
                                       // printf("\nLAY meter");
                                        if(cmd_length>=3){
                                            GetMeterAuToMixer(sock,cmd_arg[2]);
                                        }
                                        
                                    }                                    
                                    
                                    
                                    if(!strcmp(cmd_arg[1],"METERLEVELERIN12")){
                                       // printf("\nLAY meter");
                                        if(cmd_length>=3){
                                            Send_LeverMeterIn(sock,cmd_arg[2]);
                                        }
                                        
                                    }
                                    if(!strcmp(cmd_arg[1],"METERLEVELEROUT12")){
                                       // printf("\nLAY meter");
                                        if(cmd_length>=3){
                                            Send_LeverMeterOut(sock,cmd_arg[2]);
                                        }
                                        
                                    }
                                    
                                    if(!strcmp(cmd_arg[1],"METERCOMPOUT12")){
                                       // printf("\nLAY meter");
                                        if(cmd_length>=3){
                                            GetMeterCompOut(sock,cmd_arg[2]);
                                        }
                                        
                                    }

                                    //GAINCOMP12
                                    if(!strcmp(cmd_arg[1],"GAINLEVELER12")){
                                        if(cmd_length>=3){
                                            Send_LeverGain(sock,cmd_arg[2]);
                                        }
                                    }
                                    if(!strcmp(cmd_arg[1],"GAINCOMP12")){
                                        if(cmd_length>=3){
                                            Send_CompGain(sock,cmd_arg[2]);
                                        }
                                    }


                                }
                            }
                        }
                        
                        
                        
			if(strlen(buffer)>=4)
			{
			strncpy(cmd,buffer,4);
			//strcat(cmd,'\n');
			cmd[4]='\0';
			//printf("\nlenh:%s",cmd);
                        //print_hex_string(cmd,5);
			if(strncmp(cmd,"STAS",4)==0)	// lenh doc trang thai
				{
                            //printf("\n\rNhan duoc lenh stas");
					Load_Logo_Infor();
					Send_Logo_Status(sock);
                                        printf("\n\rSend stas xong");

				}
			if(strncmp(cmd,"REST",4)==0)
				{
					
					strcpy(buffer,"220 Reboot\n");
					send(sock,buffer,strlen(buffer),0);
					system("reboot");

				}
			if(strncmp(cmd,"HALT",4)==0)
				{
					halt_prog=1;
					strcpy(buffer,"220 Bye\n");
					send(sock,buffer,strlen(buffer),0);

					goto quit;
				}
			if(strncmp(cmd,"QUIT",4)==0)	// gui lenh thoat
				{
					//printf("nhan lenh quit");
					strcpy(buffer,"220 Bye\n");
					send(sock,buffer,strlen(buffer),0);
					goto quit;
				}
			if(strncmp(cmd,"DELL",4)==0)
				{
					if(Parse_Active(buffer,&n)==1)
						{
							if((n<no_logo) && (no_logo>0))
								{
									if(n<active_logo)
										active_logo--;
									strcpy(full_path,"rm //mnt//home//logos//");
									strcat(full_path,logo_list[n].file_name);
									system(full_path);									
									

									for(i=n;i<no_logo-1;++i)
										logo_list[i]=logo_list[i+1];
									no_logo--;								
									Update_Logo_Infor();
									strcpy(buffer,"220 OK\n");
									send(sock,buffer,strlen(buffer),0);			

									
								}
							else
								{
									strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);

								}	

						}	
					else
						{
									strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);

						}
				}
			if(strncmp(cmd,"AUDI",4)==0)	// Change audio property
			{
			  if(Parse_Volume(buffer,&volume1,&volume2,&volume3,&volume_master,&pan)==1)
			  {
							
							//printf("\nReply audio");
			    /*
			    
							ioctl(fd,0,128);
							data=(volume1&0xffff);							
							write(fd,&data,7); // write low volume1
							data=((volume1>>16)&0xffff);							
							write(fd,&data,8); // write low volume1
							
							data=(volume2&0xffff);							
							write(fd,&data,9); // write low volume2
							data=((volume2>>16)&0xffff);							
							write(fd,&data,10); // write low volume2
							
							data=(volume3&0xffff);							
							write(fd,&data,11); // write low volume3
							data=((volume3>>16)&0xffff);							
							write(fd,&data,12); // write low volume3
							
							data=(volume_master&0xffff);							
							write(fd,&data,13); // write low volume master
							data=((volume_master>>16)&0xffff);							
							write(fd,&data,14); // write low volume master
							
							data=((pan)&0xffff);							
							write(fd,&data,15); // write pan

                                                        */
							
							
							

							Update_Logo_Infor();
			    				strcpy(buffer,"220 OK\n");
							send(sock,buffer,strlen(buffer),0);

  
			  }
			  else
			  {
									strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);			    
			  }
			}
			if(strncmp(cmd,"PROP",4)==0)	// change property
				{
//int Parse_Property(char *str,int *x,int *y,int *pos,int *anpha)
					if(Parse_Property(buffer,&x,&y,&pos,&anpha)==1)
						{
							logo_list[pos].x=x;
							logo_list[pos].y=y;

							Update_Logo_Infor();
							if(pos==active_logo)
                                ActivateLogo(pos);
                            
                            strcpy(buffer,"220 OK\n");
							send(sock,buffer,strlen(buffer),0);
						}
					else
						{
									strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);
		
						}
				}
			if(strncmp(cmd,"VIDM",4)==0)	// change video mode
			{
			    if(Parse_Active(buffer,&i_temp)==1)
			    {
                    ChangeVideoInputMode(i_temp);
                    
				  Update_Logo_Infor();
                                    					strcpy(buffer,"220 OK\n");
					send(sock,buffer,strlen(buffer),0);
			    }
			    else
			    {
				strcpy(buffer,"500 Error\n");
				send(sock,buffer,strlen(buffer),0);

			    }
			}
			if(strncmp(cmd,"SETL",4)==0)    // set live logo
				{
					if(Parse_Active(buffer,&i_temp)==1)
						{

							if(i_temp<no_logo)
								{
									live=i_temp;
									Update_Logo_Infor();
									strcpy(buffer,"220 OK\n");
									send(sock,buffer,strlen(buffer),0);
								}
							else
								{
									strcpy(buffer,"500 Error\n");
									send(sock,buffer,strlen(buffer),0);

								}
						}
					else
						{
									strcpy(buffer,"500 Error\n");
									send(sock,buffer,strlen(buffer),0);

						}
			
				}
			if(strncmp(cmd,"SETN",4)==0)    // set live logo
				{
					if(Parse_Active(buffer,&i_temp)==1)
						{

							if(i_temp<no_logo)
								{
									normal=i_temp;
									Update_Logo_Infor();
									strcpy(buffer,"220 OK\n");
									send(sock,buffer,strlen(buffer),0);
								}
							else
								{
									strcpy(buffer,"500 Error\n");
									send(sock,buffer,strlen(buffer),0);

								}
						}
					else
						{
									strcpy(buffer,"500 Error\n");
									send(sock,buffer,strlen(buffer),0);

						}
			
				}
			if(strncmp(cmd,"SETT",4)==0) // lenh dat setting cho logo
			{
				if(Parse_Setting(buffer,&pos,&setting)==1)
				{
					if(pos<no_logo)
					{
					   logo_list[pos].setting=setting; 
					   Update_Logo_Infor();
  					strcpy(buffer,"220 OK\n");
					send(sock,buffer,strlen(buffer),0);
					   
					}
					else
					{
									strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);
					  
					}
					  
				}
			}
/////////////////
			if(strncmp(cmd,"CALL",4)==0)	// call live or active logo, 
				{
					if(Parse_Active(buffer,&i_temp)==1)
						{
							
							
							if(i_temp==0)
							{
							    // them lenh khong hien thi logo vao day
							    //
                                 
							  printf("\nKo hien thi logo");
                              DeActivateLogo();
                              /*
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
                               */ 
							  strcpy(buffer,"220 OK\n");
							  send(sock,buffer,strlen(buffer),0);
							  
							  Update_Logo_Infor();

							}
							else
							{ 
							  isfound=0;
							  for(i=0;i<no_logo;++i)
							  {
							      if(logo_list[i].setting==i_temp)
							      {
								  isfound=1;
								  break;
							      }
							      
							  }
							    if(isfound==1)
							    {
							    
							      active_logo=i;



							    if(active_logo<no_logo)
								{
                                    ActivateLogo(active_logo);
                                    //DoSetLogoActivate(1);
/*
                                                                #ifdef LOCK
pthread_mutex_lock( &mutex1 );
#endif
									strcpy(full_path,"//mnt//home//logos//");
									strcat(full_path,logo_list[active_logo].file_name);
									Display_Logo(fd,full_path,&width,&height);
									printf("\nWidth=%d",width);
									printf("\nHeight=%d",height);
									printf("\nFullPath=s",full_path);							
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
#ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif							
									Update_Logo_Infor();
*/
									strcpy(buffer,"220 OK\n");
									send(sock,buffer,strlen(buffer),0);
								}
							      else
								{
									strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);
								}
								
							    }
							    else
							    {
							      strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);
							    }
								
								
							}
						} // neu xu ly chuoi sai
			
						  else	
						    {
							strcpy(buffer,"500 error\n");
							send(sock,buffer,strlen(buffer),0);
							
						    }
						
				}	// end CALL
////////////////////
			if(strncmp(cmd,"ACTL",4)==0)	// set active logo
				{
					if(Parse_Active(buffer,&active_logo)==1)
						{

							if(active_logo<no_logo)
								{
                                                      ActivateLogo(active_logo);
                                                      
                                                      /*

                                                            #ifdef LOCK
pthread_mutex_lock( &mutex1 );
#endif
  
                                                       * 
                            

									strcpy(full_path,"//mnt//home//logos//");
									strcat(full_path,logo_list[active_logo].file_name);
									Display_Logo(fd,full_path,&width,&height);
									printf("\nWidth=%d",width);
									printf("\nHeight=%d",height);
									printf("\nFullPath=s",full_path);							
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
                                    
                                    
#ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif                                                                        							
                                                       * 
                                                       */ 
									Update_Logo_Infor();

									strcpy(buffer,"220 OK\n");
									send(sock,buffer,strlen(buffer),0);
								}
							else
								{
									strcpy(buffer,"500 error\n");
									send(sock,buffer,strlen(buffer),0);
								}
						}	
			
					else
						{
							strcpy(buffer,"500 error\n");
							send(sock,buffer,strlen(buffer),0);
							
						}
				}

			if(strncmp(cmd,"ADDL",4)==0) // neu la lenh them 1 logo vao list
				{

					if(Parse_Position(buffer,&logo_list[no_logo].x,&logo_list[no_logo].y,&logo_list[no_logo].width,&logo_list[no_logo].height,logo_list[no_logo].file_name)==1)	// neu xu ly them duoc 1 logo
						{
							logo_list[no_logo].setting=0;

							no_logo++;
							strcpy(buffer,"220 OK\n");
							send(sock,buffer,strlen(buffer),0);
							Update_Logo_Infor();
			
						}
					else
						{
							printf("Loi:KHong xu ly chuoi them logo");
							strcpy(buffer,"500 error\n");
							send(sock,buffer,strlen(buffer),0);
						}
	
				}
			}
		}
	quit:
//	printf("CLose socket hien tai");
	close(sock);
	return(arg);


#ifdef testdien
	/* Receive message */
	if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
		Die("Failed to receive initial bytes from client");
	}
	/* Send bytes and check for more incoming data in loop */
	while (received > 0) {
		/* Send back received data */
		if (send(sock, buffer, received, 0) != received) {
			Die("Failed to send bytes to client");
		}
		/* Check for more data */
		if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
			Die("Failed to receive additional bytes from client");
		}
	}
#endif
}

void* HandleClientUDP(void* arg)
{
	char buffer[BUFFSIZE];
        char cmd_buffer[BUFFSIZE];
	int received = -1;
	char cmd[10];	
	char full_path[200];
//	int sock=*(int *)arg;
	unsigned short width,height;
	int x,y,pos,anpha;
	int n;
	int isfound;
	int i;
	int i_temp;
        char    cmd_arg[200][200];
        int     cmd_length;

	unsigned short data;
	long time_out;
	int setting;
	fprintf(stdout,"welcome");

	/* Create the UDP socket */
	if ((udpsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Die("Failed to create socket");
	}
	/* Construct the server sockaddr_in structure */
	memset(&udpserver, 0, sizeof(udpserver));       /* Clear struct */
	udpserver.sin_family = AF_INET;                  /* Internet/IP */
	udpserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any IP address */
	udpserver.sin_port = htons(UDP_SERVER_PORT);       /* server port */

	/* Bind the socket */
	unsigned int serverlen = sizeof(udpserver);
	if (bind(udpsock, (struct sockaddr *) &udpserver, serverlen) < 0) {
		Die("Failed to bind server socket");
	}

	strcpy(buffer,"You are connected to HD Logo Inserter on UDP port 9999\n");
	sendto(udpsock, buffer, strlen(buffer), 0,(struct sockaddr *) &echoclient,sizeof(echoclient));

	while(halt_prog==0)
		{
			Get_Command_UDP(udpsock,buffer);
            strcpy(cmd_buffer,buffer);
			parse_string(cmd_buffer,cmd_arg,&cmd_length);

			if(cmd_length>0)
			{
                if(ctrlPanelIsAvailable == FALSE){
                    ctrlPanelIsAvailable = TRUE;    //set TRUE indicate that there is a Control Panel connected
                    setModeAlready = FALSE;
                }
                ctrlPanelTimeOut = 0;         //reset timer
				//  for(i=0;i<cmd_length;++i)
                //    printf("\n%s",cmd_arg[i]);
                if(!strcmp(cmd_arg[0],"SET")){
					if(cmd_length>=3){
						if(!strcmp(cmd_arg[1],"AUDIO_MIXER")){
							if(cmd_length>=4){
								Change_Audio_MixerUDP(udpsock,cmd_arg[2],cmd_arg[3],fd);
                            }
                        }
                        if(!strcmp(cmd_arg[1],"SETUPLEVELER")){
							if(cmd_length>=4){
								Setup_LevelerUDP(udpsock,cmd_arg[2],cmd_arg[3]);
                            }
                        }
                        if(!strcmp(cmd_arg[1],"SETUPCOMP")){
							if(cmd_length>=4){
								Setup_CompUDP(udpsock,cmd_arg[2],cmd_arg[3]);
                            }
                        }                                    
                    }
				}
                
				if(!strcmp(cmd_arg[0],"GET")){
					// co lenh get thi thuc hien lenh get
                    if(cmd_length>=3){
						if(!strcmp(cmd_arg[1],"AUDIO_MIXER")){
							Report_Audio_MixerUDP(udpsock,cmd_arg[2]);
                        }
                    }
					if(cmd_length>=3){
						if(!strcmp(cmd_arg[1],"SETUPLEVELER")){                                        
							Report_Setup_LevelerUDP(udpsock,cmd_arg[2]);                                        
                        }
                    }
                    if(cmd_length>=3){
						if(!strcmp(cmd_arg[1],"SETUPCOMP")){                                        
							Report_Setup_CompUDP(udpsock,cmd_arg[2]);                                        
                        }
                    }
                    if(cmd_length>=2){
	                    if(!strcmp(cmd_arg[1],"FUNCTION")){
		                    if(cmd_length>=3){
			                    Send_FunctionUDP(udpsock,cmd_arg[2]);
                            }
                        }
                        if(!strcmp(cmd_arg[1],"METERCOMPIN12")){
                            // printf("\nLAY meter");
                            if(cmd_length>=3){
                                GetMeterCompInUDP(udpsock,cmd_arg[2]);
                            }
                        }
                        if(!strcmp(cmd_arg[1],"METERLEVELERIN12")){
                            // printf("\nLAY meter");
                            if(cmd_length>=3){
                                Send_LeverMeterInUDP(udpsock,cmd_arg[2]);
                            }
                        }
						if(!strcmp(cmd_arg[1],"METERLEVELEROUT12")){
                            // printf("\nLAY meter");
                            if(cmd_length>=3){
                                Send_LeverMeterOutUDP(udpsock,cmd_arg[2]);
                            }
                        }
                        if(!strcmp(cmd_arg[1],"METERCOMPOUT12")){
                            // printf("\nLAY meter");
                            if(cmd_length>=3){
                                GetMeterCompOutUDP(udpsock,cmd_arg[2]);
                            }
                        }
                        //GAINCOMP12
                        if(!strcmp(cmd_arg[1],"GAINLEVELER12")){
                            if(cmd_length>=3){
                                Send_LeverGainUDP(udpsock,cmd_arg[2]);
                            }
                        }
                        if(!strcmp(cmd_arg[1],"GAINCOMP12")){
                            if(cmd_length>=3){
	                            Send_CompGainUDP(udpsock,cmd_arg[2]);
                            }
                        }
                    }
                }
            }
                        
			if(strlen(buffer)>=4)
			{
			strncpy(cmd,buffer,4);
			//strcat(cmd,'\n');
			cmd[4]='\0';
			//printf("\nlenh:%s",cmd);
			
			if(strncmp(cmd,"STAS",4)==0)	// lenh doc trang thai
				{
					Load_Logo_Infor();
					Send_Logo_Status_UDP(udpsock);
				}
			if(strncmp(cmd,"REST",4)==0)
				{
					
					strcpy(buffer,"220 Reboot\n");
					sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
					system("reboot");

				}
			if(strncmp(cmd,"HALT",4)==0)
				{
					halt_prog=1;
					strcpy(buffer,"220 Bye\n");
					sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
					goto quit;
				}
			if(strncmp(cmd,"QUIT",4)==0)	// gui lenh thoat
				{
					printf("nhan lenh quit");
					strcpy(buffer,"220 Bye\n");
					sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
					goto quit;
				}
			if(strncmp(cmd,"DELL",4)==0)
				{
					if(Parse_Active(buffer,&n)==1)
						{
							if((n<no_logo) && (no_logo>0))
								{
									if(n<active_logo)
										active_logo--;
									strcpy(full_path,"rm //mnt//home//logos//");
									strcat(full_path,logo_list[n].file_name);
									system(full_path);									
									

									for(i=n;i<no_logo-1;++i)
										logo_list[i]=logo_list[i+1];
									no_logo--;								
									Update_Logo_Infor();
									strcpy(buffer,"220 OK\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

								}
							else
								{
									strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

								}	

						}	
					else
						{
									strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

						}
				}
			if(strncmp(cmd,"AUDI",4)==0)	// Change audio property
			{
			  if(Parse_Volume(buffer,&volume1,&volume2,&volume3,&volume_master,&pan)==1)
			  {
							
							//printf("\nReply audio");
			    /*
			    
							ioctl(fd,0,128);
							data=(volume1&0xffff);							
							write(fd,&data,7); // write low volume1
							data=((volume1>>16)&0xffff);							
							write(fd,&data,8); // write low volume1
							
							data=(volume2&0xffff);							
							write(fd,&data,9); // write low volume2
							data=((volume2>>16)&0xffff);							
							write(fd,&data,10); // write low volume2
							
							data=(volume3&0xffff);							
							write(fd,&data,11); // write low volume3
							data=((volume3>>16)&0xffff);							
							write(fd,&data,12); // write low volume3
							
							data=(volume_master&0xffff);							
							write(fd,&data,13); // write low volume master
							data=((volume_master>>16)&0xffff);							
							write(fd,&data,14); // write low volume master
							
							data=((pan)&0xffff);							
							write(fd,&data,15); // write pan

                                                        */
							
							
							

							Update_Logo_Infor();
		    				strcpy(buffer,"220 OK\n");
							sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
			  }
			  else
			  {
							strcpy(buffer,"500 error\n");
							sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
			  }
			}
			if(strncmp(cmd,"PROP",4)==0)	// change property
				{
//int Parse_Property(char *str,int *x,int *y,int *pos,int *anpha)
					if(Parse_Property(buffer,&x,&y,&pos,&anpha)==1)
						{
							logo_list[pos].x=x;
							logo_list[pos].y=y;

							Update_Logo_Infor();
#ifdef LOCK
pthread_mutex_lock( &mutex1 );
#endif                                                        
							if(pos==active_logo)
								{
								strcpy(full_path,"//mnt//home//logos//");
								strcat(full_path,logo_list[active_logo].file_name);
								Display_Logo(fd,full_path,&width,&height);								  								  
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
							
							
								}
#ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif                                                        
							strcpy(buffer,"220 OK\n");
							sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
						}
					else
						{
							strcpy(buffer,"500 error\n");
							sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
		
						}
				}
			if(strncmp(cmd,"VIDM",4)==0)	// change video mode
			{
			    if(Parse_Active(buffer,&i_temp)==1)
			    {
				  if(i_temp==0)	// 1080 i
				  {
//				    ioctl(fd,0,32);
				    //ioctl(fd,0,128);
				    data=0;	
#ifdef SDONLY                                    
                                    data=1;
#endif                                    
				    //write(fd,&data,18); // write video_mode
                                    FPGA_write(fd,18,data);   
                                    Change_Ref(fd_spi_gs4911,0);

				  }
				  if(i_temp==1)	// 576 i
				  {
//				    ioctl(fd,0,32);
				    //ioctl(fd,0,128);
				    data=1;							
				    //write(fd,&data,18); // write video_mode
                                    FPGA_write(fd,18,data);   
                                    Change_Ref(fd_spi_gs4911,1);
//				    write(fd,&data,0); // write video_mode
				    
				  }
				  video_mode=i_temp;
				  Update_Logo_Infor();
			    }
			    else
			    {
				strcpy(buffer,"500 Error\n");
				sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

			    }
			}
			if(strncmp(cmd,"SETL",4)==0)    // set live logo
				{
					if(Parse_Active(buffer,&i_temp)==1)
						{

							if(i_temp<no_logo)
								{
									live=i_temp;
									Update_Logo_Infor();
									strcpy(buffer,"220 OK\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
								}
							else
								{
									strcpy(buffer,"500 Error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

								}
						}
					else
						{
									strcpy(buffer,"500 Error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

						}
			
				}
			if(strncmp(cmd,"SETN",4)==0)    // set live logo
				{
					if(Parse_Active(buffer,&i_temp)==1)
						{

							if(i_temp<no_logo)
								{
									normal=i_temp;
									Update_Logo_Infor();
									strcpy(buffer,"220 OK\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
								}
							else
								{
									strcpy(buffer,"500 Error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

								}
						}
					else
						{
									strcpy(buffer,"500 Error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));

						}
			
				}
			if(strncmp(cmd,"SETT",4)==0) // lenh dat setting cho logo
			{
				if(Parse_Setting(buffer,&pos,&setting)==1)
				{
					if(pos<no_logo)
					{
					   logo_list[pos].setting=setting; 
					   Update_Logo_Infor();
  					strcpy(buffer,"220 OK\n");
					sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
					   
					}
					else
					{
									strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
					  
					}
					  
				}
			}
/////////////////
			if(strncmp(cmd,"CALL",4)==0)	// call live or active logo, 
				{
					if(Parse_Active(buffer,&i_temp)==1)
						{
							
							
							if(i_temp==0)
							{
							    // them lenh khong hien thi logo vao day
							    //
                                DeActivateLogo();

                                strcpy(buffer,"220 OK\n");
							sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
							  Update_Logo_Infor();

							}
							else
							{ 
							  isfound=0;
							  for(i=0;i<no_logo;++i)
							  {
							      if(logo_list[i].setting==i_temp)
							      {
								  isfound=1;
								  break;
							      }
							      
							  }
							    if(isfound==1)
							    {
							    
							      active_logo=i;



							    if(active_logo<no_logo)
								{
#ifdef LOCK
pthread_mutex_lock( &mutex1 );
#endif
									strcpy(full_path,"//mnt//home//logos//");
									strcat(full_path,logo_list[active_logo].file_name);
									Display_Logo(fd,full_path,&width,&height);
									printf("\nWidth=%d",width);
									printf("\nHeight=%d",height);
									printf("\nFullPath=s",full_path);							
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

                                                                        #ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif       
                                                                        
									Update_Logo_Infor();
                                                                 
									strcpy(buffer,"220 OK\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
								}
							      else
								{
									strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
								}
								
							    }
							    else
							    {
							      strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
							    }
								
								
							}
						} // neu xu ly chuoi sai
			
						  else	
						    {
							strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
							
						    }
						
				}	// end CALL
////////////////////
			if(strncmp(cmd,"ACTL",4)==0)	// set active logo
				{
					if(Parse_Active(buffer,&active_logo)==1)
						{

							if(active_logo<no_logo)
								{
                                ActivateLogo(active_logo);
//                                ActivateLogo(active_logo);
                                    Update_Logo_Infor();
                                              
									strcpy(buffer,"220 OK\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
								}
							else
								{
									strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
								}
						}	
			
					else
						{
							strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
							
						}
				}

			if(strncmp(cmd,"ADDL",4)==0) // neu la lenh them 1 logo vao list
				{

					if(Parse_Position(buffer,&logo_list[no_logo].x,&logo_list[no_logo].y,&logo_list[no_logo].width,&logo_list[no_logo].height,logo_list[no_logo].file_name)==1)	// neu xu ly them duoc 1 logo
						{
							logo_list[no_logo].setting=0;

							no_logo++;
							strcpy(buffer,"220 OK\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
							Update_Logo_Infor();
			
						}
					else
						{
							printf("Loi:KHong xu ly chuoi them logo");
							strcpy(buffer,"500 error\n");
									sendto(udpsock,buffer,strlen(buffer),0,(struct sockaddr *) &echoclient,sizeof(echoclient));
						}
	
				}
			}
		}
	quit:
//	printf("CLose socket hien tai");
	close(udpsock);
	return(arg);
}
//----------------------------------------------------------------------------------------
void* RemoteControl(void* arg)
{

    int serversock;
    int fun_ret;
struct sockaddr_in echoserver, echoclient;    
    	/* Create the TCP socket */
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}
	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
	echoserver.sin_family = AF_INET;                  /* Internet/IP */
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
	echoserver.sin_port = htons(7007);       /* server port */
	/* Bind the server socket */
	if (bind(serversock, (struct sockaddr *) &echoserver,
                               sizeof(echoserver)) < 0) {
		Die("Failed to bind the server socket");
	}
	/* Listen on the server socket */
	if (listen(serversock, MAXPENDING) < 0) {
		Die("Failed to listen on server socket");
	}
	/* Run until cancelled */
	halt_prog=0;
        
        
        
    	while (halt_prog==0) {

		unsigned int clientlen = sizeof(echoclient);
		int clientsock;
		pthread_t child;
		/* Wait for client connection */
		if ((clientsock = accept(serversock, (struct sockaddr *) &echoclient,&clientlen)) < 0) {
			Die("Failed to accept client connection\n");
		}
		if(halt_prog==0)
		{
		fprintf(stdout, "Client connected: %s\n",
                    inet_ntoa(echoclient.sin_addr));
		//HandleClient(clientsock);
///                printf("\n\rFree memory=%ld",getTotalSystemMemory());
                //setsockopt(clientsock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(struct timeval));
                fun_ret=pthread_create(&child,NULL,HandleClient,&clientsock);
		if(fun_ret!=0){                                   
			printf("error create thread");
                        switch(fun_ret){
                            case EAGAIN:
                                printf("\n\rInsufficient resources to create another thread");
                                break;
                            case EINVAL:
                                printf("\n\rInvalid settings in attr.");
                                break;
                            case EPERM:
                                printf("\n\rNo permission to set the scheduling policy and parameters specified in attr.");
                                break;                                
                        }
                        
                }
		else
			pthread_detach(child);
		}
		else
		{
		close(clientsock);
		}
	}
	close(serversock);
}
//------------------------------------------------------------------------------------------
// ham nay nhan 1 chuoi tu thang client, ket thuc khi nhan duoc ky tu enter
int Get_Command(int sock,char *cmd)
{
	char rec[1000];
	char tmp[1000];
        
        clock_t start, stop;
        double t = 0.0;
	int index;
	int i;
	int received = -1;
	index=0;
        long time_out;
        time_out=0;
        assert((start = clock())!=-1);
	while(1)
		{
          //  printf("Go here..................");
	//		received=recv(sock,rec,1000,0); // receive data from socket
            received=recv(sock,rec,1000,MSG_DONTWAIT); // receive data from socket
			if(received>0)  // if data avaiable
				{
                            assert((start = clock())!=-1);
                            time_out=0;
                                        // copy it to cmd buffer
					for(i=0;i<received;++i)
						{
						cmd[index]=rec[i];
						index++;
						}
                            
                            
                            // check for Enter received?
			for(i=0;i<index;++i)
				{
					if(cmd[i]=='\n')
						{
						cmd[i]='\0'; //null to finish string						                                                
						return(1);                                                
						}
				}
				}
                        
                            /*time_out++;
                            if(time_out>2000000) // about 8 seconds                            
                                return(0);                            */
            
           stop = clock();
t = (double) (stop-start)/CLOCKS_PER_SEC;
                        
        if(t>5)                
            return(0);
			
		}
}
void Get_Command_UDP(int sock,char *cmd)
{
	char rec[1000];
	char tmp[1000];

	int index;
	int i;
	int received = -1;
	index=0;
	while(1)
		{
			received = recvfrom(sock, rec, 1000, 0,(struct sockaddr *) &echoclient,&clientlen);
//			received=recv(sock,rec,1000,0);
			if(received>0)
				{
#ifdef TEST				
                    printf("Received %d bytes\r\n",received);
#endif
					for(i=0;i<received;++i)
						{
						cmd[index]=rec[i];
#ifdef TEST
						printf("%x ",rec[i]);
#endif
						index++;
						}
				}
			for(i=0;i<index;++i)
				{
					if(cmd[i]=='\n')
						{
						cmd[i]='\0';
						return;
						}
				}

		}
}
//--------------------------------------------------------------------------------------

int Parse_Active(char *str,int *pos)
{
char temp_str[2000];
char string[100][100];

int i;
int j;
int k;
printf("\nChuoi vao:%s",str);
printf("\nLength=%d",strlen(str));
if(strlen(str)>5)
{
	printf("\nden day");
	for(i=0;i<(strlen(str)-5);++i)
		{
			temp_str[i]=str[i+5];
		}
	printf("\nden day1");
	j=0;
	k=0;
	string[j][k]='\n';
	for(i=0;i<(strlen(str)-5);++i)
		{
		string[j][k]=temp_str[i];
		k++;
		}
	string[j][k]='\0';	
	printf("\nActive=%s",string[0]);
	*pos=atoi(string[0]);
	printf("\nDen day");
	printf("\nDen day");
	printf("\nDen day");
	return(1);

}
else
 return(0);
}
//----------------------------------------------------------------------------------------
int Parse_VideoMode(char *str,int *pos)
{
char temp_str[2000];
char string[100][100];

int i;
int j;
int k;
printf("\nChuoi vao:%s",str);
printf("\nLength=%d",strlen(str));
if(strlen(str)>5)
{
	printf("\nden day");
	for(i=0;i<(strlen(str)-5);++i)
		{
			temp_str[i]=str[i+5];
		}
	printf("\nden day1");
	j=0;
	k=0;
	string[j][k]='\n';
	for(i=0;i<(strlen(str)-5);++i)
		{
		string[j][k]=temp_str[i];
		k++;
		}
	string[j][k]='\0';	
	printf("\nVideoMode=%s",string[0]);
	*pos=atoi(string[0]);
	printf("\nDen day");
	printf("\nDen day");
	printf("\nDen day");
	return(1);

}
else
 return(0);
}
//----------------------------------------------------------------------------------------

//PROP 25,54,1,34
int Parse_Property(char *str,int *x,int *y,int *pos,int *anpha)
{
char temp_str[2000];
char string[100][100];

int i;
int j;
int k;
printf("\nChuoi vao:%s",str);
printf("\nLength=%d",strlen(str));
if(strlen(str)>5)
{
	printf("\nden day");
	for(i=0;i<(strlen(str)-5);++i)
		{
			temp_str[i]=str[i+5];
		}
	printf("\nden day1");
	j=0;
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]==',')
				j++;
			
		}

	
	if(j!=3)
		return(0);

	j=0;
	k=0;
	string[j][k]='\n';
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]!=',')
				{	
					string[j][k]=temp_str[i];
					k++;
				}
			else
				{
					string[j][k]='\0';
					k=0;
					j++;
					//strcpy(string[j],"");
					string[j][k]='\0';
				}
		}
	string[j][k]='\0';	
	for(i=0;i<4;++i)
		{
		printf("\nChuoi:%d =%s",i,string[i]);
		}
	*x=atoi(string[0]);
	*y=atoi(string[1]);
	*pos=atoi(string[2]);
	*anpha=atoi(string[3]);

	printf("\nSo luong:%d",j);
		printf("\nden day2");
	printf("\nden day3");

	return(1);
}
else
{
	return(0);
}
}
//-----------------------------------------------------------------------------------------
int Parse_Setting(char *str,int *pos,int *setting)
{
char temp_str[2000];
char string[100][100];

int i;
int j;
int k;

if(strlen(str)>5)
{	
	for(i=0;i<(strlen(str)-5);++i)
		{
			temp_str[i]=str[i+5];
		}	
	j=0;
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]==',')
				j++;
			
		}

	
	if(j!=1)
		return(0);

	j=0;
	k=0;
	string[j][k]='\0';
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]!=',')
				{	
					string[j][k]=temp_str[i];
					k++;
				}
			else
				{
					string[j][k]='\0';
					k=0;
					j++;
					//strcpy(string[j],"");
					string[j][k]='\0';
				}
		}
	string[j][k]='\0';	
	for(i=0;i<2;++i)
		{
		printf("\nChuoi setting:%d =%s",i,string[i]);
		}
	*pos=atoi(string[0]);
	*setting=atoi(string[1]);	
  	
	return(1);
}
else
{
	return(0);
}
}
//-------------------------------------------------------------------------------------

// Ham nay xu ly chuoi thay doi audio dang
// AUDI 2452,2433,24334,34,234
// AUDI volume1,volume2,volume3,volume_master,pan
// tra ve 0 neu khong xu ly duoc chuoi
// tra ve 1 neu xu ly duoc
int Parse_Volume(char *str,int *vol1,int *vol2,int *vol3,int *vol_master,int *pan)
{
char temp_str[2000];
char string[100][100];

int i;
int j;
int k;

if(strlen(str)>5)
{	
	for(i=0;i<(strlen(str)-5);++i)
		{
			temp_str[i]=str[i+5];
		}	
	j=0;
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]==',')
				j++;
			
		}

	
	if(j!=4)
		return(0);

	j=0;
	k=0;
	string[j][k]='\0';
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]!=',')
				{	
					string[j][k]=temp_str[i];
					k++;
				}
			else
				{
					string[j][k]='\0';
					k=0;
					j++;
					//strcpy(string[j],"");
					string[j][k]='\0';
				}
		}
	string[j][k]='\0';	
	for(i=0;i<5;++i)
		{
		printf("\nChuoi audio:%d =%s",i,string[i]);
		}
	*vol1=atoi(string[0]);
	*vol2=atoi(string[1]);
	*vol3=atoi(string[2]);
	*vol_master=atoi(string[3]);
	*pan=atoi(string[4]);
  	
	return(1);
}
else
{
	return(0);
}
}
//----------------------------------------------------------------------------------------

int Parse_Position(char *str,int *x,int *y,int *width,int *height,char *logo_name)
{
char temp_str[2000];
char string[100][100];

int i;
int j;
int k;
printf("\nChuoi vao:%s",str);
printf("\nLength=%d",strlen(str));
if(strlen(str)>5)
{
	printf("\nden day");
	for(i=0;i<(strlen(str)-5);++i)
		{
			temp_str[i]=str[i+5];
		}
	printf("\nden day1");
	j=0;
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]==',')
				j++;
			
		}

	
	if(j!=4)
		return(0);

	j=0;
	k=0;
	string[j][k]='\n';
	for(i=0;i<(strlen(str)-5);++i)
		{
			if(temp_str[i]!=',')
				{	
					string[j][k]=temp_str[i];
					k++;
				}
			else
				{
					string[j][k]='\0';
					k=0;
					j++;
					//strcpy(string[j],"");
					string[j][k]='\0';
				}
		}
	string[j][k]='\0';	
/*	if(k>0)
	{
	string[j][k-1]='\n';	
	}
	else
		return(0);*/
	for(i=0;i<5;++i)
		{
		printf("\nChuoi:%d =%s",i,string[i]);
		}
	*x=atoi(string[0]);
	*y=atoi(string[1]);
	*width=atoi(string[2]);
	*height=atoi(string[3]);
	strcpy(logo_name,string[4]);
	for(i=0;i<strlen(logo_name);++i)
		{
		if(logo_name[i]=='\n')
		 	logo_name[i]='\0';
		}
	printf("\nSo luong:%d",j);
		printf("\nden day2");
	printf("\nden day3");

	return(1);
}
else
{
	return(0);
}
}
#ifdef MINH
//---------------------------------------------------------------------------------------
void *threadIP()
{
    int sock;                         /* Socket */
    int i;
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    int count;
    unsigned int recvStringLen;       /* Length of string received*/
    unsigned int sendStringLen;       /* Length of string to broadcast */

    struct sockaddr_in broadcastAddr; /* Broadcast address */
    struct sockaddr_in theirAddr;
    struct sockaddr_in rt_gateway;
    struct rtentry route;
    struct ifreq ifreqs[20];
    struct ifconf ifc;
    socklen_t addr_len;

    char *broadcastIP;                /* IP broadcast address */
    char *mac = "";
    char ip[25];
    char subnet[25];
    char *gateway;                    /* Gateway address*/
    char *p;
    char *new_ip = "";
    char *new_sub = "";
    char *new_gw = "";
    char check_mac[13];

    unsigned short broadcastPortSend1; /* Server port */
    unsigned short broadcastPortSend2;
    unsigned short broadcastPortRecv; /* Receive port */

    broadcastPortRecv = 9050;                   /* Broadcast Port to receive*/
    broadcastIP = "192.168.1.255";              /* broadcast IP address */
    broadcastPortSend1 = 6666;                   /* broadcast port to send */
    broadcastPortSend2 = 5555;
    printf("Da thiet lap xong IP va port. \r\n");


    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
   // printf("Tao socket thanh cong\r\n");

/*******************  IP/MAC Addr *********************/
    ifc.ifc_len = sizeof ifreqs;
    ifc.ifc_req = ifreqs;

    //Tao socket
     if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
     {
        DieWithError("SIOCGIFCONF");
        exit(1);
     }

    //kiem tra tat ca cac ket noi
    for (i = 0; i < ifc.ifc_len/sizeof(struct ifreq); ++i)
    {
        //lay thong tin tu ket noi xac dinh
        if (strcmp(ifreqs[i].ifr_name, CHECK_INTERFACE) == 0)
        {
          ///////////// Tim IP Address //////////////
          strcpy(ip,inet_ntoa(((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr));
          printf("IP address: [%s] \r\n",ip);

          //////////// Tim MAC Address //////////////
          mac = getmac(ifreqs[i].ifr_name);
          printf("mac address: [%s] \r\n",mac);
          strcpy(check_mac,mac);        //luu lai de kiem tra khi thay doi IP

          ///////////// Tim Subnet Mask ///////////////
          if (ioctl(sock,SIOCGIFNETMASK, &ifreqs[i]) == 0);
          strcpy(subnet, inet_ntoa(((struct sockaddr_in*)&ifreqs[i].ifr_netmask)->sin_addr));
          printf("Sub Netmask: [%s] \r\n", subnet);

          //Them gateway de tranh route block khi khac dai mang
          gateway = ip;
          //gateWayConfig(gateway);
          //system ("route add default gw 192.168.1.1 eth0");
          setDefGateway(ifreqs[i].ifr_name,gateway);

        }
    }

    //Them ky tu de gui dung' dinh dang ve server
     strcat(ip, " \n ");
     strcat(subnet, " \n ");
     strcat(mac, " \n ");
     close(sock);
/***********************************************/
     
/************ BAT DAU LAP VO TAN ***************/
 while(1)
 {
    char sendString[1024]="";               //lam moi xau ky tu moi lan su dung
    char recvString[MAXRECVSTRING+1]="";    /* Buffer for received string */

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /********************* RECV DATA *********************/
    //khoi tao cau truc nhan goi tin broadcast
    memset(&theirAddr, 0, sizeof(theirAddr));          /* Zero out structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;                /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPortRecv); /* Broadcast port */
    //printf("Da khoi tao thanh cong cau truc nhan goi tin broadcast. \r\n");

    //bat dau bind de nhan du lieu
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
    DieWithError("bind() failed");
    //printf("Da bind thanh cong, san sang nhan du lieu.");

    /* Receive a single datagram from the server */
    if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, (struct sockaddr *)&theirAddr, &addr_len)) < 0)
    DieWithError("recvfrom() failed");

    //Them ky tu ket thuc chuoi vao cuoi cung
    recvString[recvStringLen] = '\0';
	
	printf("Broadcast request received\r\n");
	printf("%s",recvString);

    /**************** SEND DATA ************************/
    //kiem tra du lieu nhan ve dung dinh dang hay khong
    if (strcmp(recvString, CHECK_SCAN) == 0 )
    {

        /* Set socket to allow broadcast */
        broadcastPermission = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
          sizeof(broadcastPermission)) < 0)
            DieWithError("setsockopt() failed");
       //printf("Tao socket option thanh cong.\r\n");

        /* Construct local address structure */
        //Sua lai cau truc broadcast de gui goi tin
        memset(&broadcastAddr, 0, sizeof(broadcastAddr));            /* Zero out structure */
        broadcastAddr.sin_family = AF_INET;                          /* Internet address family */
        broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);     /* Broadcast IP address */
        broadcastAddr.sin_port = htons(broadcastPortSend1);          /* Broadcast port */
        //printf("Da sua lai cau hinh gui goi tin thanh cong. \r\n");

        //Them IP, subnet, MAC vao goi tin se gui di
        strcat(sendString, ip);
        strcat(sendString, subnet);
        strcat(sendString, gateway);
        strcat(sendString, mac);
        strcat(sendString, DESCRIPTION);

        printf("du lieu se gui di la: %s \r\n",sendString);
        sendStringLen = strlen(sendString);
        printf ("Bat dau qua trinh gui du lieu di..\r\n");

        //Gui goi tin di
        if(sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
        &broadcastAddr, sizeof(broadcastAddr)) < 0 )
        {
            broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
           int m = sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
            &broadcastAddr, sizeof(broadcastAddr));
        }
        printf ("Da gui thanh cong 1 goi tin broadcast. \r\n");
        //sleep(2);   //Avoids flooding the network
        close(sock);
    }
    else
    {
     /*********** Yeu cau thay doi IP***********/
        //cat mot phan goi tin nhan duoc de kiem tra
        p = strtok(recvString," \n ");
        

        //Kiem tra goi tin nhan duoc co dung dinh dang yeu cau thay doi IP hay khong
        if (strcmp(p, CHECK_MESSAGE) == 0)
        {
            /*********** Cau hinh de gui ve tinh trang he thong *************/
            
            /* Set socket to allow broadcast */
            broadcastPermission = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
            sizeof(broadcastPermission)) < 0)
            DieWithError("setsockopt() failed");
            //printf("Tao socket option thanh cong.\r\n");

            /* Construct local address structure */
            //thiet lap lai cau truc broadcast de gui du lieu di
            memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
            broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
            broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);/* Broadcast IP address */
            broadcastAddr.sin_port = htons(broadcastPortSend2);         /* Broadcast port */
            //printf("Da sua lai cau hinh gui goi tin thanh cong. \r\n");
     //=================================================================

     /***** Kiem tra MAC Address  ******/
            //cat tiep mot phan goi tin nhan duoc
            p = strtok(NULL," \n ");
            printf ("Thong tin bat dau cat ra la: [%s] \r\n", p);
            //kiem tra thong tin cat ra voi MAC Address cua client
            if (strcmp(p, check_mac) == 0)
            {
              printf ("Dinh dang MAC Address nhan ve [%s] thoa man. \r\n",p);

              //Kiem tra dinh dang cua IP
              //cat tiep mot phan goi tin nhan duoc de lay IP Address
              p = strtok(NULL," \n ");
              //kiem tra dinh dang cua IP Address nhan duoc
              if (check_valid(p) == 1)
              {
                new_ip = p;         //luu IP Addresss moi neu thoa man
                printf ("Dinh dang IP nhan ve [%s] thoa man. \r\n",new_ip);

                //Kiem tra dinh dang cua Subnet Mask
                //cat tiep mot phan goi tin nhan duoc de lay Subnet Mask
                p = strtok(NULL," \n ");
                //kiem tra dinh dang cua Subnet Mask nhan duoc
                if ((check_valid(p) == 1) && (check_subnet(p) ==1))
                {
                    new_sub = p;    //luu Subnet Mask moi neu thoa man
                    printf ("Dinh dang Subnet Mask nhan ve [%s] thoa man. \r\n",new_sub);

                    //Kiem tra dinh dang cua Gateway
                    //cat tiep mot phan goi tin nhan duoc de lay Gateway
                    p = strtok(NULL," \n ");
                    //kiem tra dinh dang Gateway nhan duoc
                    if (check_valid(p) == 1)
                    {
                    new_gw = p;      //luu Gateway moi neu thoa man
                    printf ("Dinh dang Gateway nhan ve [%s] thoa man. \r\n",new_gw);

                    //Gui xac nhan ve server
                    strcpy(sendString, "");
                    strcpy(sendString,"Du lieu day du. Xin cho vai phut de khoi dong lai.");
                    sendStringLen = strlen(sendString);
                    if(sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                            &broadcastAddr, sizeof(broadcastAddr)) < 0 )
                        {
                            broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
                            int m = sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                                &broadcastAddr, sizeof(broadcastAddr));
                        }
                       //=======================================
                       /****** Change interface file ***********/
                       //Ghi IP Address, Subnet Mask, Gateway moi vao file interface
                        get_file(new_ip, new_sub, new_gw);

                        //Gui ve server IP Address, Subnet Mask, Gateway cu
                         close(sock);

                    //doi trong 3s
                    sleep (3);
                    //restart client
                    system("reboot");
                    //========================================
                    }
                    else
                    {
                        //Gui tra thong bao neu Gateway khongdung dinh dang
                        printf("Thong tin Gateway moi nhan ve sai dinh dang! \r\n");
                        strcpy(sendString, "");
                        strcpy(sendString,"Thong tin ve Gateway sai dinh dang");
                        sendStringLen = strlen(sendString);
                        if(sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                            &broadcastAddr, sizeof(broadcastAddr)) < 0 )
                        {
                            broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
                            int m = sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                                &broadcastAddr, sizeof(broadcastAddr));
                        }
                        close(sock);
                    }
                }
                else
                {
                    //Gui tra thong bao neu Subnet Mask khong dung dinh dang
                    printf("Thong tin Subnet Mask moi nhan ve sai dinh dang! \r\n");
                    strcpy(sendString, "");
                    strcpy(sendString,"Thong tin ve Subnet Mask sai dinh dang");
                    sendStringLen = strlen(sendString);
                    if(sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                       &broadcastAddr, sizeof(broadcastAddr)) < 0 )
                        {
                            broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
                            int m = sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                                &broadcastAddr, sizeof(broadcastAddr));
                        }
                    close(sock);
                }
              }
              else
              {
                  //Gui tra thong bao ve server neu IP Address khong dung dinh dang
                  printf("Thong tin IP moi nhan ve sai dinh dang! \r\n");
                  strcpy(sendString, "");
                  strcpy(sendString,"Thong tin ve IP Address sai dinh dang");
                  sendStringLen = strlen(sendString);
                  if(sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                    &broadcastAddr, sizeof(broadcastAddr)) < 0 )
                     {
                            broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
                            int m = sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                                &broadcastAddr, sizeof(broadcastAddr));
                     }
                  close(sock);
              }
           }
           else
           {
            //Thong bao neu MAC Address khong chinh xac
            printf("Thong tin MAC gui ve sai! \r\n");
            close(sock);
           }
        }
        else
        {
            //thong bao neu du lieu nhan ve khong dung dinh dang thay doi IP, Subnet, Gateway
            printf ("Du lieu gui ve khong dung dinh dang! \r\n");
            close(sock);
        }
    }   //dong the else
  
 }      //dong the while
}       //dong the thread
//--------------------------------------------------------------------------------

/********** GET MAC *************/
char *getmac(char *iface)
{
#define MAC_STRING_LENGTH 13
  char *ret = malloc(MAC_STRING_LENGTH);
  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  strcpy(s.ifr_name, iface);
  if (fd >= 0 && ret && 0 == ioctl(fd, SIOCGIFHWADDR, &s))
  {
    int i;
    for (i = 0; i < 6; ++i)
      snprintf(ret+i*2,MAC_STRING_LENGTH-i*2,"%02x",(unsigned char) s.ifr_addr.sa_data[i]);
  }
  else
  {
    DieWithError("malloc/socket/ioctl failed");
    exit(1);
  }
  return(ret);
}
//=================================================
int gateWayConfig(char *gateway)
{
        int sock = socket(AF_INET, SOCK_DGRAM, 0);

        if (sock > 0)
        {
            struct sockaddr_in *dst, *gw, *mask;
            struct rtentry route;

            memset(&route, 0, sizeof (struct rtentry));
            dst = (struct sockaddr_in *) (&(route.rt_dst));
            gw = (struct sockaddr_in *) (&(route.rt_gateway));
            mask = (struct sockaddr_in *) (&(route.rt_genmask));

            // Make sure we're talking about IP here
            dst->sin_family = AF_INET;
            gw->sin_family = AF_INET;
            mask->sin_family = AF_INET;

            // Set up the data for removing the default route
            dst->sin_addr.s_addr = 0;
            gw->sin_addr.s_addr = 0;
            mask->sin_addr.s_addr = 0;
            route.rt_flags = RTF_UP | RTF_GATEWAY;

            // Remove the default route
           //ioctl(sock, SIOCDELRT, &route);

            // Set up the data for adding the default route
            dst->sin_addr.s_addr = 0;
            gw->sin_addr.s_addr = inet_addr(gateway);
            mask->sin_addr.s_addr = 0;
            route.rt_metric = 1;
            route.rt_flags = RTF_UP | RTF_GATEWAY;

            // Remove this route if it already exists
            if (ioctl(sock, SIOCDELRT, &route)<0)
            // Add the default route
            if (ioctl(sock, SIOCADDRT, &route) == -1)
            {
                // *** The error never occurs ***
                printf("Qua trinh tao gateway bi loi! \r\n");
            }
            else
            {
                printf("Qua trinh tao gateway thanh cong. \r\n");
            }

            shutdown(sock, SHUT_RDWR);
            close(sock);
        }
 return 0;
 }
//==============================================

int setDefGateway(const char * deviceName,const char * defGateway)
{
int sockfd;
struct rtentry rm;
struct sockaddr_in ic_gateway ;// Gateway IP address
int err;

sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if (sockfd == -1)
{
printf("socket is -1\n");
return -1;
}

memset(&rm, 0, sizeof(rm));


ic_gateway.sin_family = AF_INET;
ic_gateway.sin_addr.s_addr = inet_addr(defGateway);
ic_gateway.sin_port = 0;

(( struct sockaddr_in*)&rm.rt_dst)->sin_family = AF_INET;
(( struct sockaddr_in*)&rm.rt_dst)->sin_addr.s_addr = 0;
(( struct sockaddr_in*)&rm.rt_dst)->sin_port = 0;

(( struct sockaddr_in*)&rm.rt_genmask)->sin_family = AF_INET;
(( struct sockaddr_in*)&rm.rt_genmask)->sin_addr.s_addr = 0;
(( struct sockaddr_in*)&rm.rt_genmask)->sin_port = 0;

memcpy((void *) &rm.rt_gateway, &ic_gateway, sizeof(ic_gateway));
rm.rt_flags = RTF_UP | RTF_GATEWAY;
if ((err = ioctl(sockfd, SIOCADDRT, &rm)) < 0)
{
printf("SIOCADDRT failed",err);
return -1;
}
return 0;
}
//--------------------------------------------------------------------------------------
/**************** Tao file interface *******************/
void get_file(char *ip, char *su, char *gw)
{

    FILE  *file;
    char  stringfile[200];
    char  LINE1[25];
    char  LINE2[33];
    char  LINE3[33];
    char  LINE4[33];
    char  LINE5[33];

    strcpy(stringfile,"");
    strcpy(LINE1,"auto eth0\n");
    strcpy(LINE2, "iface eth0 inet static\n");
    strcpy(LINE3, "address ");
    strcpy(LINE4, "netmask ");
    strcpy(LINE5, "gateway ");

    if ((file = fopen("//mnt//etc//network//interfaces", "wt")) == NULL)
    {
        printf ("Khong mo duoc file \r\n");
        return;
    }
     strcat(LINE3, ip);
     strcat(LINE4, su);
     strcat(LINE5, gw);
     
     strcat(LINE3, "\n");
     strcat(LINE4, "\n");
     strcat(LINE5, "\n");
     
     strcat(stringfile, LINE1);
     strcat(stringfile, LINE2);
     strcat(stringfile, LINE3);
     strcat(stringfile, LINE4);
     strcat(stringfile, LINE5);

     printf("Chuoi se ghi vao la: [%s] \r\n", stringfile);

     fputs (stringfile, file);
     fputs("\n", file);
     fflush(file);      //lam trong bo nho dem cua file de phuc vu cho lan ghi sau

     printf("Ghi thanh cong du lieu ra file \r\n");
     fclose(file);      //dong file sau khi lam xong
}
/**************** Kiem tra dinh dang *******************/
int check_valid(const char *str)
{
	unsigned int n1,n2,n3,n4;

        //Tach string thu duoc thanh 4 so
	if(sscanf(str,"%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4)
            return 0;

	if((n1 != 0) && (n1 <= 255) && (n2 <= 255) && (n3 <= 255) && (n4 <= 255)) {
		char buf[64];

                //dua 4 so n1,n2,n3,n4 vao 1 string
		sprintf(buf,"%u.%u.%u.%u",n1,n2,n3,n4);
		if(strcmp(buf,str)) return 0;
		return 1;
	}
	return 0;
}
//============================================
int check_subnet(const char *str)
{
	unsigned int n1,n2,n3,n4;

        //Tach string thu duoc thanh 4 so
	if(sscanf(str,"%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4)
            return 0;
        //dieu kien cua Subnet Mask
	if((n1 != 0) && (n1 == 255))
            if ((n2 == 0) || (n2 == 128) || (n2 == 192) || (n2 == 224) || (n2 == 240) || (n2 == 248) || (n2 == 252) || (n2 == 254) || (n2 == 255))
               if ((n3 == 0) || (n3 == 128) || (n3 == 192) || (n3 == 224) || (n3 == 240) || (n3 == 248) || (n3 == 252) || (n3 == 254) || (n3 == 255))
                   if ((n4 == 0) || (n4 == 128) || (n4 == 192) || (n4 == 224) || (n4 == 240) || (n4 == 248) || (n4 == 252))
                       if ((n1 >= n2) && (n2 >= n3) && (n3 > n4))
                       {
                        char buf[64];
                        //dua 4 so n1,n2,n3,n4 vao 1 string
                        sprintf(buf,"%u.%u.%u.%u",n1,n2,n3,n4);
                        if(strcmp(buf,str)) return 0;
                        return 1;
                        }
    return 0;
}
#endif //of MINH
