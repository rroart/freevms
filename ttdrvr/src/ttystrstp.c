#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<idbdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<dyndef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>
#include <ttyucbdef.h>
#include <ttyvecdef.h>
#include <ttytadef.h>

unsigned long tty$startio (struct _irp * i, struct _ucb * u)
{
  unsigned long sts=SS$_NORMAL;

  struct _ltrm_ucb * lt = u;

  struct _ucb * phy;

  phy = lt->ucb$l_tl_phyucb;

  if (phy==0) {
    printk("<0>" "PHY 0 %x %s\n",lt,&u->ucb$l_ddb->ddb$t_name[1]);
    return SS$_NORMAL;
  }

  u=phy;

  u->ucb$l_svapte=i->irp$l_svapte;
  u->ucb$l_boff=i->irp$l_boff;
  u->ucb$l_bcnt=i->irp$l_bcnt;

  switch (i->irp$v_fcode) {

    case IO$_WRITELBLK:
    case IO$_WRITEVBLK:
    case IO$_WRITEPBLK:
      {
	struct _tty_ucb * tty = u;
	struct _tt_port * ttp = tty->ucb$l_tt_port;
	int (*fn)();
	fn = ttp->port_start_read;
	fn(); // not yet?

	if (tty->ucb$l_tt_typahd==0) {  // this is duplicated -> own routine?
	  tty->ucb$l_tt_typahd = kmalloc (sizeof (struct _tt_type_ahd),GFP_KERNEL);
	  struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;
	  memset(ahd, 0, sizeof(struct _tt_type_ahd));
	  ahd->tty$b_ta_type = DYN$C_TYPAHD;
	  ahd->tty$l_ta_data=kmalloc(1024,GFP_KERNEL);
	  ahd->tty$l_ta_get=ahd->tty$l_ta_put=ahd->tty$l_ta_data;
	  ahd->tty$l_ta_end=(long)ahd->tty$l_ta_data+1024;
	  // the book says this should fork and dismiss character
	  tty->ucb$b_tt_outype=0;
	  // not  yet? : return;
	}

	tty->tty$v_st_write = 1;
	int chr;
	int CC;
	tty$getnextchar(&chr, &CC, u);

	tty=u;
	if (tty->ucb$b_tt_outype==0) //check
	  return sts;
	fn = ttp->port_startio;
	sts = fn(i,u);
	return sts;
      }
      break;
      
    case IO$_READLBLK:
    case IO$_READVBLK:
    case IO$_READPBLK:
      {
	struct _tty_ucb * tty = u;
	struct _tt_port * ttp = tty->ucb$l_tt_port;
	int (*fn)();
	fn = ttp->port_start_read;
	fn(); // not yet?

	if (tty->ucb$l_tt_typahd==0) {  // this is duplicated -> own routine?
	  tty->ucb$l_tt_typahd = kmalloc (sizeof (struct _tt_type_ahd),GFP_KERNEL);
	  struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;
	  memset(ahd, 0, sizeof(struct _tt_type_ahd));
	  ahd->tty$b_ta_type = DYN$C_TYPAHD;
	  ahd->tty$l_ta_data=kmalloc(1024,GFP_KERNEL);
	  ahd->tty$l_ta_get=ahd->tty$l_ta_put=ahd->tty$l_ta_data;
	  ahd->tty$l_ta_end=(long)ahd->tty$l_ta_data+1024;
	  // the book says this should fork and dismiss character
	  tty->ucb$b_tt_outype=0;
	  // not  yet? : return;
	}
	
	int chr;
	int CC;
	tty$getnextchar(&chr, &CC, u);

	tty=u;
	if (tty->ucb$b_tt_outype==0) //check
	  return sts;
	fn = ttp->port_startio;
	sts = fn(i,u);
	return sts;
      }
      break;

    /* Who knows what */

    default: {
      return (SS$_BADPARAM);
    }
  }
}

int tty$wrtstartio(struct _irp * i,struct _ucb * u) {
}
