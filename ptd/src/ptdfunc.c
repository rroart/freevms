#include <ssdef.h>
#include <descrip.h>
#include <misc.h>
#include <ucbdef.h>
#include <ccbdef.h>
#include <ddbdef.h>

#include <system_data_cells.h>

ptd$cancel( unsigned short chan) {

}

ptd$create (unsigned short *chan ,acmode ,charbuff ,buflen ,void (astadr)() ,long astprm ,ast_acmode, inadr) {
  $DESCRIPTOR(devnam_,"fta0");
  struct dsc$descriptor * devnam=&devnam_;
  struct return_values r;
  struct _ucb * u, *newucb;
  struct _ccb * c;
  int status;
  ioc$search(&r, devnam) ;
  u=r.val1;

  ioc_std$clone_ucb(u->ucb$l_ddb->ddb$ps_ucb /*&er$ucb*/,&newucb);
  //    exe$assign(dsc,&chan,0,0,0);

  status=ioc$ffchan(chan);
  c=&ctl$gl_ccbbase[*chan];
  c->ccb$b_amod=1; /* wherever this gets set */
  c->ccb$l_ucb=newucb;
  c->ccb$l_ucb->ucb$l_refc++;

  ft$create();

}

ptd$delete ( unsigned short chan) {

}

ptd$read (efn, unsigned short chan ,void (astadr)() ,long astprm readbuf, readbuf_len) {

}

ptd$readw (efn, unsigned short chan ,void (astadr)() ,long astprm readbuf, readbuf_len) {

}

ptd$set_event_notification (unsigned short chan, void (astadr)() ,long astprm ,acmode, type) {

}

ptd$write (unsigned short chan ,void (astadr)() ,long astprm, char * wrtbuf, wrtbuf_len ,echobuf ,echobuf_len) {

}

