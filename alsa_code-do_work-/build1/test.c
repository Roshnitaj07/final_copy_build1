/******************************************************************************
 *
 *
 *   ALLGO EMBEDDED SYSTEMS CONFIDENTIAL PROPRIETARY
 *
 *    (C) 2019 ALLGO EMBEDDED SYSTEMS PVT. LTD.
 *
 *   FILENAME        - test.c
 *
 *   COMPILER        - gcc 5.4.0
 *
 ******************************************************************************
 *  CHANGE HISTORY
 *   ---------------------------------------------------------------------------
 *   DATE           REVISION      		AUTHOR                  COMMENTS
 *   ---------------------------------------------------------------------------
 *   26/08/2019     1.0             Ankit Raj,Roshni         Initial Version
 *                                  Taj,Aishwarya M
 *     
 ********************************************************************************
 *   DESCRIPTION
 *   This file contains all audio player related interfaces apllication.
 *   
 ******************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include <alsa/asoundlib.h> 
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

#include"CML_AudioPlayer_Interfaces.h"
#include"cml_error_codes.h"

#define INC_VOLUME 10
#define DEC_VOLUME 10
#define MUTE 0
#define MAX_VOLUME 100
char getkey();
struct time
{
    int h,m,s;
}sTime;



CML_UINT32 time_to_sec(struct time stime1)
{
    CML_UINT32 timeinsec =(CML_UINT32) stime1.s + (stime1.m * 60) + (stime1.h * 60 * 60);
    return timeinsec;
}

int main(int argc,char **argv)
{
    eRetType estatus;
    CML_UINT32 isec;
    CML_CHAR ch;
    static CML_LONG ivolume = 50;
    scmlplayer *spmyplayer = (scmlplayer *)malloc(sizeof(scmlplayer));
    if(NULL == spmyplayer)
    {
        printf("Memory not allocated\n");
        return 0;
    }
    estatus = cml_audioplayer_init(argc , argv , spmyplayer);
    if (estatus != E_CML_SUCCESS)
    {
        printf("%s\n", cml_perror(estatus));
    }
    
    do
    {
        ch = getkey();
        switch(ch)
        {
            case 'P':
            case 'p':estatus = cml_audioplayer_pause_resume(spmyplayer);
                     if(estatus != E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     
                     break;
            case 'f':
            case 'F':estatus = cml_audioplayer_seek_forward(spmyplayer);
                     if(estatus != E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     
                     break;
            case 'g':
            case 'G':estatus = cml_audioplayer_seek_back(spmyplayer);
                     if(estatus != E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     
                     break;
            case 's':
            case 'S':estatus = cml_audioplayer_playback_stop(spmyplayer);
                     if(estatus != E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     
                     break;
            case 'w':
            case 'W':estatus = cml_audioplayer_play(spmyplayer);
                     if(estatus != E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     
                     break;
            case '+':if(ivolume == MAX_VOLUME)
                     {
                         printf("Maximum_Volume_Reached\n");
                         break;
                     }
                     ivolume += INC_VOLUME;
                     cml_audioplayer_volume_control(ivolume);
                     break;
            case '-': if(ivolume == MUTE)
                     {
                         printf("Minimum_Volume_Reached\n");
                         break;
                     }
                     ivolume-=DEC_VOLUME;
                     cml_audioplayer_volume_control(ivolume);
                     break;                     
            case 'j':
            case 'J':scanf("%2d%*c%2d%*c%2d",&sTime.h,&sTime.m,&sTime.s);
                     isec = time_to_sec(sTime);
                     estatus = cml_audioplayer_seek_to(isec, spmyplayer);
                     if(estatus !=E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     break;
            case 'n':
            case 'N':estatus = cml_audioplayer_play_next_file(spmyplayer);
                     if(estatus != E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     break;
            case 'b':
            case 'B':estatus = cml_audioplayer_play_prev_file(spmyplayer);
                     if(estatus != E_CML_SUCCESS)
                     {
                         printf("%s\n",cml_perror(estatus));
                     }
                     break;
        }
    }
        while(ch != 'q' && ch != 'Q');
        cml_audioplayer_playback_stop(spmyplayer);
        cml_audioplayer_dinit(spmyplayer);
        printf("END\n");
        return 0;
    
}

char getkey() 
{
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

