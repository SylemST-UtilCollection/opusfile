/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE libopusfile SOFTWARE CODEC SOURCE CODE. *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE libopusfile SOURCE CODE IS (C) COPYRIGHT 1994-2012           *
 * by the Xiph.Org Foundation and contributors http://www.xiph.org/ *
 *                                                                  *
 ********************************************************************/
/*For fileno()*/
#if !defined(_POSIX_SOURCE)
# define _POSIX_SOURCE 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#if defined(_WIN32)
/*We need the following two to set stdin/stdout to binary.*/
# include <io.h>
# include <fcntl.h>
#endif
#include <opusfile.h>

#if defined(OP_FIXED_POINT)
typedef opus_int16 op_sample;
# define op_read_native_stereo op_read_stereo
#else
typedef float op_sample;
# define op_read_native_stereo op_read_float_stereo
#endif

static void print_duration(FILE *_fp,ogg_int64_t _nsamples,int _frac){
  ogg_int64_t seconds;
  ogg_int64_t minutes;
  ogg_int64_t hours;
  ogg_int64_t days;
  ogg_int64_t weeks;
  _nsamples+=_frac?24:24000;
  seconds=_nsamples/48000;
  _nsamples-=seconds*48000;
  minutes=seconds/60;
  seconds-=minutes*60;
  hours=minutes/60;
  minutes-=hours*60;
  days=hours/24;
  hours-=days*24;
  weeks=days/7;
  days-=weeks*7;
  if(weeks)fprintf(_fp,"%liw",(long)weeks);
  if(weeks||days)fprintf(_fp,"%id",(int)days);
  if(weeks||days||hours){
    if(weeks||days)fprintf(_fp,"%02ih",(int)hours);
    else fprintf(_fp,"%ih",(int)hours);
  }
  if(weeks||days||hours||minutes){
    if(weeks||days||hours)fprintf(_fp,"%02im",(int)minutes);
    else fprintf(_fp,"%im",(int)minutes);
    fprintf(_fp,"%02i",(int)seconds);
  }
  else fprintf(_fp,"%i",(int)seconds);
  if(_frac)fprintf(_fp,".%03i",(int)(_nsamples/48));
  fprintf(_fp,"s");
}

static void print_size(FILE *_fp,opus_int64 _nbytes,int _metric,
 const char *_spacer){
  static const char SUFFIXES[7]={' ','k','M','G','T','P','E'};
  opus_int64 val;
  opus_int64 den;
  opus_int64 round;
  int        base;
  int        shift;
  base=_metric?1000:1024;
  round=0;
  den=1;
  for(shift=0;shift<6;shift++){
    if(_nbytes<den*base-round)break;
    den*=base;
    round=den>>1;
  }
  val=(_nbytes+round)/den;
  if(den>1&&val<10){
    if(den>=1000000000)val=(_nbytes+(round/100))/(den/100);
    else val=(_nbytes*100+round)/den;
    fprintf(_fp,"%li.%02i%s%c",(long)(val/100),(int)(val%100),
     _spacer,SUFFIXES[shift]);
  }
  else if(den>1&&val<100){
    if(den>=1000000000)val=(_nbytes+(round/10))/(den/10);
    else val=(_nbytes*10+round)/den;
    fprintf(_fp,"%li.%i%s%c",(long)(val/10),(int)(val%10),
     _spacer,SUFFIXES[shift]);
  }
  else fprintf(_fp,"%li%s%c",(long)val,_spacer,SUFFIXES[shift]);
}

