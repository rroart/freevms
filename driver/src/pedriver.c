// $Id$
// $Locker$

// Author. Roar Thron�s.

#include<linux/vmalloc.h>

#include<cddbdef.h>
#include<cdtdef.h>
#include<crbdef.h>
#include<ddbdef.h>
#include<ddtdef.h>
#include<devdef.h>
#include<dptdef.h>
#include<dyndef.h>
#include<fdtdef.h>
#include<iodef.h>
#include<ipldef.h>
#include<irpdef.h>
#include<mscpdef.h>
#include<pdtdef.h>
#include<ssdef.h>
#include<ucbdef.h>

int scs_std$sendmsg(struct _irp * i, struct _pdt * p) {
  char * buf;
  insque(buf,&p->pdt$q_comqh);
}

int scs_std$allocmsg(){

}


int scs_std$deallomsg(){

}


int scs_std$reqdata(){

}


int scs_std$senddata(){

}


int scs_std$sendmsg2(){

}


int scs_std$connect(){

}


int scs_std$dconnect(){

}



