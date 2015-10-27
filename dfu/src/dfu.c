// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified DFU source file, 2004.

/******************************************************
 *
 *   (C)opyright Digital Equipment Corp. 1995
 *   FREEWARE VERSION
 *
 *   Version : V2.2
 *
 *******************************************************/

#include <stdlib.h>
#include <string.h>

#include <clidef.h>
#include <climsgdef.h>
#include <descrip.h>
#include <stdio.h>
#include <lib$routines.h>
#include <cli$routines.h>
#include <smg$routines.h>
#include <ssdef.h>
#include <jpidef.h>
#include <smgdef.h>
#include <signal.h>
#include <tt2def.h>
#include <iodef.h>
#include <syidef.h>
#include <misc.h>
#include <starlet.h>
#include <smgdef.h>

#include "dfu.h"

#define globalvalue extern int
#define TRUE 1
#define FALSE 0
#define SYS$QIO sys$qiow
#define SYS$QIOW sys$qiow
#define SYS$ASSIGN sys$assign
#define SYS$DASSGN sys$dassgn
#define SYS$SEARCH sys$search
#define SYS$PARSE sys$parse
#define SYS$FAO sys$fao
#define SYS$ASCTIM sys$asctim
#define SYS$BINTIM sys$bintim
#define SYS$WAITFR sys$waitfr
#define SYS$GETJPIW sys$getjpiw
#define SYS$GETSYIW sys$getsyiw
#define SYS$GETDVIW sys$getdviw
#define CLI$DCL_PARSE cli$dcl_parse
#define CLI$DISPATCH cli$dispatch

/* TODO: change these to proper types */
static int i0 = 0;
static int i1 = 1;
static int i2 = 2;
static int i80 = 80;
static int i132 = 132;
static int i500 = 500;

int SYS$DCLEXH()
{
    return SS$_NORMAL;
}

int SYS$CANEXH()
{
    return SS$_NORMAL;
}

int SOR$GK_RECORD;

int check_vms_mm()
{
    int retlenaddr;
    int mem = 0;
    struct item_list_3 lst[14], syilst[2];
    syilst[0].buflen = 4;
    syilst[0].item_code = SYI$_LASTFLD;
    syilst[0].bufaddr = &mem;
    syilst[0].retlenaddr = &retlenaddr;
    syilst[1].buflen = 0;
    syilst[1].item_code = 0;

    int sts = sys$getsyi(0, 0, 0, syilst, 0, 0, 0);

    return mem;
}

int vms_mm;

int SYS$EXPREG(int pagcnt, long ret[2], int a, int b)
{
    if (vms_mm)
        return sys$expreg(pagcnt, ret, a, b);
    int size = pagcnt * 4096; // check
    ret[0] = malloc(size);
    ret[1] = ret[0] + size;
    return SS$_NORMAL;
}
int SYS$DELTVA(long in[2], long ret[2], int a)
{
    if (vms_mm)
        return sys$deltva(in, ret, a);
    free(in[0]);
    ret[0] = in[0];
    ret[1] = in[1];
    return SS$_NORMAL;
}

int verify_command(int mask);
int report_command(int mask);

struct cli_struct
{
    char * cliroutine;
    void (*fn)();
};

struct cli_struct cliroutines[] =
    {
        { "report", report_command, },
        { "verify", verify_command, },
        { 0, 0, }, };

void * get_cli_int(char * c)
{
    int i;
    for (i = 0; cliroutines[i].cliroutine; i++)
    {
        if (0 == strncmp(c, cliroutines[i].cliroutine, strlen(cliroutines[i].cliroutine)))
            return cliroutines[i].fn;
    }
    return 0;
}

globalvalue dfu_tables;
/* Next global params needed for CTRL/C trapping */

int key_tab, status, cip, orig_colls;
static int orgttchar[3], ttchar[3];

#if 0
extern char outbuf[255];
extern unsigned int ctrlc, tchan, disp_id, disp1_id, rows, colls,
disp2_id, status_id, paste_id, help_id, keyb_id;
extern Boolean smg$enable;
#else
char outbuf[255];
unsigned int ctrlc, tchan, disp_id, disp1_id, rows, colls, disp2_id, status_id, paste_id, help_id, keyb_id;
Boolean smg$enable;
#endif

