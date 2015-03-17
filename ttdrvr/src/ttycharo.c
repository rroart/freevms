// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ttydef.h>
#include <ttyucbdef.h>
#include <ttytadef.h>
#include <system_data_cells.h>
#include <ttyrbdef.h>
#include <tt2def.h>
#include <ssdef.h>
#include <irpdef.h>
#include <dyndef.h>
#include <ioc_routines.h>
#include <misc_routines.h>
#include <linux/kernel.h>

int tty$getnextchar(int * chr, int * CC, struct _ucb * u)
{
again:
    *CC=0;
    *chr=0;
    struct _tty_ucb * tty=u;
    struct _ltrm_ucb * lt=u;
    tty->ucb$b_tt_outype=0;
    if (tty->tty$v_st_write)
    {
        struct _twp * wb = tty->ucb$l_tt_wrtbuf;
        char * wrtc = wb->tty$l_wb_next;
        //    wb->tty$l_wb_next++;
        struct _irp * i = wb->tty$l_wb_irp;
        if (wb->tty$l_wb_next>=wb->tty$l_wb_end)
        {
            // end of data
            i->irp$q_tt_state &= ~TTY$M_ST_WRITE;
            tty->ucb$q_tt_state &= ~TTY$M_ST_WRITE; // here?
            //not yet: fork
            //wb->tty$l_wb_fpc=?;
            //exe_std$primitive_fork(wb,i,wb);
            struct _irp * temp_irp = lt->ucb$l_tl_phyucb->ucb$l_irp;
            // temp_irp is nonzero when telnetted. some other times too. fix later.
            if (temp_irp==0)
                temp_irp=((struct _tty_ucb *)lt->ucb$l_tl_phyucb)->ucb$l_tt_wrtbuf->tty$l_wb_irp;
            // kfree(temp_irp->irp$l_svapte); // have to get rid of this first?
            // or maybe not? got double kfree?
            temp_irp->irp$l_svapte=0; // check. mem leak with free?
            temp_irp->irp$l_iost1=SS$_NORMAL;
            // int status = com$post(temp_irp,lt->ucb$l_tl_phyucb);
            // not working with duplex?      int status = ioc$reqcom(SS$_NORMAL,0,lt->ucb$l_tl_phyucb);
            goto again;
        }
        else
        {
            // more data
            char character = * wrtc;
            tty->ucb$b_tt_lastc = character;
            *CC=1;
            tty->ucb$b_tt_outype=1;
            wb->tty$l_wb_next++;
            //i->irp$q_tt_state &= ~TTY$M_ST_WRITE;
            *chr = character;
            return;
        }
        return;
        // return  tty$putnextchar(chr,u); // this made a loop, implement some other way
    }
    if (tty->tty$v_st_read)
    {
        //  tty->tty$v_st_read=1; // mark reader?
        if (tty->ucb$l_tt_typahd==0)
            return;
        struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;

        if (ahd->tty$w_ta_inahd==0)
            return;

        *CC=1;
        tty->ucb$b_tt_outype=1;
        char * c=ahd->tty$l_ta_get;
        *chr=*c;
        ahd->tty$l_ta_get=(long)ahd->tty$l_ta_get+1;
        ahd->tty$w_ta_inahd--;

        if (ahd->tty$l_ta_get>=ahd->tty$l_ta_end)
            ahd->tty$l_ta_get=ahd->tty$l_ta_data;

        struct _tt_readbuf * bd = u->ucb$l_svapte;
        if (bd==0)
        {
#if 0
            printk("%xchr %c %x ",u->ucb$w_unit,*chr,*chr);
#endif
            goto tmp_skip;
        }
        char * bd_txt = bd->tty$l_rb_txt;

        switch (*c)
        {
        case 4:
            bd->tty$w_rb_linoff--;
            break;
        case 5:
            bd->tty$b_rb_rvffil=bd->tty$w_rb_txtoff-bd->tty$w_rb_linoff; // borrow
            bd->tty$w_rb_linoff=bd->tty$w_rb_txtoff;
            break;
        case 6:
            bd->tty$w_rb_linoff++;
            break;
        case 8:
            bd->tty$b_rb_rvffil=bd->tty$w_rb_linoff; // borrow
            bd->tty$w_rb_linoff=0;
            break;
        case 127:
#if 0
            *chr=9;
#endif
        case 9:
        {
            bd->tty$w_rb_txtoff--;
            bd->tty$w_rb_linoff--;
            int i=bd->tty$w_rb_linoff;
            for(; i<bd->tty$w_rb_txtoff; i++)
                bd_txt[i]=bd_txt[i+1];
        }
        break;
        case 13:
        {
            bd_txt[bd->tty$w_rb_txtoff++]=*c;
            bd_txt[bd->tty$w_rb_txtoff-1]=10;
            bd_txt[bd->tty$w_rb_txtoff]=0;
#if 0
            *CC=0;
            tty->ucb$b_tt_outype=0;
#endif
#if 1
            int tty$readone();
            tty$readone(0, *c, tty);
#endif
        }
        break;
        default:
        {
            bd_txt[bd->tty$w_rb_linoff]=*c;
            bd->tty$w_rb_linoff++;
            if (bd->tty$w_rb_linoff>bd->tty$w_rb_txtoff)
                bd->tty$w_rb_txtoff = bd->tty$w_rb_linoff;
        }
        break;
        }

        if (u->ucb$l_devchar2&TT2$M_LOCALECHO)
            con$startio((int)*chr, u, (int)*CC);

#if 1
        // check read 1 kludge
        struct _irp * temp_irp = lt->ucb$l_tl_phyucb->ucb$l_irp;
        struct _irp * irp = u->ucb$l_irp;
        if (irp->irp$l_qio_p2 == 1)
            goto do_reqcom;
#endif

        // use tt_term etc instead?
        if (*c==13 || *c==10)
        {
            tty->tty$v_st_eol=1;
#if 0
            // not yet? later? gets problems with nmap double 0xd 0xd
            struct _irp * i = u->ucb$l_irp; // check
            i->irp$q_tt_state &= ~TTY$M_ST_READ;
            tty->ucb$q_tt_state &= ~TTY$M_ST_READ; // here?
#endif
do_reqcom:
            {}
            struct _irp * irp = u->ucb$l_irp;
            int bcnt = 0;
            if (irp)
            {
                //      bcnt = (long)ahd->tty$l_ta_put - (long)ahd->tty$l_ta_data;
                bcnt = bd->tty$w_rb_txtoff;
                bcnt = bcnt << 16;
            }
            ioc$reqcom(SS$_NORMAL | bcnt,0,u);
        }
    }
#if 0
    ioc$reqcom(SS$_NORMAL,0,u); // not needed here? bad place?
#endif
tmp_skip:

    return 1;

}
