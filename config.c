#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "control.h"

extern struct leveler_item leveler_param;
extern struct comp_item comp_param;
extern struct audio_mixer audio_mixer_param;
extern struct key_mixer   key_mixer_param;
extern struct reference reference_param;
extern struct frame_sync frame_sync_param;
extern struct agc_item alc_param;
extern struct audio_delay_item audio_delay_param;
extern struct auto_mixer auto_mixer_param;

extern pthread_mutex_t mutex1;
void SaveAudioParam(){
    FILE *fout;
int i;

#ifdef LOCK
pthread_mutex_lock( &mutex1 );
#endif

fout=fopen("//mnt//home//admin//audio_cfg.bin","wb");
if(fout==NULL)
	{
		printf("\nKhong mo duoc file");
                goto exit_prog;
	}	
fwrite(&leveler_param,sizeof(struct leveler_item),1,fout);
fwrite(&comp_param,sizeof(struct comp_item),1,fout);
fwrite(&audio_mixer_param,sizeof(struct audio_mixer),1,fout);
fwrite(&key_mixer_param,sizeof(struct key_mixer),1,fout);
fwrite(&reference_param,sizeof(struct reference),1,fout);
fwrite(&frame_sync_param,sizeof(struct frame_sync),1,fout);
fwrite(&alc_param,sizeof(struct agc_item),1,fout);
fwrite(&audio_delay_param,sizeof(struct audio_delay_item),1,fout);
fwrite(&auto_mixer_param,sizeof(struct audio_mixer),1,fout);
fclose(fout);
//printf("\n\rLuu lai cau hinh--------------------");
exit_prog:

i=0;
#ifdef LOCK
pthread_mutex_unlock( &mutex1 );
#endif


}
void LoadAudioParam(){
        FILE *fin;
fin=fopen("//mnt//home//admin//audio_cfg.bin","rb");
if(fin==NULL)
	{
		printf("Khong mo duoc file config");
		return;
	}

fread(&leveler_param,sizeof(struct leveler_item),1,fin);
fread(&comp_param,sizeof(struct comp_item),1,fin);
fread(&audio_mixer_param,sizeof(struct audio_mixer),1,fin);
fread(&key_mixer_param,sizeof(struct key_mixer),1,fin);
fread(&reference_param,sizeof(struct reference),1,fin);
fread(&frame_sync_param,sizeof(struct frame_sync),1,fin);
fread(&alc_param,sizeof(struct agc_item),1,fin);
fread(&audio_delay_param,sizeof(struct audio_delay_item),1,fin);
fread(&auto_mixer_param,sizeof(struct audio_mixer),1,fin);
fclose(fin);

}