static FILE *xp;
int astrtn(void);
int refresh(void); /* Ast routines called by CTRLC,Y and W */
unsigned int prim_hand(int signal[], int mech[]);
unsigned int brdcst_ast(void);

main(int argc, char *argv[])

/*  MAIN

 Purpose : 1 Get privilege mask
 2 Setup SMG environment unless no VT or DFU$NOSMG is set
 3 Get and Parse command (syntax only)
 4 Dispatch to correct subroutine

 Inputs  : Command line (if specified through foreign command)

 Outputs : returns last status code to DCL in  case
 of single command processing. In interactive mode
 always returns SS$_NORMAL.
 */
{
    const rms_eof = 98938, smg$_eof = 1213442;
    struct
    {
        short status, count;
        int extra;
    } iosb;
    static char command_line[255], *e;
    unsigned int out_len, ret_len, prvmask;
    void reset_ctrl(), clean_ctrlc(), prev_screen(), next_screen(), dump_screen(), toggle_width();
    int smg_flag, x, y, i, ttype, flags;
    int cursor_on = SMG$M_CURSOR_ON;
    $DESCRIPTOR(input_line, command_line);
    $DESCRIPTOR(prompt, "DFU> ");
    $DESCRIPTOR(terminal, "SYS$COMMAND");
    $DESCRIPTOR(top_txt, "< DFU V2.2 (Freeware) >");
    $DESCRIPTOR(status_txt, "Statistics");
    $DESCRIPTOR(do_key, "DO");
    $DESCRIPTOR(pf2, "PF2");
    $DESCRIPTOR(pf4, "PF4");
    $DESCRIPTOR(prev, "PREV_SCREEN");
    $DESCRIPTOR(next, "NEXT_SCREEN");
    $DESCRIPTOR(select, "SELECT");
    $DESCRIPTOR(help, "HELP");

    /* First find out how we got called ( by RUN, or a foreign command */
    ret_len = 0;
#if 0
    status = lib$get_foreign(&input_line,0,&ret_len,0);
#else
    status = 1;
#if 0
    strcpy(command_line,argv[1]);
#endif
#endif
    out_len = ret_len;

    smg$enable = TRUE;
    key_tab = 0;
    disp2_id = 0;
    cip = 0;

    setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all
#if 0
            smg$enable = FALSE;
            vms_mm = check_vms_mm();
#else
    /* Now create the SMG environment */

    colls = 80;
    rows = 24;
    flags = SMG$M_KEEP_CONTENTS;
    smg$create_pasteboard(&paste_id, 0, &rows, &colls, &flags, &ttype, 0);
    if ((e = (char *) getenv("DFU$NOSMG")) && *e)
        smg$enable = FALSE;
    else
    {
        if (ttype != SMG$K_VTTERMTABLE)
            smg$enable = FALSE;
        if (ttype != SMG$K_VTTERMTABLE)
            smg$delete_pasteboard(&paste_id, &i0);
    }

    smg$create_virtual_keyboard(&keyb_id, 0, 0, 0, 0);
    if (smg$enable)
    /* Setup key table */
    {
        int display_attributes = SMG$M_BORDER;
        int rendition_set = SMG$M_BOLD;
        smg$erase_pasteboard(&paste_id, 0, 0, 0, 0, 0, 0);
        smg$create_key_table(&key_tab);
        colls -= 2;
        orig_colls = colls;
        smg_flag = SMG$M_KEY_NOECHO + SMG$M_KEY_TERMINATE;
        smg$add_key_def(&key_tab, &do_key, 0, &smg_flag, &do_key, 0);
        smg$add_key_def(&key_tab, &pf4, 0, &smg_flag, &pf4, 0);
        smg$add_key_def(&key_tab, &prev, 0, &smg_flag, &prev, 0);
        smg$add_key_def(&key_tab, &next, 0, &smg_flag, &next, 0);
        smg$add_key_def(&key_tab, &pf2, 0, &smg_flag, &help, 0);
        smg$add_key_def(&key_tab, &help, 0, &smg_flag, &help, 0);
        smg$add_key_def(&key_tab, &select, 0, &smg_flag, &select, 0);
        smg$create_virtual_display(&i500, &colls, &disp1_id, &display_attributes, 0, 0);
        x = 508 - rows;
        y = rows - 7;
        smg$create_viewport(&disp1_id, &x, &i1, &y, &colls);
        smg$create_virtual_display(&i2, &colls, &status_id, 0, 0, 0);
        smg$create_virtual_display(&i2, &colls, &disp2_id, 0, 0, 0);
        smg$set_broadcast_trapping(&paste_id, brdcst_ast, 0);
        smg$label_border(&disp1_id, &top_txt, 0, 0, &rendition_set, 0, 0);
        smg$label_border(&status_id, &status_txt, 0, 0, &rendition_set, 0, 0);
        smg$paste_virtual_display(&disp1_id, &paste_id, &i2, &i2, 0);
        x = rows - 4;
        smg$paste_virtual_display(&status_id, &paste_id, &x, &i2, 0);
        x = rows - 1;
        smg$paste_virtual_display(&disp2_id, &paste_id, &x, &i2, 0);
        x = 508 - rows;
        smg$set_cursor_abs(&disp1_id, &x, &i1);
        smg$set_cursor_abs(&disp2_id, &i1, &i1);
        smg$begin_pasteboard_update(&paste_id);
    }
#endif

    sprintf(outbuf, "\n     Disk and File Utilities for OpenVMS DFU V2.2");
    put_disp();
    sprintf(outbuf, "     Freeware version");
    put_disp();
    sprintf(outbuf, "     Copyright � 1995 Digital Equipment Corporation\n");
    put_disp();

    if (smg$enable)
    {
        /* Enter additional info */
        sprintf(outbuf, "     DFU functions are : \n");
        put_disp();
        sprintf(outbuf, "     DEFRAGMENT : Defragment files");
        put_disp();
        sprintf(outbuf, "     DELETE     : Delete files by File-ID; delete directory (trees)");
        put_disp();
        sprintf(outbuf, "     DIRECTORY  : Manipulate directories");
        put_disp();
        sprintf(outbuf, "     REPORT     : Generate a complete disk report");
        put_disp();
        sprintf(outbuf, "     SEARCH     : Fast file search");
        put_disp();
        sprintf(outbuf, "     SET        : Modify file attributes");
        put_disp();
        sprintf(outbuf, "     UNDELETE   : Recover deleted files");
        put_disp();
        sprintf(outbuf, "     VERIFY     : Check and repair disk structure");
        put_disp();
        smg$end_pasteboard_update(&paste_id);
    }

    prvmask = 0;
    status = dfu_check_access(&prvmask); /*Get the privilege mask */

    /* Setup terminal channel for control purposes; get the terminal chars */
    status = SYS$ASSIGN(&terminal, &tchan, 0, 0, 0);
    status = SYS$QIOW(0, tchan, IO$_SENSEMODE, 0, 0, 0, &orgttchar, 12, 0, 0, 0, 0);
    for (i = 0; i < 3; i++)
        ttchar[i] = orgttchar[i];
    ttchar[2] &= ~TT2$M_EDITING; /* Clear line edit bit */
    clean_ctrlc(); /* Enable CTRL/W if needed */

    if (ret_len == 0)
    {
        if (smg$enable)
            status = smg$read_composed_line(&keyb_id, &key_tab, &input_line, &prompt, &out_len, &disp2_id, 0, 0, 0, 0, 0);
        else
            status = smg$read_composed_line(&keyb_id, 0, &input_line, &prompt, &out_len, 0, 0, 0, 0, 0, 0);
    }

    memcpy(command_line, input_line.dsc$a_pointer, input_line.dsc$w_length);
    cip = 1;

    /* Main loop starts here. Get a command and pasre it*/
    for (;;)
    {
        /* loop forever until EXIT is entered */
        if (status == smg$_eof)
            status = exit_command(prvmask);
        if ((status & 1) != 1)
            goto endfor;
        if (out_len == 0)
            goto endfor;

        /* First catch special screen commands */
        if (smg$enable)
        {
            status = strncmp(command_line, "PREV_SCREEN", 11);
            if (status == 0)
            {
                prev_screen();
                goto endfor;
            }
            status = strncmp(command_line, "DO", 2);
            if (status == 0)
            {
                status = spawn_command(prvmask);
                goto endfor;
            }
            status = strncmp(command_line, "PF4", 3);
            if (status == 0)
            {
                dump_screen();
                goto endfor;
            }
            status = strncmp(command_line, "NEXT_SCREEN", 11);
            if (status == 0)
            {
                next_screen();
                goto endfor;
            }
            status = strncmp(command_line, "SELECT", 6);
            if (status == 0)
            {
                toggle_width();
                goto endfor;
            }

            smg$erase_display(&disp1_id, 0, 0, 0, 0);
            smg$erase_display(&status_id, 0, 0, 0, 0);
            smg$change_viewport(&disp1_id, &x, &i1, &y, &colls);
            smg$set_cursor_abs(&disp1_id, &x, &i1);
        }

        /* Catch the CLI errors do avoid disrupting the SMG screen... */
#if 0
        VAXC$ESTABLISH(prim_hand);
#endif
        status = CLI$DCL_PARSE(&input_line, &dfu_tables, 0 /* not yet lib$get_input*/, 0, &prompt); // check added & before dfu_tables
#if 0
                VAXC$ESTABLISH(NULL);
#endif
        if (status == CLI$_NOCOMD)
            singlemsg(0, status);
        if ((status & 1) != 1)
            goto endfor;
        else
        /* Now dispatch if no errors */
        {
            reset_ctrl();
            CLI$DISPATCH(prvmask);
            clean_ctrlc();
            cip = 0;
            status = brdcst_ast();
            if (smg$enable)
                smg$set_cursor_mode(&paste_id, &cursor_on);
        }
        endfor: if (ret_len != 0)
        {
            /* Single command processing , so exit here */
            status += 0x10000000; /* Do not echo the error on DCL level */
            if (smg$enable)
            {
                if (colls != orig_colls)
                    toggle_width();
                smg$set_cursor_abs(&disp2_id, &i2, &i1);
            }
            exit(status);
        }
        /* Get next line */
        cip = 0;
#if 1
        if (smg$enable)
        {
            smg$erase_line(&disp2_id, &i1, &i1);
            smg$set_cursor_abs(&disp2_id, &i1, &i1);
            status = smg$read_composed_line(&keyb_id, &key_tab, &input_line, &prompt, &out_len, &disp2_id, 0, 0, 0, 0, 0); /*Get next command */
            cip = 1;
        }
        else
            status = smg$read_composed_line(&keyb_id, 0, &input_line, &prompt, &out_len, 0, 0, 0, 0, 0, 0); /*Get next command */
#else
        printf("%s",prompt.dsc$a_pointer);
        out_len = read(0,command_line,254);
        out_len--;
        command_line[out_len]=0;
        if (strncmp(command_line,"exit",4)==0)
        return 0;
#endif
    }
} /* END of MAIN */
int help_command(int mask)
/*  HELP

 Purpose : call on-line help
 Output  : contents of DFUHLP helplib.
 */
{
    unsigned int x, flag = 1, tmp = 0;
#if 0
    static char help[80];
    $DESCRIPTOR(help_key ,help);
    $DESCRIPTOR(help_item, "helpkey");
    $DESCRIPTOR(help_lib,"dfuhlp");
    $DESCRIPTOR(item,"DFU ");
    $DESCRIPTOR(hlp_txt,"< DFU Help Screen >");

    /* Check if a help item was entered */
    if (cli$present(&help_item) == CLI$_PRESENT)
    cli$get_value(&help_item,&help_key,&help_key);
    else
    help_key.dsc$w_length=0;

    /* Add 'DFU ' to the help key */
    help_key.dsc$w_length +=4;
    str$concat(&help_key,&item,&help_key);
    x = rows - 4;
    /* Reenable line editing... */
    status = SYS$QIOW(0,tchan, IO$_SETMODE,0,0,0,&orgttchar,12,0,0,0,0);
    if (smg$enable)
    {
        smg$create_virtual_display(&x, &colls, &help_id, 0 , 0, 0);
        smg$label_border(&help_id, &hlp_txt, 0, 0,&SMG$M_BOLD);
        smg$paste_virtual_display(&help_id, &paste_id, &i2,&i2,0);
        status = smg$put_help_text(&help_id, &keyb_id, &help_key, &help_lib,0,0);
        smg$unpaste_virtual_display(&help_id, &paste_id);
        smg$delete_virtual_display(&help_id);
    }
    else
    status = lbr$output_help(lib$put_output,0,&help_key,
            &help_lib,&flag,lib$get_input);
    if (status != SS$_NORMAL)
    {
        sprintf(outbuf,"%%DFU-E-HELPERR, Error opening help library,");
        put_disp();
        singlemsg(0,status);
    }
#endif
    return (1);
} /*END help_command */

