/******************************************************************************
 *
 *
 *   ALLGO EMBEDDED SYSTEMS CONFIDENTIAL PROPRIETARY
 *
 *    (C) 2019 ALLGO EMBEDDED SYSTEMS PVT. LTD.
 *
 *   FILENAME        - CML_types.h
 *
 *   COMPILER        - gcc 5.4.0
 *
 ******************************************************************************
 *  CHANGE HISTORY
 *   ---------------------------------------------------------------------------
 *   DATE           REVISION      		AUTHOR                  COMMENTS
 *   ---------------------------------------------------------------------------
 *   26/08/2019     1.0         	Ankit Raj,Roshini         Initial Version
 *   					        	Taj,Aishwarya M
 *  
 ********************************************************************************
 *   DESCRIPTION
 *   This file contains all audio player related internal header functions implimentation.
 *   
 ******************************************************************************/
#include<strings.h>
#include<stdlib.h>
#include"cml_error_codes.h"
#include"CML_types.h"
#include"CML_AudioPlayer_types.h"

#define MAX_AUDIO_FILES_RANGE 10000

CML_VOID swap (scmlmyplaylist *splist1, scmlmyplaylist *splist2)  
{ 
    scmlmyplaylist temp = *splist1;  
    *splist1 = *splist2; 
    *splist2 = temp;  
}
CML_VOID cml_shuffle( scmlmyplaylist samyplaylist[] , scmlaudioinfo *spfileinfo)
{
    srand (time(NULL));  
    for (int i = spfileinfo->icount - 1; i >= 0; i--)  
    {
        int j = rand() % (i + 1); 
        swap(&samyplaylist[i], &samyplaylist[j]);  
    }   
}


eRetType cml_checkinput(CML_UINT32 iNoOfargs,CML_CHAR *pacargslist[])
{
    CML_CHAR *pc;
    eRetType ciret;
    if(iNoOfargs < 2)
    {
        ciret = E_CML_NO_ARGUMENTS;
    }
    else
    {
        if((strcmp(pacargslist[1],"-s"))== 0 || (strcmp(pacargslist[1],"-S"))== 0)
        {   pc=strrchr(pacargslist[2],'.');
            if(strcmp(pc,".wav")==0 || strcmp(pc,".txt")==0)
            {   
                ciret = E_CML_SHUFFLE;
            }
            else if(NULL == pacargslist[2])
            {
                ciret = E_CML_INVALID_ARGUMENTS_PASSED ;
                
            }
        }
        else
        {
            pc=strrchr(pacargslist[1],'.');
            if(strcmp(pc,".wav")==0 || strcmp(pc,".txt")==0)
            {
                ciret = E_CML_SUCCESS;
            }
            else
            {
                ciret = E_CML_INVALID_ARGUMENTS_PASSED;
            }
            
        }
    }
    return ciret;
}


eRetType cml_myplaylist(CML_INT32 iNoOfargs , CML_CHAR *pacargslist[] , CML_INT32 eshufflag , scmlmyplaylist samyplaylist[] , scmlaudioinfo *spfileinfo)
{
    eRetType mpret;
    CML_FILE *fp;
    CML_CHAR str[256]; 
    CML_INT32 i,j=1;
    spfileinfo->icount=0;
    CML_CHAR *pfext,fline[256];
    CML_CHAR clen;
    mpret = E_CML_SUCCESS;
    if(eshufflag)
    {
        j=2;
    }

    for(i=j;i<iNoOfargs;i++)
    {
        pfext=strrchr(pacargslist[i],'.');
        if(strcmp(pfext,".wav")==0)
        {
            clen=strlen(pacargslist[i]);
            samyplaylist[spfileinfo->icount].pfilepath=(char*)malloc((clen+1)*sizeof(char));
            if(NULL == samyplaylist[spfileinfo->icount].pfilepath)
            {
                mpret = E_CML_MEMORY_NOT_ALLOCATED;
                continue;
            }
            strncpy( samyplaylist[spfileinfo->icount++].pfilepath,pacargslist[i],clen);
        }
        else if(strcmp ( pfext,".txt" ) == 0)
        {
            fp=fopen(pacargslist[i],"r");
            if(NULL == fp)
            {
                printf("couldn,t open file empty file at %dth arguments\n",i);
                continue;
            }
            else
            {
                while(fgets ( fline, sizeof (fline), fp ) != NULL)
                {
                    if(strcmp( fline,"\n")!=0)
                    {
                        pfext=strrchr(fline, '.');
                        if(strcmp(pfext, ".wav\n") == 0)
                        {
                            fline[strlen(fline)-1]='\0';
                            clen=strlen(fline);
                            samyplaylist[spfileinfo->icount].pfilepath=(char*)malloc((clen+1)*sizeof(char));
                            if(NULL == samyplaylist[spfileinfo->icount].pfilepath)
                            {
                                mpret = E_CML_MEMORY_NOT_ALLOCATED;
                                continue;
                            }
                            strncpy( samyplaylist[spfileinfo->icount++].pfilepath, fline,clen);
                        }
                    }
                }
            }
            fclose(fp);
        } 
        else
        {
            printf("invalid file extension at %dth arguments\n",i);
        }
        
        if(spfileinfo->icount >= MAX_AUDIO_FILES_RANGE)
        {
            mpret = E_CML_CANNOT_CREATE_PLAYLIST_EXCEEDED_AUDIO_RANGE;
            return mpret;
        }
    }
    printf("Number Of Files:%d\n",spfileinfo->icount);
    return mpret;         
}


CML_VOID cml_extractheaderinfo(scmlwavheaderinfo *spsetconfig , scmlaudioinfo *spfile)
{
    // CML_UINT32 iheader;
    CML_CHAR *cheader;
    cheader=(CML_CHAR*)malloc(44);

    if(NULL == cheader)
    {
        printf("CML MEMORY NOT ALLOCATED");
    }

    //iheader=open(samyplaylist[sCurFile.icurfileindex].pfilepath,O_RDONLY);

    if(read(spfile->ifiledesc,cheader,44)!=44)
    {
        printf("Unable to read header data\n");
        return;
    }
   //printf("%s\n", scmlplaylist.pfilepath);
    /*overall size*/
    spsetconfig->isize=(*(CML_UINT32 *)(cheader+4));
    printf("Overall Size:%u\n", spsetconfig->isize);
    /*no of channels*/
    spsetconfig->uwchannel=(*(CML_UINT32 *)(cheader+22));
 printf("Channel:%d",spsetconfig->uwchannel);
      if(1==spsetconfig->uwchannel)
     printf("(MONO)\n");
 else printf("(Stereo)\n");
 
    /*Sample Rate*/
    spsetconfig->isamplerate=(*(CML_UINT32 *)(cheader+24));
 printf("Sample rate:%u\n", spsetconfig->isamplerate);
    /*BYTE RATE*/
    spsetconfig->ibyterate=(*(CML_UINT32 *)(cheader+28));
 printf("Byte rate:%u\n", spsetconfig->ibyterate);
    /*Time*/
    spsetconfig->iduration= (int)(spsetconfig->isize)/(spsetconfig->ibyterate);
 printf("Duration in sec:%d\n", spsetconfig->iduration);
    //close(iheader);
    free(cheader);

}
