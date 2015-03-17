// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>

#include<starlet.h>
#include<ssdef.h>
#include<dvidef.h>
#include<system_data_cells.h>
#include<misc.h>
#include<ddbdef.h>
#include<pridef.h>
#include<ucbdef.h>
#include <exe_routines.h>
#include <ioc_routines.h>
#include <sch_routines.h>

#include<linux/sched.h>

struct _generic_64
{
    long long l;
};

asmlinkage int exe$getdvi(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, struct _generic_64 *nullarg);

asmlinkage int exe$getdvi_wrap(struct struct_getdvi *s)
{
    return exe$getdvi(s->efn,s->chan,s->devnam,s->itmlst,s->iosb,s->astadr,s->astprm,s->nullarg);
}

struct _ddb * contxt=0;

asmlinkage int exe$getdvi(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, struct _generic_64 *nullarg)
{

    struct _ddb * d;
    struct item_list_3 * it=itmlst;
    struct _ucb * u = 0;

    exe$clref(efn);
    // check and do with iosb
    // check quota
    // check chan no?
    // then check devnam

    if (chan==0 && devnam!=0)
    {
        struct return_values r,r2;
        int status=ioc$search(&r,devnam);
        if (status==SS$_NORMAL)
            u=r.val1;
        else
            return status;
    }
    if (chan!=0)
    {
        struct _ccb * c = &ctl$gl_ccbbase[chan];
        u = c->ccb$l_ucb;
    }

    // check itemcode with dvsdef defs
    if (contxt)
        d=contxt;
    else
        d=ioc$gl_devlist;
    // sch$iolockr
    while (it->item_code)
    {
        int * bufaddr_int = it->bufaddr;
        switch (it->item_code)
        {
        case DVI$_FULLDEVNAM:
        case DVI$_DEVNAM:
            if (chan!=0 || u!=0 )
            {
                struct _ddb * d = u->ucb$l_ddb;
                memcpy(it->bufaddr, &d->ddb$t_name[1], 3);
                if (d->ddb$t_name[1] != 'd')
                    snprintf(&it->bufaddr[3],3,"%d",u->ucb$w_unit);
                else
                    snprintf(&it->bufaddr[3],3,"%03d",u->ucb$w_unit);
            }
            else
            {
                memcpy(it->bufaddr,&d->ddb$t_name[1],15);
            }
            if (it->retlenaddr)
            {
                short int * len = it->retlenaddr;
                *len = strlen(it->bufaddr);
            }
            break;
        case DVI$_UNIT:
            *bufaddr_int=u->ucb$w_unit;
            break;
        case DVI$_PID:
            *bufaddr_int=1; // telnetd needs to have this at non-zero
            //memcpy(it->bufaddr, &u->ucb$l_pid, 4);
            break;
        case DVI$_OWNUIC:
        {
            int i=0;
            memcpy(it->bufaddr, &i, 4); // implement later
        }
        break;
        case DVI$_NEXTDEVNAM:
        case DVI$_ROOTDEVNAM:
        case DVI$_VOLCOUNT:
        case DVI$_VOLNUMBER:
        {
            int i=0;
            memcpy(it->bufaddr, &i, 4); // implement later
        }
        break;
        case DVI$_DEVCLASS:
        {
            int * i = it->bufaddr;
            *i=u->ucb$b_devclass;
        }
        break;
        case DVI$_DEVCHAR:
        {
            int * i = it->bufaddr;
            *i=u->ucb$l_devchar;
        }
        break;
        case DVI$_MAXBLOCK:
        {
            int * i = it->bufaddr;
            *i=((struct _dt_ucb *)u)->ucb$l_maxblock;
        }
        break;
        case DVI$_DEVTYPE:
        {
            int * i = it->bufaddr;
            *i=u->ucb$b_devtype;
        }
        break;
        default:
            printk("getdvi item_code %x not implemented\n",it->item_code);
            break;
        }
        it++;
    }

    struct _pcb * pcb = ctl$gl_pcb;
    sch$postef(pcb->pcb$l_pid, PRI$_NULL, efn);

    if (iosb)
        iosb->iosb$w_status=SS$_NORMAL;

    contxt=d->ddb$ps_link;
    if (contxt==0)
        return SS$_NOMOREDEV;

    return SS$_NORMAL;

}

asmlinkage int exe$getdviw(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, struct _generic_64 *nullarg)
{

    /* I think this is about it */

    int status=exe$getdvi(efn,chan,devnam,itmlst,iosb,astadr,astprm,nullarg);
    if ((status&1)==0) return status;
    return exe$synch(efn,iosb);

}

asmlinkage int exe$getdviw_wrap(struct struct_getdvi *s)
{
    return exe$getdviw(s->efn,s->chan,s->devnam,s->itmlst,s->iosb,s->astadr,s->astprm,s->nullarg);
}

asmlinkage int exe$device_scan(void *return_devnam, unsigned short int *retlen, void *search_devnam, void *itmlst, unsigned long long *contxt)
{
    struct _ddb * d;
    struct item_list_3 * it=itmlst;
    // check itemcode with dvsdef defs
    if (*contxt)
        d=*contxt;
    else
        d=ioc$gl_devlist;
    // sch$iolockr
again:
    ioc$scan_iodb_usrctx(&d);
    if (d==0)
        return SS$_NOMOREDEV;

    while (it->item_code)
    {
        switch (it->item_code)
        {
        case DVI$_DEVNAM:
            memcpy(it->bufaddr,&d->ddb$t_name[1],15);
            break;

        }
        it++;
    }
    return SS$_NORMAL;

}