int exit_command(int mask)
/*  EXIT

 Purpose : Exit Program with SS$_NORMAL
 Output  : Return to DCL

 */
{
    void clean_ctrlc(), toggle_width();

    clean_ctrlc();
    if (smg$enable)
    {
        smg$disable_broadcast_trapping(&paste_id);
        if (colls != orig_colls)
            toggle_width();
        smg$set_cursor_abs(&disp2_id, &i2, &i1);
        smg$delete_pasteboard(&paste_id, &i0);
    }
    exit(1);
    return (1);
} /*END exit_command */

int spawn_command(int mask)
{
#if 0
    /* Saves the screen and spawns a subprocess */

    int save_id;
    $DESCRIPTOR(prompt,"DFU_sub$ ");

    if (smg$enable)
    {
        smg$save_physical_screen(&paste_id, &save_id);
        smg$disable_broadcast_trapping(&paste_id);
    }
    printf("%%DFU-I-SPAWN, Creating subprocess...\n");
    status = lib$spawn(0,0,0,0,0,0,0,0,0,0,&prompt,0,0);
    /* Next call is a workaround for a bug in TTdriver which may inadvertently
     reset the terminal width */
    if (colls > 80) printf("\x1b[?3h");
    else printf("\x1b[?3l");
    if (smg$enable)
    {
        smg$restore_physical_screen(&paste_id, &save_id);
        smg$set_broadcast_trapping(&paste_id,brdcst_ast,0);
    }
    if ((status & 1) !=1) singlemsg(0,status);
#endif
    return (1);
}

