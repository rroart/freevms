// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<descrip.h>
#include<system_data_cells.h>
#include<ssdef.h>
#include<stdarg.h>

asmlinkage int exe$faol(void * ctrstr , int * outlen , void * outbuf , int * prmlst) {
  int * argv=prmlst;
  struct dsc$descriptor * in=ctrstr;
  struct dsc$descriptor * out=outbuf;
  int in_dex=0,out_dex=0;
  char * in_p=in->dsc$a_pointer;
  char * out_p=out->dsc$a_pointer;
  signed int argv_num=0;
  while (in_dex<in->dsc$w_length) {
    char c;
    c=in_p[in_dex++];
    switch (c) {
    case '!':
      c=in_p[in_dex++];
      switch (c) {
      case 'A':
	c=in_p[in_dex++];
	switch (c) {
	case 'C':
	  {
	    char * bufc=argv[argv_num++];
	    char len=*bufc++;
	    memcpy(&out_p[out_dex],bufc,len);
	    out_dex+=len;
	  }
	  break;
	case 'F':
	  // not quite implemented yet
	case 'D':
	  {
	    char len=argv[argv_num++];
	    char * bufc=argv[argv_num++];
	    memcpy(&out_p[out_dex],bufc,len);
	    out_dex+=len;
	  }
	  break;
	case 'S':
	  {
	    struct dsc$descriptor * d=argv[argv_num++];
	    char len=d->dsc$w_length;
	    char * bufc=d->dsc$a_pointer;
	    memcpy(&out_p[out_dex],bufc,len);
	    out_dex+=len;
	  }
	  break;
	case 'Z':
	  {
	    // do overflow check later
	    char * bufc=argv[argv_num++];
	    char len=strlen(bufc);
	    memcpy(&out_p[out_dex],bufc,len);
	    out_dex+=len;
	  }
	  break;
	default:
	  printk("fao !A%c stuff not recognized\n",c);
	}
	break;
      case 'O':
      case 'X':
      case 'Z':
      case 'S':
      case 'U':
      case 'I':
	{
	  char type=0;
	  long mask=0;
	  int num=0;
	  switch (c) {
	  case 'O':
	    type='o';
	    break;
	  case 'X':
	    type='x';
	    break;
	  case 'Z':
	  case 'I':
	  case 'S':
	  case 'U':
	    type='d';
	    break;
	  }
	  c=in_p[in_dex++];
	  switch (c) {
	  case 'B':
	    mask=0xff;
	    break;
	  case 'W':
	    mask=0xffff;
	    break;
	  case 'A':
	  case 'L':
	    mask=0xffffffff;
	    break;
	  default:
	    printk("fao !O/X/Z%c stuff not recognized\n",c);
	    break;
	  }
	  num=mask&argv[argv_num++];
	  char * format="% ";
	  format[1]=type;
	  out_dex+=sprintf(&out_p[out_dex],format,num);
	}
	break;
      case '/':
	out_p[out_dex++]='\n';
	break;
      case '_':
	out_p[out_dex++]='\t';
	break;
      case '^':
	// not quite right
	out_p[out_dex++]='\t';
	break;
      case '!':
	out_p[out_dex++]='!';
	break;
      case '%':
	c=in_p[in_dex++];
	switch (c) {
	case 'T':
	  {
	    long long * l=argv[argv_num++];
	    if (l==0)
	      l=&exe$gq_systime;
	    struct dsc$descriptor d;
	    char chr[32];
	    d.dsc$w_length=32;
	    d.dsc$a_pointer=chr;
	    short int len;
	    exe$asctim(&len,&d,l,0);
	    memcpy(&out_p[out_dex],d.dsc$a_pointer,len);
	    out_dex+=len;
	  }
	  break;
	default:
	  printk("fao !percent %c not recognized\n",c);
	}
	break;
      default:
	printk("fao !%c not recognized\n",c);
	break;
      }
      break;
    default:
      out_p[out_dex++]=c;
    }
  }
#if 0
  sprintf(out->dsc$w_apointer,format,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16]);
#endif
  if (outlen) (*outlen)=out_dex;
  return SS$_NORMAL;
}

asmlinkage int exe$fao(void * ctrstr , int * outlen , void * outbuf , ...) {
  va_list args;
  int argv[18],argc=0;
  va_start(args,outbuf);
  while(argc<17) {
    argv[argc]=va_arg(args,int);
    argc++;
  }
  va_end(args);
  return exe$faol(ctrstr,outlen,outbuf,argv);
}