int main(int _argc,const char **_argv){
  OggOpusFile *of;
  ogg_int64_t  pcm_offset;
  ogg_int64_t  pcm_print_offset;
  ogg_int64_t  nsamples;
  opus_int32   bitrate;
  int          ret;
  int          prev_li;
  int          is_ssl;
#if defined(_WIN32)
# undef fileno
# define fileno _fileno
  /*We need to set stdin/stdout to binary mode. Damn windows.*/
  /*Beware the evil ifdef. We avoid these where we can, but this one we
     cannot.
    Don't add any more.
    You'll probably go to hell if you do.*/
  _setmode(fileno(stdin),_O_BINARY);
  _setmode(fileno(stdout),_O_BINARY);
#endif
  if(_argc!=2){
    fprintf(stderr,"Usage: %s <file.opus>\n",_argv[0]);
    return EXIT_FAILURE;
  }
  is_ssl=0;
  if(strcmp(_argv[1],"-")==0){
    OpusFileCallbacks cb={NULL,NULL,NULL,NULL};
    of=op_open_callbacks(op_fdopen(&cb,fileno(stdin),"rb"),&cb,NULL,0,&ret);
  }
  else{
    /*Try to treat the argument as a URL.*/
    of=op_open_url(_argv[1],&ret,OP_SSL_SKIP_CERTIFICATE_CHECK(1),NULL);
#if 0
    if(of==NULL){
      OpusFileCallbacks  cb={NULL,NULL,NULL,NULL};
      void              *fp;
      /*For debugging: force a file to not be seekable.*/
      fp=op_fopen(&cb,_argv[1],"rb");
      cb.seek=NULL;
      cb.tell=NULL;
      of=op_open_callbacks(fp,&cb,NULL,0,NULL);
    }
#else
    if(of==NULL)of=op_open_file(_argv[1],&ret);
    /*This is not a very good check, but at least it won't give false
       positives.*/
    else is_ssl=strncmp(_argv[1],"https:",6)==0;
#endif
  }
  if(of==NULL){
    fprintf(stderr,"Failed to open file '%s': %i\n",_argv[1],ret);
    return EXIT_FAILURE;
  }
  if(op_seekable(of)){
    ogg_int64_t duration;
    opus_int64  size;
    fprintf(stderr,"Total number of links: %i\n",op_link_count(of));
    duration=op_pcm_total(of,-1);
    fprintf(stderr,"Total duration: ");
    print_duration(stderr,duration,3);
    fprintf(stderr," (%li samples @ 48 kHz)\n",(long)duration);
    size=op_raw_total(of,-1);
    fprintf(stderr,"Total size: ");
    print_size(stderr,size,0,"");
    fprintf(stderr,"\n");
  }
  prev_li=-1;
  nsamples=0;
  pcm_offset=op_pcm_tell(of);
  if(pcm_offset!=0){
    fprintf(stderr,"Non-zero starting PCM offset: %li\n",(long)pcm_offset);
  }
  pcm_print_offset=pcm_offset-48000;
  bitrate=0;
  for(;;){
    ogg_int64_t next_pcm_offset;
    op_sample   pcm[120*48*2];
    int         li;
    ret=op_read_native_stereo(of,pcm,sizeof(pcm)/sizeof(*pcm));
    if(ret<0){
      fprintf(stderr,"\nError decoding '%s': %i\n",_argv[1],ret);
      if(is_ssl)fprintf(stderr,"Possible truncation attack?\n");
      ret=EXIT_FAILURE;
      break;
    }
    li=op_current_link(of);
    if(li!=prev_li){
      const OpusHead *head;
      const OpusTags *tags;
      int             ci;
      /*We found a new link.
        Print out some information.*/
      fprintf(stderr,"Decoding link %i:                          \n",li);
      head=op_head(of,li);
      fprintf(stderr,"  Channels: %i\n",head->channel_count);
      if(op_seekable(of)){
        ogg_int64_t duration;
        opus_int64  size;
        duration=op_pcm_total(of,li);
        fprintf(stderr,"  Duration: ");
        print_duration(stderr,duration,3);
        fprintf(stderr," (%li samples @ 48 kHz)\n",(long)duration);
        size=op_raw_total(of,li);
        fprintf(stderr,"  Size: ");
        print_size(stderr,size,0,"");
        fprintf(stderr,"\n");
      }
      if(head->input_sample_rate){
        fprintf(stderr,"  Original sampling rate: %lu Hz\n",
         (unsigned long)head->input_sample_rate);
      }
      tags=op_tags(of,li);
      fprintf(stderr,"  Encoded by: %s\n",tags->vendor);
      for(ci=0;ci<tags->comments;ci++){
        fprintf(stderr,"  %s\n",tags->user_comments[ci]);
      }
      fprintf(stderr,"\n");
      if(!op_seekable(of)){
        pcm_offset=op_pcm_tell(of)-ret;
        if(pcm_offset!=0){
          fprintf(stderr,"Non-zero starting PCM offset in link %i: %li\n",
           li,(long)pcm_offset);
        }
      }
    }
    if(li!=prev_li||pcm_offset>=pcm_print_offset+48000){
      opus_int32 next_bitrate;
      opus_int64 raw_offset;
      next_bitrate=op_bitrate_instant(of);
      if(next_bitrate>=0)bitrate=next_bitrate;
      raw_offset=op_raw_tell(of);
      fprintf(stderr,"\r ");
      print_size(stderr,raw_offset,0,"");
      fprintf(stderr,"  ");
      print_duration(stderr,pcm_offset,0);
      fprintf(stderr,"  (");
      print_size(stderr,bitrate,1," ");
      fprintf(stderr,"bps)                    \r");
      pcm_print_offset=pcm_offset;
    }
    next_pcm_offset=op_pcm_tell(of);
    if(pcm_offset+ret!=next_pcm_offset){
      fprintf(stderr,"\nPCM offset gap! %li+%i!=%li\n",
       (long)pcm_offset,ret,(long)next_pcm_offset);
    }
    pcm_offset=next_pcm_offset;
    if(ret<=0){
      ret=EXIT_SUCCESS;
      break;
    }
    if(!fwrite(pcm,sizeof(*pcm)*2,ret,stdout)){
      fprintf(stderr,"\nError writing decoded audio data: %s\n",
       strerror(errno));
      ret=EXIT_FAILURE;
      break;
    }
    nsamples+=ret;
    prev_li=li;
  }
  op_free(of);
  if(ret==EXIT_SUCCESS){
    fprintf(stderr,"\nDone: played ");
    print_duration(stderr,nsamples,3);
    fprintf(stderr," (%li samples @ 48 kHz).\n",(long)nsamples);
  }
  return ret;
}