void prev_screen(void)
{
    int row_start, direction, cnt;

    smg$get_viewport_char(&disp1_id, &row_start, 0, 0, 0);
    direction = SMG$M_DOWN;
    cnt = row_start - 1;
    if (cnt <= 1)
        smg$ring_bell(&disp1_id);
    else if (cnt < (rows - 10))
    {
        smg$scroll_viewport(&disp1_id, &direction, &cnt);
    }
    else
    {
        cnt = (rows - 10);
        smg$scroll_viewport(&disp1_id, &direction, &cnt);
    }
}

void next_screen(void)
{
    int row_start, direction, cnt;

    smg$get_viewport_char(&disp1_id, &row_start, 0, 0, 0);
    direction = SMG$M_UP;
    cnt = 508 - rows - row_start;
    if (cnt <= 1)
        smg$ring_bell(&disp1_id);
    else if (cnt < (rows - 10))
    {
        smg$scroll_viewport(&disp1_id, &direction, &cnt);
    }
    else
    {
        cnt = (rows - 10);
        smg$scroll_viewport(&disp1_id, &direction, &cnt);
    }
}

int dump_actn(struct dsc$descriptor *out_str, int user_arg)
{
    fprintf(xp, "%.*s\n", out_str->dsc$w_length, out_str->dsc$a_pointer);
    return (1);
}

void dump_screen(void)
/* Routine to dump the screen contents to a file */
{
    char fname[] = "DFU_SCREEN.TXT";

#if 0
    xp = fopen(fname,"w","mrs=132","rfm=var","ctx=rec","rat=cr","rop=WBH");
#endif
    status = smg$put_pasteboard(&paste_id, dump_actn, 0, 0);
    if ((status & 1) != 1)
    {
        sprintf(outbuf, "%%DFU-E-DUMPERR, Error dumping screen contents,");
        put_disp();
        singlemsg(0, status);
    }
    else
    {
        sprintf(outbuf, "%%DFU-I-DUMPDONE, Screen contents saved in DFU_SCREEN.TXT");
        put_disp();
    }
    fclose(xp);
}

void toggle_width(void)
/* Routine to change terminal width */
{
    if (colls > 80)
        smg$change_pbd_characteristics(&paste_id, &i80, &colls, 0, 0, 0, 0);
    else
        smg$change_pbd_characteristics(&paste_id, &i132, &colls, 0, 0, 0, 0);
    colls -= 2;
    smg$change_virtual_display(&disp1_id, 0, &colls, 0, 0, 0);
    smg$change_viewport(&disp1_id, 0, 0, 0, &colls);
    smg$change_virtual_display(&status_id, 0, &colls, 0, 0, 0);
    smg$change_virtual_display(&disp2_id, 0, &colls, 0, 0, 0);
}

int dfu_check_access(int *mask)
/*
 Check if the user has DFU_ALLPRIV identifier granted.
 */

{
    int i, j, rights_size, ret_addr, pages;
#if 0
    unsigned short len;
    int jpi$_rights_size , jpi$_process_rights;
    struct _itm
    {
        short buflen, itemcode;
        int *bufadr, *retlen;
    }item_list[3];
    struct _rt
    {
        int rights[2];
    }*rights_list;
    static char ascii_id[255];
    $DESCRIPTOR(ascii_descr , ascii_id);
    Boolean found;

    jpi$_rights_size = 817;
    jpi$_process_rights = 814;
    *mask = 0; /*Assume identifier not granted */
    item_list[0].buflen = 4;
    item_list[0].itemcode = jpi$_rights_size;
    item_list[0].bufadr = &rights_size;
    item_list[0].retlen = 0;

    item_list[1].buflen = 0;
    item_list[1].itemcode = 0; /* End of list */
    status = SYS$GETJPIW(0,0,0,&item_list,0,0,0);
    if ((status & 1) !=1)
    {
        singlemsg(0,status);
        return(status);
    }
    /* Rightssize is returned in bytes, so rightssize/8 -1 is
     number of rights. Now allocate enough dynamic space to get
     all the rights in. */

    j = (rights_size/8) + 1;
    ret_addr = malloc(j*8);
    rights_list = (struct _rt *) ret_addr;

    item_list[0].buflen = j*8;
    item_list[0].itemcode = jpi$_process_rights;
    item_list[0].bufadr = (int *) rights_list;
    item_list[0].retlen = 0;

    item_list[1].buflen = 0;
    item_list[1].itemcode = 0; /* End of list */

    status = SYS$GETJPIW(0,0,0,&item_list,0,0,0);
    if ((status & 1) !=1)
    {
        singlemsg(0,status);
        return(status);
    }
    i = 1;
    found = FALSE;
    while (!found)
    {
        status = SYS$IDTOASC(rights_list->rights[0], &len, &ascii_descr, 0,0,0);
        status = strncmp(ascii_id, "DFU_ALLPRIV", len);
        if (status == 0)
        {
            *mask = -1;
            found = TRUE;
        }
        else
        {
            i++;
            rights_list++;
            if (i > j) found = TRUE;
        }
    }
    free(ret_addr);
#endif
    return (1);
}

void reset_ctrl(void)
/* Reset CTRLC/Y on the terminal channel */

{
#if 0
    int stat, func;
    struct
    {
        short status, count;
        int extra;
    }iosb;
    unsigned int t_mask[2];

    /* Disable the F6 key */
    status = SYS$QIOW(0,tchan, IO$_SETMODE,0,0,0,&ttchar,12,0,0,0,0);
    ctrlc = 0;
    /* Enable CTRLC/Y trapping by setting CTRL C and Y as out-of-band chars*/
    t_mask[0] = 0;
    t_mask[1] = 1 << 3;
    t_mask[1] += (1 << 25);
    status = SYS$QIOW(0,tchan,(IO$_SETMODE+IO$M_OUTBAND),&iosb,
            0,0,astrtn,&t_mask,0,0,0,0);
#endif
}

int astrtn(void)
/* Ast routine called by CTRLC/Y
 Set CTRLC to 1 and reset the Trapping */

{
    ctrlc = 1;
    sprintf(outbuf, "     Current command ABORTING...");
    put_status(2);
    return (1);
}

void clean_ctrlc(void)
/* Cleanup CTRLC channel */

{
    struct
    {
        short status, count;
        int extra;
    } iosb;
    unsigned int t_mask[2];
#if 0

    status = SYS$QIOW(0,tchan, IO$_SETMODE,0,0,0,&orgttchar,12,0,0,0,0);
    if (!smg$enable)
    status = SYS$QIOW(0,tchan,(IO$_SETMODE+IO$M_OUTBAND),&iosb,
            0,0,0,0,0,0,0,0);
    else
    {
        t_mask[0] = 0;
        t_mask[1] = 1 << 23;
        status = SYS$QIOW(0,tchan,(IO$_SETMODE+IO$M_OUTBAND),&iosb,
                0,0,refresh,&t_mask,0,0,0,0);
    }
#endif
    status = 1;
}

int refresh(void)
{
#if 0
    status = smg$cancel_input(&keyb_id);
#endif
    status = smg$repaint_screen(&paste_id);
    return (1);
}

int strindex(char *input_string, char *symbol, int len)
/*
 Give the location of the SYMBOL in INPUT_STRING
 */

{
    int i;

    i = 0;
    while ((i < len) && (input_string[i] != *symbol))
        i++;
    return (i < len) ? i : -1;
}

void put_status(int x)
/* Display statistics info */

{
    $DESCRIPTOR(stat_descr, outbuf);
    if (smg$enable)
    {
        int flags = SMG$M_ERASE_LINE;
        stat_descr.dsc$w_length = strlen(outbuf);
        smg$put_chars(&status_id, &stat_descr, &x, &i2, &flags, 0, 0, 0);
    }
}

void put_disp(void)
{
    $DESCRIPTOR(to_disp, outbuf);

    if (smg$enable)
    {
        int flags = SMG$M_WRAP_CHAR;
        to_disp.dsc$w_length = strlen(outbuf);
        smg$put_line(&disp1_id, &to_disp, 0, 0, 0, &flags, 0);
    }
    else
        printf("%s\n", outbuf);
}

void singlemsg(int stat1, int stat)
/* Gets a system message and displays it.
 Unlike LIB$SIGNAL it will not terminate the
 program on fatal errors
 */
{
#if 0
    int len;
    $DESCRIPTOR(mesg_desc , outbuf);

    if (stat1 !=0)
    {
        SYS$GETMSG(stat1,&mesg_desc.dsc$w_length, &mesg_desc,0,0);
        outbuf[mesg_desc.dsc$w_length]='\0';
        put_disp();
        mesg_desc.dsc$w_length = 255;
    }
    SYS$GETMSG(stat,&mesg_desc.dsc$w_length, &mesg_desc,0,0);
    outbuf[mesg_desc.dsc$w_length]='\0';
    put_disp();
    mesg_desc.dsc$w_length = 255;
#endif
}

int actrtn(struct dsc$descriptor *out_str, int user_arg)
/* user action routine to catch SYS$PUTMSG */
{
    sprintf(outbuf, "%.*s", out_str->dsc$w_length, out_str->dsc$a_pointer);
    put_disp();
    return (0);
}

unsigned int prim_hand(int signal[], int mech[])
{
    /* Use the signal array to pass the error to PUTMSG
     PUTMSG in turn will call ACTRTN which will use SMG output
     in stead of just dumping SYS$OUTPUT/ERROR
     */

    int params;
    signal[0] -= 2;
#if 0
    SYS$PUTMSG(signal, actrtn,0,params);
#endif
    return SS$_CONTINUE;
}

unsigned int brdcst_ast(void)
/* Displays the broadcast messages */
{
    static char msg[160];
    int len;
    $DESCRIPTOR(brdcst_msg, msg);

    if (cip == 1)
        return (1); /* exit if DFU is busy */
#if 0
    status = smg$cancel_input(&keyb_id);
#endif
    smg$set_cursor_abs(&status_id, &i2, &i2);
    status = smg$get_broadcast_message(&paste_id, &brdcst_msg, &len, 0);
#if 0
    while (status == SS$_NORMAL)
    {
        brdcst_msg.dsc$w_length = len;
        smg$put_line(&disp1_id, &brdcst_msg, 0, 0 ,0 ,&SMG$M_WRAP_CHAR, 0, 0);
        brdcst_msg.dsc$w_length = 160;
        status = smg$get_broadcast_message(&paste_id, &brdcst_msg, &len,0);
    }
#endif
    return (1);
}
