//+++2002-08-17
//    Copyright (C) 2001,2002  Mike Rieker, Beverly, MA USA
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; version 2 of the License.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---2002-08-17

/************************************************************************/
/*									*/
/*  Command-line interpreter						*/
/*									*/
/************************************************************************/

#if 0
#include "ozone.h"

#include "oz_crtl_malloc.h"
#include "oz_io_console.h"
#include "oz_io_fs.h"
#include "oz_io_mutex.h"
#include "oz_io_pipe.h"
#include "oz_io_timer.h"
#include "oz_knl_ast.h"
#include "oz_knl_devio.h"
#include "oz_knl_hw.h"
#include "oz_knl_lock.h"
#include "oz_knl_logname.h"
#include "oz_knl_sdata.h"
#include "oz_knl_status.h"
#include "oz_knl_thread.h"
#include "oz_knl_userjob.h"
#include "oz_sys_callknl.h"
#include "oz_sys_dateconv.h"
#include "oz_sys_event.h"
#include "oz_sys_exhand.h"
#include "oz_sys_handle.h"
#include "oz_sys_io.h"
#include "oz_sys_io_fs.h"
#include "oz_sys_io_fs_printf.h"
#include "oz_sys_logname.h"
#include "oz_sys_password.h"
#include "oz_sys_spawn.h"
#include "oz_sys_thread.h"
#include "oz_sys_userjob.h"
#include "oz_sys_xprintf.h"
#include "oz_util_start.h"
#endif

#include <stdio.h> 

#include <iodef.h>
#include <jpidef.h>
#include <lckdef.h>
#include <lnmdef.h>
#include <psldef.h>
#include <ssdef.h> 
#include <statedef.h>
#include <descrip.h> 
#include <starlet.h>

#include <string.h>

#define oz_util_h_console stdout

#define CMDSIZ 4096
#define MAXLBLNAM 64
#define MAXSYMNAM 64
#define CMDLNMTBL "OZ_CLI_TABLES"
#define LASTPROCLNM "OZ_LAST_PROCESS"
#define LASTHREADLNM "OZ_LAST_THREAD"
#define SECATTRSIZE 4096

#define SYMBOLSTARTCHAR(c) ((c == '_') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
#define SYMBOLCHAR(c) (SYMBOLSTARTCHAR(c) || ((c >= '0') && (c <= '9')))

#define SHOWOPT_PROC_THREADS 0x1
#define SHOWOPT_JOB_PROCS 0x2
#define SHOWOPT_JOB_THREADS (SHOWOPT_PROC_THREADS | SHOWOPT_JOB_PROCS)
#define SHOWOPT_USER_JOBS 0x4
#define SHOWOPT_USER_PROCS (SHOWOPT_JOB_PROCS | SHOWOPT_USER_JOBS)
#define SHOWOPT_USER_THREADS (SHOWOPT_PROC_THREADS | SHOWOPT_USER_PROCS)
#define SHOWOPT_DEVICE_IOCHANS 0x8
#define SHOWOPT_SYSTEM_USERS 0x10
#define SHOWOPT_SYSTEM_JOBS (SHOWOPT_USER_JOBS | SHOWOPT_SYSTEM_USERS)
#define SHOWOPT_SYSTEM_PROCS (SHOWOPT_JOB_PROCS | SHOWOPT_SYSTEM_JOBS)
#define SHOWOPT_SYSTEM_THREADS (SHOWOPT_PROC_THREADS | SHOWOPT_SYSTEM_PROCS)
#define SHOWOPT_SYSTEM_DEVICES 0x20
#define SHOWOPT_SYSTEM_IOCHANS (SHOWOPT_DEVICE_IOCHANS | SHOWOPT_SYSTEM_DEVICES)

#define SHOWOPT_SECURITY 0x00010000
#define SHOWOPT_OBJADDR  0x00020000

#define SHOWNHDR_THREAD 0x80000000
#define SHOWNHDR_PROCESS 0x40000000
#define SHOWNHDR_JOB 0x20000000
#define SHOWNHDR_USER 0x10000000
#define SHOWNHDR_IOCHAN 0x8000000
#define SHOWNHDR_DEVICE 0x4000000
#define SHOWNHDR_SYSTEM 0x2000000

#define SHOLNMFLG_SECURITY 0x00000001

#define OZ_THREAD_NAMESIZE (16)
#define OZ_DEVUNIT_NAMESIZE (16)

#define OZ_IO_PIPES_TEMPLATE "pipes"

#define OZ_DATELONG_ELEMENTS 3

#define OZ_USERNAME_MAX 16
#define OZ_JOBNAME_MAX 16
#define OZ_PROCESS_NAMESIZE 16
#define OZ_DEVUNIT_ALIASIZE 16
#define OZ_DEVUNIT_DESCSIZE 16
#define OZ_DEVCLASS_NAMESIZE 16
#define OZ_DATELONG_DAYNUMBER 2
#define OZ_LOGNAME_MAXNAMSZ 16
#define OZ_PASSWORD_MAX 16
#define OZ_IO_MUTEX_CREATE 42
#define OZ_FS_MAXFNLEN 42
#define OZ_TIMER_RESOLUTION 10000000
#define OZ_DATELONG_FRACTION 0
#define OZ_DATELONG_SECOND 1
#define OZ_DATELONG_ELEMENTS 3
#define OZ_THREAD_STATE_MAX 14
#define OZ_EVENT_NAMESIZE 32

int oz_sys_event_inc() { }
int oz_sys_event_nwait() { } 
int oz_sys_event_set() { }
int OZ_IO_mutex_create() { }
int mutex_create() { }
int OZ_IO_fs_create() { }
int fs_create() { }
int OZ_Console_modebuff() { }
int OZ_IO_console_getmode() { }
int OZ_IO_fs_open() { }
int OZ_IO_fs_readrec() { }
int console_getmode() { }
int console_modebuff() { }
int fs_open() { }
int fs_readrec() { }

int oz_sys_handle_release() { }
int oz_sys_handle_getinfo() { }
int oz_sys_event_create() { return 1; }
int oz_sys_iochan_getunitname() { }
int sys$suspnd() { }
int oz_sys_spawn() { }
int oz_sys_io() { }
int oz_sys_io_dealloc() { }
int oz_sys_callknl() { }
int oz_sys_password_change() { }
int oz_sys_job_create() { }
int oz_sys_io_alloc() { }
int oz_sys_thread_getbyid() { }
int oz_sys_thread_orphan() { }
int oz_sys_thread_abort() { }
int oz_sys_process_getbyid() { }
int oz_sys_datebin_decode() { }
int oz_sys_daynumber_weekday() { }
int oz_sys_exhand_create() { }
int oz_sys_thread_create() { }
int oz_sys_condhand_signal() { }
int oz_sys_io_start() { }

unsigned long oz_hw_atomic_inc_long(unsigned long *l, char c) {
  (*l)++;
  return (*l);
}

typedef enum { OZ_OBJTYPE_UNKNOWN,              //  0
               OZ_OBJTYPE_AST,                  //  1
               OZ_OBJTYPE_EVENT,                //  2
               OZ_OBJTYPE_PROCESS,              //  3
               OZ_OBJTYPE_SECATTR,              //  4
               OZ_OBJTYPE_SECKEYS,              //  5
               OZ_OBJTYPE_SECTION,              //  6
               OZ_OBJTYPE_THREAD,               //  7
               OZ_OBJTYPE_KTHREAD,              //  8
               OZ_OBJTYPE_EXHAND,               //  9
               OZ_OBJTYPE_DEVUNIT,              //  A
               OZ_OBJTYPE_DEVDRIVER,            //  B
               OZ_OBJTYPE_DEVCLASS,             //  C
               OZ_OBJTYPE_IOOP,                 //  D
               OZ_OBJTYPE_IOCHAN,               //  E
               OZ_OBJTYPE_IOSELECT,             //  F
               OZ_OBJTYPE_LOWIPL,               // 10
               OZ_OBJTYPE_HANDLETBL,            // 11
               OZ_OBJTYPE_IMAGE,                // 12
               OZ_OBJTYPE_LOGNAME,              // 13
               OZ_OBJTYPE_LOGNAMESEARCH,        // 14
               OZ_OBJTYPE_CONDHAND,             // 15
               OZ_OBJTYPE_TIMER,                // 16
               OZ_OBJTYPE_IOPARSE,              // 17
               OZ_OBJTYPE_SECLOCK,              // 18
               OZ_OBJTYPE_DPAR,                 // 19
               OZ_OBJTYPE_USER,                 // 1A
               OZ_OBJTYPE_JOB,                  // 1B
               OZ_OBJTYPE_QUOTA,                // 1C
               OZ_OBJTYPE_CACHE,                // 1D
               OZ_OBJTYPE_DCACHE,               // 1E
               OZ_OBJTYPE_NCACHE,               // 1F
               OZ_OBJTYPE_LIOD,                 // 20
               OZ_OBJTYPE_LIOR,                 // 21
               OZ_OBJTYPE_SHUTHAND,             // 22
               OZ_OBJTYPE_SECLKWZ,              // 23
               OZ_OBJTYPE_LOG,                  // 24

               OZ_OBJTYPE_MAX                   // 25
} OZ_Objtype;

typedef struct { unsigned long attr;
  void *buff;
} OZ_Logvalue;

unsigned long oz_hw_atoz (const char *s, int *usedup)

{
  char c;
  const char *p;
  unsigned long accum;

  accum = 0;
  for (p = s; (c = *p) != 0; p ++) {
    if ((c >= 'A') && (c <= 'F')) c -= 'A' - 10;
    else if ((c >= 'a') && (c <= 'f')) c -= 'a' - 10;
    else if ((c < '0') || (c > '9')) break;
    else c -= '0';
    accum = accum * 16 + c;
  }

  if (usedup != NULL) *usedup = p - s;
  return (accum);
}

unsigned long oz_hw_atoi (const char *s, int *usedup)

{
  char c;
  const char *p;
  unsigned long accum;

  p = s;
  if ((p[0] == '0') && ((p[1] == 'x') || (p[1] == 'X'))) {
    accum = oz_hw_atoz (p + 2, usedup);
    if (usedup != NULL) *usedup += 2;
    return (accum);
  }

  accum = 0;
  for (; (c = *p) != 0; p ++) {
    if (c < '0') break;
    if (c > '9') break;
    accum = accum * 10 + c - '0';
  }

  if (usedup != NULL) *usedup = p - s;
  return (accum);
}

void oz_hw_itoa (unsigned long valu, unsigned long size, char *buff)

{
  char temp[3*sizeof valu];
  int i;

  i = sizeof temp;
  temp[--i] = 0;
  do {
    temp[--i] = (valu % 10) + '0';
    valu /= 10;
  } while (valu != 0);
  strncpy (buff, temp + i, size);
}

typedef struct Command Command;

typedef unsigned long (*Entry) (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *param, int argc, const char *argv[]);

struct Command { int async;	/* 0: cannot run as a thread; 1: ok to run as a thread */
				/* - Basically anything that accesses cli internal static data (like symbol tables and script stacks) cannot be run as a thread. */
				/*   Also, things like 'abort/wait/suspend/resume thread' and 'run' do their own ctrl-Y processing so do not need to be async.   */
                 char *name;	/* command name string; multiple words separated by a single space */
                 Entry entry;	/* entrypoint to the routine */
                 void *param;	/* param to pass to the routine */
                 char *help;	/* corresponding help string */
               };

typedef struct Label Label;

struct Label { Label *next;
               char name[MAXLBLNAM];
               unsigned long blkoffs;
               unsigned long bytoffs;
             };

typedef enum { SYMTYPE_STRING, 
               SYMTYPE_INTEGER
             } Symtype;

typedef struct Symbol Symbol;

struct Symbol { Symbol *next;
                char name[MAXSYMNAM];
                Symtype symtype;
                unsigned long (*func) (Symbol *symbol, char *strp, char **rtnp, void *valuep);
                unsigned long ivalue;
                char svalue[1];
              };

typedef struct Script Script;

struct Script { Script *next;		/* next outermost script */
                unsigned long h_input;	/* input of next outermost script */
                unsigned long h_output;	/* output of next outermost script */
                unsigned long h_error;	/* error of next outermost script */
                Symbol *symbols;	/* symbols of next outermost script */
                Label *labels;		/* labels of next outermost script */
              };

typedef enum {
	OPTYPE_UNKNOWN, 
	OPTYPE_ADD, 
	OPTYPE_BITAND, 
	OPTYPE_BITOR, 
	OPTYPE_BITXOR, 
	OPTYPE_BOOLAND, 
	OPTYPE_BOOLOR, 
	OPTYPE_BOOLXOR, 
	OPTYPE_DIVIDE, 
	OPTYPE_EQ, 
	OPTYPE_GT, 
	OPTYPE_GE, 
	OPTYPE_LT, 
	OPTYPE_LE, 
	OPTYPE_NE, 
	OPTYPE_MODULUS, 
	OPTYPE_MULTIPLY, 
	OPTYPE_SHLEFT, 
	OPTYPE_SHRIGHT, 
	OPTYPE_SUBTRACT } Optype;

typedef struct { char *opname;
                 Optype optype;
               } Operator;

static const Operator operators[] = {
	"||", OPTYPE_BOOLOR, 
	"^^", OPTYPE_BOOLXOR, 
	">>", OPTYPE_SHRIGHT, 
	">=", OPTYPE_GE, 
	"==", OPTYPE_EQ, 
	"<>", OPTYPE_NE, 
	"<=", OPTYPE_LE, 
	"<<", OPTYPE_SHLEFT, 
	"&&", OPTYPE_BOOLAND, 
	"!=", OPTYPE_NE, 
	"|",  OPTYPE_BITOR, 
	"^",  OPTYPE_BITXOR, 
	">",  OPTYPE_GT, 
	"<",  OPTYPE_LT, 
	"/",  OPTYPE_DIVIDE, 
	"-",  OPTYPE_SUBTRACT, 
	"+",  OPTYPE_ADD, 
	"*",  OPTYPE_MULTIPLY, 
	"&",  OPTYPE_BITAND, 
	"%",  OPTYPE_MODULUS, 
	NULL, OPTYPE_UNKNOWN };

typedef struct Pipebuf Pipebuf;
typedef struct Runopts Runopts;

struct Pipebuf { Pipebuf *next;
                 Runopts *runopts;
                 unsigned long rlen;
                 char data[256];
               };

struct Runopts { const char *defdir;		/* -directory name */
                 const char *error_name;	/* -error file name */
                 const char *ersym_name;	/* -ersym symbol name */
                 const char *exit_name;		/* -exit event flag logical name */
                 const char *init_name;		/* -init event flag logical name */
                 const char *input_name;	/* -input file name */
                 const char *insym_name;	/* -insym symbol name */
                 const char *job_name;		/* -job name */
                 const char *output_name;	/* -output file name */
                 const char *outsym_name;	/* -outsym symbol name */
                 const char *process_name;	/* -process logical name */
                 const char *thread_name;	/* -thread logical name */
                 int timeit;			/* -timeit flag */
                 int wait;			/* opposite of -nowait setting */
                 int orphan;			/* -orphan flag */
                 unsigned long h_err;		/* -error io channel handle */
                 unsigned long h_exit;		/* -exit event flag handle */
                 unsigned long h_init;		/* -init event flag handle */
                 unsigned long h_in;		/* -input io channel handle */
                 unsigned long h_job;		/* -job handle */
                 unsigned long h_out;		/* -output io channel handle */
                 unsigned long h_process;		/* created process handle (0 for internal commands) */
                 unsigned long h_thread;		/* created thread handle */

                 unsigned long h_ersym;		/* read ersym data from this pipe handle */
                 unsigned long h_insym;		/* write insym data to this pipe handle */
                 unsigned long h_outsym;		/* read outsym data from this pipe handle */
                 unsigned long h_evsym;		/* this event sets when symbol stream completes */
                 Pipebuf *ersym_pipebuf_qh;	/* list of buffers for -ersym */
                 Pipebuf **ersym_pipebuf_qt;
                 char *insym_data;		/* pointer to buffer for -insym */
                 Pipebuf *outsym_pipebuf_qh;	/* list of buffers for -outsym */
                 Pipebuf **outsym_pipebuf_qt;
               };

static const struct { char *name; unsigned long valu; } lockmodes[LCK$K_EXMODE+1] = 
                    { "NL", LCK$K_NLMODE, "CR", LCK$K_CRMODE, "CW", LCK$K_CWMODE, "PR", LCK$K_PRMODE, "PW", LCK$K_PWMODE, "EX", LCK$K_EXMODE };

static char *pn, skiplabel[MAXLBLNAM];
static char *defaulttables;
static int ctrly_hit, exited, verify;
static Label *labels;
static unsigned long h_s_console;
static unsigned long h_event_ctrly;
static unsigned long h_s_error;
static unsigned long h_s_input;
static unsigned long h_s_output;
static unsigned long h_timer;
#if 0
static OZ_IO_fs_getinfo1 scriptinfo;
static OZ_IO_fs_readrec scriptread;
#endif
static Script *scripts;
static Symbol *symbols;

static void startendmsg (const char *name);
static void ctrly_enable (void);
static void ctrly_ast (void *dummy, unsigned long status, void *mchargs);
static void ctrlt_enable (void);
static void ctrlt_ast (void *dummy, unsigned long status, void *mchargs);
static void exit_script (void);
static char *proclabels (char *cmdbuf);
static unsigned long execute (char *cmdbuf);
static unsigned long exechopped (int argc, const char *argv[], const char *input, const char *output, int nowait);
static unsigned long substitute (char *inbuf, char **outbuf_r);
static unsigned long eval_handle (char *inbuf, char **inbuf_r, unsigned long *outval_r, char *objtype, char termch);
static unsigned long eval_integer (char *inbuf, char **inbuf_r, unsigned long *outval_r, char termch);
static unsigned long eval_string (char *inbuf, char **inbuf_r, char **outbuf_r, char termch);
static unsigned long evaluate (char *inbuf, char **inbuf_r, Symbol **symbol_r, char termch);
static unsigned long get_operand (char *inbuf, char **inbuf_r, Symbol **symbol_r);
static Optype get_operator (char *inbuf, char **inbuf_r);
static char *cvt_sym_to_str (Symbol *symbol);
static unsigned long func_collapse    (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_compress    (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_date_add    (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_date_dow    (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_date_now    (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_date_sub    (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_date_tzconv (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_event_inc   (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_event_set   (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_field       (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_h_info      (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_len         (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_lnm_attrs   (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_lnm_lookup  (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_lnm_nvalues (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_lnm_object  (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_lnm_string  (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_loc         (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_lowercase   (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_process     (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_sub         (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_thread      (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_trim        (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_uppercase   (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long func_verify      (Symbol *symbol, char *strp, char **rtnp, void *valuep);
static unsigned long decode_objtype_string (const char *s_objtype, unsigned char *b_objtype_r);
static const char *encode_objtype_string (unsigned char b_objtype);
static void def_func (char *name, unsigned long (*func) (Symbol *symbol, char *strp, char **rtnp, void *valuep), Symtype symtype, char *help);
static void setscriptsyms (int argc, const char *argv[]);
static void insert_symbol (Symbol *symbol, unsigned long (*func) (Symbol *symbol, char *strp, char **rtnp, void *valuep), unsigned long level);
static Symbol *lookup_symbol (const char *name, unsigned long level, unsigned long *level_r);
static Command *decode_command (int argc, const char **argv, Command *cmdtbl, int *argc_r);
static int cmpcmdname (int argc, const char **argv, char *name);
static unsigned long logname_getobj (const char *name, unsigned char objtype, unsigned long *h_object_r);
static unsigned long logname_creobj (const char *name, unsigned char objtype, unsigned long h_object);
static unsigned long delete_logical (unsigned long h_error, const char *default_table_name, int argc, const char *argv[]);
static unsigned long extcommand (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long runimage (unsigned long h_error, Runopts *runopts, const char *image, int argc, const char *argv[]);
static unsigned long decode_runopts (const char *input, const char *output, int nowait, unsigned long h_input, unsigned long h_output, unsigned long h_error, int *argc_r, const char ***argv_r, Runopts *runopts);
static unsigned long setup_runopts (unsigned long h_error, Runopts *runopts) { }
static unsigned long finish_runopts (unsigned long h_error, Runopts *runopts) { }
static unsigned long crepipepair (unsigned long *h_read_r, unsigned long *h_write_r);
static void cleanup_runopts (Runopts *runopts) { }
static void start_ersym_read (Runopts *runopts);
static void ersym_read_ast (void *pipebufv, unsigned long status, void *mchargs);
static void start_insym_write (Runopts *runopts);
static void insym_write_ast (void *runoptsv, unsigned long status, void *mchargs);
static void start_outsym_read (Runopts *runopts);
static void outsym_read_ast (void *pipebufv, unsigned long status, void *mchargs);
static void deferoutsym (const char *sym_name, Pipebuf *pipebuf_qh);
static unsigned long show_device_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts);
static unsigned long show_device_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_device, unsigned long *showopts);
static unsigned long show_iochan_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts);
static unsigned long show_iochan_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_iochan, unsigned long *showopts);
static unsigned long show_job_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts);
static unsigned long show_job_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_job, unsigned long *showopts);
static unsigned long show_logical_table (unsigned long h_output, unsigned long h_error, int level, unsigned long sholnmflg, unsigned long logtblatr, const char *table_name, unsigned long h_table) { }
static unsigned long show_logical_name (unsigned long h_output, unsigned long h_error, int level, unsigned long sholnmflg, int table, unsigned long h_logname);
static unsigned long show_process_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts);
static unsigned long show_process_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_process, unsigned long *showopts);
static void show_symbol (unsigned long h_output, unsigned long h_error, Symbol *symbol);
static unsigned long show_system (unsigned long h_output, unsigned long h_error, unsigned long *showopts);
static unsigned long show_thread_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts);
static unsigned long show_thread_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_thread, unsigned long *showopts);
static void show_thread_seckeys (unsigned long h_output, unsigned long h_thread);
static void show_secattr (unsigned long h_output, unsigned long h_object, unsigned long secattrcode, int prefix_w, const char *prefix);
static void *secmalloc (void *dummy, unsigned long osize, void *obuff, unsigned long nsize);
static unsigned long show_user_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts);
static unsigned long show_user_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_user, unsigned long *showopts);
static unsigned long wait_thread (unsigned long h_error, unsigned long h_thread) { }
static unsigned long wait_events (unsigned long nevents, unsigned long *h_events);

/* Internal command declarations */

static unsigned long int_abort_thread         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_allocate_device      (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_change_password      (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_close_handle         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_event         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_file          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_job           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_logical_name  (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_logical_table (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_mutex         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_symbol        (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_deallocate_device    (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_delete_logical_name  (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_delete_logical_table (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_echo                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_exit                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_goto                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_help                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_if                   (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_more                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_open_file            (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_open_mutex           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_read_file            (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_resume_thread        (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_script               (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_set_console          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_set_default          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_set_event_interval   (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_set_mutex            (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_set_thread           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_datetime        (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_device          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_default         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_event           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_iochan          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_job             (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_logical_name    (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_logical_table   (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_job             (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_mutex           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_process         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_symbol          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_system          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_thread          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_user            (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_show_volume          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_suspend_thread       (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_wait_event           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_wait_mutex           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_wait_thread          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }
static unsigned long int_wait_until           (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]){ }

static Command intcmd[] = {
	0, "abort thread",         int_abort_thread,         NULL, "[-id <thread_id>] [-nowait] [-status <status>] [<thread_logical_name>]", 
	0, "allocate device",      int_allocate_device,      NULL, "[-user] [-job] [-process] [-thread] <device_name>", 
	0, "change password",      int_change_password,      NULL, "[<old_password> [<new_password>]]", 
	1, "close handle",         int_close_handle,         NULL, "<handle> ...", 
	0, "create event",         int_create_event,         NULL, "<logical_name> <event_name>", 
	1, "create file",          int_create_file,          NULL, "[-lockmode <lockmode>] [-logical <logical_name>] <file_name>", 
	0, "create job",           int_create_job,           NULL, "<logical_name> <job_name>", 
	0, "create logical name",  int_create_logical_name,  NULL, "[-kernel] [-nooutermode] [-nosupersede] <logical_name> [-copy <logical_name>] [-link <logical_name>] [-object <handle>] [-terminal] [-value <value>] <value> ...", 
	0, "create logical table", int_create_logical_table, NULL, "[-kernel] [-nooutermode] [-nosupersede] <table_name>", 
	1, "create mutex",         int_create_mutex,         NULL, "<iochan_logical_name> <mutex_device_name> <mutex_name>", 
	0, "create symbol",        int_create_symbol,        NULL, "[-global] [-integer] [-level <n>] [-string] <name> <value> ...",
	0, "deallocate device",    int_deallocate_device,    NULL, "<device_name>", 
	0, "delete logical name",  int_delete_logical_name,  NULL, "<logical_name>", 
	0, "delete logical table", int_delete_logical_table, NULL, "<table_name>", 
	1, "echo",                 int_echo,                 NULL, "<string> ...", 
	0, "exit",                 int_exit,                 NULL, "[<status>]", 
	0, "goto",                 int_goto,                 NULL, "<label>",
	1, "help",                 int_help,                 NULL, "", 
	0, "if",                   int_if,                   NULL, "<integervalue> <statement ...>", 
	1, "more",                 int_more,                 NULL, "[-columns <number>] [-number] [-rows <number>] [<file>]", 
	1, "open file",            int_open_file,            NULL, "[-lockmode <lockmode>] [-logical <logical_name>] <file_name>", 
	1, "open mutex",           int_open_mutex,           NULL, "<iochan_logical_name> <mutex_device_name> <mutex_name>", 
	1, "read file",            int_read_file,            NULL, "[-logical <logical_name>] [-prompt <prompt>] [-size <size>] [-terminator <terminator>] <symbol_name>", 
	0, "resume thread",        int_resume_thread,        NULL, "[-id <thread_id>] [<thread_logical_name>]", 
	0, "script",               int_script,               NULL, "<script_name> [<args> ...]", 
	1, "set console",          int_set_console,          NULL, "[-columns <columns>] [-[no]linewrap] [-rows <rows>] [<logical_name>]", 
	0, "set default",          int_set_default,          NULL, "<directory>", 
	0, "set event interval",   int_set_event_interval,   NULL, "<event_logname> <interval> [<basetime>]", 
	1, "set mutex",            int_set_mutex,            NULL, "[-express] [-noqueue] <iochan_logical_name> <new_mode>", 
	0, "set thread",           int_set_thread,           NULL, "[-id <thread_id>] [<thread_logical_name>] [-creates <secattr>] [-priority <basepri>] [-secattr <secattr>] [-seckeys <seckeys>]", 
	0, "show time",            int_show_datetime,        NULL, "", 
	0, "show devices",         int_show_device,          NULL, "[<device_logical_name> ...] [-iochans] [-objaddr] [-security]", 
	0, "show default",         int_show_default,         NULL, "", 
	1, "show event",           int_show_event,           NULL, "<event_logical_name> ...", 
	1, "show iochan",          int_show_iochan,          NULL, "[<iochan_logical_name> ...] [-objaddr] [-security]", 
	1, "show job",             int_show_job,             NULL, "[<job_logical_name> ...] [-objaddr] [-processes] [-security] [-threads]", 
	1, "show logical name",    int_show_logical_name,    NULL, "<logical_name> [-security]", 
	1, "show logical table",   int_show_logical_table,   NULL, "[<table_name>] [-security]", 
	1, "show mutex",           int_show_mutex,           NULL, "<iochan_logical_name>", 
	1, "show process",         int_show_process,         NULL, "[<process_logical_name> ...] [-objaddr] [-security] [-threads]", 
	1, "show symbol",          int_show_symbol,          NULL, "[<symbol_name> ...]", 
	0, "show system",          int_show_system,          NULL, "[-devices] [-iochans] [-job] [-processes] [-security] [-threads]", 
	1, "show thread",          int_show_thread,          NULL, "[-id <thread_id>] [-objaddr] [-security] [<thread_logical_name> ...]", 
	1, "show user",            int_show_user,            NULL, "[<user_logical_name> ...] [-jobs] [-objaddr] [-processes] [-security] [-threads]", 
	1, "show volume",          int_show_volume,          NULL, "[<device_name> ...]", 
	0, "suspend thread",       int_suspend_thread,       NULL, "[-id <thread_id>] [<thread_logical_name>]", 
	0, "wait event",           int_wait_event,           NULL, "<logical_name> ...", 
	1, "wait mutex",           int_wait_mutex,           NULL, "<iochan_logical_name>", 
	0, "wait thread",          int_wait_thread,          NULL, "[-id <thread_id>] [<thread_logical_name>]", 
	1, "wait until",           int_wait_until,           NULL, "<datetime>", 
	// the following are really not internal
	// waiting for dcltables.exe
	0, "mount", 1, 0, "/vms$common/sysexe/mount",
	0, "directory", 1, 0, "/vms$common/sysexe/directory",
	0, "copy", 1, 0, "/vms$common/sysexe/copy",
	0, "delete", 1, 0, "/vms$common/sysexe/delete",
	0, "difference", 1, 0, "/vms$common/sysexe/difference",
	0, "extend", 1, 0, "/vms$common/sysexe/extend",
	0, "search", 1, 0, "/vms$common/sysexe/search",
	0, "type", 1, 0, "/vms$common/sysexe/type",
	0, NULL, NULL, NULL, NULL };


const char oz_s_logname_defaulttables[] = "DEFAULT";

const char oz_sys_copyright[] = "C";

unsigned long main (int argc, char *argv[])

{
  char cmdbuf[CMDSIZ], *p;
  int i;
  unsigned long cmdlen, sts;
  unsigned long h_event;
  Symbol *symbol;

  defaulttables  = oz_s_logname_defaulttables;

  exited         = 0;
  scripts        = NULL;
  skiplabel[0]   = 0;
  symbols        = NULL;
  verify         = 0;

  h_s_console = 0;
  h_s_error   = stderr;
  h_s_input   = stdin;
  h_s_output  = stdout;

  /* Put argc/argv's in symbols oz_arg0... Set symbol oz_nargs to the number of values. */

  setscriptsyms (argc, (const char **)argv);

  /* Process command line arguments */

  pn = "oz_cli";
  if (argc > 0) pn = argv[0];

  for (i = 1; i < argc; i ++) {

    /* -console <device> : use the alternate device as the console (for control-Y, -T, etc) */

    if (strcmp (argv[i], "-console") == 0) {
      if (++ i >= argc) goto usage;
      if ((h_s_console != 0) && (h_s_console != oz_util_h_console)) oz_sys_handle_release (PSL$C_KERNEL, h_s_console);
      //      sts = sys$assign( &desc, &h_s_console,PSL$C_KERNEL,0,0);
      if (sts != SS$_NORMAL) {
        fprintf (h_s_error, "%s: error %u assigning channel to console %s\n", pn, sts, argv[i]);
        return (sts);
      }
    }

    /* -exec <rest_of_line> : execute the single rest_of_line command */

    if (strcmp (argv[i], "-exec") == 0) {
      argc -= i;
      argv += i;
      break;
    }

    /* -interactive : set up default console device */

    if (strcmp (argv[i], "-interactive") == 0) {
      if ((h_s_console != 0) && (h_s_console != oz_util_h_console)) oz_sys_handle_release (PSL$C_KERNEL, h_s_console);
      h_s_console = oz_util_h_console;
      continue;
    }

    /* -nointeractive : no console device */

    if (strcmp (argv[i], "-nointeractive") == 0) {
      if ((h_s_console != 0) && (h_s_console != oz_util_h_console)) oz_sys_handle_release (PSL$C_KERNEL, h_s_console);
      h_s_console = 0;
      continue;
    }

    /* -noverify : turn off verify */

    if (strcmp (argv[i], "-noverify") == 0) {
      verify = 0;
      continue;
    }

    /* -verify : turn on verify */

    if (strcmp (argv[i], "-verify") == 0) {
      verify = 1;
      continue;
    }

    goto usage;
  }
  if (i == argc) argc = 0;

  if (h_s_console != 0) {
    fprintf (h_s_console, "%s\n", oz_sys_copyright);
    startendmsg ("start");
  }

  /* Set up handle to timer */

#if 0
  sts = sys$assign( "timer", &h_timer,PSL$C_KERNEL,0,0);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u assigning channel to timer\n", sts);
    h_timer = 0;
  }
#endif

  /* Set up expression function declarations in symbol table */

  def_func ("oz_collapse",    func_collapse,    SYMTYPE_STRING,  "collapsed = oz_collapse (string)");
  def_func ("oz_compress",    func_compress,    SYMTYPE_STRING, "compressed = oz_compress (string)");
  def_func ("oz_date_add",    func_date_add,    SYMTYPE_STRING,        "sum = oz_date_add (date1, date2)");
  def_func ("oz_date_dow",    func_date_dow,    SYMTYPE_STRING,  "dayofweek = oz_date_dow (date)");
  def_func ("oz_date_now",    func_date_now,    SYMTYPE_STRING,        "now = oz_date_now");
  def_func ("oz_date_sub",    func_date_sub,    SYMTYPE_STRING,       "diff = oz_date_sub (date1, date2)");
  def_func ("oz_date_tzconv", func_date_tzconv, SYMTYPE_STRING,       "date = oz_date_tzconv (date, conv)");
  def_func ("oz_field",       func_field,       SYMTYPE_STRING,      "field = oz_field (index, separator, string)");
  def_func ("oz_event_inc",   func_event_inc,   SYMTYPE_INTEGER,       "old = oz_event_inc (handle, inc)");
  def_func ("oz_event_set",   func_event_set,   SYMTYPE_INTEGER,       "old = oz_event_set (handle, new)");
  def_func ("oz_h_info",      func_h_info,      SYMTYPE_STRING,     "string = oz_h_info (handle, item)");
  def_func ("oz_len",         func_len,         SYMTYPE_INTEGER,    "length = oz_len (string)");
  def_func ("oz_lnm_attrs",   func_lnm_attrs,   SYMTYPE_STRING,      "attrs = oz_lnm_attrs (h_logname, index)");
  def_func ("oz_lnm_lookup",  func_lnm_lookup,  SYMTYPE_STRING,  "h_logname = oz_lnm_lookup (logname, procmode)");
  def_func ("oz_lnm_nvalues", func_lnm_nvalues, SYMTYPE_INTEGER,   "nvalues = oz_lnm_nvalues (h_logname)");
  def_func ("oz_lnm_object",  func_lnm_object,  SYMTYPE_STRING,   "h_object = oz_lnm_object (h_logname, index, objtype)");
  def_func ("oz_lnm_string",  func_lnm_string,  SYMTYPE_STRING,     "string = oz_lnm_string (h_logname, index)");
  def_func ("oz_loc",         func_loc,         SYMTYPE_INTEGER,    "offset = oz_loc (haystack, needle) (length if not found)");
  def_func ("oz_lowercase",   func_lowercase,   SYMTYPE_STRING,  "lowercase = oz_lowercase (string)");
  def_func ("oz_process",     func_process,     SYMTYPE_STRING,  "h_process = oz_process (process_id)");
  def_func ("oz_sub",         func_sub,         SYMTYPE_STRING,  "substring = oz_sub (size, offset, string)");
  def_func ("oz_thread",      func_thread,      SYMTYPE_STRING,   "h_thread = oz_thread (thread_id)");
  def_func ("oz_trim",        func_trim,        SYMTYPE_STRING,    "trimmed = oz_trim (string)");
  def_func ("oz_uppercase",   func_uppercase,   SYMTYPE_STRING,  "uppercase = oz_uppercase (string)");
  def_func ("oz_verify",      func_verify,      SYMTYPE_INTEGER, "oldverify = oz_verify (-1:nc 0:off 1:on)");

  /* Create an event flag to use for I/O, and one for control-Y */

  sts = oz_sys_event_create (PSL$C_KERNEL, "oz_cli i/o", &h_event);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u creating event flag\n", sts);
    return (sts);
  }

  sts = oz_sys_event_create (PSL$C_KERNEL, "oz_cli ctrl-Y", &h_event_ctrly);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u creating event flag\n", sts);
    return (sts);
  }

  /* Set up and enable ctrl-Y and ctrl-T */

  ctrly_hit = 0;
  ctrly_enable ();
  ctrlt_enable ();

  /* Set up a status symbol (oz_status) */

  symbol = malloc (sizeof *symbol);
  strcpy (symbol -> name, "oz_status");
  symbol -> symtype = SYMTYPE_INTEGER;
  symbol -> ivalue = SS$_NORMAL;
  insert_symbol (symbol, NULL, 0);

  /* Set up command read prompt */

#if 0
  memset (&scriptread, 0, sizeof scriptread);
  scriptread.size = sizeof cmdbuf - 1;
  scriptread.buff = cmdbuf;
  scriptread.rlen = &cmdlen;
  scriptread.trmsize = 1;
  scriptread.trmbuff = "\n";
  scriptread.pmtsize = 7;
  scriptread.pmtbuff = "oz_cli>";

  memset (&scriptinfo, 0, sizeof scriptinfo);
#endif

  /* If -exec, execute the one command they supply                                     */
  /* But it might be a 'script' command, so check for that after executing the command */

  if (argc > 0) {
    h_s_input = 0;
    symbol -> ivalue = exechopped (argc - 1, (const char **)(argv + 1), NULL, NULL, 0);
  }

  /* Process commands from the input file */

  while (!exited && (h_s_input != 0)) {

    /* If control-Y was pressed, wipe out scripts and return to top level */

    if (ctrly_hit) {
      ctrly_hit = 0;
      skiplabel[0] = 0;
      while (scripts != NULL) {
        exit_script ();
      }
#if 0
      scriptread.atblock = 0;
      scriptread.atbyte  = 0;
#endif
    }

    /* Read command line from input file (but first get current file position) */

#if 0
    sts = oz_sys_io (PSL$C_KERNEL, h_s_input, h_event, OZ_IO_FS_GETINFO1, sizeof scriptinfo, &scriptinfo);
    if (sts != SS$_NORMAL) scriptinfo.curblock = 0;
    if ((sts == SS$_NORMAL) || (sts == SS$_ILLIOFUNC)) {
      if (scriptread.atblock != 0) {
        scriptinfo.curblock = scriptread.atblock;
        scriptinfo.curbyte  = scriptread.atbyte;
      }
      sts = oz_sys_io (PSL$C_KERNEL, h_s_input, h_event, IO$_READLBLK, sizeof scriptread, &scriptread);
    }
    scriptread.atblock = 0;
    scriptread.atbyte  = 0;
#endif

    /* If read error, close this script and resume next outer level */

    sts = SS$_NORMAL;

    if (sts != SS$_NORMAL) {
      if (sts != SS$_ENDOFFILE) {					/* error message if not end-of-file */
        fprintf (h_s_error, "oz_cli: error %u reading from input\n", sts);
        symbol -> ivalue = sts;						/* ... and set the error status */
      }
      exit_script ();							/* in any case, pop script level */
      if ((sts == SS$_ENDOFFILE) && exited && (oz_sys_io (PSL$C_KERNEL, h_s_input, h_event, IO$_SENSEMODE, 0, NULL) == SS$_NORMAL)) {
        fprintf (h_s_error, "oz_cli: use 'exit' command to log out\n"); /* don't allow end-of-file to exit interactive session */
        exited = 0;
      }
    }

    /* Otherwise, process the command line */

    else {
      fprintf(stdout,"$ ");
      fflush(stdout);
      //scanf("%s",cmdbuf);
      bzero(cmdbuf,CMDSIZ);
      read(0,cmdbuf,CMDSIZ);
      cmdlen=strlen(cmdbuf)-1;
      //      bcopy(p,cmdbuf,cmdlen);
      //      free(p);
      cmdbuf[cmdlen] = 0;						/* null terminate command line */
      p = proclabels (cmdbuf);						/* process any labels that are present */
      if (skiplabel[0] == 0) {						/* ignore line if skipping to a particular label */
        if (verify) fprintf (h_s_output, "%s\n", cmdbuf);	/* ok, echo if verifying turned on */
        sts = execute (p);						/* execute the command line */
        if (sts != SS$_BRANCHSTARTED) symbol -> ivalue = sts;			/* save the new status */
      }
    }
  }

  /* End of outermost script, return last oz_status symbol value */

  if (h_s_console != 0) startendmsg ("end");
  return (symbol -> ivalue);

  /* Option error, print usage line and exit */

usage:
  fprintf (stderr, "%s: usage: %s [-console <device>] [-exec <rest_of_line>] [-interactive] [-noverify] [-verify]\n", pn, pn);
  return (SS$_BADPARAM);
}

/* Write session starting and ending message, but only if input is a console device */

typedef struct { unsigned long size;
  const void *buff;
  unsigned long trmsize;
  const void *trmbuff;
} OZ_IO_console_write;

typedef struct { unsigned long size;            /* read buffer size */
  void *buff;            /* read buffer address */
  unsigned long *rlen;           /* where to return length read */
  unsigned long pmtsize;         /* prompt buffer size */
  const void *pmtbuff;   /* prompt buffer address */
  unsigned long timeout;         /* 0 = no timeout, else milliseconds */
  int noecho;            /* 0 = echo, 1 = don't echo */
} OZ_IO_console_read;

typedef struct { unsigned long mask[8];         /* 256 bit character mask */
  int wiperah;           /* 0 : leave read-ahead alone; 1 : wipe 
			    read-ahead */
  int terminal;          /* 0 : process character normally; 1 : d
			    on't process normally */
  int abortread;         /* 0 : leave reads intact; 1 : abort pen
			    ding reads */
  unsigned char *ctrlchar;       /* where to return the character */
} OZ_IO_console_ctrlchar;

static void startendmsg (const char *name)

{
  unsigned long long now;
  char buf[64];
  OZ_IO_console_write console_write;

  sys$gettim(&now);
  snprintf (buf, sizeof buf, "%s: session %sing at %19.19t\n", pn, name, now);
  memset (&console_write, 0, sizeof console_write);
  console_write.size = strlen (buf);
  console_write.buff = buf;
  oz_sys_io (PSL$C_KERNEL, h_s_input, 0, IO$_WRITELBLK, sizeof console_write, &console_write);
}

/************************************************************************/
/*									*/
/*  Enable and process control-Y detection on h_s_console device	*/
/*									*/
/************************************************************************/

static void ctrly_enable (void)

{
  unsigned long sts;
  OZ_IO_console_ctrlchar ctrly;

  if (h_s_console != 0) {
    memset (&ctrly, 0, sizeof ctrly);
    ctrly.mask[0] = 1 << ('Y' - '@');
    sts = oz_sys_io_start (PSL$C_KERNEL, h_s_console, NULL, 0, ctrly_ast, NULL, IO$_SETMODE, sizeof ctrly, &ctrly);
    if (sts != SS$_ALLSTARTED) fprintf (h_s_console, "oz_cli ctrly_enable: error %u enabling ctrl-Y detection\n", sts);
  }
}

/* This routine gets called when ctrl-Y is pressed.                                                          */
/* It also gets called when the thread exits, because the main program calls oz_sys_io_abort on the channel. */

static void ctrly_ast (void *dummy, unsigned long status, void *mchargs)

{
  if (status == SS$_NORMAL) {
    ctrly_hit = 1;
  } else if (status != SS$_ABORT) {
    fprintf (h_s_console, "oz_cli ctrly_ast: error %u ctrl-Y completion\n", status);
  }

  ctrly_enable ();
}

/************************************************************************/
/*									*/
/*  Enable and process control-T detection on h_s_console device	*/
/*									*/
/************************************************************************/

static void ctrlt_enable (void)

{
  unsigned long sts;
  OZ_IO_console_ctrlchar ctrlt;

  if (h_s_console != 0) {
    memset (&ctrlt, 0, sizeof ctrlt);
    ctrlt.mask[0] = 1 << ('T' - '@');
    sts = oz_sys_io_start (PSL$C_KERNEL, h_s_console, NULL, 0, ctrlt_ast, NULL, IO$_SETMODE, sizeof ctrlt, &ctrlt);
    if (sts != SS$_ALLSTARTED) fprintf (h_s_console, "oz_cli ctrlt_enable: error %u enabling ctrl-T detection\n", sts);
  }
}

/* This routine gets called when ctrl-T is pressed.                                                          */
/* It also gets called when the thread exits, because the main program calls oz_sys_io_abort on the channel. */

static void ctrlt_ast (void *dummy, unsigned long status, void *mchargs)

{
  char t_name[OZ_THREAD_NAMESIZE], u_quota[256];
  char state[16];
  unsigned long index, sts;
  unsigned long long now, t_tis_com, t_tis_run, t_tis_wev;
  unsigned long h_lastproc, h_nextproc;
  unsigned long h_lastthread, h_nextthread;
  unsigned char t_state;
  unsigned long t_id;

  unsigned long p_item = { 
	JPI$M_FILL1, sizeof h_nextproc, &h_nextproc, NULL };

  unsigned long p_items[] = {
	JPI$M_FILL1, sizeof h_nextproc,   &h_nextproc,   NULL, 
	JPI$M_FILL1, sizeof h_nextthread, &h_nextthread, NULL };

  unsigned long t_items[] = { 
	JPI$M_FILL1,    sizeof h_nextthread, &h_nextthread, NULL, 
        JPI$_STATE,   sizeof t_state,      &t_state,      NULL, 
        JPI$M_FILL1, sizeof t_tis_run,    &t_tis_run,    NULL, 
        JPI$M_FILL1, sizeof t_tis_com,    &t_tis_com,    NULL, 
        JPI$M_FILL1, sizeof t_tis_wev,    &t_tis_wev,    NULL, 
        JPI$_PID,      sizeof t_id,         &t_id,         NULL, 
        JPI$_PRCNAM,    sizeof t_name,        t_name,       NULL };

  unsigned long u_item = {
	JPI$M_FILL1, sizeof u_quota, u_quota, NULL };

  h_lastproc   = 0;
  h_nextproc   = 0;
  h_lastthread = 0;
  h_nextthread = 0;

  /* Check the status */

  if (status != SS$_NORMAL) {
    if (status == SS$_ABORT) ctrlt_enable ();
    else fprintf (h_s_console, "oz_cli ctrlt_ast: error %u ctrl-T completion\n", status);
    return;
  }

  /* First line is the current date/time and the quota */

  sts = oz_sys_handle_getinfo (0, 1, &u_item, &index);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_console, "oz_cli: error %u getting user info\n", sts);
    goto cleanup;
  }
  sys$gettim(&now);
  fprintf (h_s_console, "\noz_cli  %.22t  (%s)\n", now, u_quota);

  /* Get handle to the first process in this job */

  sts = oz_sys_handle_getinfo (0, 1, &p_item, &index);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_console, "oz_cli: error %u getting first process handle\n", sts);
    goto cleanup;
  }

  /* Repeat as long as we find processes in this job */

  while (h_nextproc != 0) {
    h_lastproc = h_nextproc;
    h_nextproc = 0;

    /* Find out process */

    sts = oz_sys_handle_getinfo (h_lastproc, sizeof p_items / sizeof *p_items, p_items, &index);
    if (sts != SS$_NORMAL) {
      fprintf (h_s_console, "oz_cli: error %u getting process info\n", sts);
      goto cleanup;
    }

    /* Repeat as long as we find threads in the process */

    while (h_nextthread != 0) {
      h_lastthread = h_nextthread;
      h_nextthread = 0;

      /* Find out about thread */

      sts = oz_sys_handle_getinfo (h_lastthread, sizeof t_items / sizeof *t_items, t_items, &index);
      if (sts != SS$_NORMAL) {
        fprintf (h_s_console, "oz_cli: error %u getting next thread info\n", sts);
        goto cleanup;
      }

      /* Print out info about thread */

      switch (t_state) {
        case SCH$C_CUR: { strcpy (state, "CUR"); break; }
        case SCH$C_COM: { strcpy (state, "COM"); break; }
        case SCH$C_LEF: { strcpy (state, "LEF"); break; }
#if 0
        case OZ_THREAD_STATE_INI: { strcpy (state, "INI"); break; }
        case OZ_THREAD_STATE_ZOM: { strcpy (state, "ZOM"); break; }
#endif
        default: { snprintf (state, sizeof state, "%3d", t_state); };
      }
      fprintf (h_s_console, "%u: %s  %#.3t/%#.3t/%#.3t  %s\n", t_id, state, t_tis_run, t_tis_com, t_tis_wev, t_name);

      oz_sys_handle_release (PSL$C_KERNEL, h_lastthread);
      h_lastthread = 0;
    }
    oz_sys_handle_release (PSL$C_KERNEL, h_lastproc);
    h_lastproc = 0;
  }

cleanup:
  if (h_nextproc   != 0) oz_sys_handle_release (PSL$C_KERNEL, h_nextproc);
  if (h_lastproc   != 0) oz_sys_handle_release (PSL$C_KERNEL, h_lastproc);
  if (h_nextthread != 0) oz_sys_handle_release (PSL$C_KERNEL, h_nextthread);
  if (h_lastthread != 0) oz_sys_handle_release (PSL$C_KERNEL, h_lastthread);
  ctrlt_enable ();
}

/************************************************************************/
/*									*/
/*  Close current script and re-activate next outer script		*/
/*  If we're already at outermost script, set the exited flag		*/
/*									*/
/************************************************************************/

static void exit_script (void)

{
  Label *label;
  Script *script;
  Symbol *symbol;

  script = scripts;					/* point to script block */
  if (script == NULL) exited = 1;			/* if this is outermost, say we have exited */
  else {
    if (skiplabel[0] != 0) {
      fprintf (h_s_error, "oz_cli: undefined label %s in script\n", skiplabel);
      skiplabel[0] = 0;
    }
    oz_sys_handle_release (PSL$C_KERNEL, h_s_input);	/* this is an inner script, close it */
    h_s_input = script -> h_input;			/* get next outer script handle */
    if (h_s_output != script -> h_output) {		/* restore its output file */
      oz_sys_handle_release (PSL$C_KERNEL, h_s_output);
      h_s_output = script -> h_output;
    }
    if (h_s_error  != script -> h_error)  {		/* restore its error file */
      oz_sys_handle_release (PSL$C_KERNEL, h_s_error);
      h_s_error  = script -> h_error;
    }
    while ((label = labels) != NULL) {			/* free off this ones labels */
      labels = label -> next;
      free (label);
    }
    while ((symbol = symbols) != NULL) {		/* free off this ones symbols */
      symbols = symbol -> next;
      free (symbol);
    }
    labels  = script -> labels;				/* restore outer script's labels */
    symbols = script -> symbols;			/* restore outer script's symbols */
    scripts = script -> next;				/* unlink and free script save block */
    free (script);
  }
}

/************************************************************************/
/*									*/
/*  Process the optional label at the beginning of a command line	*/
/*									*/
/************************************************************************/

static char *proclabels (char *cmdbuf)

{
  char c, labelname[MAXSYMNAM], *p, *q;
  Label *label;

  p = cmdbuf;

  /* Skip over leading spaces */

  while (((c = *p) != 0) && (c <= ' ')) p ++;

  /* Keep scanning from there for either a ': ' or just a space or end-of-line */
  /* If we get a ': ', then we have a label definition */

  for (q = p; (c = *q) > ' '; q ++) {
    if ((c == ':') && (q[1] <= ' ')) {

      /* Found a label, put name in 'labelname' */

      *q = 0;
      strncpy (labelname, p, sizeof labelname);
      *(q ++) = ':';

      /* See if we already have such a label for this script */

      for (label = labels; label != NULL; label = label -> next) {
        if (strcmp (label -> name, labelname) == 0) break;
      }

      /* If not, create a new label definition */

      if (label == NULL) {
        label = malloc (sizeof *label);
        label -> next = labels;
        strncpy (label -> name, labelname, sizeof label -> name);
        labels = label;
      }

      /* Either way, store the current record's address in the label definition */

#if 0
      label -> blkoffs = scriptinfo.curblock;
      label -> bytoffs = scriptinfo.curbyte;
#endif

      /* If we are skipping to a particular label, see if we are there */

      if ((skiplabel[0] != 0) && (strcmp (skiplabel, label -> name) == 0)) skiplabel[0] = 0;

      /* There or not, return pointer in command line past the label */

      return (q);
    }
  }

  /* Got to a space before we found a colon, so there is no label */

  return (p);
}

/************************************************************************/
/*									*/
/*  This routine executes a command line				*/
/*									*/
/*    Input:								*/
/*									*/
/*	cmdbuf = null terminated command line string			*/
/*									*/
/*    Output:								*/
/*									*/
/*	execute = completion status					*/
/*									*/
/************************************************************************/

static unsigned long execute (char *cmdbuf)

{
  char c, *input, *newbuf, pipei[OZ_DEVUNIT_NAMESIZE], pipeo[OZ_DEVUNIT_NAMESIZE];
  const char **v;
  int dq, i, j, n, s, sq;
  unsigned long sts;
  unsigned long h_pipei, h_pipeo;

  /* Substitute in variables */

  sts = substitute (cmdbuf, &newbuf);
  if (sts != SS$_NORMAL) return (sts);

  /* Chop command line up into parameter vector */

  v = malloc ((strlen (newbuf) / 2 + 2) * sizeof *v);

  input   = NULL;				/* no piping going on */
  h_pipei = 0;
  h_pipeo = 0;
  i = 0;					/* start at beg of command line */
restart:
  j = i;					/* no output chars yet */
  n = 0;					/* no output tokens yet */
  dq = 0;					/* not inside double quotes */
  sq = 0;					/* not inside single quotes */
  s = 1;					/* skip leading spaces */
  for (; (c = newbuf[i]) != 0; i ++) {		/* loop through all input chars */
    if (!dq && !sq && (c == '#')) break;	/* stop if unquoted comment character */
    if (!dq && !sq && (c == '|') && (newbuf[i+1] == '>')) goto piping;
    if (!sq && (c == '"')) dq = 1 - dq;		/* if double quote outside single quotes, flip double quote flag */
    else if (!dq && (c == '\'')) sq = 1 - sq;	/* if single quote outside double quotes, flip single quote flag */
    else if (dq || sq || (c > ' ')) {		/* otherwise, check for printables (or quoted non-printables) */
      if (c == '\\') {				/* ok, check for backslash (the escape char) */
        c = newbuf[++i];			/* if so, get the next char */
        if (c == 0) break;			/* (abort loop if eol) */
        if (c == 'b') c = '\b';
        if (c == 'n') c = '\n';
        if (c == 'r') c = '\r';
        if (c == 't') c = '\t';
      }
      if (s) {
        v[n++] = newbuf + j;			/* start new token if we were skipping spaces */
        s = 0;					/* no longer skipping spaces */
      }
      newbuf[j++] = c;				/* store printable or escaped char in output buffer */
    } else if (!s) {				/* non-printable, see if first non-printable */
      newbuf[j++] = 0;				/* first non-printable, null-terminate prior string */
      s = 1;					/* ... and remember we're skipping spaces */
    }
  }
  newbuf[j] = 0;				/* make sure last token is null terminated */
  v[n] = NULL;					/* put a null pointer on the end of vector */

  /* Execute the chopped up command - maybe input is a pipe, output is normal, don't force -nowait */

  sts = exechopped (n, v, input, NULL, 0);

  /* Free off temp buffers */

rtn:
  free (newbuf);
  free (v);

  /* Close any pipe handles */

  if (h_pipei != 0) oz_sys_handle_release (PSL$C_KERNEL, h_pipei);
  if (h_pipeo != 0) oz_sys_handle_release (PSL$C_KERNEL, h_pipeo);

  /* Return completion status */

  return (sts);

  /* Piping using |> */

piping:
  i += 2;								/* skip over the |> */
  newbuf[j] = 0;							/* make sure last arg string is terminated */
  v[n] = NULL;								/* terminate the arg list with a null */
  //  sts = sys$assign( OZ_IO_PIPES_TEMPLATE, &h_pipeo,PSL$C_KERNEL,0,0); /* create a stream-style pipe */
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u creating pipe\n", sts);
    h_pipeo = 0;
    goto rtn;
  }
  sts = oz_sys_iochan_getunitname (h_pipeo, sizeof pipeo - 1, pipeo);	/* get the pipe's device name */
  if (sts != SS$_NORMAL) oz_sys_condhand_signal (2, sts, 0);
  strcat (pipeo, ":");							/* put a colon on the end */
  sts = exechopped (n, v, input, pipeo, 1);				/* execute the command using that pipe as the -output */
									/* ... and forcing a -nowait option */
  if ((sts != SS$_NORMAL) && (sts != SS$_BRANCHSTARTED)) goto rtn;		/* all done if some failure */
  if (h_pipei != 0) oz_sys_handle_release (PSL$C_KERNEL, h_pipei);	/* if input was a pipe, it's safe to release handle because exechopped made its own */
  strcpy (pipei, pipeo);						/* set the new pipe as input to the next command on line */
  input   = pipei;
  h_pipei = h_pipeo;
  h_pipeo = 0;
  goto restart;								/* go process next command on line following |> */
}

/************************************************************************/
/*									*/
/*  Execute chopped-up command						*/
/*									*/
/*    Input:								*/
/*									*/
/*	argc = argument count						*/
/*	argv = argument vector						*/
/*									*/
/*    Output:								*/
/*									*/
/*	exechopped = SS$_BRANCHSTARTED : command just started			*/
/*	                   else : command's status			*/
/*									*/
/************************************************************************/

typedef struct { Runopts runopts;
                 Command *command;
                 int argc;
                 const char **argv;
                 signed long refcount;
               } Tc;

static unsigned long threadcommand (void *tcv);
static void threadcommandexit (void *tcv, unsigned long status);

static unsigned long exechopped (int argc, const char *argv[], const char *input, const char *output, int nowait)

{
  const char **aargv;
  int aargc, i;
  unsigned long sts;
  Tc *tc;
  char reallyext=0;

  /* Ignore blank lines */

  if (argc == 0) return (SS$_BRANCHSTARTED);

  /* Decode the prefix options in case of internal async command */

  tc = malloc (sizeof *tc);			/* allocate threadcommand buffer */
  tc -> argc = argc;				/* fill it in with prefix options from command line args */
  tc -> argv = argv;
  sts = decode_runopts (input, output, nowait, h_s_input, h_s_output, h_s_error, &(tc -> argc), &(tc -> argv), &(tc -> runopts));
  if (sts != SS$_NORMAL) {
    free (tc);					/* some error processing prefixes, release buffer */
    return (sts);				/* return error status */
  }
  if (nowait) tc -> runopts.wait = 0;		/* maybe force -nowait */

  /* Check for internal commands */

  tc -> command = decode_command (tc -> argc, tc -> argv, intcmd, &i);
  if (tc -> command && tc -> command -> entry == 1) {
    tc -> argv[0] = tc -> command -> help;
    tc -> command = 0;
    reallyext=1;
  }
  if (tc -> command != NULL) {
    tc -> argc -= i;
    tc -> argv += i;

    /* Internal commands flagged 'async' run as a thread */

    if (tc -> command -> async) {
      if ((tc -> runopts.defdir != NULL) || (tc -> runopts.job_name != NULL) || (tc -> runopts.orphan) || (tc -> runopts.process_name != NULL)) {
        fprintf (h_s_error, "oz_cli: -directory, -job, -orphan and -process options illegal on '%s' command\n", tc -> command -> name);
        cleanup_runopts (&(tc -> runopts));
        free (tc);
        return (SS$_BADPARAM);
      }
      sts = setup_runopts (h_s_error, &(tc -> runopts));		/* open corresponding handles */
      if (sts == SS$_NORMAL) {
        tc -> refcount = 2;						/* don't free tc until both thread and I are done with it */
        sts = oz_sys_thread_create (PSL$C_KERNEL, 0, 0, tc -> runopts.h_init, tc -> runopts.h_exit, 0, 
                                    threadcommand, tc, 1 /*OZ_ASTMODE_ENABLE*/, tc -> command -> name, &(tc -> runopts.h_thread));
        if (sts != SS$_NORMAL) {
          fprintf (h_s_error, "oz_cli: error %u creating thread for %s\n", sts, tc -> command -> name);
          cleanup_runopts (&(tc -> runopts));				/* couldn't create thread, close off runopts */
          free (tc);							/* then free the tc */
        } else {
          sts = finish_runopts (h_s_error, &(tc -> runopts));		/* thread started, finish with runopts */
          if (oz_hw_atomic_inc_long (&(tc -> refcount), -1) == 0) {	/* dec ref count */
            cleanup_runopts (&(tc -> runopts));				/* I made them go zero, close runopts */
            free (tc);							/* ... and free the tc */
          }
        }
      }
    }

    /* Otherwise, they are simply called */

    else {
      if ((tc -> runopts.defdir != NULL) || (tc -> runopts.job_name != NULL) || (tc -> runopts.process_name != NULL) || (tc -> runopts.timeit) || (tc -> runopts.orphan) 
       || (tc -> runopts.exit_name != NULL) || (tc -> runopts.init_name != NULL) || (tc -> runopts.thread_name != NULL) || !(tc -> runopts.wait)) {
        fprintf (h_s_error, "oz_cli: -directory, -exit, -init, -job, -nowait, -orphan, -process, -thread and -timeit options illegal on '%s' command\n", tc -> command -> name);
        cleanup_runopts (&(tc -> runopts));
        free (tc);
        return (SS$_BADPARAM);
      }
      sts = (*(tc -> command -> entry)) (tc -> runopts.h_in, 
                                         tc -> runopts.h_out, 
                                         tc -> runopts.h_err, 
                                         tc -> command -> name, 
                                         tc -> command -> param, 
                                         tc -> argc, 
                                         tc -> argv);
      cleanup_runopts (&(tc -> runopts));
      free (tc);
    }
  }

  /* If not, assume it is an external command - it reprocesses the options */

  else {
    free (tc);
    aargc = argc;
    aargv = argv;
    if ((input != NULL) || (output != NULL) || nowait) {
      aargv = malloc ((aargc + 5) * sizeof *argv);
      aargc = 0;
      if (input  != NULL) { aargv[aargc++] = "-input";  aargv[aargc++] = input;  }
      if (output != NULL) { aargv[aargc++] = "-output"; aargv[aargc++] = output; }
      if (nowait)         { aargv[aargc++] = "-nowait"; }
      memcpy (aargv + aargc, argv, argc * sizeof *argv);
      aargc += argc;
    }
    sts = extcommand (h_s_input, h_s_output, h_s_error, "", reallyext /*NULL*/, aargc, aargv);
    if (aargv != argv) free (aargv);
  }

  return (sts);
}

/* This executes an internal command as a thread */

static unsigned long threadcommand (void *tcv)

{
  unsigned long sts;
  Tc *tc;

  tc = tcv;
  sts = oz_sys_exhand_create (PSL$C_KERNEL, threadcommandexit, tc);
  if (sts != SS$_NORMAL) fprintf (tc -> runopts.h_err, "oz_cli: error %u creating exit handler\n", sts);
  else sts = (*(tc -> command -> entry)) (tc -> runopts.h_in, 
                                          tc -> runopts.h_out, 
                                          tc -> runopts.h_err, 
                                          tc -> command -> name, 
                                          tc -> command -> param, 
                                          tc -> argc, 
                                          tc -> argv);
  return (sts);
}

static void threadcommandexit (void *tcv, unsigned long status)

{
  Tc *tc;

  tc = tcv;
  if (oz_hw_atomic_inc_long (&(tc -> refcount), -1) == 0) {	/* dec ref count */
    cleanup_runopts (&(tc -> runopts));				/* I made them go zero, close runopts */
    free (tc);							/* ... and free the tc */
  }
}

/************************************************************************/
/*									*/
/*  This routine takes a command line and makes all the variable 	*/
/*  substitutions for expressions enclosed by {...}.  Strings enclosed 	*/
/*  by '...' are excluded from substitution.				*/
/*									*/
/*    Input:								*/
/*									*/
/*	inbuf = input string buffer (null terminated)			*/
/*									*/
/*    Output:								*/
/*									*/
/*	substitute = SS$_NORMAL : successful				*/
/*	                   else : error status				*/
/*	*outbuf_r = output string buffer pointer (malloc'd)		*/
/*									*/
/************************************************************************/

static unsigned long substitute (char *inbuf, char **outbuf_r)

{
  char c, *evalbuf, *outbuf, *p;
  int i, j, l, m, o, q;
  Symbol *symbol;
  unsigned long sts;

  *outbuf_r = NULL;

  m = strlen (inbuf) + 8;			/* allocate an output buffer */
  outbuf = malloc (m);				/* ... a little bigger than input */
  o = 0;					/* nothing in output buffer yet */

  q = 0;					/* not in quotes */
  for (i = 0; (c = inbuf[i]) != 0; i ++) {	/* loop through input string */
    if (c == '\\') {				/* check for backslash */
      c = inbuf[++i];				/* that means next char is literal */
      if (c == 0) break;
      outbuf[o++] = '\\';			/* put backslash in output buffer */
						/* ... and copy escaped char to output buffer */
    } else if (c == '\'') {			/* check for single quote */
      q = 1 - q;				/* if so, flip quote flag */
						/* ... and copy it to output buffer */
    } else if (!q && (c == '{')) {		/* check for opening brace */
      c = inbuf[++i];				/* get character following the { */
      if (SYMBOLSTARTCHAR (c)) {		/* special case for {symbolname} */
        for (j = i; (c = inbuf[++j]) != 0;) if (!SYMBOLCHAR (c)) break;
        if (c == '}') {
          inbuf[j] = 0;				/* ok, look up the symbol name */
          symbol = lookup_symbol (inbuf + i, 0, NULL);
          inbuf[j] = '}';
          if (symbol == NULL) {			/* if undefined, ... */
            i = j;				/* ... just skip over it */
            continue;
          }
        }
      }
      sts = eval_string (inbuf + i, &p, &evalbuf, 0); /* ok, evaluate string, should terminate on a } */
      if (sts != SS$_NORMAL) {
        free (outbuf);				/* evaluation error, free output buffer */
        return (sts);				/* return error status */
      }
      if (*p != '}') {
        fprintf (h_s_error, "oz_cli: expected } terminator at %s\n", p);
        free (outbuf);
        return (SS$_BADPARAM);
      }
      i = p - inbuf;				/* ok, point at the '}' (the i++ of the for stmt will skip over it) */
      l = strlen (evalbuf);			/* get length of result */
      if (o + l >= m) {				/* see if it would overflow output */
        m += l + strlen (inbuf + i) + 8;	/* if so, malloc a new output */
        p = malloc (m);
        memcpy (p, outbuf, o);
        free (outbuf);
        outbuf = p;
      }
      memcpy (outbuf + o, evalbuf, l);		/* concat onto output buffer */
      o += l;
      free (evalbuf);				/* all done with temp buffer */
      continue;					/* don't put } in output buffer */
    }

    /* Put character c in output buffer */

    if (o + 1 >= m) {				/* see if enough room for it and one other */
						/* this allows for this char and a null terminator */
      m += strlen (inbuf + i) + 8;		/* if not, malloc a new output */
      p = malloc (m);
      memcpy (p, outbuf, o);
      free (outbuf);
      outbuf = p;
    }
    outbuf[o++] = c;				/* there is room, put char in buff */
  }

  outbuf[o] = 0;				/* done, null terminate output */
  *outbuf_r = outbuf;				/* return outbuf pointer */
  return (SS$_NORMAL);				/* return success status */
}

/************************************************************************/
/*									*/
/*  Evaluate the expression up to a terminator character		*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Evaluate handle expression						*/
/*									*/
/*    Input:								*/
/*									*/
/*	inbuf   = start of expression string				*/
/*	termch  = expected termination char (or 0 to not check)		*/
/*	objtype = object type string ("" if don't care)			*/
/*									*/
/*    Output:								*/
/*									*/
/*	eval_handle = SS$_NORMAL : successful evaluation		*/
/*	                    else : error status				*/
/*	*inbuf_r  = points after termination character in string	*/
/*	*outval_r = handle result					*/
/*									*/
/************************************************************************/

static unsigned long eval_handle (char *inbuf, char **inbuf_r, unsigned long *outval_r, char *objtype, char termch)

{
  int usedup;
  unsigned long sts;
  Symbol *symbol;

  sts = evaluate (inbuf, inbuf_r, &symbol, termch);
  if (sts != SS$_NORMAL) return (sts);

  switch (symbol -> symtype) {
    case SYMTYPE_INTEGER: {
      fprintf (h_s_error, "oz_cli: %s handle value required at %u\n", objtype, symbol -> ivalue);
      free (symbol);
      return (SS$_BADPARAM);
    }
    case SYMTYPE_STRING: {
      if (symbol -> svalue[0] == 0) {
        *outval_r = 0;
        break;
      }
      *outval_r = oz_hw_atoz (symbol -> svalue, &usedup);
      if ((symbol -> svalue[usedup] != ':') || ((objtype[0] != 0) && (strcmp (symbol -> svalue + usedup + 1, objtype) != 0))) {
        fprintf (h_s_error, "oz_cli: %s handle value required at %s\n", objtype, symbol -> svalue);
        free (symbol);
        return (SS$_BADPARAM);
      }
      break;
    }
  }

  free (symbol);
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Evaluate integer expression						*/
/*									*/
/*    Input:								*/
/*									*/
/*	inbuf = start of expression string				*/
/*	termch = expected termination char (or 0 to not check)		*/
/*									*/
/*    Output:								*/
/*									*/
/*	eval_integer = SS$_NORMAL : successful evaluation		*/
/*	                     else : error status			*/
/*	*inbuf_r = points after termination character in string		*/
/*	*outval_r = integer result					*/
/*									*/
/************************************************************************/

static unsigned long eval_integer (char *inbuf, char **inbuf_r, unsigned long *outval_r, char termch)

{
  int usedup;
  unsigned long sts;
  Symbol *symbol;

  sts = evaluate (inbuf, inbuf_r, &symbol, termch);
  if (sts != SS$_NORMAL) return (sts);

  switch (symbol -> symtype) {
    case SYMTYPE_INTEGER: {
      *outval_r = symbol -> ivalue;
      break;
    }
    case SYMTYPE_STRING: {
      *outval_r = oz_hw_atoi (symbol -> svalue, &usedup);
      if (symbol -> svalue[usedup] != 0) {
        fprintf (h_s_error, "oz_cli: integer value required at %s\n", symbol -> svalue);
        free (symbol);
        return (SS$_BADPARAM);
      }
      break;
    }
  }

  free (symbol);
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Evaluate string expression						*/
/*									*/
/*    Input:								*/
/*									*/
/*	inbuf = start of expression string				*/
/*	termch = expected termination char (or 0 to not check)		*/
/*									*/
/*    Output:								*/
/*									*/
/*	eval_string = SS$_NORMAL : successful evaluation		*/
/*	                    else : error status				*/
/*	*inbuf_r = points after termination character in string		*/
/*	*outbuf_r = points to null terminated malloc'd result		*/
/*	            (caller must free string when done)			*/
/*									*/
/************************************************************************/

static unsigned long eval_string (char *inbuf, char **inbuf_r, char **outbuf_r, char termch)

{
  unsigned long sts;
  Symbol *symbol;

  sts = evaluate (inbuf, inbuf_r, &symbol, termch);
  if (sts == SS$_NORMAL) {
    *outbuf_r = cvt_sym_to_str (symbol);
    free (symbol);
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*  Evaluate expression and determine its type				*/
/*									*/
/*    Input:								*/
/*									*/
/*	inbuf = start of expression string				*/
/*	termch = expected termination char (or 0 to not check)		*/
/*									*/
/*    Output:								*/
/*									*/
/*	evaluate = SS$_NORMAL : successful evaluation			*/
/*	                 else : error status				*/
/*	*inbuf_r = points after termination character in string		*/
/*	*symbol_r = points to malloc'd temp symbol-like block		*/
/*	            caller must free it when done with it		*/
/*									*/
/************************************************************************/

static unsigned long evaluate (char *inbuf, char **inbuf_r, Symbol **symbol_r, char termch)

{
  char *p, *q;
  int cmpstr;
  unsigned long ivalue, sts;
  Optype optype;
  Symbol *lsymbol, *rsymbol;

  *symbol_r = NULL;

  sts = get_operand (inbuf, &inbuf, &lsymbol);				/* get left-hand operand */
  if (sts != SS$_NORMAL) return (sts);

get_op:
  optype = get_operator (inbuf, &inbuf);				/* get operator */

  if (optype == OPTYPE_UNKNOWN) {
    if (termch != 0) {							/* maybe check termination character */
      if (*inbuf != termch) {
        fprintf (h_s_error, "oz_cli: bad expression terminator at %s, expected %c\n", inbuf, termch);
        return (SS$_BADPARAM);
      }
      inbuf ++;								/* skip over terminator */
      while ((*inbuf != 0) && (*inbuf <= ' ')) inbuf ++;		/* skip trailing spaces */
    }

    *inbuf_r = inbuf;							/* return pointer to terminating character */
    *symbol_r = lsymbol;
    return (SS$_NORMAL);
  }

  sts = get_operand (inbuf, &inbuf, &rsymbol);				/* get right-hand operand */
  if (sts != SS$_NORMAL) return (sts);

  switch (optype) {

    case OPTYPE_ADD: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue += rsymbol -> ivalue;
      } else {
        p = cvt_sym_to_str (lsymbol);
        q = cvt_sym_to_str (rsymbol);
        free (lsymbol);
        ivalue = strlen (p) + strlen (q);
        lsymbol = malloc (ivalue + 1 + sizeof *lsymbol);
        lsymbol -> symtype = SYMTYPE_STRING;
        strcpy (lsymbol -> svalue, p);
        strcat (lsymbol -> svalue, q);
        free (p);
        free (q);
      }
      goto free_rsym;
    }

    case OPTYPE_BITAND: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue &= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform AND between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_BITOR: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue |= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform OR between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_BITXOR: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue ^= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform XOR between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_BOOLAND: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue = (lsymbol -> ivalue && rsymbol -> ivalue);
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform AND between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_BOOLOR: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue = (lsymbol -> ivalue || rsymbol -> ivalue);
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform OR between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_BOOLXOR: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue = ((lsymbol -> ivalue != 0) ^ (rsymbol -> ivalue != 0));
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform XOR between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_DIVIDE: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue /= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform DIVIDE between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_EQ: { ivalue = 1; goto compare; }
    case OPTYPE_GT: { ivalue = 2; goto compare; }
    case OPTYPE_GE: { ivalue = 3; goto compare; }
    case OPTYPE_LT: { ivalue = 4; goto compare; }
    case OPTYPE_LE: { ivalue = 5; goto compare; }
    case OPTYPE_NE: { ivalue = 6; goto compare; }

    case OPTYPE_MODULUS: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue %= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform MODULUS between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_MULTIPLY: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue *= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform MULTIPLY between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_SHLEFT: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue <<= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform SHIFT between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_SHRIGHT: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue >>= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform SHIFT between two integers\n");
      return (SS$_BADPARAM);
    }

    case OPTYPE_SUBTRACT: {
      if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
        lsymbol -> ivalue -= rsymbol -> ivalue;
        goto free_rsym;
      }
      fprintf (h_s_error, "oz_cli: can only perform SUBTRACT between two integers\n");
      return (SS$_BADPARAM);
    }

  }

  /* Compare operations: ivalue<0> if eq, ivalue<1> if gt, ivalue<2> if lt */

compare:
  if ((lsymbol -> symtype == SYMTYPE_INTEGER) && (rsymbol -> symtype == SYMTYPE_INTEGER)) {
    if (lsymbol -> ivalue > rsymbol -> ivalue) lsymbol -> ivalue = 2;
    else if (lsymbol -> ivalue < rsymbol -> ivalue) lsymbol -> ivalue = 4;
    else lsymbol -> ivalue = 1;
  } else {
    p = cvt_sym_to_str (lsymbol);
    q = cvt_sym_to_str (rsymbol);
    cmpstr = strcmp (p, q);
    free (p);
    free (q);
    if (cmpstr > 0) lsymbol -> ivalue = 2;
    else if (cmpstr < 0) lsymbol -> ivalue = 4;
    else lsymbol -> ivalue = 1;
    lsymbol -> symtype = SYMTYPE_INTEGER;
  }
  lsymbol -> ivalue = ((lsymbol -> ivalue & ivalue) != 0);
free_rsym:
  free (rsymbol);
  goto get_op;
}

/************************************************************************/
/*									*/
/*  Get operand from string and determine its type			*/
/*									*/
/*    Input:								*/
/*									*/
/*	inbuf = pointer to input string					*/
/*									*/
/*    Output:								*/
/*									*/
/*	get_operand = SS$_NORMAL : successful				*/
/*	                    else : error status				*/
/*	*inbuf_r = pointer to char following operand			*/
/*	*symbol_r = pointer to temporary symbol-like malloc'd struct	*/
/*	            caller must free it when done with it		*/
/*									*/
/************************************************************************/

static unsigned long get_operand (char *inbuf, char **inbuf_r, Symbol **symbol_r)

{
  char c, d, *p, *q, *svalue;
  int usedup;
  unsigned long ivalue, sts;
  Symbol *symbol, *xsymbol;

  *symbol_r = NULL;

  for (p = inbuf; (c = *p) != 0; p ++) if (c > ' ') break;		/* skip spaces */

  /* Maybe it's an integer constant (starts with 0..9) */

  if ((c >= '0') && (c <= '9')) {
    symbol = malloc (strlen (p) + sizeof *symbol);			/* allocate symbol block for the integer */
    symbol -> symtype = SYMTYPE_INTEGER;				/* set its type */
    symbol -> ivalue = oz_hw_atoi (p, &usedup);				/* convert the integer string to binary */
    p += usedup;
    goto rtnsym;
  }

  /* Maybe it's a string constant (enclosed in matching ' or ") */

  if ((c == '"') || (c == '\'')) {					/* check for ' or " */
    symbol = malloc (strlen (p) + sizeof *symbol);			/* ok, allocate symbol buffer big enough to hold it */
    symbol -> symtype = SYMTYPE_STRING;					/* say it is of type string */
    q = symbol -> svalue;						/* point to where to put the string */
    while ((d = *(++ p)) != 0) {					/* get next char in input */
      if (d == c) {							/* check for terminating character */
        p ++;								/* if so, point past the terminating character */
        break;								/* ... and all done */
      }
      if (d == '\\') {							/* check for escape character */
        d = *(++ p);							/* if so, get the following character */
        if (d == 0) break;						/* ... stop if we hit end of input string */
        if (d == 'b') d = '\b';
        if (d == 'n') d = '\n';
        if (d == 'r') d = '\r';
        if (d == 't') d = '\t';
      }
      *(q ++) = d;							/* store character in output buffer */
    }
    *q = 0;								/* done, null terminate output string */
    goto rtnsym;
  }

  /* Symbols start with _, a-z, A-Z */

  if (SYMBOLSTARTCHAR (c)) {
    for (q = p; (c = *q) != 0; q ++) if (!SYMBOLCHAR (c)) break;	/* scan buffer for end of symbol name string */
    *q = 0;								/* null terminate symbol name */
    xsymbol = lookup_symbol (p, 0, NULL);				/* look it up wherever it can be found */
    if (xsymbol == NULL) {
      fprintf (h_s_error, "oz_cli: undefined symbol %s\n", p);
      *q = c;
      return (0 /*OZ_UNDEFSYMBOL*/);
    }
    *q = c;								/* restore terminating character */
    p = q;								/* found, point just past symbol name */

    switch (xsymbol -> symtype) {					/* copy the value to a new temp symbol */

      /* Integer variable */

      case SYMTYPE_INTEGER: {
        if (xsymbol -> func != NULL) {					/* see if it's a function name */
          sts = (*(xsymbol -> func)) (xsymbol, p, &p, &ivalue);		/* if so, execute function */
          if (sts != SS$_NORMAL) return (sts);				/* abort if error executing function */
        } else {
          ivalue = xsymbol -> ivalue;					/* simple variable, get the value */
        }
        symbol = malloc (sizeof *symbol);				/* put value in a temp symbol block */
        symbol -> symtype = SYMTYPE_INTEGER;
        symbol -> ivalue = ivalue;
        goto rtnsym;
      }

      /* String variable */

      case SYMTYPE_STRING: {
        if (xsymbol -> func != NULL) {					/* see if it's a function name */
          sts = (*(xsymbol -> func)) (xsymbol, p, &p, &svalue);		/* if so, execute function */
          if (sts != SS$_NORMAL) return (sts);				/* abort if error executing function */
          symbol = malloc (strlen (svalue) + 1 + sizeof *symbol);	/* copy to temp symbol block */
          symbol -> symtype = SYMTYPE_STRING;
          strcpy (symbol -> svalue, svalue);
          free (svalue);
        } else {
          symbol = malloc (strlen (xsymbol -> svalue) + 1 + sizeof *symbol); /* simple variable, copy to temp symbol block */
          symbol -> symtype = SYMTYPE_STRING;
          strcpy (symbol -> svalue, xsymbol -> svalue);
        }
        goto rtnsym;
      }
    }
  }

  fprintf (h_s_error, "oz_cli: invalid operand %s\n", p);
  return (SS$_BADPARAM);

rtnsym:
  *symbol_r = symbol;							/* return pointer to symbol block */
  while (((c = *p) != 0) && (c <= ' ')) p ++;				/* skip following spaces */
  *inbuf_r = p;								/* return pointer to next thing in input */
  return (SS$_NORMAL);							/* successful */
}

/************************************************************************/
/*									*/
/*  Get operator from input string					*/
/*									*/
/*    Input:								*/
/*									*/
/*	inbuf = pointer to input string					*/
/*									*/
/*    Output:								*/
/*									*/
/*	get_operator = operator type					*/
/*	*inbuf_r = pointer to char following operator			*/
/*									*/
/************************************************************************/

static Optype get_operator (char *inbuf, char **inbuf_r)

{
  char c, *p;
  int i;
  const Operator *operator;

  for (p = inbuf; (c = *p) != 0; p ++) if (c > ' ') break;			/* skip spaces */

  for (operator = operators; operator -> opname != NULL; operator ++) {		/* loop through operator table */
    for (i = 0; operator -> opname[i] == p[i]; i ++) if (p[i] == 0) break;	/* see if the name matches */
    if (operator -> opname[i] == 0) {
      p += i;									/* matches, increment past the string */
      while (((c = *p) != 0) && (c <= ' ')) p ++;				/* skip spaces */
      *inbuf_r = p;
      return (operator -> optype);
    }
  }

  return (OPTYPE_UNKNOWN);
}

/************************************************************************/
/*									*/
/*  Convert symbol to string						*/
/*									*/
/*    Input:								*/
/*									*/
/*	symbol = pointer to symbol block				*/
/*									*/
/*    Output:								*/
/*									*/
/*	cvt_sym_to_str = malloc'd string				*/
/*	                 caller must free it when done with it		*/
/*									*/
/************************************************************************/

static char *cvt_sym_to_str (Symbol *symbol)

{
  char *p;

  switch (symbol -> symtype) {
    case SYMTYPE_INTEGER: {
      p = malloc (32);
      oz_hw_itoa (symbol -> ivalue, 32, p);
      return (p);
    }
    case SYMTYPE_STRING: {
      p = malloc (strlen (symbol -> svalue) + 1);
      strcpy (p, symbol -> svalue);
      return (p);
    }
  }

  return (NULL);
}

/************************************************************************/
/*									*/
/*  All internal symbol functions:					*/
/*									*/
/*    Input:								*/
/*									*/
/*	symbol = pointer to symbol that defines function		*/
/*	strp = pointer to input string just past function name		*/
/*									*/
/*    Output:								*/
/*									*/
/*	func = SS$_NORMAL : successful evaluation			*/
/*	             else : evaluation error				*/
/*	*valuep = resultant value					*/
/*									*/
/*    Note:								*/
/*									*/
/*	For SYMTYPE_INTEGER, valuep points to a unsigned long			*/
/*	For SYMTYPE_STRING, valuep points to a char * that is malloc'd 	*/
/*	                    by these functions				*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Collapse out all whitespace						*/
/*									*/
/*	collapsed = oz_collapse (string)				*/
/*									*/
/************************************************************************/

static unsigned long func_collapse (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char c, *p, *q, *string;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;			/* skip spaces */
  if (*(strp ++) != '(') {						/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, rtnp, &string, ')');				/* evaluate the string expression */
  if (sts == SS$_NORMAL) {
    q = string;								/* overwrite input with output */
    for (p = string; (c = *p) != 0; p ++) {				/* scan through input string */
      if (c > ' ') *(q ++) = c;						/* ... and output the printable chars */
    }
    *q = 0;								/* null terminate output string */
    *((char **)valuep) = string;					/* return pointer to string buffer */
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Compress out redundant whitespace					*/
/*									*/
/*	compressed = oz_compress (string)				*/
/*									*/
/************************************************************************/

static unsigned long func_compress (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char c, *p, *q, *string;
  int s;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;			/* skip spaces */
  if (*(strp ++) != '(') {						/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, rtnp, &string, ')');				/* evaluate the string expression */
  if (sts == SS$_NORMAL) {
    s = 0;								/* we're not skipping spaces yet */
    q = string;								/* overwrite input with output */
    for (p = string; (c = *p) != 0; p ++) {				/* scan through input string */
      if (c > ' ') {							/* check for printable char */
        s = 0;								/* if so, don't skip the first space that follows it */
        *(q ++) = c;							/* ... and output the printable char */
      } else if (!s) {							/* nonprintable, see if redundant */
        s = 1;								/* not redundant, next one would be */
        *(q ++) = ' ';							/* ... and output a space */
      }
    }
    *q = 0;								/* null terminate output string */
    *((char **)valuep) = string;					/* return pointer to string buffer */
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Add two date strings						*/
/*									*/
/*	sum = oz_date_add (date1, date2)				*/
/*									*/
/************************************************************************/

static unsigned long func_date_add (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *svalue1, *svalue2;
  int rc1, rc2;
  unsigned long sts;
  unsigned long long date1, date2, now;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, &strp, &svalue1, ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_string (strp, &strp, &svalue2, ')');
  if (sts != SS$_NORMAL) {
    free (svalue1);
    return (sts);
  }
  sys$gettim(&now);
  //  now = oz_sys_datebin_tzconv (now, OZ_DATEBIN_TZCONV_UTC2LCL, 0);
  rc1 = sys$bintim ( svalue1, &date1);
  rc2 = sys$bintim ( svalue2, &date2);
  if (rc1 == 0) {
    fprintf (h_s_error, "oz_cli: bad date string %s\n", svalue1);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  if (rc2 == 0) {
    fprintf (h_s_error, "oz_cli: bad date string %s\n", svalue2);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  if ((rc1 > 0) && (rc2 > 0)) {
    fprintf (h_s_error, "oz_cli: cannot add two absolute dates %s and %s\n", svalue1, svalue2);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  *rtnp = strp;
  free (svalue1);
  free (svalue2);
  //  OZ_HW_DATEBIN_ADD (date1, date1, date2);
  svalue1 = malloc (32);
  rc1 = (rc1 < 0) && (rc2 < 0);
  sys$asctim (32, svalue1, &date1, 0);
  *((char **)valuep) = svalue1;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Get day-of-week string						*/
/*									*/
/*	dayofweek = oz_date_dow (date)					*/
/*									*/
/************************************************************************/

static const char *const daysofweek[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

static unsigned long func_date_dow (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *svalue;
  int rc;
  unsigned long datelongs[OZ_DATELONG_ELEMENTS], sts;
  unsigned long long date, now;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, &strp, &svalue, ')');
  if (sts != SS$_NORMAL) return (sts);
  sys$gettim(&now);
  //  now = oz_sys_datebin_tzconv (now, OZ_DATEBIN_TZCONV_UTC2LCL, 0);
  rc  = sys$bintim ( svalue, &date);
  if (rc <= 0) {
    fprintf (h_s_error, "oz_cli: bad date string %s\n", svalue);
    free (svalue);
    return (SS$_BADPARAM);
  }
  *rtnp  = strp;
  svalue = realloc (svalue, 12);
  oz_sys_datebin_decode (date, datelongs);
  strcpy (svalue, daysofweek[oz_sys_daynumber_weekday(datelongs[OZ_DATELONG_DAYNUMBER])]);
  *((char **)valuep) = svalue;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Get current datetime						*/
/*									*/
/*	now = oz_date_now						*/
/*									*/
/************************************************************************/

static unsigned long func_date_now (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *dates;
  unsigned long long dateb;

  *rtnp = strp;
  sys$gettim(&dateb);
  //  dateb = oz_sys_datebin_tzconv (dateb, OZ_DATEBIN_TZCONV_UTC2LCL, 0);
  dates = malloc (32);
  sys$asctim (32, dates, &dateb, 0);
  *((char **)valuep) = dates;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Subtract two date strings						*/
/*									*/
/*	diff = oz_date_sub (date1, date2)				*/
/*									*/
/************************************************************************/

static unsigned long func_date_sub (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *svalue1, *svalue2;
  int rc1, rc2;
  unsigned long sts;
  unsigned long long date1, date2, now;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, &strp, &svalue1, ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_string (strp, &strp, &svalue2, ')');
  if (sts != SS$_NORMAL) {
    free (svalue1);
    return (sts);
  }
  sys$gettim(&now);
  //  now = oz_sys_datebin_tzconv (now, OZ_DATEBIN_TZCONV_UTC2LCL, 0);
  rc1 = sys$bintim ( svalue1, &date1);
  rc2 = sys$bintim ( svalue2, &date2);
  if (rc1 == 0) {
    fprintf (h_s_error, "oz_cli: bad date string %s\n", svalue1);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  if (rc2 == 0) {
    fprintf (h_s_error, "oz_cli: bad date string %s\n", svalue2);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  if ((rc1 < 0) && (rc2 > 0)) {
    fprintf (h_s_error, "oz_cli: cannot subtract absolute date %s from delta date %s\n", svalue2, svalue1);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  *rtnp = strp;
  free (svalue1);
  free (svalue2);
  //  OZ_HW_DATEBIN_SUB (date1, date1, date2);
  svalue1 = malloc (32);
  rc1 = (rc1 < 0) || (rc2 > 0);
  sys$asctim (32, svalue1, &date1, 0);
  *((char **)valuep) = svalue1;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Timezone conversion							*/
/*									*/
/*	date = oz_date_tzconv (date, conv)				*/
/*									*/
/************************************************************************/

static unsigned long func_date_tzconv (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *svalue1, *svalue2;
  int lcltoutc, rc1;
  unsigned long sts;
  unsigned long long date1, now;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, &strp, &svalue1, ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_string (strp, &strp, &svalue2, ')');
  if (sts != SS$_NORMAL) {
    free (svalue1);
    return (sts);
  }
  lcltoutc = -1;
  if (strcasecmp (svalue2, "utctolcl") == 0) lcltoutc = 0;
  if (strcasecmp (svalue2, "lcltoutc") == 0) lcltoutc = 1;
  if (lcltoutc < 0) {
    fprintf (h_s_error, "oz_cli: bad conversion type %s ('lcltoutc' or 'utctolcl')\n", svalue2);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  now = 0;					// for 'lcltoutc', things like 'now' or 'today' don't make sense
  if (!lcltoutc) sys$gettim(&now);	// for 'utctolcl', they do sort of
  rc1 = sys$bintim ( svalue1, &date1);
  if (rc1 <= 0) {
    fprintf (h_s_error, "oz_cli: bad absolute date string %s\n", svalue1);
    free (svalue1);
    free (svalue2);
    return (SS$_BADPARAM);
  }
  *rtnp = strp;
  free (svalue1);
  free (svalue2);
  //  date1 = oz_sys_datebin_tzconv (date1, lcltoutc ? OZ_DATEBIN_TZCONV_LCL2UTC : OZ_DATEBIN_TZCONV_UTC2LCL, 0);
  svalue1 = malloc (32);
  sys$asctim (32, svalue1, &date1, 0);
  *((char **)valuep) = svalue1;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Extract a field							*/
/*									*/
/*	field = oz_field (index, separator, string)			*/
/*									*/
/************************************************************************/

static unsigned long func_field (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *p, *q, *separator, *string;
  int l;
  unsigned long index, sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_integer (strp, &strp, &index, ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_string (strp, &strp, &separator, ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_string (strp, &strp, &string, ')');
  if (sts != SS$_NORMAL) {
    free (separator);
    return (sts);
  }
  *rtnp = strp;

  l = strlen (separator);
  for (p = string; (q = strstr (p, separator)) != NULL; p = q + l) {
    if (index == 0) break;
    -- index;
  }
  if (index != 0) {
    free (string);
    *((char **)valuep) = separator;
  } else {
    free (separator);
    *((char **)valuep) = string;
    l = q - p;
    if (q == NULL) l = strlen (p);
    memmove (string, p, l);
    string[l] = 0;
  }
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Increment / Set event flag value					*/
/*									*/
/*	oz_event_inc/set (handle, value)				*/
/*									*/
/************************************************************************/

static unsigned long event_incset (unsigned long (*incset) (unsigned long procmode, unsigned long h_event, signed long inc, signed long *value_r), 
                           Symbol *symbol, char *strp, char **rtnp, void *valuep);

static unsigned long func_event_inc (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  return (event_incset (oz_sys_event_inc, symbol, strp, rtnp, valuep));
}

static unsigned long func_event_set (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  return (event_incset (oz_sys_event_set, symbol, strp, rtnp, valuep));
}

static unsigned long event_incset (unsigned long (*incset) (unsigned long procmode, unsigned long h_event, signed long inc, signed long *value_r), 
                           Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  signed long value;
  unsigned long sts;
  unsigned long handle;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_handle (strp, &strp, &handle, "", ',');				/* get the handle */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  sts = eval_integer (strp, &strp, &value, ')');				/* get the item string */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  *rtnp = strp;									/* return pointer just past the ) */
  sts = (*incset) (PSL$C_KERNEL, handle, value, valuep);			/* increment or set the event flag */
  if (sts != SS$_NORMAL) fprintf (h_s_error, "oz_cli: error %u performing %s\n", sts, symbol -> name);
  return (sts);									/* return status */
}

/************************************************************************/
/*									*/
/*  Get handle info							*/
/*									*/
/*	string = oz_h_info (handle, item)				*/
/*									*/
/************************************************************************/

typedef enum { HINFO_OBJTYPE, HINFO_HANDLE, HINFO_LONG, HINFO_ULONG, HINFO_STRING, HINFO_THREADSTATE, HINFO_DATEBINDEL, HINFO_DATEBINABS, HINFO_LOCKMODE } Hinfo;

static const struct {
	const char *name;       unsigned long code;                 unsigned long size;                Hinfo conv; } hinfotbl[] = {
	"objtype",              JPI$M_FILL1,              sizeof (unsigned char),       HINFO_OBJTYPE, 
	"user_handle",          JPI$M_FILL1,          sizeof (unsigned long),        HINFO_HANDLE, 
	"user_refcount",        JPI$M_FILL1,        sizeof (signed long),             HINFO_LONG, 
	"user_lognamdir",       JPI$M_FILL1,       sizeof (unsigned long),        HINFO_HANDLE, 
	"user_lognamtbl",       LNM$_TABLE,       sizeof (unsigned long),        HINFO_HANDLE, 
	"user_name",            JPI$_USERNAME,            OZ_USERNAME_MAX,           HINFO_STRING, 
	"user_first",           JPI$M_FILL1,           sizeof (unsigned long),        HINFO_HANDLE, 
	"user_next",            JPI$M_FILL1,            sizeof (unsigned long),        HINFO_HANDLE, 
	"job_handle",           JPI$M_FILL1,           sizeof (unsigned long),        HINFO_HANDLE, 
	"job_name",             JPI$M_FILL1,             OZ_JOBNAME_MAX,            HINFO_STRING, 
	"job_refcount",         JPI$M_FILL1,         sizeof (signed long),             HINFO_LONG, 
	"job_lognamdir",        JPI$M_FILL1,        sizeof (unsigned long),        HINFO_HANDLE, 
	"job_lognamtbl",        JPI$M_FILL1,        sizeof (unsigned long),        HINFO_HANDLE, 
	"job_processcount",     JPI$M_FILL1,     sizeof (unsigned long),            HINFO_ULONG, 
	"job_first",            JPI$M_FILL1,            sizeof (unsigned long),        HINFO_HANDLE, 
	"job_next",             JPI$M_FILL1,             sizeof (unsigned long),        HINFO_HANDLE, 
	"process_handle",       JPI$M_FILL1,       sizeof (unsigned long),        HINFO_HANDLE, 
	"process_refcount",     JPI$M_FILL1,     sizeof (signed long),             HINFO_LONG, 
	"process_name",         JPI$M_FILL1,         OZ_PROCESS_NAMESIZE,       HINFO_STRING, 
	"process_id",           JPI$M_FILL1,           sizeof (unsigned long),     HINFO_ULONG, 
	"process_lognamdir",    JPI$M_FILL1,    sizeof (unsigned long),        HINFO_HANDLE, 
	"process_lognamtbl",    JPI$M_FILL1,    sizeof (unsigned long),        HINFO_HANDLE, 
	"process_threadcount",  JPI$M_FILL1,  sizeof (unsigned long),            HINFO_LONG, 
	"process_first",        JPI$M_FILL1,        sizeof (unsigned long),        HINFO_HANDLE, 
	"process_next",         JPI$M_FILL1,         sizeof (unsigned long),        HINFO_HANDLE, 
	"thread_handle",        JPI$M_FILL1,        sizeof (unsigned long),        HINFO_HANDLE, 
	"thread_refcount",      JPI$M_FILL1,      sizeof (signed long),             HINFO_LONG, 
	"thread_state",         JPI$_STATE,         sizeof (unsigned char),  HINFO_THREADSTATE, 
	"thread_tis_run",       JPI$M_FILL1,       sizeof (unsigned long long),       HINFO_DATEBINDEL, 
	"thread_tis_com",       JPI$M_FILL1,       sizeof (unsigned long long),       HINFO_DATEBINDEL, 
	"thread_tis_wev",       JPI$M_FILL1,       sizeof (unsigned long long),       HINFO_DATEBINDEL, 
	"thread_tis_zom",       JPI$M_FILL1,       sizeof (unsigned long long),       HINFO_DATEBINDEL, 
	"thread_tis_ini",       JPI$M_FILL1,       sizeof (unsigned long long),       HINFO_DATEBINABS, 
	"thread_exitsts",       JPI$M_FILL1,       sizeof (unsigned long),            HINFO_ULONG, 
	"thread_exitevent",     JPI$M_FILL1,     sizeof (unsigned long),        HINFO_HANDLE, 
	"thread_name",          JPI$_PRCNAM,          OZ_THREAD_NAMESIZE,        HINFO_STRING, 
	"thread_id",            JPI$_PID,            sizeof (unsigned long),      HINFO_ULONG, 
	"thread_first",         JPI$M_FILL1,         sizeof (unsigned long),        HINFO_HANDLE, 
	"thread_next",          JPI$M_FILL1,          sizeof (unsigned long),        HINFO_HANDLE, 
	"device_handle",        JPI$M_FILL1,        sizeof (unsigned long),        HINFO_HANDLE, 
	"device_refcount",      JPI$M_FILL1,      sizeof (signed long),             HINFO_LONG, 
	"device_iochancount",   JPI$M_FILL1,   sizeof (unsigned long),            HINFO_ULONG, 
	"device_first",         JPI$M_FILL1,         sizeof (unsigned long),        HINFO_HANDLE, 
	"device_next",          JPI$M_FILL1,          sizeof (unsigned long),        HINFO_HANDLE, 
	"device_unitname",      JPI$M_FILL1,      OZ_DEVUNIT_NAMESIZE,       HINFO_STRING, 
	"device_classname",     JPI$M_FILL1,     OZ_DEVCLASS_NAMESIZE,      HINFO_STRING, 
	"device_aliasname",     JPI$M_FILL1,     OZ_DEVUNIT_ALIASIZE,       HINFO_STRING, 
	"iochan_handle",        JPI$M_FILL1,        sizeof (unsigned long),        HINFO_HANDLE, 
	"iochan_refcount",      JPI$M_FILL1,      sizeof (signed long),             HINFO_LONG, 
	"iochan_lockmode",      JPI$M_FILL1,      sizeof (unsigned long),      HINFO_LOCKMODE, 
	"iochan_first",         JPI$M_FILL1,         sizeof (unsigned long),        HINFO_HANDLE, 
	"iochan_next",          JPI$M_FILL1,          sizeof (unsigned long),        HINFO_HANDLE, 
	"user_jobcount",        JPI$M_FILL1,        sizeof (unsigned long),            HINFO_ULONG, 
	"device_unitdesc",      JPI$M_FILL1,      OZ_DEVUNIT_DESCSIZE,       HINFO_STRING, 
	"system_boottime",      JPI$M_FILL1,      sizeof (unsigned long long),       HINFO_DATEBINABS, 
	"system_phypagetotal",  JPI$M_FILL1,  sizeof (unsigned long),       HINFO_ULONG, 
	"system_phypagefree",   JPI$M_FILL1,   sizeof (unsigned long),       HINFO_ULONG, 
	"system_phypagel2size", JPI$M_FILL1, sizeof (unsigned long),            HINFO_ULONG, 
	"system_npptotal",      JPI$M_FILL1,      sizeof (unsigned long),       HINFO_ULONG, 
	"system_nppinuse",      JPI$M_FILL1,      sizeof (unsigned long),       HINFO_ULONG, 
	"system_npppeak",       JPI$M_FILL1,       sizeof (unsigned long),       HINFO_ULONG, 
	"system_pgptotal",      JPI$M_FILL1,      sizeof (unsigned long),       HINFO_ULONG, 
	"system_pgpinuse",      JPI$M_FILL1,      sizeof (unsigned long),       HINFO_ULONG, 
	"system_pgppeak",       JPI$M_FILL1,       sizeof (unsigned long),       HINFO_ULONG, 
	"system_cpucount",      JPI$M_FILL1,      sizeof (signed long),             HINFO_LONG, 
	"system_cpusavail",     JPI$M_FILL1,     sizeof (unsigned long),            HINFO_ULONG, 
	"system_syspagetotal",  JPI$M_FILL1,  sizeof (unsigned long),            HINFO_ULONG, 
	"system_syspagefree",   JPI$M_FILL1,   sizeof (unsigned long),            HINFO_ULONG, 
	"system_usercount",     JPI$M_FILL1,     sizeof (unsigned long),            HINFO_ULONG, 
	"system_devicecount",   JPI$M_FILL1,   sizeof (unsigned long),            HINFO_ULONG, 
	"thread_basepri",       JPI$_PRIB,       sizeof (unsigned long),            HINFO_ULONG, 
	"thread_curprio",       JPI$_PRI,       sizeof (unsigned long),            HINFO_ULONG, 
	"thread_parent",        JPI$M_FILL1,        sizeof (unsigned long),        HINFO_HANDLE, 
	NULL };

static unsigned long func_h_info (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char itembuff[256], *item_s, *out;
  int i;
  unsigned long sts;
  unsigned long handle;
  unsigned long itemlist;
  unsigned long lockmode;
  unsigned char objtype;
  unsigned char threadstate;

  /* Parse handle and item arguments */

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_handle (strp, &strp, &handle, "", ',');				/* get the handle */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  sts = eval_string (strp, &strp, &item_s, ')');				/* get the item string */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  *rtnp = strp;									/* return pointer just past the ) */

  /* Decode item arg */

  for (i = 0; hinfotbl[i].name != NULL; i ++) {
    if (strcasecmp (hinfotbl[i].name, item_s) == 0) break;
  }
  if (hinfotbl[i].name == NULL) {
    fprintf (h_s_error, "oz_cli: invalid handle item code %s\n", item_s);
    free (item_s);
    return (SS$_BADPARAM);
  }
  free (item_s);

  /* Build item list */

#if 0
  itemlist.code = hinfotbl[i].code;
  itemlist.size = hinfotbl[i].size;
  itemlist.buff = itembuff;
  itemlist.rlen = NULL;

  if (itemlist.size > sizeof itembuff) oz_crash ("oz_cli: hinfotbl[%d].size is %u", i, itemlist.size);

  /* Get handle info */

  sts = oz_sys_handle_getinfo (handle, 1, &itemlist, NULL);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting handle info\n", sts);
    return (sts);
  }
#endif

  /* Produce output string */

  switch (hinfotbl[i].conv) {
    case HINFO_OBJTYPE: {
      objtype = *(unsigned char *)itembuff;
      out = malloc (32);
      strcpy (out, encode_objtype_string (objtype));
      break;
    }
    case HINFO_HANDLE: {
      handle = *(unsigned long *)itembuff;
      if (handle == 0) {
        out = malloc (1);
        out[0] = 0;
      } else {
#if 0
        itemlist.code = JPI$M_FILL1;
        itemlist.size = sizeof objtype;
        itemlist.buff = &objtype;
        sts = oz_sys_handle_getinfo (handle, 1, &itemlist, NULL);
        if (sts != SS$_NORMAL) {
          fprintf (h_s_error, "oz_cli: error %u getting handle info\n", sts);
          return (sts);
        }
#endif
        out = malloc (24);
        snprintf (out, 24, "%8.8X:%s", handle, encode_objtype_string (objtype));
      }
      break;
    }
    case HINFO_LONG: {
      out = malloc (16);
      snprintf (out, 16, "%d", *(signed long *)itembuff);
      break;
    }
    case HINFO_STRING: {
      out = malloc (strlen (itembuff) + 1);
      strcpy (out, itembuff);
      break;
    }
    case HINFO_ULONG: {
      out = malloc (16);
      snprintf (out, 16, "%u", *(unsigned long *)itembuff);
      break;
    }
    case HINFO_THREADSTATE: {
      threadstate = *(unsigned char *)itembuff;
      out = malloc (16);
      switch (threadstate) {
        case SCH$C_CUR: { strcpy (out, "RUN"); break; }
        case SCH$C_COM: { strcpy (out, "COM"); break; }
        case SCH$C_LEF: { strcpy (out, "WEV"); break; }
#if 0
        case OZ_THREAD_STATE_INI: { strcpy (out, "INI"); break; }
        case OZ_THREAD_STATE_ZOM: { strcpy (out, "ZOM"); break; }
#endif
        default: { snprintf (out, 16, "%d", threadstate); break; }
      }
      break;
    }
    case HINFO_DATEBINDEL: {
      out = malloc (32);
      sys$asctim (32, out, &*(unsigned long long *)itembuff, 0);
      break;
    }
    case HINFO_DATEBINABS: {
      out = malloc (32);
      sys$asctim (32, out, &*(unsigned long long *)itembuff, 0);
      break;
    }
    case HINFO_LOCKMODE: {
      lockmode = *(unsigned long *)itembuff;
      out = malloc (16);
      switch (lockmode) {
        case LCK$K_NLMODE: { strcpy (out, "NL"); break; }
        case LCK$K_CRMODE: { strcpy (out, "CR"); break; }
        case LCK$K_CWMODE: { strcpy (out, "CW"); break; }
        case LCK$K_PRMODE: { strcpy (out, "PR"); break; }
        case LCK$K_PWMODE: { strcpy (out, "PW"); break; }
        case LCK$K_EXMODE: { strcpy (out, "EX"); break; }
        default: { snprintf (out, 16, "%d", lockmode); break; }
      }
      break;
    }
  }

  *((char **)valuep) = out;							/* return the substring pointer */
  return (SS$_NORMAL);								/* successful */
}

/************************************************************************/
/*									*/
/*  Determine length of a string					*/
/*									*/
/*	length = oz_len (string)					*/
/*									*/
/************************************************************************/

static unsigned long func_len (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *svalue;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, &strp, &svalue, ')');				/* evaluate the string expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  *rtnp = strp;									/* return pointer just past the ) */
  *((unsigned long *)valuep) = strlen (svalue);						/* return the length of the string */
  free (svalue);								/* free off result malloc'd by eval_string */
  return (SS$_NORMAL);								/* successful */
}

/************************************************************************/
/*									*/
/*  Get logical name's attributes					*/
/*									*/
/*	attributes = oz_lnm_attrs (h_logical_name, index)		*/
/*									*/
/************************************************************************/

static unsigned long func_lnm_attrs (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *out;
  int i;
  unsigned long index, lognamatr, logvalatr, sts;
  unsigned long h_logname;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_handle (strp, &strp, &h_logname, "logname", ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_integer (strp, rtnp, &index, ')');
  if (sts != SS$_NORMAL) return (sts);

  /* Get the attributes */

  sts = sys$trnlnm (0, h_logname,  NULL,  0,  NULL);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting logical's attributes\n", sts);
    return (sts);
  }

  /* Convert bitmask attributes to strings */

  out = malloc (64);
  out[0] = 0;
#if 0
  if (lognamatr & OZ_LOGNAMATR_NOOUTERMODE) strcat (out, "nooutermode ");
  if (lognamatr & OZ_LOGNAMATR_NOSUPERSEDE) strcat (out, "nosupersede ");
  if (logvalatr & OZ_LOGVALATR_OBJECT)      strcat (out, "object ");
#endif
  if (lognamatr & LNM$M_TABLE)       strcat (out, "table ");
  if (logvalatr & LNM$M_TERMINAL)    strcat (out, "terminal ");
  i = strlen (out);
  if ((i > 0) && (out[i-1] == ' ')) out[--i] = 0;

  *((char **)valuep) = out;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Lookup logical name							*/
/*									*/
/*	handle = oz_lnm_lookup ("logical_name", "procmode")		*/
/*									*/
/************************************************************************/

static unsigned long func_lnm_lookup (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *logname, *procmode, *out;
  unsigned long sts;
  unsigned long h_logname, h_table;
  unsigned long b_procmode;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, &strp, &logname, ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_string (strp, &strp, &procmode, ')');
  if (sts != SS$_NORMAL) {
    free (logname);
    return (sts);
  }

  b_procmode = PSL$C_USER;
  if (strcasecmp (procmode, "user") == 0) b_procmode = PSL$C_USER;
  else if (strcasecmp (procmode, "kernel") == 0) b_procmode = PSL$C_KERNEL;
  else {
    fprintf (h_s_error, "oz_cli: bad processor mode %s (kernel or user)\n", procmode);
    free (logname);
    free (procmode);
    return (SS$_BADPARAM);
  }
  *rtnp = strp;

  /* Look it up */

  sts = sys$trnlnm (0, 0,  defaulttables,  PSL$C_USER,  &h_table);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u looking up default tables (%s)\n", sts, defaulttables);
    free (logname);
    free (procmode);
    return (sts);
  }
  sts = sys$trnlnm (0, h_table,  logname,  b_procmode,  &h_logname);
  oz_sys_handle_release (PSL$C_KERNEL, h_table);
  if (sts != SS$_NORMAL) {
    out = malloc (1);
    out[0] = 0;
  } else {
    out = malloc (17);
    snprintf (out, 17, "%8.8X:logname", h_logname);
  }
  *((char **)valuep) = out;
  free (logname);
  free (procmode);
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Get logical name's number of values					*/
/*									*/
/*	number_of_values = oz_lnm_nvalues (h_logical_name)		*/
/*									*/
/************************************************************************/

static unsigned long func_lnm_nvalues (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  unsigned long sts;
  unsigned long h_logname;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_handle (strp, rtnp, &h_logname, "logname", ')');
  if (sts != SS$_NORMAL) return (sts);

  /* Get the number of values the logical name has */

  sts = sys$trnlnm (0, h_logname,  NULL,  0,  valuep);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting logical's attributes\n", sts);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Get logical name value's object handle				*/
/*									*/
/*	handle = oz_lnm_object (h_logical_name, index, "objtype")	*/
/*									*/
/************************************************************************/

static unsigned long func_lnm_object (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *out, *s_objtype;
  unsigned long index, sts;
  unsigned long h_logname, h_object;
  unsigned char b_objtype;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_handle (strp, &strp, &h_logname, "logname", ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_integer (strp, &strp, &index, ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_string (strp, &strp, &s_objtype, ')');
  if (sts != SS$_NORMAL) return (sts);
  sts = decode_objtype_string (s_objtype, &b_objtype);
  free (s_objtype);
  if (sts != SS$_NORMAL) return (sts);
  *rtnp = strp;

  /* Get an handle to the object */

  sts = sys$trnlnm (0, h_logname, index,  0,  NULL);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting logical value's handle\n", sts);
    return (sts);
  }

  /* Make handle string */

  out = malloc (24);
  out[0] = 0;
  if (h_object != 0) snprintf (out, 24, "%8.8X:%s", h_object, encode_objtype_string (b_objtype));
  *((char **)valuep) = out;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Get logical name value's string					*/
/*									*/
/*	string = oz_lnm_string (h_logical_name, index)			*/
/*									*/
/************************************************************************/

static unsigned long func_lnm_string (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *out;
  unsigned long index, rlen, sts;
  unsigned long h_logname;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;
  if (*(strp ++) != '(') {
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_handle (strp, &strp, &h_logname, "logname", ',');
  if (sts != SS$_NORMAL) return (sts);
  sts = eval_integer (strp, &strp, &index, ')');
  if (sts != SS$_NORMAL) return (sts);
  *rtnp = strp;

  /* Get string's value */

  sts = sys$trnlnm (0, h_logname, index,  0,  NULL);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting logical's value\n", sts);
    return (sts);
  }
  out = malloc (rlen + 1);
  sts = sys$trnlnm (0, h_logname, index,  rlen + 1,  NULL);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting logical's value\n", sts);
    return (sts);
  }
  out[rlen] = 0;
  *((char **)valuep) = out;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Locate 'needle' in 'haystack'					*/
/*									*/
/*	offset = oz_loc (haystack, needle)				*/
/*	length if not found						*/
/*									*/
/************************************************************************/

static unsigned long func_loc (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *haystack, *needle, *p;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, &strp, &haystack, ',');				/* evaluate the haystack string expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  sts = eval_string (strp, &strp, &needle, ')');				/* evaluate the needle string expression */
  if (sts != SS$_NORMAL) {
    free (haystack);
    return (sts);
  }
  *rtnp = strp;									/* return pointer just past the ) */
  p = strstr (haystack, needle);						/* do it */
  if (p != NULL) *((unsigned long *)valuep) = p - haystack;				/* return offset if found */
  else *((unsigned long *)valuep) = strlen (haystack);					/* return length if not found */
  free (haystack);								/* free off results malloc'd by eval_string */
  free (needle);
  return (SS$_NORMAL);								/* successful */
}

/************************************************************************/
/*									*/
/*  Convert input string to lower case					*/
/*									*/
/*	lowercase = oz_lowercase (string)				*/
/*									*/
/************************************************************************/

static unsigned long func_lowercase (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char c, *p, *string;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;			/* skip spaces */
  if (*(strp ++) != '(') {						/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, rtnp, &string, ')');				/* evaluate the string expression */
  if (sts == SS$_NORMAL) {
    for (p = string; (c = *p) != 0; p ++) if ((c >= 'A') && (c <= 'Z')) *p = c + 'a' - 'A';
    *((char **)valuep) = string;
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Find a process by its id number					*/
/*									*/
/*	h_process = oz_process (process_id)				*/
/*									*/
/************************************************************************/

static unsigned long func_process (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *out;
  unsigned long h_process;
  unsigned long processid;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_integer (strp, &strp, &processid, ')');				/* evaluate the processid expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */

  sts = oz_sys_process_getbyid (processid, &h_process);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting process-id %u handle\n", sts, processid);
    return (sts);
  }

  *rtnp = strp;

  /* Make handle string */

  out = malloc (24);
  snprintf (out, 24, "%8.8X:%s", h_process, encode_objtype_string (OZ_OBJTYPE_PROCESS));
  *((char **)valuep) = out;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Extract a substring							*/
/*									*/
/*	substring = oz_sub (length, offset, string)			*/
/*									*/
/************************************************************************/

static unsigned long func_sub (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *string, *substring;
  unsigned long length, offset, sts, totalen;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_integer (strp, &strp, &length, ',');				/* evaluate the length expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  sts = eval_integer (strp, &strp, &offset, ',');				/* evaluate the offset expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  sts = eval_string (strp, &strp, &string, ')');				/* evaluate the string expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  *rtnp = strp;									/* return pointer just past the ) */
  totalen = strlen (string);							/* get total length of the original string */
  if (length > totalen - offset) length = totalen - offset;			/* chop off length at end of string */
  if (offset > totalen) offset = totalen;					/* chop off offset at end of string */
  substring = malloc (length + 1);						/* malloc a substring buffer */
  memcpy (substring, string + offset, length);					/* copy in the substring */
  substring[length] = 0;							/* null terminate it */
  free (string);								/* free off the string buffer */
  *((char **)valuep) = substring;						/* return the substring pointer */
  return (SS$_NORMAL);								/* successful */
}

/************************************************************************/
/*									*/
/*  Find a thread by its id number					*/
/*									*/
/*	h_thread = oz_thread (thread_id)				*/
/*									*/
/************************************************************************/

static unsigned long func_thread (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char *out;
  unsigned long h_thread;
  unsigned long threadid;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_integer (strp, &strp, &threadid, ')');				/* evaluate the threadid expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */

  sts = oz_sys_thread_getbyid (threadid, &h_thread);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u getting thread-id %u handle\n", sts, threadid);
    return (sts);
  }

  *rtnp = strp;

  /* Make handle string */

  out = malloc (24);
  snprintf (out, 24, "%8.8X:%s", h_thread, encode_objtype_string (OZ_OBJTYPE_THREAD));
  *((char **)valuep) = out;
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*  Trim leading/trailing whitespace					*/
/*									*/
/*	trimmed = oz_trim (string)					*/
/*									*/
/************************************************************************/

static unsigned long func_trim (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char c, *p, *q, *string;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;			/* skip spaces */
  if (*(strp ++) != '(') {						/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, rtnp, &string, ')');				/* evaluate the string expression */
  if (sts == SS$_NORMAL) {
    for (p = string; (c = *p) != 0; p ++) if (c > ' ') break;		/* trim leading whitespace */
    for (q = p + strlen (p); q > p; -- q) if (q[-1] > ' ') break;	/* trim trailing whitespace */
    memmove (string, p, q - p);						/* shift result up to beginning of buffer */
    string[q-p] = 0;							/* null terminate string */
    *((char **)valuep) = string;					/* return pointer to string buffer */
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Convert input string to upper case					*/
/*									*/
/*	uppercase = oz_uppercase (string)				*/
/*									*/
/************************************************************************/

static unsigned long func_uppercase (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  char c, *p, *string;
  unsigned long sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;			/* skip spaces */
  if (*(strp ++) != '(') {						/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_string (strp, rtnp, &string, ')');				/* evaluate the string expression */
  if (sts == SS$_NORMAL) {
    for (p = string; (c = *p) != 0; p ++) if ((c >= 'a') && (c <= 'z')) *p = c + 'A' - 'a';
    *((char **)valuep) = string;
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Set new verify setting						*/
/*									*/
/*	oldverify = oz_verify (newverify)				*/
/*									*/
/************************************************************************/

static unsigned long func_verify (Symbol *symbol, char *strp, char **rtnp, void *valuep)

{
  unsigned long newverify, sts;

  while ((*strp != 0) && (*strp <= ' ')) strp ++;				/* skip spaces */
  if (*(strp ++) != '(') {							/* make sure it starts with an open paren and skip it */
    fprintf (h_s_error, "oz_cli: missing ( following %s at %s\n", symbol -> name, -- strp);
    return (SS$_IVPARAM);
  }
  sts = eval_integer (strp, &strp, &newverify, ')');				/* evaluate the length expression */
  if (sts != SS$_NORMAL) return (sts);						/* return error status if any failure */
  *rtnp = strp;

  *((unsigned long *)valuep) = verify;							/* return old verify value */

  if (((signed long)newverify) >= 0) verify = newverify;				/* maybe set new verify value */
  return (SS$_NORMAL);								/* successful */
}

static struct { unsigned char b; const char *s; } objtyptbl[] = { 
	OZ_OBJTYPE_UNKNOWN, "unknown", 
	OZ_OBJTYPE_EVENT,   "event", 
	OZ_OBJTYPE_DEVUNIT, "devunit", 
	OZ_OBJTYPE_IOCHAN,  "iochan", 
	OZ_OBJTYPE_JOB,     "job", 
	OZ_OBJTYPE_LOGNAME, "logname", 
	OZ_OBJTYPE_PROCESS, "process", 
	OZ_OBJTYPE_THREAD,  "thread", 
	OZ_OBJTYPE_USER,    "user", 
	0, NULL };

static unsigned long decode_objtype_string (const char *s_objtype, unsigned char *b_objtype_r)

{
  int i;

  for (i = 0; objtyptbl[i].s != NULL; i ++) {
    if (strcasecmp (s_objtype, objtyptbl[i].s) == 0) {
      *b_objtype_r = objtyptbl[i].b;
      return (SS$_NORMAL);
    }
  }
  fprintf (h_s_error, "oz_cli: unknown object type %s\n", s_objtype);
  return (SS$_BADPARAM);
}

static const char *encode_objtype_string (unsigned char b_objtype)

{
  int i;

  for (i = 0; objtyptbl[i].s != NULL; i ++) {
    if (objtyptbl[i].b == b_objtype) return (objtyptbl[i].s);
  }
  fprintf (h_s_error, "oz_cli: unknown object type %d\n", b_objtype);
  return ("");
}

/************************************************************************/
/*									*/
/*  Define a function symbol						*/
/*									*/
/*    Input:								*/
/*									*/
/*	name = name string of the function				*/
/*	func = entrypoint to process the function			*/
/*	symtype = symbol type of function return value			*/
/*	help = help string (copied to svalue)				*/
/*									*/
/************************************************************************/

static void def_func (char *name, unsigned long (*func) (Symbol *symbol, char *strp, char **rtnp, void *valuep), Symtype symtype, char *help)

{
  Symbol *symbol;

  symbol = malloc (sizeof *symbol + strlen (help) + 1);
  strcpy (symbol -> name, name);
  symbol -> symtype = symtype;
  strcpy (symbol -> svalue, help);
  insert_symbol (symbol, func, 0);
}

/************************************************************************/
/*									*/
/*  Set symbols passed to newly started script				*/
/*									*/
/*    Input:								*/
/*									*/
/*	h_s_input, _output, _error = its handles			*/
/*	argc = count of arguments					*/
/*	argv = vector of arguments					*/
/*									*/
/************************************************************************/

static void setscriptsyms (int argc, const char *argv[])

{
  int i;
  Symbol *symbol;

  static char      *names[3] = { "oz_h_input", "oz_h_output", "oz_h_error" };
  static unsigned long *hands[3] = {  &h_s_input,   &h_s_output,   &h_s_error  };

  /* Define handle symbols for the input, output and error files */

  for (i = 0; i < 3; i ++) {
    symbol = malloc (16 + sizeof *symbol);
    strcpy (symbol -> name, names[i]);
    symbol -> symtype = SYMTYPE_STRING;
    snprintf (symbol -> svalue, 32, "%8.8X:iochan", *(hands[i]));
    insert_symbol (symbol, NULL, 0);
  }

  /* Define oz_nargs symbol denoting the number of arguments present */

  symbol = malloc (sizeof *symbol);
  strcpy (symbol -> name, "oz_nargs");
  symbol -> symtype = SYMTYPE_INTEGER;
  symbol -> ivalue  = argc;
  insert_symbol (symbol, NULL, 0);

  /* Define oz_arg_<n> for each argument present */

  for (i = 0; i < argc; i ++) {
    symbol = malloc (strlen (argv[i]) + 1 + sizeof *symbol);
    strcpy (symbol -> name, "oz_arg_");
    oz_hw_itoa (i, sizeof symbol -> name - 7, symbol -> name + 7);
    symbol -> symtype = SYMTYPE_STRING;
    strcpy (symbol -> svalue, argv[i]);
    insert_symbol (symbol, NULL, 0);
  }
}

/************************************************************************/
/*									*/
/*  Insert symbol into symbol list, replacing any identically named symbol
/*									*/
/*    Input:								*/
/*									*/
/*	symbol = filled in						*/
/*	func = fill into symbol -> func					*/
/*	level = 0 : the current local symbol level			*/
/*	        1 : the next outer symbol level				*/
/*	       -1 : global						*/
/*									*/
/************************************************************************/

static void insert_symbol (Symbol *symbol, unsigned long (*func) (Symbol *symbol, char *strp, char **rtnp, void *valuep), unsigned long level)

{
  int i;
  Script *script;
  Symbol **lsymbol, *xsymbol;

  symbol -> func = func;

  lsymbol = &symbols;							/* use current symbols if level 0 */
  if (level != 0) {
    for (script = scripts; script != NULL; script = script -> next) {	/* link through outer script levels */
      lsymbol = &(script -> symbols);
      if (-- level == 0) break;
    }
  }

  for (; (xsymbol = *lsymbol) != NULL; lsymbol = &(xsymbol -> next)) {	/* scan through list of existing symbols */
    i = strcmp (xsymbol -> name, symbol -> name);			/* compare list symbol to symbol to be inserted */
    if (i == 0) {
      *lsymbol = xsymbol -> next;					/* exact match, remove old symbol from list */
      free (xsymbol);
    }
    if (i >= 0) break;							/* stop if list symbol name >= name to be inserted */
  }
  symbol -> next = *lsymbol;						/* link new symbol here */
  *lsymbol = symbol;
}

/************************************************************************/
/*									*/
/*  Lookup symbol							*/
/*									*/
/*    Input:								*/
/*									*/
/*	name = symbol name to lookup					*/
/*	level = 0 : start looking in current level			*/
/*	        1 : start looking in next outer level			*/
/*	       -1 : look only in global level				*/
/*									*/
/*    Output:								*/
/*									*/
/*	lookup_symbol = NULL : symbol not found				*/
/*	                else : pointer to symbol			*/
/*	*level_r = level it was found in				*/
/*									*/
/************************************************************************/

static Symbol *lookup_symbol (const char *name, unsigned long level, unsigned long *level_r)

{
  unsigned long levr;
  Script *script;
  Symbol *symbol;

  symbol = symbols;							/* start with current symbols if level 0 */
  script = NULL;
  levr = 0;

  if (level != 0) {
    for (script = scripts; script != NULL; script = script -> next) {	/* link through outer script levels */
      levr ++;
      symbol = script -> symbols;
      if (-- level == 0) break;
    }
  }

  while (1) {
    for (; symbol != NULL; symbol = symbol -> next) {			/* search that table for the symbol */
      if (strcmp (name, symbol -> name) == 0) {
        if (level_r != NULL) *level_r = levr;				/* found, return the level number */
        return (symbol);						/* return the pointer */
      }
    }
    if (script == NULL) script = scripts;				/* not in that table, point to next outer level */
    else script = script -> next;
    if (script == NULL) return (NULL);					/* no more levels, it's not found */
    levr ++;
    symbol = script -> symbols;
  }
}

/************************************************************************/
/*									*/
/*  Decode keyword from command table					*/
/*									*/
/*    Input:								*/
/*									*/
/*	name = command string to decode					*/
/*	cmdtbl = table to decode it from				*/
/*									*/
/*    Output:								*/
/*									*/
/*	decode_command = NULL : keyword not found			*/
/*	                 else : pointer to entry in keyword table	*/
/*									*/
/************************************************************************/

static Command *decode_command (int argc, const char **argv, Command *cmdtbl, int *argc_r)

{
  char *p;
  Command *cmd1;
  int i, j, len1;

  cmd1 = NULL;					/* no entry matched so far */
  len1 = 0;					/* length of last matched entry */

  for (i = 0; (p = cmdtbl[i].name) != NULL; i ++) { /* loop through table */
    j = cmpcmdname (argc, argv, p);		/* compare the name */
    if (j > len1) {				/* see if better match than last */
      cmd1 = cmdtbl + i;			/* ok, save table pointer */
      len1 = j;					/* save number of words used */
      *argc_r = j;
    }
  }

  return (cmd1);				/* return pointer to best match entry (or NULL) */
}

/************************************************************************/
/*									*/
/*  Compare argc/argv to a multiple-word command name string		*/
/*									*/
/*    Input:								*/
/*									*/
/*	argc = number of entries in the argv array			*/
/*	argv = pointer to array of char string pointers			*/
/*	name = multiple-word command name string			*/
/*	       each word must be separated by exactly one space		*/
/*									*/
/*    Output:								*/
/*									*/
/*	cmpcmdname = 0 : doesn't match					*/
/*	          else : number of elements of argv used up		*/
/*									*/
/************************************************************************/

static int cmpcmdname (int argc, const char **argv, char *name)

{
  char *p;
  int j, l;

  p = name;					/* point to multiple-word command name string */
  for (j = 0; j < argc;) {			/* compare to given command words */
    l = strlen (argv[j]);			/* get length of command word */
    if (strncasecmp (argv[j], p, l) != 0) return (0); /* compare */
    p += l;					/* word matches, point to next in multiple-word name */
    j ++;					/* increment to next argv */
#if 0
    if (*(p ++) != ' ') {			/* see if more words in multiple-word string */
      if (*(-- p) != 0) return (0);		/* if not, fail if not an exact end */
      return (j);				/* success, return number of words */
    }
#else
    if (j<argc) {
      while(*p!=' ' && (p<(name+strlen(name)))) {
	p++;
      }
      if (*p==0) return j;
      p++;
    } else {
      return (j);
    }
#endif
  }
  return (0);					/* argv too short to match multiple-word name */
}

/************************************************************************/
/*									*/
/*  Get object associated with logical name				*/
/*									*/
/*    Input:								*/
/*									*/
/*	name = logical name (in form [table%]name)			*/
/*	objtype = object's type						*/
/*									*/
/*    Output:								*/
/*									*/
/*	logname_getobj = SS$_NORMAL : successfully retrieved		*/
/*	                       else : error status			*/
/*	*h_object_r = handle to object					*/
/*									*/
/************************************************************************/

static unsigned long logname_getobj (const char *name, unsigned char objtype, unsigned long *h_object_r)

{
  unsigned long sts;
  unsigned long h_logname, h_table;

  sts = sys$trnlnm (0, 0,  defaulttables,  PSL$C_USER,  &h_table);		/* look up table in default directories */
  if (sts != SS$_NORMAL) fprintf (h_s_error, "oz_cli: error %u looking up default tables (%s)\n", sts, defaulttables);
  else {
    sts = sys$trnlnm (0, h_table,  name,  PSL$C_USER,  &h_logname);		/* look up the logical name in the table */
    oz_sys_handle_release (PSL$C_KERNEL, h_table);							/* release table's handle */
    if (sts == SS$_NORMAL) {
      sts = sys$trnlnm (0, h_logname, 0,  NULL,  objtype);	/* get handle to object the logical points to */
      oz_sys_handle_release (PSL$C_KERNEL, h_logname);						/* no longer need the logical */
    }
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Create logical name to point to an object				*/
/*									*/
/*    Input:								*/
/*									*/
/*	name = logical name (in form [table%]name)			*/
/*	objtype = object's type						*/
/*	h_object = handle to object					*/
/*									*/
/*    Output:								*/
/*									*/
/*	logname_creobj = SS$_NORMAL : successfully created		*/
/*	                       else : error status			*/
/*									*/
/************************************************************************/

static unsigned long logname_creobj (const char *name, unsigned char objtype, unsigned long h_object)

{
  unsigned long sts;
  unsigned long h_table;
  OZ_Logvalue logvalue;

#if 0
  logvalue.attr = OZ_LOGVALATR_OBJECT;
#endif
  logvalue.buff = (void *)h_object;

  sts = sys$trnlnm (0, 0,  defaulttables,  PSL$C_USER,  &h_table);	/* look up table in default directories */
  if (sts == SS$_NORMAL) {
    sts = sys$crelnm (0, h_table,  PSL$C_USER,  name,  NULL);	/* create the logical name in the table */
    oz_sys_handle_release (PSL$C_KERNEL, h_table);						/* release table's handle */
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*	abort thread [<logical_name>]					*/
/*									*/
/*		-nowait							*/
/*		-status <status>					*/
/*									*/
/************************************************************************/

static unsigned long int_abort_thread (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i, usedup, wait;
  unsigned long exitst, sts;
  unsigned long h_thread;
  unsigned long thread_id;

  exitst       = SS$_ABORT;
  logical_name = LASTHREADLNM;
  thread_id    = 0;
  wait         = 1;

  for (i = 0; i < argc; i ++) {

    /* Id - thread id number */

    if (strcmp (argv[i], "-id") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing thread id number after -id\n");
        return (SS$_IVPARAM);
      }
      thread_id = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad thread id number %s after -id\n", argv[i]);
        return (SS$_BADPARAM);
      }
      logical_name = NULL;
      continue;
    }

    /* Nowait - just return after queuing the abort request */

    if (strcmp (argv[i], "-nowait") == 0) {
      wait = 0;
      continue;
    }

    /* Status - specify the status it will exit with */

    if (strcmp (argv[i], "-status") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing status value after -status\n");
        return (SS$_IVPARAM);
      }
      exitst = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad status value %s after -status\n", argv[i]);
        return (SS$_BADPARAM);
      }
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    logical_name = argv[i];
    thread_id    = 0;
  }

  /* Get handle to thread from logical name or id number */

  if (thread_id != 0) {
    sts = oz_sys_thread_getbyid (thread_id, &h_thread);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting handle to thread id %u\n", sts, thread_id);
      return (sts);
    }
  } else {
    sts = logname_getobj (logical_name, OZ_OBJTYPE_THREAD, &h_thread);
    if (sts != SS$_NORMAL) return (sts);
  }

  /* Abort the thread */

  sts = oz_sys_thread_abort (h_thread, exitst);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u aborting thread\n", sts);
  }

  /* If didn't specify -nowait, wait for the thread to complete (or control-Y) */

  else if (wait) sts = wait_thread (h_error, h_thread);

  /* Release the handle */

  oz_sys_handle_release (PSL$C_KERNEL, h_thread);

  /* Anyway, return composite status */

  return (sts);
}

/************************************************************************/
/*									*/
/*	allocate device <devicename>					*/
/*		-user, -job, -process, -thread				*/
/*									*/
/************************************************************************/

static unsigned long int_allocate_device (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *devname;
  int i;
  unsigned long sts;
  unsigned char objtype;

  devname = NULL;
  objtype = OZ_OBJTYPE_JOB;

  for (i = 0; i < argc; i ++) {

    /* -user = allocate to the user */

    if (strcmp (argv[i], "-user") == 0) {
      objtype = OZ_OBJTYPE_USER;
      continue;
    }

    /* -job = allocate to the job */

    if (strcmp (argv[i], "-job") == 0) {
      objtype = OZ_OBJTYPE_JOB;
      continue;
    }

    /* -process = allocate to the process */

    if (strcmp (argv[i], "-process") == 0) {
      objtype = OZ_OBJTYPE_PROCESS;
      continue;
    }

    /* -thread = allocate to the thread */

    if (strcmp (argv[i], "-thread") == 0) {
      objtype = OZ_OBJTYPE_THREAD;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the device name */

    if (devname != NULL) {
      fprintf (h_error, "oz_cli: can only have one device name\n");
      return (SS$_IVPARAM);
    }
    devname = argv[i];
  }

  if (devname == NULL) {
    fprintf (h_error, "oz_cli: missing device name\n");
    return (SS$_IVPARAM);
  }

  sts = oz_sys_io_alloc (devname, 0, objtype);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u allocating %s\n", sts, devname);
  return (sts);
}

/************************************************************************/
/*									*/
/*	change password [<old_password> [<new_password>]]		*/
/*									*/
/************************************************************************/

static unsigned long readpromptne (const char *prompt, int bufsiz, char *buffer);

static unsigned long int_change_password (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char againbuf[OZ_PASSWORD_MAX], newpwbuf[OZ_PASSWORD_MAX], oldpwbuf[OZ_PASSWORD_MAX];
  const char *newpw, *oldpw;
  unsigned long sts;

  if (argc > 2) {
    fprintf (h_error, "oz_cli: change password [<old_password> [<new_password>]]\n");
    return (SS$_BADPARAM);
  }

  oldpw = argv[0];
  if (argc < 1) {
    sts = readpromptne ("Old password: ", sizeof oldpwbuf, oldpwbuf);
    if (sts != SS$_NORMAL) return (sts);
    oldpw = oldpwbuf;
  }

  newpw = argv[1];
  if (argc < 2) {
getagain:
    sts = readpromptne ("New password: ", sizeof newpwbuf, newpwbuf);
    if (sts != SS$_NORMAL) return (sts);
    sts = readpromptne ("New pw again: ", sizeof againbuf, againbuf);
    if (sts != SS$_NORMAL) return (sts);
    if (strcmp (newpwbuf, againbuf) != 0) {
      fprintf (h_error, "oz_cli: doesn't match\n");
      goto getagain;
    }
    newpw = newpwbuf;
  }

  sts = oz_sys_password_change (oldpw, newpw);
  if (sts == SS$_NORMAL) fprintf (h_error, "oz_cli: password changed\n");
  else fprintf (h_error, "oz_cli: error %u changing password\n", sts);
  return (sts);
}

/* Read with prompt but/and no echo */

static unsigned long readpromptne (const char *prompt, int bufsiz, char *buffer)

{
  OZ_IO_console_read console_read;
  OZ_IO_console_write console_write;
  unsigned long buflen, sts;

  memset (&console_read, 0, sizeof console_read);
  console_read.size    = bufsiz - 1;
  console_read.buff    = buffer;
  console_read.rlen    = &buflen;
  console_read.pmtsize = strlen (prompt);
  console_read.pmtbuff = prompt;
  console_read.noecho  = 1;
  sts = oz_sys_io (PSL$C_KERNEL, h_s_console, 0, IO$_READLBLK, sizeof console_read, &console_read);
  if (sts == SS$_NORMAL) buffer[buflen] = 0;
  memset (&console_write, 0, sizeof console_write);
  console_write.size = 1;
  console_write.buff = "\n";
  oz_sys_io (PSL$C_KERNEL, h_s_console, 0, IO$_WRITELBLK, sizeof console_write, &console_write);
  return (sts);
}

/************************************************************************/
/*									*/
/*	close handle <handle> ...					*/
/*									*/
/************************************************************************/

static unsigned long int_close_handle (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, usedup;
  unsigned long sts;
  unsigned long h_object;
  unsigned char objtype;
  Script *script;

  for (i = 0; i < argc; i ++) {
    h_object = oz_hw_atoz (argv[i], &usedup);
    if (argv[i][usedup++] != ':') goto bad_handle;
    if (decode_objtype_string (argv[i] + usedup, &objtype) != SS$_NORMAL) goto bad_handle;
    if (objtype == OZ_OBJTYPE_IOCHAN) {
      if (h_object == h_s_input)  goto script_handle;
      if (h_object == h_s_output) goto script_handle;
      if (h_object == h_s_error)  goto script_handle;
      for (script = scripts; script != NULL; script = script -> next) {
        if (h_object == script -> h_input)  goto script_handle;
        if (h_object == script -> h_output) goto script_handle;
        if (h_object == script -> h_error)  goto script_handle;
      }
    }
    sts = oz_sys_handle_release (PSL$C_KERNEL, h_object);
    if (sts != SS$_NORMAL) {
       fprintf (h_error, "oz_cli: error %u closing handle %s\n", sts, argv[i]);
       return (sts);
    }
  }

  return (SS$_NORMAL);

bad_handle:
  fprintf (h_error, "oz_cli: bad handle %s\n", argv[i]);
  return (SS$_BADPARAM);

script_handle:
  fprintf (h_error, "oz_cli: script handle %s\n", argv[i]);
  return (SS$_BADPARAM);
}

/************************************************************************/
/*									*/
/*	create event <logical_name> <event_name>			*/
/*									*/
/************************************************************************/

static unsigned long int_create_event (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *event_name, *logical_name;
  unsigned long sts;
  unsigned long h_event;

  if (argc != 2) {
    fprintf (h_error, "oz_cli: create job <logical_name> <job_name>\n");
    return (SS$_IVPARAM);
  }

  logical_name = argv[0];
  event_name   = argv[1];

  /* Create event flag and assign logical to it */

  sts = oz_sys_event_create (PSL$C_KERNEL, event_name, &h_event);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u creating event flag %s\n", sts, event_name);
  else {
    sts = logname_creobj (logical_name, OZ_OBJTYPE_EVENT, h_event);
    if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u assigning logical name %s to event flag\n", sts, logical_name);
    oz_sys_handle_release (PSL$C_KERNEL, h_event);
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	create file <file_name>						*/
/*									*/
/*		-lockmode <lock_mode>					*/
/*		-logical <logical_name> 				*/
/*									*/
/************************************************************************/

static unsigned long int_create_file (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i, j;
  unsigned long sts;
  unsigned long h_iochan;
#if 0
  OZ_IO_fs_create fs_create;

  memset (&fs_create, 0, sizeof fs_create);
  fs_create.lockmode = LCK$K_EXMODE;
  fs_create.ignclose = 1;
  logical_name = NULL;

  for (i = 0; i < argc; i ++) {

    /* Lockmode - specify the lock mode to open it with */

    if (strcmp (argv[i], "-lockmode") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing lock mode after -lockmode\n");
        return (SS$_IVPARAM);
      }
      for (j = 0; j < (LCK$K_EXMODE+1); j ++) if (strcasecmp (lockmodes[j].name, argv[i]) == 0) break;
      if (j == (LCK$K_EXMODE+1)) {
        fprintf (h_error, "oz_cli: bad lock mode %s\n", argv[i]);
        return (SS$_BADPARAM);
      }
      fs_create.lockmode = lockmodes[j].valu;
      continue;
    }

    /* Logical - specify the logical name to assign to it */

    if (strcmp (argv[i], "-logical") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing logical name after -logical\n");
        return (SS$_IVPARAM);
      }
      logical_name = argv[i];
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* Not an option, it is the filename */

    fs_create.name = argv[i];
  }

  /* Make sure we got a file name in there somewhere */

  if (fs_create.name == NULL) {
    fprintf (h_error, "oz_cli: missing file name\n");
    return (SS$_IVPARAM);
  }

  /* Create file and assign logical name to the channel                          */
  /* If no logical is given, the file will be closed when the handle is released */

  sts = oz_sys_io_fs_create (sizeof fs_create, &fs_create, 0, &h_iochan);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u creating file %s\n", sts, fs_create.name);
  else {
    if (logical_name != NULL) {
      sts = logname_creobj (logical_name, OZ_OBJTYPE_IOCHAN, h_iochan);
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u assigning logical %s to file %s\n", sts, logical_name, fs_create.name);
    }
    oz_sys_handle_release (PSL$C_KERNEL, h_iochan);
  }
#endif
  return (sts);
}

/************************************************************************/
/*									*/
/*	create job <logical_name> <job_name>				*/
/*									*/
/************************************************************************/

static unsigned long int_create_job (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *jobname, *logname;
  unsigned long sts;
  unsigned long h_job;

  if (argc != 2) {
    fprintf (h_error, "oz_cli: create job <logical_name> <job_name>\n");
    return (SS$_IVPARAM);
  }

  logname = argv[0];
  jobname = argv[1];

  /* Create job and assign logical to it */

  sts = oz_sys_job_create (jobname, &h_job);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u creating job %s\n", sts, jobname);
  else {
    sts = logname_creobj (logname, OZ_OBJTYPE_JOB, h_job);
    if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u assigning logical name %s to job\n", sts, logname);
    oz_sys_handle_release (PSL$C_KERNEL, h_job);
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	create logical name <logical_name> <value0> <value1> ...	*/
/*									*/
/************************************************************************/

typedef struct { unsigned long h_table;
                 const char *logical_name;
                 unsigned long lognamatr;
                 unsigned long nvalues;
                 OZ_Logvalue *values;
               } Crelognampar;

typedef struct Handleclose { struct Handleclose *next;
                             unsigned long handle;
                           } Handleclose;

static unsigned long crelognam (unsigned long cprocmode, void *crelognamparv);

static unsigned long int_create_logical_name (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char buff[OZ_LOGNAME_MAXNAMSZ];
  Crelognampar crelognampar;
  Handleclose *handleclose, *handlecloses;
  int i, kernel, usedup;
  unsigned long j, logvalatr, nvalues, rlen, sts;
  unsigned long h_logname, h_object;
  OZ_Logvalue *newvals;
  unsigned char objtype;

  crelognampar.logical_name = NULL;
  crelognampar.lognamatr = 0;
  crelognampar.nvalues = 0;
  crelognampar.values = malloc (argc * sizeof *crelognampar.values);
  crelognampar.values[0].attr = 0;
  handlecloses = NULL;
  kernel = 0;

  /* Find the table to put the logical in */

  sts = sys$trnlnm (0, 0,  defaulttables,  kernel ? PSL$C_KERNEL : PSL$C_USER,  &crelognampar.h_table);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u looking up %s\n", sts, defaulttables);
    return (sts);
  }

  /* Process command line args */

  for (i = 0; i < argc; i ++) {

    /* Copy - copy the values of the logical name given as the next argument */

    if (strcmp (argv[i], "-copy") == 0) {

      /* -copy is followed by the logical name to be copied */

      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing logical name after -copy\n");
        sts = SS$_IVPARAM;
        goto rtn;
      }

      /* Lookup that logical name */

      sts = sys$trnlnm (0, crelognampar.h_table,  argv[i],  kernel ? PSL$C_KERNEL : PSL$C_USER,  &h_logname);
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u looking up logical %s for -copy\n", sts, argv[i]);
        goto rtn;
      }

      /* If it has more than one value, expand the values array to accomodate all possibilities */

      if (nvalues > 1) {
        newvals = malloc ((crelognampar.nvalues + nvalues + argc - i) * sizeof *crelognampar.values);
        memcpy (newvals, crelognampar.values, crelognampar.nvalues * sizeof *newvals);
        free (crelognampar.values);
        crelognampar.values = newvals;
      }

      /* Copy each value to the values array */

      for (j = 0; j < nvalues; j ++) {
        sts = sys$trnlnm (0, h_logname, j,  sizeof buff,  &logvalatr);
        if (sts != SS$_NORMAL) {
          fprintf (h_error, "oz_cli: error %u reading logical %s value %u for -copy\n", sts, argv[i], j);
          oz_sys_handle_release (PSL$C_KERNEL, h_logname);
          return (sts);
        }
        crelognampar.values[crelognampar.nvalues+j].attr = logvalatr;
        crelognampar.values[crelognampar.nvalues+j].buff = (void *)h_object;
        if (logvalatr & 1/*OZ_LOGVALATR_OBJECT*/) {
          handleclose = malloc (sizeof *handleclose);
          handleclose -> next   = handlecloses;
          handleclose -> handle = h_object;
          handlecloses = handleclose;
        } else {
          crelognampar.values[crelognampar.nvalues+j].buff = malloc (rlen + 1);
          memcpy (crelognampar.values[crelognampar.nvalues+j].buff, buff, rlen + 1);
        }
      }
      crelognampar.nvalues += j;

      /* Release source logical and continue processing command line */

      oz_sys_handle_release (PSL$C_KERNEL, h_logname);
      crelognampar.values[crelognampar.nvalues].attr = 0;
      continue;
    }

    /* Kernel - create logical in kernel mode */

    if (strcmp (argv[i], "-kernel") == 0) {
      kernel = 1;
      continue;
    }

    /* Link - link to the logical name given as the next argument */

    if (strcmp (argv[i], "-link") == 0) {

      /* -link is followed by the logical name to be copied */

      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing logical name after -link\n");
        sts = SS$_IVPARAM;
        goto rtn;
      }

      /* Lookup that logical name */

      sts = sys$trnlnm (0, crelognampar.h_table,  argv[i],  kernel ? PSL$C_KERNEL : PSL$C_USER,  &h_logname);
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u looking up logical %s for -link\n", sts, argv[i]);
        goto rtn;
      }
      handleclose = malloc (sizeof *handleclose);
      handleclose -> next   = handlecloses;
      handleclose -> handle = h_logname;
      handlecloses = handleclose;

      /* Set up value that points to that logical as an object */

      crelognampar.values[crelognampar.nvalues].attr |= 1;//OZ_LOGVALATR_OBJECT;
      crelognampar.values[crelognampar.nvalues++].buff = (void *)h_logname;
      crelognampar.values[crelognampar.nvalues].attr = 0;
      continue;
    }

    /* Nooutermode - do not allow outer mode versions of logical */

    if (strcmp (argv[i], "-nooutermode") == 0) {
      crelognampar.lognamatr |= 0; //OZ_LOGNAMATR_NOOUTERMODE;
      continue;
    }

    /* Nosupersede - logical is not allowed to be overwritten, must be deassigned first */

    if (strcmp (argv[i], "-nosupersede") == 0) {
      crelognampar.lognamatr |= 2;//OZ_LOGNAMATR_NOSUPERSEDE;
      continue;
    }

    /* Object - the next arg is in internal handle format */

    if (strcmp (argv[i], "-object") == 0) {

      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing handle after -object\n");
        sts = SS$_IVPARAM;
        goto rtn;
      }

      h_object = oz_hw_atoz (argv[i], &usedup);
      if ((argv[i][usedup] != ':') || (decode_objtype_string (argv[i] + usedup + 1, &objtype) != SS$_NORMAL)) {
        fprintf (h_error, "oz_cli: bad handle %s\n", argv[i]);
        sts = SS$_BADPARAM;
        goto rtn;
      }

      crelognampar.values[crelognampar.nvalues].attr |= 1;//OZ_LOGVALATR_OBJECT;
      crelognampar.values[crelognampar.nvalues++].buff = (void *)h_object;
      crelognampar.values[crelognampar.nvalues].attr = 0;
      continue;
    }

    /* Terminal - the next value specified should not be recursively translated */

    if (strcmp (argv[i], "-terminal") == 0) {
      crelognampar.values[crelognampar.nvalues].attr |= LNM$M_TERMINAL;
      continue;
    }

    /* Value - next arg is a value (maybe beginning with an hyphen) */

    if (strcmp (argv[i], "-value") == 0) {
      if (++ i >= argc) {
        fprintf (h_error, "oz_cli: missing value string after -value\n");
        sts = SS$_IVPARAM;
        goto rtn;
      }
      rlen = strlen (argv[i]);
      crelognampar.values[crelognampar.nvalues].buff = malloc (rlen + 1);
      memcpy (crelognampar.values[crelognampar.nvalues++].buff, argv[i], rlen + 1);
      crelognampar.values[crelognampar.nvalues].attr = 0;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      sts = SS$_IVPARAM;
      goto rtn;
    }

    /* No option - this is the logical name or a value */

    if (crelognampar.logical_name == NULL) {
      crelognampar.logical_name = argv[i];
    } else {
      rlen = strlen (argv[i]);
      crelognampar.values[crelognampar.nvalues].buff = malloc (rlen + 1);
      memcpy (crelognampar.values[crelognampar.nvalues++].buff, argv[i], rlen + 1);
      crelognampar.values[crelognampar.nvalues].attr = 0;
    }
  }

  /* Make sure we got a logical name in there somewhere */

  if (crelognampar.logical_name == NULL) {
    fprintf (h_error, "oz_cli: missing logical name\n");
    sts = SS$_IVPARAM;
    goto rtn;
  }

  /* Create the logical name */

#if 0
  oz_sys_printkp (0, "  crelognampar.h_table      %x\n", crelognampar.h_table);
  oz_sys_printkp (0, "  crelognampar.logical_name %p '%s'\n", crelognampar.logical_name, crelognampar.logical_name);
  oz_sys_printkp (0, "  crelognampar.lognamatr    %x\n", crelognampar.lognamatr);
  oz_sys_printkp (0, "  crelognampar.nvalues      %u\n", crelognampar.nvalues);
  oz_sys_printkp (0, "  crelognampar.values       %p\n", crelognampar.values);
  for (j = 0; j < crelognampar.nvalues; j ++) {
    oz_sys_printkp (0, "  crelognampar.values[%u].attr %x\n", j, crelognampar.values[j].attr);
    if (crelognampar.values[j].attr & 1/*OZ_LOGVALATR_OBJECT*/) oz_sys_printkp (0, "  crelognampar.values[%u].buff %x\n", j, (unsigned long)crelognampar.values[j].buff);
    else oz_sys_printkp (0, "  crelognampar.values[%u].buff %p '%s'\n", j, crelognampar.values[j].buff, crelognampar.values[j].buff);
  }
#endif

  if (!kernel) sts = crelognam (PSL$C_USER, &crelognampar);
  else sts = oz_sys_callknl (crelognam, &crelognampar);

  if ((sts != SS$_NORMAL) && (sts != SS$_SUPERSEDE)) fprintf (h_error, "oz_cli: error %u creating logical name %s\n", sts, crelognampar.logical_name);

  /* All done, release memory for any strings and release handles for any objects in the values array */

rtn:
  while (crelognampar.nvalues > 0) {
    j = -- crelognampar.nvalues;
    if (!(crelognampar.values[j].attr & 1/*OZ_LOGVALATR_OBJECT*/)) free (crelognampar.values[j].buff);
  }
  while ((handleclose = handlecloses) != NULL) {
    handlecloses = handleclose -> next;
    oz_sys_handle_release (PSL$C_KERNEL, handleclose -> handle);
    free (handleclose);
  }

  /* Free off the values array */

  free (crelognampar.values);

  /* Return completion status */

  oz_sys_handle_release (PSL$C_KERNEL, crelognampar.h_table);

  return (sts);
}

/* Create the logical name, either in kernel or user mode */

static unsigned long crelognam (unsigned long cprocmode, void *crelognamparv)

{
  Crelognampar *p;
  unsigned long sts;

  p = crelognamparv;
  sts = sys$crelnm (0, p -> h_table,  PSL$C_KERNEL,  p -> logical_name,  NULL);
  return (sts);
}

/************************************************************************/
/*									*/
/*	create logical table <logical_name>				*/
/*									*/
/*		-kernel							*/
/*		-nooutermode						*/
/*		-nosupersede						*/
/*									*/
/************************************************************************/

typedef struct { const char *table_name;
                 unsigned long lognamatr;
               } Crelogtblpar;

static unsigned long crelogtbl (unsigned long cprocmode, void *crelogtblparv);

static unsigned long int_create_logical_table (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  Crelogtblpar crelogtblpar;
  int i, kernel;
  unsigned long sts;

  crelogtblpar.table_name = NULL;
  crelogtblpar.lognamatr  = LNM$M_TABLE;
  kernel = 0;

  for (i = 0; i < argc; i ++) {

    /* Kernel - create logical in kernel mode */

    if (strcmp (argv[i], "-kernel") == 0) {
      kernel = 1;
      continue;
    }

    /* Nooutermode - do not allow outer mode versions of logical */

    if (strcmp (argv[i], "-nooutermode") == 0) {
      crelogtblpar.lognamatr |= 0; //OZ_LOGNAMATR_NOOUTERMODE;
      continue;
    }

    /* Nosupersede - logical is not allowed to be overwritten, must be deassigned first */

    if (strcmp (argv[i], "-nosupersede") == 0) {
      crelogtblpar.lognamatr |= 2;//OZ_LOGNAMATR_NOSUPERSEDE;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    crelogtblpar.table_name = argv[i];
  }

  /* Make sure we got a logical name in there somewhere */

  if (crelogtblpar.table_name == NULL) {
    fprintf (h_error, "oz_cli: missing table name\n");
    return (SS$_IVPARAM);
  }

  /* Create the table and return status */

  if (!kernel) sts = crelogtbl (PSL$C_USER, &crelogtblpar);
  else sts = oz_sys_callknl (crelogtbl, &crelogtblpar);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u creating logical table %s\n", sts, crelogtblpar.table_name);
  return (sts);
}

static unsigned long crelogtbl (unsigned long cprocmode, void *crelogtblparv)

{
  Crelogtblpar *p;
  unsigned long sts;

  p = crelogtblparv;
  sts = sys$crelnm (0, 0,  PSL$C_KERNEL,  p -> table_name,  NULL);
  return (sts);
}

/************************************************************************/
/*									*/
/*	create mutex <logical_name> <template> <mutex_name>		*/
/*									*/
/************************************************************************/

static unsigned long int_create_mutex (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
#if 0
  const char *logical_name, *mutex_name, *template;
  unsigned long sts;
  unsigned long h_iochan;
  OZ_IO_mutex_create mutex_create;

  if (argc != 3) {
    fprintf (h_error, "oz_cli: missing parameters\n");
    return (SS$_IVPARAM);
  }

  logical_name = argv[0];
  template     = argv[1];
  mutex_name   = argv[2];

  //  sts = sys$assign( template, &h_iochan,PSL$C_KERNEL,0,0);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u assigning channel to %s\n", sts, template);
    return (sts);
  }

#if 0
  memset (&mutex_create, 0, sizeof mutex_create);
  mutex_create.namesize = strlen (mutex_name);
  mutex_create.namebuff = mutex_name;

  sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, OZ_IO_MUTEX_CREATE, sizeof mutex_create, &mutex_create);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u creating mutex %s on %s\n", sts, mutex_name, template);
  else {
    sts = logname_creobj (logical_name, OZ_OBJTYPE_IOCHAN, h_iochan);
    if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u assigning logical %s to mutex %s on %s\n", sts, logical_name, mutex_name, template);
  }

  oz_sys_handle_release (PSL$C_KERNEL, h_iochan);
#endif

  return (sts);
#endif
}

/************************************************************************/
/*									*/
/*	create symbol <name> <value0> <value1> ...			*/
/*									*/
/*		-global							*/
/*		-level <n>						*/
/*		-integer						*/
/*		-string							*/
/*									*/
/************************************************************************/

static unsigned long int_create_symbol (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char *p, *paramval, *strvalue;
  const char *symbol_name;
  int eval, first_value, i, l, level, usedup;
  unsigned long sts;
  Symbol *symbol;
  Symtype symtype;

  eval        = 0;			/* args are the strings themselves */
  first_value = 0;			/* no symbols found yet */
  level       = 0;			/* define in current level */
  symbol_name = NULL;			/* symbol name not found yet */
  symtype     = SYMTYPE_STRING;		/* default type is string */

  for (i = 0; i < argc; i ++) {

    /* Global - define symbol in outermost level */

    if (strcmp (argv[i], "-global") == 0) {
      level = -1;
      continue;
    }

    /* Integer - value is an integer expression */

    if (strcmp (argv[i], "-integer") == 0) {
      symtype = SYMTYPE_INTEGER;
      continue;
    }

    /* Level - specify the definition level */

    if (strcmp (argv[i], "-level") == 0) {
      if (++ i == argc) {
        fprintf (h_error, "oz_cli: missing symbol level\n");
        return (SS$_IVPARAM);
      }
      level = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad symbol level %s\n", argv[i]);
        return (SS$_BADPARAM);
      }
      continue;
    }

    /* String - value is an string expression */

    if (strcmp (argv[i], "-string") == 0) {
      symtype = SYMTYPE_STRING;
      eval = 1;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the symbol name followed by a list of values */

    symbol_name = argv[i];
    first_value = ++ i;
    break;
  }

  /* Make sure we got a symbol name in there somewhere */

  if (symbol_name == NULL) {
    fprintf (h_error, "oz_cli: missing symbol name\n");
    return (SS$_IVPARAM);
  }

  /* Concat remaining args as a parameter value string */

  l = 1;
  for (i = first_value; i < argc; i ++) l += strlen (argv[i]) + 1;
  paramval = malloc (l);
  paramval[0] = 0;
  for (i = first_value; i < argc; i ++) {
    if (i > first_value) strcat (paramval, " ");
    strcat (paramval, argv[i]);
  }

  /* Create symbol */

  symbol = NULL;

  switch (symtype) {
    case SYMTYPE_INTEGER: {
      symbol = malloc (sizeof *symbol);					/* allocate memory block */
      sts = eval_integer (paramval, &p, &(symbol -> ivalue), 0);
      if ((sts == SS$_NORMAL) && (*p != 0)) {
        fprintf (h_error, "oz_cli: bad integer expression %s\n", argv[first_value]);
        sts = SS$_BADPARAM;
      }
      if (sts != SS$_NORMAL) {
        free (paramval);
        free (symbol);							/* bad value, free memory block */
        return (sts);							/* return error status */
      }
      break;
    }
    case SYMTYPE_STRING: {
      if (eval) {
        sts = eval_string (paramval, &p, &strvalue, 0);
        if ((sts == SS$_NORMAL) && (*p != 0)) {
          free (strvalue);
          fprintf (h_error, "oz_cli: bad string expression %s\n", paramval);
          sts = SS$_BADPARAM;
        }
        free (paramval);
        if (sts != SS$_NORMAL) return (sts);
        paramval = strvalue;
      }
      symbol = malloc (strlen (paramval) + 1 + sizeof *symbol);
      strcpy (symbol -> svalue, paramval);
      break;
    }
  }
  free (paramval);

  /* Insert in list and return success status */

  strncpy (symbol -> name, symbol_name, sizeof symbol -> name);
  symbol -> symtype = symtype;
  insert_symbol (symbol, NULL, level);
  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*	deallocate device <devicename>					*/
/*									*/
/************************************************************************/

static unsigned long int_deallocate_device (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  unsigned long sts;

  if (argc != 1) {
    fprintf (h_error, "oz_cli: must have exactly one device name\n");
    return (SS$_IVPARAM);
  }

  sts = oz_sys_io_dealloc (argv[0]);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u deallocating %s\n", sts, argv[0]);
  return (sts);
}

/************************************************************************/
/*									*/
/*	delete logical name <logical_name>				*/
/*	delete logical table <logical_name>				*/
/*									*/
/************************************************************************/

static unsigned long int_delete_logical_name (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  return (delete_logical (h_error, defaulttables, argc, argv));
}

static unsigned long int_delete_logical_table (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  return (delete_logical (h_error, NULL, argc, argv));
}

static unsigned long delete_logical (unsigned long h_error, const char *default_table_name, int argc, const char *argv[])

{
  const char *logical_name;
  int i;
  unsigned long sts;
  unsigned long h_logname, h_table;

  logical_name = NULL;

  for (i = 0; i < argc; i ++) {

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    logical_name = argv[i];
  }

  /* Make sure we got a logical name in there somewhere */

  if (logical_name == NULL) {
    fprintf (h_error, "oz_cli: missing logical name\n");
    return (SS$_IVPARAM);
  }

  /* Delete logical, return the status */

  sts = sys$trnlnm (0, 0,  default_table_name,  PSL$C_USER,  &h_table);
  if (sts == SS$_NORMAL) {
    sts = sys$trnlnm (0, h_table,  logical_name,  PSL$C_USER,  &h_logname);
    if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u looking up logical %s\n", sts, logical_name);
    else {
      sts = sys$dellnm (h_logname,h_logname,0);
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u deleting logical %s\n", sts, logical_name);
      oz_sys_handle_release (PSL$C_KERNEL, h_logname);
    }
    oz_sys_handle_release (PSL$C_KERNEL, h_table);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*	echo <string> ...						*/
/*									*/
/************************************************************************/

static unsigned long int_echo (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i;

  for (i = 0; i < argc; i ++) {
    if (i == 0) fprintf (h_output, "%s", argv[i]);
    else fprintf (h_output, " %s", argv[i]);
  }
  fprintf (h_output, "\n");

  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*	exit [<status>]							*/
/*									*/
/************************************************************************/

static unsigned long int_exit (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, usedup;
  unsigned long exitst;

  /* Always be sure to exit out of whatever script we're in */

  exit_script ();

  /* Now try to get the status from the command line */

  exitst = SS$_NORMAL;

  for (i = 0; i < argc; i ++) {

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - specify the status it will exit with */

    exitst = oz_hw_atoi (argv[i], &usedup);
    if (argv[i][usedup] != 0) {
      fprintf (h_error, "oz_cli: bad status value %s\n", argv[i]);
      return (SS$_BADPARAM);
    }
  }

  return (exitst);
}

/************************************************************************/
/*									*/
/*	goto <label>							*/
/*									*/
/************************************************************************/

static unsigned long int_goto (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  Label *label;

  if (argc != 1) {
    fprintf (h_error, "oz_cli: missing label from goto\n");
    return (SS$_IVPARAM);
  }

  /* Set up name of label to skip to */

  strncpy (skiplabel, argv[0], sizeof skiplabel);

  /* Try to find label in list of defined labels for the script */

  for (label = labels; label != NULL; label = label -> next) {
    if (strcmp (skiplabel, label -> name) == 0) break;
  }

  /* If found, reposition to the spot in the script where we found it last */

#if 0
  if (label != NULL) {
    scriptread.atblock = label -> blkoffs;
    scriptread.atbyte  = label -> bytoffs;
  }
#endif

  /* Anyway, return SS$_BRANCHSTARTED so we dont affect the oz_status variable */

  return (SS$_BRANCHSTARTED);
}

/************************************************************************/
/*									*/
/*	help								*/
/*									*/
/************************************************************************/

static unsigned long int_help (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  Command *command;
  unsigned long sts;

  fprintf (h_output, "\nInternal commands:\n");

  for (command = intcmd; command -> name != NULL; command ++) {
    fprintf (h_output, "  %c%s %s\n", command -> async ? '*' : ' ', command -> name, command -> help);
  }

  fprintf (h_output, "\n  all can be prefixed by any of the following options:\n");
  fprintf (h_output, "    -error <error_file>\n");
  fprintf (h_output, "    -ersym <error_symbol>\n");
  fprintf (h_output, "    -input <input_file>\n");
  fprintf (h_output, "    -insym <input_symbol>\n");
  fprintf (h_output, "    -output <output_file>\n");
  fprintf (h_output, "    -outsym <output_symbol>\n");
  fprintf (h_output, "\n  * can also be prefixed by any of the following options:\n");
  fprintf (h_output, "    -exit <exit_event_flag>\n");
  fprintf (h_output, "    -init <init_event_flag>\n");
  fprintf (h_output, "    -nowait\n");
  fprintf (h_output, "    -thread <thread_logical>\n");
  fprintf (h_output, "    -timeit\n");

  fprintf (h_output, "\nExternal commands from " CMDLNMTBL ":\n");
  sts = show_logical_table (h_output, h_error, 0, 0, 0, CMDLNMTBL, 0);

  fprintf (h_output, "\n  plus any image name prefixed by the keyword 'run'\n");

  fprintf (h_output, "\n  all can be prefixed by any of the following options:\n");
  fprintf (h_output, "    -directory <default_directory>\n");
  fprintf (h_output, "    -error <error_file>\n");
  fprintf (h_output, "    -ersym <error_symbol>\n");
  fprintf (h_output, "    -exit <exit_event_flag>\n");
  fprintf (h_output, "    -init <init_event_flag>\n");
  fprintf (h_output, "    -input <input_file>\n");
  fprintf (h_output, "    -insym <input_symbol>\n");
  fprintf (h_output, "    -job <job_logical>\n");
  fprintf (h_output, "    -nowait\n");
  fprintf (h_output, "    -output <output_file>\n");
  fprintf (h_output, "    -outsym <output_symbol>\n");
  fprintf (h_output, "    -process <process_logical>\n");
  fprintf (h_output, "    -thread <thread_logical>\n");
  fprintf (h_output, "    -timeit\n");

  return (sts);
}

/************************************************************************/
/*									*/
/*	if <integervalue> <statement ...>				*/
/*									*/
/************************************************************************/

static unsigned long int_if (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int usedup;
  unsigned long exp, sts;

  /* There must be at least a value and one word to the statement */

  if (argc < 2) {
    fprintf (h_error, "oz_cli: missing expression and/or statement\n");
    return (SS$_IVPARAM);
  }

  /* Convert the value to an integer */

  exp = oz_hw_atoi (argv[0], &usedup);
  if (argv[0][usedup] != 0) {
    fprintf (h_error, "oz_cli: invalid integer value %s\n", argv[0]);
    return (SS$_BADPARAM);
  }

  /* If value is non-zero, execute the statement, else nop */

  sts = SS$_BRANCHSTARTED;
  if (exp) sts = exechopped (argc - 1, argv + 1, NULL, NULL, 0);
  return (sts);
}

/************************************************************************/
/*									*/
/*	more [-number] [<file>]						*/
/*									*/
/************************************************************************/

typedef struct { int number;
                 int shownum;
                 unsigned long fileline;
                 int i;
                 int columns;
                 int rows;
                 int displine;
                 int linewrap;
                 char dispbuff[1];
               } More;

static unsigned long moreputchar (char c, More *more);
static unsigned long moreflush (More *more);

static unsigned long int_more (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char c, filebuff[256];
  const char *file;
  int i, j, number, usedup;
  unsigned long filerlen, sts, sts2;
  More *more;
#if 0
  OZ_Console_modebuff console_modebuff;
  OZ_IO_console_getmode console_getmode;
  OZ_IO_fs_open fs_open;
  OZ_IO_fs_readrec fs_readrec;

  /* Get screen dimensions to get defaults for columns and rows */

  memset (&console_getmode, 0, sizeof console_getmode);
  console_getmode.size = sizeof console_modebuff;
  console_getmode.buff = &console_modebuff;
  sts = oz_sys_io (PSL$C_KERNEL, h_s_console, 0, IO$_SENSEMODE, sizeof console_getmode, &console_getmode);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u getting console screen dimensions\n", sts);
    return (sts);
  }
  if (console_modebuff.columns == 0) console_modebuff.columns = 80;
  if (console_modebuff.rows    == 0) console_modebuff.rows    = 25;

  /* Parse arguments */

  file = NULL;
  number = 0;
  for (i = 0; i < argc; i ++) {
    if (strcmp (argv[i], "-number") == 0) {
      number = 1;
      continue;
    }
    if (strcmp (argv[i], "-columns") == 0) {
      if (++ i >= argc) {
        fprintf (h_error, "oz_cli: missing number of columns\n");
        return (SS$_IVPARAM);
      }
      console_modebuff.columns = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad number of columns %s\n", argv[i]);
        return (SS$_BADPARAM);
      }
      continue;
    }
    if (strcmp (argv[i], "-rows") == 0) {
      if (++ i >= argc) {
        fprintf (h_error, "oz_cli: missing number of rows\n");
        return (SS$_IVPARAM);
      }
      console_modebuff.rows = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad number of rows %s\n", argv[i]);
        return (SS$_BADPARAM);
      }
      continue;
    }
    if (argv[i][0] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_BADPARAM);
    }
    if (file == NULL) {
      file = argv[i];
      continue;
    }
    fprintf (h_error, "oz_cli: only one filename allowed\n");
    return (SS$_BADPARAM);
  }

  /* If an file was given, open it */

  if (file != NULL) {
    memset (&fs_open, 0, sizeof fs_open);
    fs_open.name = file;
    fs_open.lockmode = LCK$K_CRMODE;
    sts = oz_sys_io_fs_open (sizeof fs_open, &fs_open, 0, &h_input);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u opening file %s\n", sts, file);
      return (sts);
    }
  }

  /* Set up minimum number of columns and rows */

  if (console_modebuff.columns < 2) console_modebuff.columns = 2;
  if (number && (console_modebuff.columns < 10)) console_modebuff.columns = 10;
  if (number) console_modebuff.columns -= 8;
  if (console_modebuff.rows < 3) console_modebuff.rows = 3;

  /* Set up context block */

  more = malloc (sizeof *more + console_modebuff.columns + 2);	/* malloc context block */
  memset (more, 0, sizeof *more + console_modebuff.columns + 2); /* ... and clear it out */
  more -> number   = number;					/* whether or not to display file line numbers */
  more -> shownum  = 1;						/* we have to display it on the first line of output */
  more -> columns  = console_modebuff.columns;			/* number of data displayable columns (not including numbers) */
  more -> rows     = console_modebuff.rows;			/* total number of rows on the screen */
  more -> linewrap = (console_modebuff.linewrap > 0);		/* linewrap is enabled */

  /* Read input records and display them */

  memset (&fs_readrec, 0, sizeof fs_readrec);			/* clear unknown parameters */
  fs_readrec.size    = sizeof filebuff;				/* size to read */
  fs_readrec.buff    = filebuff;				/* where to read */
  fs_readrec.rlen    = &filerlen;				/* get the length read */
  fs_readrec.trmsize = 1;					/* set up standard terminator */
  fs_readrec.trmbuff = "\n";
  fs_readrec.pmtsize = 1;					/* set up standard prompt */
  fs_readrec.pmtbuff = ">";
  while (1) {
    filerlen = 0;
    sts2 = oz_sys_io (PSL$C_KERNEL, h_input, 0, IO$_READLBLK, sizeof fs_readrec, &fs_readrec);
    for (j = 0; j < filerlen; j ++) {
      c = filebuff[j];
      if (c >= 127) continue;
      if (c >= ' ') { sts = moreputchar (c, more); if (sts != SS$_BRANCHSTARTED) goto rtn; continue; }
      switch (c) {
        case  8: { if (more -> i > 0) more -> i --; break; }
        case  9: { do { sts = moreputchar (' ', more); if (sts != SS$_BRANCHSTARTED) goto rtn; } while (more -> i & 7); break; }
        case 10: { sts = moreflush (more); if (sts != SS$_BRANCHSTARTED) goto rtn; more -> shownum = 1; break; }
        case 13: { more -> i = 0; break; }
      }
    }
    sts = sts2;
    if (sts == SS$_NORMAL) { sts = moreflush (more); if (sts != SS$_BRANCHSTARTED) goto rtn; more -> shownum = 1; }
    else if (sts != SS$_NOTORIGIN) { if (more -> i != 0) { sts = moreflush (more); if (sts != SS$_BRANCHSTARTED) goto rtn; } sts = sts2; break; }
  }
  if (sts != SS$_ENDOFFILE) fprintf (h_error, "oz_cli: error %u reading file\n", sts);

  /* Clean up */
#endif
rtn:
  if (file != NULL) oz_sys_handle_release (PSL$C_KERNEL, h_input);
  free (more);
  return (sts);
}

static unsigned long moreputchar (char c, More *more)

{
  unsigned long sts;

  if (more -> i == more -> columns) {
    sts = moreflush (more);
    if (sts != SS$_BRANCHSTARTED) return (sts);
  }
  more -> dispbuff[more->i++] = c;
  return (SS$_BRANCHSTARTED);
}

static unsigned long moreflush (More *more)

{
  char crb;
  unsigned long sts;
  OZ_IO_console_read console_read;
  OZ_IO_console_write console_write;

  if (++ (more -> displine) % (more -> rows - 2) == 0) {
    memset (&console_read, 0, sizeof console_read);		/* this is to read the continuation */
    console_read.size    = 1;
    console_read.buff    = &crb;
    console_read.pmtsize = 41;
    console_read.pmtbuff = "oz_cli: more: CR to continue, ^Z to exit>";
    console_read.noecho  = 1;
    sts = oz_sys_io (PSL$C_KERNEL, h_s_console, 0, IO$_READLBLK, sizeof console_read, &console_read);
    memset (&console_write, 0, sizeof console_write);
    console_write.trmsize = 1;
    console_write.trmbuff = "\n";
    oz_sys_io (PSL$C_KERNEL, h_s_console, 0, IO$_WRITELBLK, sizeof console_write, &console_write);
    if (sts == SS$_ENDOFFILE) return (SS$_NORMAL);		/* eof means to stop here (but successfully) */
    if ((sts != SS$_NORMAL) && (sts != SS$_NOTORIGIN)) {
      fprintf (h_s_error, "oz_cli: error %u reading continuation prompt\n", sts);
      return (sts); 
    }
  }
  if (!(more -> linewrap) || (strlen (more -> dispbuff) < more -> columns)) strcat (more -> dispbuff, "\n");
  if (!(more -> number)) fprintf (h_s_console, "%s", more -> dispbuff);
  else if (!(more -> shownum)) fprintf (h_s_console, "        %s", more -> dispbuff);
  else {
    fprintf (h_s_console, "%7d %s", ++ (more -> fileline), more -> dispbuff);
    more -> shownum = 0;
  }
  memset (more -> dispbuff, 0, more -> columns + 2);
  more -> i = 0;
  return (SS$_BRANCHSTARTED);
}

/************************************************************************/
/*									*/
/*	open file <file_name>						*/
/*									*/
/*		-lockmode <lock_mode>					*/
/*		-logical <logical_name> 				*/
/*									*/
/************************************************************************/

static unsigned long int_open_file (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i, j;
  unsigned long sts;
  unsigned long h_iochan;
#if 0
  OZ_IO_fs_open fs_open;

  memset (&fs_open, 0, sizeof fs_open);
  fs_open.lockmode = LCK$K_PRMODE;
  fs_open.ignclose = 1;
  logical_name = NULL;

  for (i = 0; i < argc; i ++) {

    /* Lockmode - specify the lock mode to open it with */

    if (strcmp (argv[i], "-lockmode") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing lock mode after -lockmode\n");
        return (SS$_IVPARAM);
      }
      for (j = 0; j < (LCK$K_EXMODE+1); j ++) if (strcasecmp (lockmodes[j].name, argv[i]) == 0) break;
      if (j == (LCK$K_EXMODE+1)) {
        fprintf (h_error, "oz_cli: bad lock mode %s\n", argv[i]);
        return (SS$_BADPARAM);
      }
      fs_open.lockmode = lockmodes[j].valu;
      continue;
    }

    /* Logical - specify the logical name to assign to it */

    if (strcmp (argv[i], "-logical") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing logical name after -logical\n");
        return (SS$_IVPARAM);
      }
      logical_name = argv[i];
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* Not an option, it is the filename */

    fs_open.name = argv[i];
  }

  /* Make sure we got a file name in there somewhere */

  if (fs_open.name == NULL) {
    fprintf (h_error, "oz_cli: missing file name\n");
    return (SS$_IVPARAM);
  }

  /* Open file and assign logical name to the channel                            */
  /* If no logical is given, the file will be closed when the handle is released */

  sts = oz_sys_io_fs_open (sizeof fs_open, &fs_open, 0, &h_iochan);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u opening file %s\n", sts, fs_open.name);
  else {
    if (logical_name != NULL) {
      sts = logname_creobj (logical_name, OZ_OBJTYPE_IOCHAN, h_iochan);
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u assigning logical %s to file %s\n", sts, logical_name, fs_open.name);
    }
    oz_sys_handle_release (PSL$C_KERNEL, h_iochan);
  }
#endif

  return (sts);
}

/************************************************************************/
/*									*/
/*	open mutex <logical_name> <template> <mutex_name>		*/
/*									*/
/************************************************************************/

static unsigned long int_open_mutex (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name, *mutex_name, *template;
  unsigned long sts;
  unsigned long h_iochan;
#if 0
  OZ_IO_mutex_lookup mutex_lookup;

  if (argc != 3) {
    fprintf (h_error, "oz_cli: missing parameters\n");
    return (SS$_IVPARAM);
  }

  logical_name = argv[0];
  template     = argv[1];
  mutex_name   = argv[2];

  //  sts = sys$assign( template, &h_iochan,PSL$C_KERNEL,0,0);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u assigning channel to %s\n", sts, template);
    return (sts);
  }

  memset (&mutex_lookup, 0, sizeof mutex_lookup);
  mutex_lookup.namesize = strlen (mutex_name);
  mutex_lookup.namebuff = mutex_name;

  sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, OZ_IO_MUTEX_LOOKUP, sizeof mutex_lookup, &mutex_lookup);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u opening mutex %s on %s\n", sts, mutex_name, template);
  else {
    sts = logname_creobj (logical_name, OZ_OBJTYPE_IOCHAN, h_iochan);
    if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u assigning logical %s to mutex %s on %s\n", sts, logical_name, mutex_name, template);
  }

  oz_sys_handle_release (PSL$C_KERNEL, h_iochan);
#endif

  return (sts);
}

/************************************************************************/
/*									*/
/*	read file -logical <logical_name> <symbol>			*/
/*									*/
/************************************************************************/

static unsigned long int_read_file (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name, *symbol_name;
  int i, j, usedup;
  unsigned long rlen, sts;
  unsigned long h_iochan;
#if 0
  OZ_IO_fs_readrec fs_readrec;
  Symbol *symbol;

  memset (&fs_readrec, 0, sizeof fs_readrec);
  logical_name = "OZ_INPUT";
  symbol_name  = NULL;

  fs_readrec.size = 256;			/* fill in default read size */
  fs_readrec.rlen = &rlen;			/* where to return length actually read */
  fs_readrec.trmsize = 1;			/* fill in default terminator */
  fs_readrec.trmbuff = "\n";

  for (i = 0; i < argc; i ++) {

    /* Logical - specify the logical name of the file to read */

    if (strcmp (argv[i], "-logical") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing logical name after -logical\n");
        return (SS$_IVPARAM);
      }
      logical_name = argv[i];
      continue;
    }

    /* Prompt - specify prompt string */

    if (strcmp (argv[i], "-prompt") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing prompt string after -prompt\n");
        return (SS$_IVPARAM);
      }
      fs_readrec.pmtsize = strlen (argv[i]);
      fs_readrec.pmtbuff = argv[i];
      continue;
    }

    /* Size - specify buffer size */

    if (strcmp (argv[i], "-size") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing size value after -size\n");
        return (SS$_IVPARAM);
      }
      fs_readrec.size = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: non-integer buffer size %s after -size\n", argv[i]);
        return (SS$_BADPARAM);
      }
      continue;
    }

    /* Terminator - specify terminator string */

    if (strcmp (argv[i], "-terminator") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing terminator string after -terminator\n");
        return (SS$_IVPARAM);
      }
      fs_readrec.trmsize = strlen (argv[i]);
      fs_readrec.trmbuff = argv[i];
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* Not an option, it is the symbol */

    symbol_name = argv[i];
  }

  /* Make sure we got a symbol name in there somewhere */

  if (symbol_name == NULL) {
    fprintf (h_error, "oz_cli: missing symbol name\n");
    return (SS$_IVPARAM);
  }

  /* Get handle to iochan from logical name.  If no logical name given, use OZ_INPUT. */

  sts = logname_getobj (logical_name, OZ_OBJTYPE_IOCHAN, &h_iochan);
  if (sts != SS$_NORMAL) return (sts);

  /* Allocate a read buffer */

  if (fs_readrec.size != 0) fs_readrec.buff = malloc (fs_readrec.size);

  /* Read the record */

  sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, IO$_READLBLK, sizeof fs_readrec, &fs_readrec);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u reading file %s\n", sts, logical_name);

  /* Assign symbol */

  if (symbol_name != NULL) {
    symbol = malloc (rlen + 1 + sizeof *symbol);			/* allocate symbol buffer */
    memcpy (symbol -> svalue, fs_readrec.buff, rlen);			/* copy in record data */
    symbol -> svalue[rlen] = 0;						/* null terminate it */
    strncpy (symbol -> name, symbol_name, sizeof symbol -> name);	/* set up symbol's name */
    symbol -> symtype = SYMTYPE_STRING;					/* it is always type string */
    insert_symbol (symbol, NULL, 0);					/* insert in local symbol list */
  }

  /* Free buffer, release the handle and return read status */

  if (fs_readrec.size != 0) free (fs_readrec.buff);
  oz_sys_handle_release (PSL$C_KERNEL, h_iochan);
#endif
  return (sts);
}

/************************************************************************/
/*									*/
/*	resume thread [<logical_name>]					*/
/*									*/
/************************************************************************/

static unsigned long int_resume_thread (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i, usedup;
  unsigned long sts;
  unsigned long h_logname, h_table, h_thread;
  unsigned long thread_id;

  logical_name = LASTHREADLNM;
  thread_id    = 0;

  for (i = 0; i < argc; i ++) {

    /* Id - thread id number */

    if (strcmp (argv[i], "-id") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing thread id number after -id\n");
        return (SS$_IVPARAM);
      }
      thread_id = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad thread id number %s after -id\n", argv[i]);
        return (SS$_BADPARAM);
      }
      logical_name = NULL;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    logical_name = argv[i];
    thread_id    = 0;
  }

  /* Get handle to thread from logical name.  If no logical name given, use last thread's handle. */

  if (thread_id != 0) {
    sts = oz_sys_thread_getbyid (thread_id, &h_thread);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting handle to thread id %u\n", sts, thread_id);
      return (sts);
    }
  } else {
    sts = logname_getobj (logical_name, OZ_OBJTYPE_THREAD, &h_thread);
    if (sts != SS$_NORMAL) return (sts);
  }

  /* Resume the thread */

  sts = sys$resume(h_thread,0);
  if ((sts != SS$_WASCLR) && (sts != SS$_WASSET)) {
    fprintf (h_error, "oz_cli: error %u resuming thread\n", sts);
  }

  /* Release the handle */

  oz_sys_handle_release (PSL$C_KERNEL, h_thread);

  /* Anyway, return composite status */

  return (sts);
}

/************************************************************************/
/*									*/
/*  Run an external command whose name is given in OZ_CLI_TABLES	*/
/*									*/
/*	The command name may be prefixed by any of these options:	*/
/*									*/
/*		-directory <default_directory>				*/
/*		-error <error_file>					*/
/*		-ersym <error_symbol>					*/
/*		-exit <exit_event_flag>					*/
/*		-init <init_event_flag>					*/
/*		-input <input_file>					*/
/*		-insym <input_symbol>					*/
/*		-job <job_logical>					*/
/*		-nowait							*/
/*		-orphan							*/
/*		-output <output_file>					*/
/*		-outsym <output_symbol>					*/
/*		-process <process_logical>				*/
/*		-thread <thread_logical>				*/
/*		-timeit							*/
/*									*/
/************************************************************************/

static unsigned long extcommand (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char command[256], image[256];
  const char *imagep;
  int j, l, length, nwords;
  unsigned long exitst, nvalues, sts;
  unsigned long h_command, h_logname, h_table;
  Runopts runopts;

  /* Decode command line options */

  sts = decode_runopts (NULL, NULL, 0, h_input, h_output, h_error, &argc, &argv, &runopts);
  if (sts != SS$_NORMAL) return (sts);

  if (argc == 0) {
    fprintf (h_error, "oz_cli: missing command\n");
    return (SS$_IVPARAM);
  }

  /* If it's a RUN command, it is followed by the image filename then args for the image */

  if (strcasecmp (argv[0], "run") == 0 || strcasecmp (argv[0], "mcr") == 0 || dummy) {
    if (dummy) {
      imagep = argv[0];
    } else {
      if (argc == 1) {											/* must have an image name there */
	fprintf (h_error, "oz_cli: missing image name after 'run' command\n");
	return (SS$_IVPARAM);
      }
      imagep = argv[1];
      argc --;												/* point to image name */
      argv ++;

      if (strcasecmp (argv[-1], "run") == 0) {
	argc=0;
	argv[1]=0;
      }
    }

  }

  /* Otherwise, look for the command name in the OZ_CLI_TABLES logical name table(s)   */

  else {
#if 0
    sts = sys$trnlnm (0, 0,  CMDLNMTBL,  PSL$C_USER,  &h_table);		/* point to the OZ_CLI_TABLES table list */
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u looking up " CMDLNMTBL "\n", sts);
      return (sts);
    }
    h_command = 0;											/* no command logical found yet */
    command[0] = 0;
    nwords = 0;
    for (j = 0; j < argc; j ++) {
      l = strlen (command) + 1 + strlen (argv[j]);							/* make sure there's room for next command word in buffer */
      if (l >= sizeof command) break;
      if (j != 0) strcat (command, " ");								/* stuff next command word in string buffer */
      strcat (command, argv[j]);
      sts = sys$trnlnm (0, h_table,  command,  PSL$C_USER,  &h_logname);	/* look up the corresponding logical */
      if (sts == SS$_NORMAL) {
        if (h_command != 0) oz_sys_handle_release (PSL$C_KERNEL, h_command);				/* if found, release any old one we had */
        h_command = h_logname;										/* save the new one */
        nvalues = exitst;										/* save number of values on the logical */
        nwords = j + 1;											/* remember how many command line words were used */
        length = strlen (command);									/* remember length of command that matched */
      }
    }
    oz_sys_handle_release (PSL$C_KERNEL, h_table);							/* release OZ_CLI_TABLES pointer */
    if (h_command == 0) {										/* see if any match was made */
      fprintf (h_error, "oz_cli: unknown command %s\n", command);				/* error if not */
      return (SS$_IVPARAM);
    }
    command[length] = 0;										/* chop it off where we matched it */
    if (nvalues == 0) {											/* if logical name has no values, command is disabled */
      oz_sys_handle_release (PSL$C_KERNEL, h_command);
      fprintf (h_error, "oz_cli: disabled command %s\n", command);
      return (SS$_IVPARAM);
    }

    sts = sys$trnlnm (0, h_command, 0,  sizeof image,  NULL);		/* get image name */
    if (sts != SS$_NORMAL) {
      oz_sys_handle_release (PSL$C_KERNEL, h_command);
      fprintf (h_error, "oz_cli: error %u getting image name for command %s\n", sts, command);
      return (SS$_IVPARAM);
    }
    imagep = image;

    oz_sys_handle_release (PSL$C_KERNEL, h_command);							/* all done with command logical */

    /* Skip over the words in the command to point to first argument          */
    /* Then put the string of words that made up the command just before them */

    argv[--nwords] = command;
    argc -= nwords;
    argv += nwords;
#else
    printf("%DCL-W-IVVERB, unrecognized command verb - check validity and spelling\n");
#endif
  }

  /* Run the image */

  sts = runimage (h_error, &runopts, imagep, argc, argv);
  return (sts);
}

/************************************************************************/
/*									*/
/*  Spawn an image and optionally wait for it to exit			*/
/*									*/
/*    Input:								*/
/*									*/
/*	h_error = error message output handle				*/
/*	runopts = run command options (-nowait, -directory, etc)	*/
/*	image   = image to run (xxx.oz)					*/
/*	argc    = argument count to pass to image (incl command name)	*/
/*	argv    = argument vector to pass to image (incl command name)	*/
/*									*/
/*    Output:								*/
/*									*/
/*	runimage = status						*/
/*	runopts cleaned up						*/
/*									*/
/************************************************************************/

static unsigned long runimage (unsigned long h_error, Runopts *runopts, const char *image, int argc, const char *argv[])

{
  unsigned long sts;

  /* Open the handles */

  if (fork()==0) {
    execv(image,argv);
  }

  return SS$_NORMAL;

  sts = setup_runopts (h_error, runopts);
  if (sts != SS$_NORMAL) goto rtn;

  /* Spawn the image */
  posix_spawn();

  sts = oz_sys_spawn (runopts -> h_job, 
                      image, 
                      runopts -> h_in, 
                      runopts -> h_out, 
                      runopts -> h_err, 
                      runopts -> h_init, 
                      runopts -> h_exit, 
                      runopts -> defdir, 
                      argc, 
                      argv, 
                      NULL, 
                      &(runopts -> h_thread), 
                      &(runopts -> h_process));
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u spawning image %s\n", sts, image);

  /* Maybe orphan it then wait for it to finish */

  else {
    if (runopts -> orphan) oz_sys_thread_orphan (runopts -> h_thread);
    sts = finish_runopts (h_error, runopts);
  }

  /* Release all handles we may have opened */

rtn:
  cleanup_runopts (runopts);

  /* Return the composite status */

  return (sts);
}


/************************************************************************/
/*									*/
/*  Decode 'run' options						*/
/*									*/
/*		-directory <default_directory>				*/
/*		-error <error_file>					*/
/*		-ersym <error_symbol>					*/
/*		-exit <exit_event_flag>					*/
/*		-init <init_event_flag>					*/
/*		-input <input_file>					*/
/*		-insym <input_symbol>					*/
/*		-job <job_logical>					*/
/*		-nowait							*/
/*		-orphan							*/
/*		-output <output_file>					*/
/*		-outsym <output_symbol>					*/
/*		-process <process_logical>				*/
/*		-thread <thread_logical>				*/
/*		-timeit							*/
/*									*/
/*    Input:								*/
/*									*/
/*	input    = NULL : no default -input option value		*/
/*	           else : default -input option value			*/
/*	output   = NULL : no default -output option value		*/
/*	           else : default -output option value			*/
/*	nowait   = 0 : do not default a -nowait option			*/
/*	           1 : default is a -nowait option			*/
/*	h_input  = default input handle					*/
/*	h_output = default output handle				*/
/*	h_error  = default error handle					*/
/*	*argc_r  = argument count					*/
/*	*argv_r  = argument vector					*/
/*									*/
/*    Output:								*/
/*									*/
/*	decode_runopts  = SS$_NORMAL : successful			*/
/*	                        else : error status			*/
/*	*argc_r/*argv_r = above options stripped off			*/
/*	*runopts = filled in from above options				*/
/*									*/
/************************************************************************/

static unsigned long decode_runopts (const char *input, const char *output, int nowait, unsigned long h_input, unsigned long h_output, unsigned long h_error, int *argc_r, const char ***argv_r, Runopts *runopts)

{
  const char **argv;
  int argc, i;

  memset (runopts, 0, sizeof *runopts);

  runopts -> input_name  = input;
  runopts -> output_name = output;
  runopts -> wait = !nowait;

  runopts -> h_err = h_error;
  runopts -> h_in  = h_input;
  runopts -> h_out = h_output;

  argc = *argc_r;
  argv = *argv_r;

  for (i = 0; i < argc; i ++) {

    /* Specify a default directory for it */

    if (strcmp (argv[i], "-directory") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing directory name after -directory\n");
        return (SS$_IVPARAM);
      }
      runopts -> defdir = argv[i];
      continue;
    }

    /* Specify an error file for it */

    if (strcmp (argv[i], "-error") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing error file name after -error\n");
        return (SS$_IVPARAM);
      }
      runopts -> error_name = argv[i];
      runopts -> ersym_name = NULL;
      continue;
    }

    /* Specify an error symbol for it */

    if (strcmp (argv[i], "-ersym") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing error symbol name after -ersym\n");
        return (SS$_IVPARAM);
      }
      runopts -> ersym_name = argv[i];
      runopts -> error_name = NULL;
      continue;
    }

    /* Specify an exit event flag for it */

    if (strcmp (argv[i], "-exit") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing exit event flag logical name after -exit\n");
        return (SS$_IVPARAM);
      }
      runopts -> exit_name = argv[i];
      continue;
    }

    /* Specify an init event flag for it */

    if (strcmp (argv[i], "-init") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing init event flag logical name after -init\n");
        return (SS$_IVPARAM);
      }
      runopts -> init_name = argv[i];
      continue;
    }

    /* Specify an input file for it */

    if (strcmp (argv[i], "-input") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing input file name after -input\n");
        return (SS$_IVPARAM);
      }
      runopts -> input_name = argv[i];
      runopts -> insym_name = NULL;
      continue;
    }

    /* Specify an input symbol for it */

    if (strcmp (argv[i], "-insym") == 0) {
      Symbol *symbol;

      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing input symbol name after -insym\n");
        return (SS$_IVPARAM);
      }
      runopts -> insym_name = argv[i];
      runopts -> input_name = NULL;
      symbol = lookup_symbol (runopts -> insym_name, 0, NULL);
      if (symbol == NULL) {
        fprintf (h_error, "oz_cli: input symbol %s not defined\n", runopts -> insym_name);
        return (SS$_BADPARAM);
      }
      if ((symbol -> symtype != SYMTYPE_STRING) && (symbol -> func != NULL)) {
        fprintf (h_error, "oz_cli: input symbol %s must be a string\n", runopts -> insym_name);
        return (SS$_BADPARAM);
      }
      if (runopts -> insym_data != NULL) free (runopts -> insym_data);
      runopts -> insym_data = strdup (symbol -> svalue);
      continue;
    }

    /* Specify a job for it */

    if (strcmp (argv[i], "-job") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing job logical name after -job\n");
        return (SS$_IVPARAM);
      }
      runopts -> job_name = argv[i];
      continue;
    }

    /* Say we don't want to wait for it */

    if (strcmp (argv[i], "-nowait") == 0) {
      runopts -> wait = 0;
      continue;
    }

    /* Make it an orphan */

    if (strcmp (argv[i], "-orphan") == 0) {
      runopts -> orphan = 1;
      continue;
    }

    /* Specify an output file for it */

    if (strcmp (argv[i], "-output") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing output file name after -output\n");
        return (SS$_IVPARAM);
      }
      runopts -> output_name = argv[i];
      runopts -> outsym_name = NULL;
      continue;
    }

    /* Specify an output symbol for it */

    if (strcmp (argv[i], "-outsym") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing output symbol name after -outsym\n");
        return (SS$_IVPARAM);
      }
      runopts -> outsym_name = argv[i];
      runopts -> output_name = NULL;
      continue;
    }

    /* Specify a process logical to be created */

    if (strcmp (argv[i], "-process") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing logical name after -process\n");
        return (SS$_IVPARAM);
      }
      runopts -> process_name = argv[i];
      continue;
    }

    /* Specify a thread logical to be created */

    if (strcmp (argv[i], "-thread") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing logical name after -thread\n");
        return (SS$_IVPARAM);
      }
      runopts -> thread_name = argv[i];
      continue;
    }

    /* Say we want it's times printed out on exit */

    if (strcmp (argv[i], "-timeit") == 0) {
      runopts -> timeit = 1;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* Not an option, it is the command name followed by the args */

    break;
  }

  /* Make sure we got command name in there somewhere */

  if (i == argc) {
    fprintf (h_error, "oz_cli: missing command\n");
    return (SS$_IVPARAM);
  }

  /* Return what's left of the command line */

  *argc_r = argc - i;
  *argv_r = argv + i;

  return (SS$_NORMAL);
}

#if 0

/************************************************************************/
/*									*/
/*  This routine opens files and sets up event flags, etc		*/
/*									*/
/*    Input:								*/
/*									*/
/*	runopts = as returned by decode_runopts				*/
/*									*/
/*    Output:								*/
/*									*/
/*	setup_runopts = SS$_NORMAL : successful				*/
/*	                      else : error status			*/
/*	*runopts = handles opened					*/
/*									*/
/************************************************************************/

static unsigned long setup_runopts (unsigned long h_error, Runopts *runopts)

{
  unsigned long sts;
#if 0
  OZ_IO_fs_create fs_create;
  OZ_IO_fs_open fs_open;

  /* Maybe we will need an evsym event flag */

  if ((runopts -> ersym_name != NULL) || (runopts -> insym_name != NULL) || (runopts -> outsym_name != NULL)) {
    sts = oz_sys_event_create (PSL$C_KERNEL, "-*sym pipe close", &(runopts -> h_evsym));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating -*sym pipe close event flag\n", sts);
      goto rtn;
    }
  }

  /* Open error file if they supplied its name */

  if (runopts -> error_name != NULL) {
    memset (&fs_create, 0, sizeof fs_create);
    fs_create.name = runopts -> error_name;
    fs_create.lockmode = LCK$K_CWMODE;
    fs_create.ignclose = 1;
    sts = oz_sys_io_fs_create (sizeof fs_create, &fs_create, 0, &(runopts -> h_err));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating error file %s\n", sts, runopts -> error_name);
      goto rtn;
    }
  }

  /* Create error symbol pipe if they supplied symbol name */

  if (runopts -> ersym_name != NULL) {
    sts = crepipepair (&(runopts -> h_ersym), &(runopts -> h_err));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating error symbol pipe %s\n", sts, runopts -> ersym_name);
      goto rtn;
    }
  }

  /* Get the exit event flag if they supplied its logical name */

  if (runopts -> exit_name != NULL) {
    sts = logname_getobj (runopts -> exit_name, OZ_OBJTYPE_EVENT, &(runopts -> h_exit));
    if (sts != SS$_NORMAL) goto rtn;
  }

  /* Get the init event flag if they supplied its logical name */

  if (runopts -> init_name != NULL) {
    sts = logname_getobj (runopts -> init_name, OZ_OBJTYPE_EVENT, &(runopts -> h_init));
    if (sts != SS$_NORMAL) goto rtn;
  }

  /* Open input file if they supplied its name */

  if (runopts -> input_name != NULL) {
    memset (&fs_open, 0, sizeof fs_open);
    fs_open.name = runopts -> input_name;
    fs_open.lockmode = LCK$K_CRMODE;
    fs_open.ignclose = 1;
    sts = oz_sys_io_fs_open (sizeof fs_open, &fs_open, 0, &(runopts -> h_in));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u opening input file %s\n", sts, runopts -> input_name);
      goto rtn;
    }
  }

  /* Create input symbol pipe if they supplied symbol name */

  if (runopts -> insym_name != NULL) {
    sts = crepipepair (&(runopts -> h_in), &(runopts -> h_insym));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating input symbol pipe %s\n", sts, runopts -> insym_name);
      goto rtn;
    }
  }

  /* Get job if they supplied its logical name */

  if (runopts -> job_name != NULL) {
    sts = logname_getobj (runopts -> job_name, OZ_OBJTYPE_JOB, &(runopts -> h_job));
    if (sts != SS$_NORMAL) goto rtn;
  }

  /* Create output file if they supplied its name */

  if (runopts -> output_name != NULL) {
    memset (&fs_create, 0, sizeof fs_create);
    fs_create.name = runopts -> output_name;
    fs_create.lockmode = LCK$K_CWMODE;
    fs_create.ignclose = 1;
    sts = oz_sys_io_fs_create (sizeof fs_create, &fs_create, 0, &(runopts -> h_out));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating output file %s\n", sts, runopts -> output_name);
      goto rtn;
    }
  }

  /* Create output symbol pipe if they supplied symbol name */

  if (runopts -> outsym_name != NULL) {
    sts = crepipepair (&(runopts -> h_outsym), &(runopts -> h_out));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating output symbol pipe %s\n", sts, runopts -> outsym_name);
      goto rtn;
    }
  }

  /* If there is no exit event flag specified, create temp one here so wait_thread routine will have something to wait for */

  if (runopts -> exit_name == NULL) {
    sts = oz_sys_event_create (PSL$C_KERNEL, "oz_cli temp exit", &(runopts -> h_exit));
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating temp exit event flag\n", sts);
      goto rtn;
    }
  }

  return (SS$_NORMAL);

  /* Some error setting up, release all handles we may have opened and buffers that were allocated */

rtn:
  cleanup_runopts (runopts);

  /* Return the error status */

#endif
  return (sts);
}

/************************************************************************/
/*									*/
/*  This routine is called after the command has been spawned.  It 	*/
/*  creates process/thread logicals and optionally waits for the 	*/
/*  thread to exit.							*/
/*									*/
/*  It also starts reading/writing the symbol pipes if necessary.	*/
/*									*/
/*    Input:								*/
/*									*/
/*	runopts = run options struct pointer				*/
/*									*/
/*    Output:								*/
/*									*/
/*	finish_runopts = completion status				*/
/*									*/
/************************************************************************/

static unsigned long finish_runopts (unsigned long h_error, Runopts *runopts)

{
  unsigned long rtnsts, sts;

  rtnsts = SS$_NORMAL;

  /* Save the handle as the most recent thread (default for wait thread command) */
  /* Note that we may overwrite the previous values (which releases them)        */
  /* If there is an error, keep going but remember the status                    */

  sts = logname_creobj (LASTHREADLNM, OZ_OBJTYPE_THREAD, runopts -> h_thread);
  if ((sts != SS$_NORMAL) && (sts != SS$_SUPERSEDE)) {
    fprintf (h_error, "oz_cli: error %u creating thread logical " LASTHREADLNM "\n", sts);
    rtnsts = sts;
  }

  if (runopts -> h_process != 0) {
    sts = logname_creobj (LASTPROCLNM, OZ_OBJTYPE_PROCESS, runopts -> h_process);
    if ((sts != SS$_NORMAL) && (sts != SS$_SUPERSEDE)) {
      fprintf (h_error, "oz_cli: error %u creating process logical " LASTPROCLNM "\n", sts);
      rtnsts = sts;
    }
  }

  /* Create logicals for thread and process handles */

  if (runopts -> thread_name != NULL) {
    sts = logname_creobj (runopts -> thread_name, OZ_OBJTYPE_THREAD, runopts -> h_thread);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating thread logical %s\n", sts, runopts -> thread_name);
      if (rtnsts == SS$_NORMAL) rtnsts = sts;
    }
  }

  if ((runopts -> h_process != 0) && (runopts -> process_name != NULL)) {
    sts = logname_creobj (runopts -> process_name, OZ_OBJTYPE_PROCESS, runopts -> h_process);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u creating process logical %s\n", sts, runopts -> process_name);
      if (rtnsts == SS$_NORMAL) rtnsts = sts;
    }
  }

  /* If -ersym, -insym and/or -outsym specified, start our end of the pipe I/O */

  if (runopts -> ersym_name  != NULL) {
    runopts -> ersym_pipebuf_qt  = &(runopts -> ersym_pipebuf_qh);
    start_ersym_read  (runopts);
  }
  if (runopts -> insym_name  != NULL) start_insym_write (runopts);
  if (runopts -> outsym_name != NULL) {
    runopts -> outsym_pipebuf_qt = &(runopts -> outsym_pipebuf_qh);
    start_outsym_read (runopts);
  }

  /* If waiting, we wait for it to complete.  Also allow it to be suspended by control-Y. */

  if (runopts -> wait) {
    sts = wait_thread (h_error, runopts -> h_thread);		/* wait for it (or control-Y) */
    if (rtnsts == SS$_NORMAL) rtnsts = sts;			/* return the status */
  }

  return (rtnsts);
}

/************************************************************************/
/*									*/
/*  Create a pair of pipes						*/
/*									*/
/************************************************************************/

static unsigned long crepipepair (unsigned long *h_read_r, unsigned long *h_write_r)

{
#if 0
  char pipename[OZ_DEVUNIT_NAMESIZE];
  OZ_IO_fs_create fs_create;
  OZ_IO_fs_open fs_open;
  unsigned long sts;

  //  sts = sys$assign( OZ_IO_PIPES_TEMPLATE, h_write_r,PSL$C_KERNEL,0,0);		// create pipe and get writable channel
  if (sts == SS$_NORMAL) {
    sts = oz_sys_iochan_getunitname (*h_write_r, sizeof pipename, pipename);				// assign readable channel to same pipe
    //    if (sts == SS$_NORMAL) sts = sys$assign( pipename, h_read_r,PSL$C_KERNEL,0,0);
    if (sts == SS$_NORMAL) {
      memset (&fs_create, 0, sizeof fs_create);								// set the 'writer' flag so deassign will write an eof
      fs_create.ignclose = 1;										// ignore closes, just do eof thing when channel deassigned
      sts = oz_sys_io (PSL$C_KERNEL, *h_write_r, 0, IO$_CREATE, sizeof fs_create, &fs_create);
      if (sts == SS$_NORMAL) {
        memset (&fs_open, 0, sizeof fs_open);								// set the 'reader' flag for the read channel
        fs_open.ignclose = 1;
        sts = oz_sys_io (PSL$C_KERNEL, *h_read_r, 0, IO$_ACCESS, sizeof fs_open, &fs_open);
      }
      if (sts != SS$_NORMAL) oz_sys_handle_release (PSL$C_KERNEL, *h_read_r);
    }
    if (sts != SS$_NORMAL) oz_sys_handle_release (PSL$C_KERNEL, *h_write_r);
  }

  return (sts);
#endif
}

/************************************************************************/
/*									*/
/*  This routine closes down the run options' related handles		*/
/*									*/
/************************************************************************/

static void cleanup_runopts (Runopts *runopts)

{
  /* If the thread got going and they want the times printed, print them out now */

  if (runopts -> timeit && (runopts -> h_thread != 0)) {
    char t_name[OZ_THREAD_NAMESIZE];
    unsigned long long t_tis_com, t_tis_run, t_tis_wev;
    unsigned long sts;

    unsigned long t_items[] = {
	JPI$_PRCNAM,    sizeof t_name,        t_name,       NULL, 
	JPI$M_FILL1, sizeof t_tis_run,    &t_tis_run,    NULL, 
	JPI$M_FILL1, sizeof t_tis_com,    &t_tis_com,    NULL, 
	JPI$M_FILL1, sizeof t_tis_wev,    &t_tis_wev,    NULL };

    sts = oz_sys_handle_getinfo (runopts -> h_thread, 4, t_items, NULL);
    if (sts != SS$_NORMAL) fprintf (h_s_error, "oz_cli: error %u getting thread info\n", sts);
    else fprintf (h_s_error, "oz_cli: %#.3t/%#.3t/%#.3t  %s\n", t_tis_run, t_tis_com, t_tis_wev, t_name);
  }

  /* Close sub-process' error/input/output handles                           */
  /* If there was an -ersym and/or -outsym pipe, this should write the EOF's */

  if ((runopts -> error_name  != NULL) || (runopts -> ersym_name  != NULL)) {
    oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_err);
  }
  if ((runopts -> input_name  != NULL) || (runopts -> insym_name  != NULL)) {
    oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_in);
  }
  if ((runopts -> output_name != NULL) || (runopts -> outsym_name != NULL)) {
    oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_out);
  }

  /* Close misc handles */

  if (runopts -> h_exit    != 0) oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_exit);
  if (runopts -> h_init    != 0) oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_init);
  if (runopts -> h_job     != 0) oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_job);
  if (runopts -> h_process != 0) oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_process);
  if (runopts -> h_thread  != 0) oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_thread);

  /* If -insym, close our write handle.  If write hasn't completed yet, this should abort it, so we wait for write to complete. */

  if (runopts -> insym_name != NULL) {
    oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_insym);	// release handle, abort any pending write
    while (runopts -> insym_data != NULL) {				// see if write ast has executed yet
      oz_sys_event_wait (PSL$C_KERNEL, runopts -> h_evsym, 0);	// if not, wait for it
      oz_sys_event_set (PSL$C_KERNEL, runopts -> h_evsym, 0, NULL);	// clear in case of false alarm
    }
  }

  /* If there was an -ersym and/or -outsym, wait for the EOF's then close the read handles and create symbols */

  if (runopts -> h_evsym != 0) {
    while ((runopts -> ersym_pipebuf_qt != NULL) || (runopts -> outsym_pipebuf_qt != NULL)) {
      oz_sys_event_wait (PSL$C_KERNEL, runopts -> h_evsym, 0);
      oz_sys_event_set (PSL$C_KERNEL, runopts -> h_evsym, 0, NULL);
    }
    if (runopts -> ersym_name  != NULL) {
      oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_ersym);
      deferoutsym (runopts -> ersym_name,  runopts -> ersym_pipebuf_qh);
    }
    if (runopts -> outsym_name != NULL) {
      oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_outsym);
      deferoutsym (runopts -> outsym_name, runopts -> outsym_pipebuf_qh);
    }
  }

  /* Done with the -*sym event flag */

  if (runopts -> h_evsym != 0) oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_evsym);

  /* Nothing in here is any good any more */

  memset (runopts, 0, sizeof *runopts);
}

/************************************************************************/
/*									*/
/*  Read error symbol data into string of buffers			*/
/*									*/
/************************************************************************/

static void start_ersym_read (Runopts *runopts)

{
#if 0
  OZ_IO_fs_readrec fs_readrec;
  Pipebuf *pipebuf;
  unsigned long sts;

  pipebuf = malloc (sizeof *pipebuf);
  pipebuf -> runopts = runopts;
  memset (&fs_readrec, 0, sizeof fs_readrec);
  fs_readrec.size = sizeof pipebuf -> data;
  fs_readrec.buff = pipebuf -> data;
  fs_readrec.rlen = &(pipebuf -> rlen);
  sts = oz_sys_io_start (PSL$C_KERNEL, runopts -> h_ersym, NULL, 0, ersym_read_ast, pipebuf, 
                         IO$_READLBLK, sizeof fs_readrec, &fs_readrec);
  if (sts != SS$_ALLSTARTED) oz_sys_thread_queueast (PSL$C_KERNEL, 0, ersym_read_ast, pipebuf, sts);
}

static void ersym_read_ast (void *pipebufv, unsigned long status, void *mchargs)

{
  Pipebuf *pipebuf;
  Runopts *runopts;

  pipebuf = pipebufv;
  runopts = pipebuf -> runopts;

  if (status == SS$_NORMAL) {
    *(runopts -> ersym_pipebuf_qt) = pipebuf;
    runopts -> ersym_pipebuf_qt = &(pipebuf -> next);
    start_ersym_read (runopts);
  } else {
    free (pipebuf);
    *(runopts -> ersym_pipebuf_qt) = NULL;
    runopts -> ersym_pipebuf_qt = NULL;
    if (status != SS$_ENDOFFILE) {
      fprintf (h_s_error, "oz_cli: error %u reading %s error pipe\n", status, runopts -> ersym_name);
    }
    oz_sys_event_set (PSL$C_KERNEL, runopts -> h_evsym, 1, NULL);
  }
#endif
}

/************************************************************************/
/*									*/
/*  Write input symbol data from symbol buffer				*/
/*									*/
/************************************************************************/

static void start_insym_write (Runopts *runopts)

{
#if 0
  int len;
  OZ_IO_fs_writerec fs_writerec;
  unsigned long sts;

  memset (&fs_writerec, 0, sizeof fs_writerec);

  len = strlen (runopts -> insym_data);
  if ((len == 0) || (runopts -> insym_data[len-1] != '\n')) {
    fs_writerec.trmsize = 1;
    fs_writerec.trmbuff = "\n";
  }
  fs_writerec.size = len;
  fs_writerec.buff = runopts -> insym_data;

  sts = oz_sys_io_start (PSL$C_KERNEL, runopts -> h_insym, NULL, 0, insym_write_ast, runopts, 
                         IO$_WRITELBLK, sizeof fs_writerec, &fs_writerec);
  if (sts != SS$_ALLSTARTED) insym_write_ast (runopts, sts, NULL);
#endif
}

static void insym_write_ast (void *runoptsv, unsigned long status, void *mchargs)

{
#if 0
  Runopts *runopts;

  runopts = runoptsv;
  if (status != SS$_NORMAL) {
    fprintf (h_s_error, "oz_cli: error %u writing %s input pipe\n", status, runopts -> insym_name);
  }
  free (runopts -> insym_data);
  runopts -> insym_data = NULL;
  oz_sys_handle_release (PSL$C_KERNEL, runopts -> h_insym);
  runopts -> h_insym = 0;
  oz_sys_event_set (PSL$C_KERNEL, runopts -> h_evsym, 1, NULL);
#endif
}

/************************************************************************/
/*									*/
/*  Read output symbol data into string of buffers			*/
/*									*/
/************************************************************************/

static void start_outsym_read (Runopts *runopts)

{
#if 0
  OZ_IO_fs_readrec fs_readrec;
  Pipebuf *pipebuf;
  unsigned long sts;

  pipebuf = malloc (sizeof *pipebuf);
  pipebuf -> runopts = runopts;
  memset (&fs_readrec, 0, sizeof fs_readrec);
  fs_readrec.size = sizeof pipebuf -> data;
  fs_readrec.buff = pipebuf -> data;
  fs_readrec.rlen = &(pipebuf -> rlen);
  sts = oz_sys_io_start (PSL$C_KERNEL, runopts -> h_outsym, NULL, 0, outsym_read_ast, pipebuf, 
                         IO$_READLBLK, sizeof fs_readrec, &fs_readrec);
  if (sts != SS$_ALLSTARTED) oz_sys_thread_queueast (PSL$C_KERNEL, 0, outsym_read_ast, pipebuf, sts);
#endif
}

static void outsym_read_ast (void *pipebufv, unsigned long status, void *mchargs)

{
#if 0
  Pipebuf *pipebuf;
  Runopts *runopts;

  pipebuf = pipebufv;
  runopts = pipebuf -> runopts;

  if (status == SS$_NORMAL) {
    *(runopts -> outsym_pipebuf_qt) = pipebuf;
    runopts -> outsym_pipebuf_qt = &(pipebuf -> next);
    start_outsym_read (runopts);
  } else {
    free (pipebuf);
    *(runopts -> outsym_pipebuf_qt) = NULL;
    runopts -> outsym_pipebuf_qt = NULL;
    if (status != SS$_ENDOFFILE) {
      fprintf (h_s_error, "oz_cli: error %u reading %s error pipe\n", status, runopts -> outsym_name);
    }
    oz_sys_event_set (PSL$C_KERNEL, runopts -> h_evsym, 1, NULL);
  }
#endif
}

/************************************************************************/
/*									*/
/*  Define -ersym/-outsym symbol					*/
/*									*/
/************************************************************************/

static void deferoutsym (const char *sym_name, Pipebuf *pipebuf_qh)

{
  Pipebuf *pipebuf;
  Symbol *symbol;
  unsigned long rlen;

  rlen = 1;								// tally up all buffers plus a terminating null
  for (pipebuf = pipebuf_qh; pipebuf != NULL; pipebuf = pipebuf -> next) rlen += pipebuf -> rlen;

  symbol = malloc (rlen + sizeof *symbol);				// allocate a symbol struct for it
  memset (symbol, 0, sizeof *symbol);					// clear the header portion
  strncpy (symbol -> name, sym_name, sizeof symbol -> name);		// copy in the symbol name
  symbol -> symtype = SYMTYPE_STRING;					// set the symbol type to string
  rlen = 0;								// use this for offset in symbol value buffer
  while ((pipebuf = pipebuf_qh) != NULL) {				// repeat as long as there are pipe buffers to process
    pipebuf_qh = pipebuf -> next;					// unlink pipe buffer
    memcpy (symbol -> svalue + rlen, pipebuf -> data, pipebuf -> rlen);	// copy the data
    rlen += pipebuf -> rlen;						// increment offset for next one
    free (pipebuf);							// free off this one
  }
  if ((rlen > 0) && (symbol -> svalue[rlen-1] == '\n')) -- rlen;	// remove a trailing \n
  symbol -> svalue[rlen] = 0;						// null terminate the string

  insert_symbol (symbol, NULL, 0);					// insert symbol into symbol table
}

/************************************************************************/
/*									*/
/*	script <script_name> [<args> ...]				*/
/*									*/
/************************************************************************/

static unsigned long int_script (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
#if 0
  unsigned long sts;
  unsigned long h_script;
  OZ_IO_fs_open fs_open;
  Script *script;

  /* -input option is illegal */

  if (h_input != h_s_input) {
    fprintf (h_error, "oz_cli: -input option illegal on '%s' command\n", name);
    return (SS$_BADPARAM);
  }

  /* Make sure we at least have a script name */

  if (argc == 0) {
    fprintf (h_error, "oz_cli: missing script name\n");
    return (SS$_IVPARAM);
  }

  /* Try to open the script file */

  memset (&fs_open, 0, sizeof fs_open);
  fs_open.name = argv[0];
  fs_open.lockmode = LCK$K_CRMODE;
  fs_open.ignclose = 1;
  sts = oz_sys_io_fs_open (sizeof fs_open, &fs_open, 0, &h_script);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u opening script %s\n", sts, argv[0]);
    return (sts);
  }

  /* Ok, stack the current input file, labels and symbols */

  script = malloc (sizeof *script);
  script -> next = scripts;
  script -> h_input  = h_s_input;
  script -> h_output = h_s_output;
  script -> h_error  = h_s_error;
  script -> symbols  = symbols;
  script -> labels   = labels;
  scripts = script;

  /* Set up new script and symbols */

  h_s_input  = h_script;
  h_s_output = h_output;
  h_s_error  = h_error;
  symbols    = NULL;
  labels     = NULL;

  setscriptsyms (argc, argv);

  /* Success */

  return (SS$_NORMAL);
#endif
}

/************************************************************************/
/*									*/
/*	set console [-columns <columns>] [-[no]linewrap] [-rows <rows>] [<logical_name>]
/*									*/
/************************************************************************/

static unsigned long int_set_console (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
#if 0
  char devname[OZ_DEVUNIT_NAMESIZE];
  int i, usedup;
  OZ_Console_modebuff modebuff;
  unsigned long h_console, h_logname, h_table;
  OZ_IO_console_setmode console_setmode;
  unsigned long logvalatr, sts;

  h_console = h_s_console;

  memset (&modebuff, 0, sizeof modebuff);

  for (i = 0; i < argc; i ++) {

    /* -columns - specify number of columns on terminal screen */

    if (strcmp (argv[i], "-columns") == 0) {
      if (++ i >= argc) {
        fprintf (h_error, "oz_cli: missing column number\n");
        sts = SS$_IVPARAM;
        goto rtnsts;
      }
      modebuff.columns = oz_hw_atoi (argv[i], &usedup);
      if ((usedup == 0) || (argv[i][usedup] != 0)) {
        fprintf (h_error, "oz_cli: bad column number %s\n", argv[i]);
        sts = SS$_BADPARAM;
        goto rtnsts;
      }
      continue;
    }

    /* -linewrap - tell it to wrap line if go off end */

    if (strcmp (argv[i], "-linewrap") == 0) {
      modebuff.linewrap = 1;
      continue;
    }

    /* -nolinewrap - tell it to chop line if go off end */

    if (strcmp (argv[i], "-nolinewrap") == 0) {
      modebuff.linewrap = -1;
      continue;
    }

    /* -rows - specify number of rows on screen */

    if (strcmp (argv[i], "-rows") == 0) {
      if (++ i >= argc) {
        fprintf (h_error, "oz_cli: missing row number\n");
        sts = SS$_IVPARAM;
        goto rtnsts;
      }
      modebuff.rows = oz_hw_atoi (argv[i], &usedup);
      if ((usedup == 0) || (argv[i][usedup] != 0)) {
        fprintf (h_error, "oz_cli: bad row number %s\n", argv[i]);
        sts = SS$_BADPARAM;
        goto rtnsts;
      }
      continue;
    }

    /* End of options */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      sts = SS$_BADPARAM;
      goto rtnsts;
    }

    /* First and only arg is console device name */

    if (h_console == h_s_console) {
      sts = sys$trnlnm (0, 0,  defaulttables,  PSL$C_USER,  &h_table);
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u looking up default tables (%s)\n", sts, defaulttables);
        goto rtnsts;
      }
      sts = sys$trnlnm (0, h_table,  argv[i],  PSL$C_USER,  &h_logname);
      oz_sys_handle_release (PSL$C_KERNEL, h_table);
      if (sts == SS$_NOLOGNAME) {
	//        sts = sys$assign( argv[i], &h_console,PSL$C_KERNEL,0,0);
        if (sts != SS$_NORMAL) {
          fprintf (h_error, "oz_cli: error %u assigning channel to console %s\n", sts, argv[i]);
          goto rtnsts;
        }
        continue;
      }
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u looking up logical %s\n", sts, argv[i]);
        goto rtnsts;
      }
      sts = sys$trnlnm (0, h_logname,  NULL,  0,  NULL);
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u getting logical %s attributes\n", sts, argv[i]);
        oz_sys_handle_release (PSL$C_KERNEL, h_logname);
        goto rtnsts;
      }
      if (logvalatr & 1/*OZ_LOGVALATR_OBJECT*/) {
        sts = sys$trnlnm (0, h_logname, 0,  0,  NULL);
        oz_sys_handle_release (PSL$C_KERNEL, h_logname);
        if (sts != SS$_NORMAL) {
          fprintf (h_error, "oz_cli: error %u getting console iochan from logical %s\n", sts, argv[i]);
          goto rtnsts;
        }
      } else {
        sts = sys$trnlnm (0, h_logname, 0,  sizeof devname,  NULL);
        oz_sys_handle_release (PSL$C_KERNEL, h_logname);
        if (sts != SS$_NORMAL) {
          fprintf (h_error, "oz_cli: error %u getting console devname from logical %s\n", sts, argv[i]);
          goto rtnsts;
        }
	//        sts = sys$assign( devname, &h_console,PSL$C_KERNEL,0,0);
        if (sts != SS$_NORMAL) {
          fprintf (h_error, "oz_cli: error %u assigning channel to console %s\n", sts, devname);
          goto rtnsts;
        }
      }
      continue;
    }

    /* No other args allowed */

    fprintf (h_error, "oz_cli: extra parameter %s\n", argv[i]);
    sts = SS$_BADPARAM;
    goto rtnsts;
  }

  memset (&console_setmode, 0, sizeof console_setmode);
  console_setmode.size = sizeof modebuff;
  console_setmode.buff = &modebuff;
  sts = oz_sys_io (PSL$C_KERNEL, h_console, 0, IO$_SETMODE, sizeof console_setmode, &console_setmode);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u setting console mode\n", sts);

rtnsts:
  if (h_console != h_s_console) oz_sys_handle_release (PSL$C_KERNEL, h_console);
  return (sts);
#endif
}

#endif


/************************************************************************/
/*									*/
/*	set default <directory>						*/
/*									*/
/************************************************************************/

static unsigned long int_set_default (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char dirnambuff[OZ_DEVUNIT_NAMESIZE+OZ_FS_MAXFNLEN];
  const char *xargv[3];
  int i;
  unsigned long sts;
  unsigned long h_dir;
#if 0
  OZ_IO_fs_getinfo1 fs_getinfo1;
  OZ_IO_fs_getinfo2 fs_getinfo2;
  OZ_IO_fs_open fs_open;
#endif

  /* We should have exactly one argument - the directory name */

  if (argc != 1) {
    fprintf (h_error, "oz_cli: missing directory parameter\n");
    return (SS$_IVPARAM);
  }

  chdir(argv[0]);
  return SS$_NORMAL;

  /* Open the directory as a file - but we don't need to access the contents */

#if 0
  memset (&fs_open, 0, sizeof fs_open);
  fs_open.name = argv[0];
  fs_open.lockmode = LCK$K_NLMODE;
  sts = oz_sys_io_fs_open (sizeof fs_open, &fs_open, 0, &h_dir);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u finding directory %s\n", sts, argv[0]);
    return (sts);
  }
#endif
  /* Make sure it is a directory */

#if 0
  memset (&fs_getinfo1, 0, sizeof fs_getinfo1);
  sts = oz_sys_io (PSL$C_KERNEL, h_dir, 0, OZ_IO_FS_GETINFO1, sizeof fs_getinfo1, &fs_getinfo1);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u determining if %s is a directory\n", sts, argv[0]);
    oz_sys_handle_release (PSL$C_KERNEL, h_dir);
    return (sts);
  }
  if (!(fs_getinfo1.filattrflags & OZ_FS_FILATTRFLAG_DIRECTORY)) {
    fprintf (h_error, "oz_cli: %s is not a directory\n", argv[0]);
    oz_sys_handle_release (PSL$C_KERNEL, h_dir);
    return (OZ_FILENOTADIR);
  }
#endif

  /* Get the resultant name */

  sts = oz_sys_iochan_getunitname (h_dir, sizeof dirnambuff, dirnambuff);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u getting directory's device name\n", sts);
    oz_sys_handle_release (PSL$C_KERNEL, h_dir);
    return (sts);
  }
  i = strlen (dirnambuff);
  dirnambuff[i++] = ':';
#if 0
  memset (&fs_getinfo2, 0, sizeof fs_getinfo2);
  fs_getinfo2.filnamsize = sizeof dirnambuff - i;
  fs_getinfo2.filnambuff = dirnambuff + i;
  sts = oz_sys_io (PSL$C_KERNEL, h_dir, 0, OZ_IO_FS_GETINFO2, sizeof fs_getinfo2, &fs_getinfo2);
  oz_sys_handle_release (PSL$C_KERNEL, h_dir);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u getting %s name\n", sts, argv[0]);
    return (sts);
  }
#endif

  /* Create the OZ_DEFAULT_DIR logical name */

  xargv[0] = "-terminal";
  xargv[1] = "OZ_DEFAULT_DIR";
  xargv[2] = dirnambuff;
  sts = int_create_logical_name (h_input, h_output, h_error, name, NULL, 3, xargv);
  if (sts == SS$_SUPERSEDE) sts = SS$_NORMAL;
  return (sts);
}

#if 0


/************************************************************************/
/*									*/
/*	set event interval <logical_name> <interval> [<basetime>]	*/
/*									*/
/************************************************************************/

static unsigned long int_set_event_interval (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  unsigned long long basetime, interval;
  unsigned long h_event;
  unsigned long sts;

  if ((argc != 2) && (argc != 3)) {
    fprintf (h_error, "oz_cli: invalid arguments <logical_name> <interval> [<basetime>]\n");
    return (SS$_IVPARAM);
  }

  if (sys$bintim ( argv[1], &interval) >= 0) {			// decode interval string
    fprintf (h_error, "oz_cli: bad interval %s, must be delta format\n", argv[1]);	// must be in delta format
    return (SS$_BADPARAM);
  }

  sys$gettim(&basetime);

  if (argc > 2) {
    if (sys$bintim ( argv[2], &basetime) <= 0) {			// decode basetime string
      fprintf (h_error, "oz_cli: bad basetime %s, must be absolute\n", argv[2]);
      return (SS$_BADPARAM);
    }
    //    basetime = oz_sys_datebin_tzconv (basetime, OZ_DATEBIN_TZCONV_LCL2UTC, 0);			// convert local to UTC'
  }

  sts = logname_getobj (argv[0], OZ_OBJTYPE_EVENT, &h_event);					// get event flag handle
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u looking up event logical %s\n", sts, argv[0]);
    return (sts);
  }

  sts = oz_sys_event_setimint (h_event, interval, basetime);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u setting event %s interval\n", sts, argv[0]);
  oz_sys_handle_release (PSL$C_KERNEL, h_event);
  return (sts);
}

/************************************************************************/
/*									*/
/*	set mutex [-express] [-noqueue] <iochan_logical_name> <new_mode>*/
/*									*/
/************************************************************************/

static unsigned long int_set_mutex (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name, *new_mode;
  int i;
  unsigned long sts;
  unsigned long h_iochan;
  //  OZ_IO_mutex_setmode mutex_setmode;

  //  memset (&mutex_setmode, 0, sizeof mutex_setmode);

  logical_name = NULL;
  new_mode     = NULL;

  for (i = 0; i < argc; i ++) {
    if (strcmp (argv[i], "-express") == 0) {
      //      mutex_setmode.flags |= OZ_IO_MUTEX_SETMODE_FLAG_EXPRESS;
      continue;
    }
    if (strcmp (argv[i], "-noqueue") == 0) {
      //mutex_setmode.flags |= OZ_IO_MUTEX_SETMODE_FLAG_NOQUEUE;
      continue;
    }
    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_BADPARAM);
    }
    if (logical_name == NULL) {
      logical_name = argv[i];
      continue;
    }
    if (new_mode == NULL) {
      new_mode = argv[i];
      continue;
    }
    fprintf (h_error, "oz_cli: extra parameter %s\n", argv[i]);
    return (SS$_BADPARAM);
  }

  if (logical_name == NULL) {
    fprintf (h_error, "oz_cli: missing logical_name parameter\n");
    return (SS$_IVPARAM);
  }

  if (new_mode == NULL) {
    fprintf (h_error, "oz_cli: missing new_mode parameter\n");
    return (SS$_IVPARAM);
  }

#if 0
  for (mutex_setmode.newmode = 0; mutex_setmode.newmode < (LCK$K_EXMODE+1); mutex_setmode.newmode ++) {
    if (strcasecmp (lockmodes[mutex_setmode.newmode].name, new_mode) == 0) break;
  }
  if (mutex_setmode.newmode == (LCK$K_EXMODE+1)) {
    fprintf (h_error, "oz_cli: bad new_mode parameter %s\n");
    return (SS$_BADPARAM);
  }
#endif

  sts = logname_getobj (logical_name, OZ_OBJTYPE_IOCHAN, &h_iochan);
  if (sts != SS$_NORMAL) return (sts);

#if 0
  sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, OZ_IO_MUTEX_SETMODE, sizeof mutex_setmode, &mutex_setmode);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u setting mutex %s to %s\n", sts, logical_name, new_mode);
#endif

  oz_sys_handle_release (PSL$C_KERNEL, h_iochan);

  return (sts);
}

/************************************************************************/
/*									*/
/*	set thread [-id thread_id] [threadlogical] [-seckeys seckeys]	*/
/*									*/
/************************************************************************/

static unsigned long int_set_thread (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *createsstr, *logical_name, *secattrstr, *seckeysstr;
  int i, usedup;
  unsigned long h_thread;
  unsigned long thread_id;
  unsigned long basepri, sts;

  basepri      = 0;
  createsstr   = NULL;
  logical_name = LASTHREADLNM;
  secattrstr   = NULL;
  seckeysstr   = NULL;
  thread_id    = 0;

  for (i = 0; i < argc; i ++) {

    /* Default create security attributes */

    if (strcmp (argv[i], "-creates") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing default create security attributes after -creates\n");
        return (SS$_IVPARAM);
      }
      createsstr = argv[i];
      continue;
    }

    /* Id - thread id number */

    if (strcmp (argv[i], "-id") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing thread id number after -id\n");
        return (SS$_IVPARAM);
      }
      thread_id = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad thread id number %s after -id\n", argv[i]);
        return (SS$_BADPARAM);
      }
      logical_name = NULL;
      continue;
    }

    /* Priority - base priority */

    if (strcmp (argv[i], "-priority") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing base priority number after -priority\n");
        return (SS$_IVPARAM);
      }
      basepri = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad base priority number %s after -priority\n", argv[i]);
        return (SS$_BADPARAM);
      }
      continue;
    }

    /* Security attributes */

    if (strcmp (argv[i], "-secattr") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing security attributes after -secattr\n");
        return (SS$_IVPARAM);
      }
      secattrstr = argv[i];
      continue;
    }

    /* Security keys */

    if (strcmp (argv[i], "-seckeys") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing security keys after -seckeys\n");
        return (SS$_IVPARAM);
      }
      seckeysstr = argv[i];
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    logical_name = argv[i];
    thread_id    = 0;
  }

  /* Get handle to thread from logical name.  If no logical name given, use last thread's handle. */

  if (thread_id != 0) {
    sts = oz_sys_thread_getbyid (thread_id, &h_thread);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting handle to thread id %u\n", sts, thread_id);
      return (sts);
    }
  } else {
    sts = logname_getobj (logical_name, OZ_OBJTYPE_THREAD, &h_thread);
    if (sts != SS$_NORMAL) return (sts);
  }

  /* Maybe change the base priority */

  if (basepri != 0) {
    sts = oz_sys_thread_setbasepri (h_thread, basepri);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u setting thread's base priority\n", sts);
      goto rtnsts;
    }
  }

  /* Maybe change the default create security attributes (who can access what the thread creates) */

  if (createsstr != NULL) {
    unsigned long secattrsize;
    void *secattrbuff;

    sts = oz_sys_secattr_str2bin (strlen (createsstr), createsstr, NULL, secmalloc, NULL, &secattrsize, &secattrbuff);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u parsing secattr %s\n", sts, createsstr);
      goto rtnsts;
    }
    sts = oz_sys_thread_setdefcresecattr (h_thread, secattrsize, secattrbuff);
    free (secattrbuff);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u setting thread's defcresecattr\n", sts);
      goto rtnsts;
    }
  }

  /* Maybe change the security attributes (who can access the thread) */

  if (secattrstr != NULL) {
    unsigned long secattrsize;
    void *secattrbuff;

    sts = oz_sys_secattr_str2bin (strlen (secattrstr), secattrstr, NULL, secmalloc, NULL, &secattrsize, &secattrbuff);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u parsing secattr %s\n", sts, secattrstr);
      goto rtnsts;
    }
    sts = oz_sys_thread_setsecattr (h_thread, secattrsize, secattrbuff);
    free (secattrbuff);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u setting thread's secattr\n", sts);
      goto rtnsts;
    }
  }

  /* Maybe change the security keys (what the thread can access) */

  if (seckeysstr != NULL) {
    unsigned long seckeyssize;
    void *seckeysbuff;

    sts = oz_sys_seckeys_str2bin (strlen (seckeysstr), seckeysstr, secmalloc, NULL, &seckeyssize, &seckeysbuff);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u parsing seckeys %s\n", sts, seckeysstr);
      goto rtnsts;
    }
    sts = oz_sys_thread_setseckeys (h_thread, seckeyssize, seckeysbuff);
    free (seckeysbuff);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u setting thread's seckeys\n", sts);
      goto rtnsts;
    }
  }

  /* Done */

rtnsts:
  oz_sys_handle_release (PSL$C_KERNEL, h_thread);
  return (sts);
}


#endif

/************************************************************************/
/*									*/
/*	show datetime							*/
/*									*/
/************************************************************************/

static unsigned long int_show_datetime (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  unsigned long long now;
  char timestr[23]; 
  $DESCRIPTOR(atimenow, timestr); 

  if (argc != 0) {
    fprintf (h_error, "oz_cli: %s must have no arguments\n", name);
    return (SS$_BADPARAM);
  }

  sys$gettim(&now);
  sys$asctim(0,&atimenow,&now,0);
  fprintf (h_output, "%s\n",atimenow.dsc$a_pointer);
  return (SS$_NORMAL);
}


/************************************************************************/
/*									*/
/*	show default							*/
/*									*/
/************************************************************************/

static unsigned long int_show_default (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *xargv[1];
  unsigned long sts;
  char buf[80];

  getcwd(buf,80);

  printf("%s\n",buf);

  return SS$_NORMAL;

  xargv[0] = "OZ_DEFAULT_DIR";
  sts = int_show_logical_name (h_input, h_output, h_error, name, NULL, 1, xargv);
  return (sts);
}


/************************************************************************/
/*									*/
/*	show device [<device_logical_name> ...]				*/
/*									*/
/*		-iochans : show iochans of the devices			*/
/*									*/
/************************************************************************/

static unsigned long int_show_device (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])
     // eventually move something to showdevice.c
{
  int i, x;
  unsigned long showopts, sts;

  x = 0;
  showopts = 0;

  show_device();
  return SS$_NORMAL;

  for (i = 0; i < argc; i ++) {

    /* Iochans - display iochan information for the device */

    if (strcmp (argv[i], "-iochans") == 0) {
      showopts |= SHOWOPT_DEVICE_IOCHANS;
      continue;
    }

    /* Objaddr - show objects kernel memory address */

    if (strcmp (argv[i], "-objaddr") == 0) {
      showopts |= SHOWOPT_OBJADDR;
      continue;
    }

    /* Security */

    if (strcmp (argv[i], "-security") == 0) {
      showopts |= SHOWOPT_SECURITY;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is a device logical name */

    x = 1;
    sts = show_device_bylogical (h_output, h_error, argv[i], &showopts);
    if (sts != SS$_NORMAL) break;
  }

  return (sts);
}

#if 0

/************************************************************************/
/*									*/
/*  Display device info given logical name				*/
/*									*/
/************************************************************************/

static unsigned long show_device_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts)

{
  unsigned long sts;
  unsigned long h_device;

  sts = logname_getobj (logical, OZ_OBJTYPE_UNKNOWN, &h_device);
  //  if (sts != SS$_NORMAL) sts = sys$assign( logical, &h_device,PSL$C_KERNEL,0,0);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting device %s\n", sts, logical);
  else {
    sts = show_device_byhandle (h_output, h_error, h_device, showopts);
    oz_sys_handle_release (PSL$C_KERNEL, h_device);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Display info for a device given its handle				*/
/*									*/
/************************************************************************/

static unsigned long show_device_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_device, unsigned long *showopts)

{
  unsigned long index, sts;
  unsigned long h_lastiochan, h_nextiochan;

  char d_aliasname[OZ_DEVUNIT_ALIASIZE], d_unitdesc[OZ_DEVUNIT_DESCSIZE], d_unitname[OZ_DEVUNIT_NAMESIZE];
  signed long d_refcount;
  unsigned long d_iochancount;
  void *d_objaddr;

  unsigned long items[] = { 
        JPI$M_FILL1,    sizeof d_refcount,    &d_refcount,    NULL, 
        JPI$M_FILL1, sizeof d_iochancount, &d_iochancount, NULL, 
	JPI$M_FILL1,   sizeof d_aliasname,    d_aliasname,   NULL, 
        JPI$M_FILL1,    sizeof d_unitname,     d_unitname,    NULL, 
	JPI$M_FILL1,    sizeof d_unitdesc,     d_unitdesc,    NULL, 
        JPI$M_FILL1,            sizeof d_objaddr,     &d_objaddr,     NULL };

  unsigned long i_item = { JPI$M_FILL1, sizeof h_nextiochan, &h_nextiochan, NULL };

  sts = oz_sys_handle_getinfo (h_device, sizeof items / sizeof *items, items, &index);

  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting device info [%u]\n", sts, index);
  else {
    if (!(*showopts & SHOWNHDR_DEVICE)) {
      *showopts |= SHOWNHDR_DEVICE;
      if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "          ");
      fprintf (h_output, "D  refc  iochans  aliasname  unitname      unitdesc\n");
    }
    if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "%8x: ", d_objaddr);
    fprintf (h_output, "d  %4d  %7u  %-9s  %-12s  %s\n", d_refcount, d_iochancount, d_aliasname, d_unitname, d_unitdesc);
    if (*showopts & SHOWOPT_SECURITY) show_secattr (h_output, h_device, JPI$M_FILL1, 2, "");
    if (*showopts & SHOWOPT_DEVICE_IOCHANS) {
      sts = oz_sys_handle_getinfo (h_device, 1, &i_item, &index);
      //      i_item.code = JPI$M_FILL1;
      while ((sts == SS$_NORMAL) && (h_nextiochan != 0)) {
        h_lastiochan = h_nextiochan;
        sts = show_iochan_byhandle (h_output, h_error, h_nextiochan, showopts);
        if (sts == SS$_NORMAL) sts = oz_sys_handle_getinfo (h_lastiochan, 1, &i_item, &index);
        oz_sys_handle_release (PSL$C_KERNEL, h_lastiochan);
      }
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting first/next iochan for device\n", sts);
    }
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	show event <event_logname> ...					*/
/*									*/
/************************************************************************/

static unsigned long int_show_event (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char intervalstr[32], evname[OZ_EVENT_NAMESIZE], nextwhenstr[32];
  int i;
  signed long value;
  unsigned long long interval, nextwhen;
  unsigned long h_event;
  unsigned long sts;

  unsigned long items[] = { 
        JPI$M_FILL1,    sizeof value,    &value,    NULL, 
	JPI$M_FILL1,     sizeof evname,    evname,   NULL, 
        JPI$M_FILL1, sizeof interval, &interval, NULL, 
	JPI$M_FILL1, sizeof nextwhen, &nextwhen, NULL };

  for (i = 0; i < argc; i ++) {

    sts = logname_getobj (argv[i], OZ_OBJTYPE_EVENT, &h_event);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u looking up event logical %s\n", sts, argv[i]);
      return (sts);
    }

    sts = oz_sys_handle_getinfo (h_event, sizeof items / sizeof items[0], items, NULL);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u retrieving event %s info\n", sts, argv[i]);
      return (sts);
    }

    if (i == 0) {
      fprintf (h_output, "E  %24s  %27s  %10s  %s\n", "interval", "nextwhen", "value", "name");
    }

    if (interval == 0) {
      strcpy (intervalstr, "");
      strcpy (nextwhenstr, "");
    } else {
      sys$asctim (32, intervalstr, &interval, 0);
      //      nextwhen = oz_sys_datebin_tzconv (nextwhen, OZ_DATEBIN_TZCONV_UTC2LCL, 0);
      sys$asctim (32, nextwhenstr, &nextwhen, 0);
    }

    fprintf (h_output, "e  %24s  %27s  %10d  %s\n", intervalstr, nextwhenstr, value, evname);
    oz_sys_handle_release (PSL$C_KERNEL, h_event);
  }

  return (SS$_NORMAL);
}

/************************************************************************/
/*									*/
/*	show iochan [<iochan_logical_name> ...]				*/
/*									*/
/************************************************************************/

static unsigned long int_show_iochan (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, x;
  unsigned long showopts, sts;

  x = 0;
  showopts = 0;

  for (i = 0; i < argc; i ++) {

    /* Objaddr - show objects kernel memory address */

    if (strcmp (argv[i], "-objaddr") == 0) {
      showopts |= SHOWOPT_OBJADDR;
      continue;
    }

    /* Security */

    if (strcmp (argv[i], "-security") == 0) {
      showopts |= SHOWOPT_SECURITY;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is a iochan logical name */

    x = 1;
    sts = show_iochan_bylogical (h_output, h_error, argv[i], &showopts);
    if (sts != SS$_NORMAL) break;
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*  Display iochan info given logical name				*/
/*									*/
/************************************************************************/

static unsigned long show_iochan_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts)

{
  unsigned long sts;
  unsigned long h_iochan;

  sts = logname_getobj (logical, OZ_OBJTYPE_UNKNOWN, &h_iochan);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting iochan logical %s\n", sts, logical);
  else {
    sts = show_iochan_byhandle (h_output, h_error, h_iochan, showopts);
    oz_sys_handle_release (PSL$C_KERNEL, h_iochan);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Display info for a iochan given its handle				*/
/*									*/
/************************************************************************/

static unsigned long show_iochan_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_iochan, unsigned long *showopts)

{
  char lockmode[8];
  unsigned long index, sts;

  signed long i_refcount;
  unsigned long i_lockmode;
  void *i_objaddr;

  unsigned long items[] = { 
        JPI$M_FILL1, sizeof i_refcount, &i_refcount, NULL, 
        JPI$M_FILL1, sizeof i_lockmode, &i_lockmode, NULL, 
        JPI$M_FILL1,         sizeof i_objaddr,  &i_objaddr,  NULL };

  sts = oz_sys_handle_getinfo (h_iochan, sizeof items / sizeof *items, items, &index);

  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting iochan info [%u]\n", sts, index);
  else {
    if (!(*showopts & SHOWNHDR_IOCHAN)) {
      *showopts |= SHOWNHDR_IOCHAN;
      if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "          ");
      fprintf (h_output, "I    refc  lockmode\n");
    }
    switch (i_lockmode) {
      case LCK$K_NLMODE: { strcpy (lockmode, "NL"); break; }
      case LCK$K_CRMODE: { strcpy (lockmode, "CR"); break; }
      case LCK$K_CWMODE: { strcpy (lockmode, "CW"); break; }
      case LCK$K_PRMODE: { strcpy (lockmode, "PR"); break; }
      case LCK$K_PWMODE: { strcpy (lockmode, "PW"); break; }
      case LCK$K_EXMODE: { strcpy (lockmode, "EX"); break; }
      default: { snprintf (lockmode, sizeof lockmode, "%d", i_lockmode); };
    }
    if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "%8x: ", i_objaddr);
    fprintf (h_output, "i    %4d  %8s\n", i_refcount, lockmode);
    if (*showopts & SHOWOPT_SECURITY) show_secattr (h_output, h_iochan, JPI$M_FILL1, 4, "");
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	show job [<job_logical_name> ...]				*/
/*									*/
/*		-processes						*/
/*		-threads						*/
/*									*/
/************************************************************************/

static unsigned long int_show_job (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, x;
  unsigned long showopts, sts;

  x = 0;
  showopts = 0;

  for (i = 0; i < argc; i ++) {

    /* Objaddr - show objects kernel memory address */

    if (strcmp (argv[i], "-objaddr") == 0) {
      showopts |= SHOWOPT_OBJADDR;
      continue;
    }

    /* Processes - display process information for the job */

    if (strcmp (argv[i], "-processes") == 0) {
      showopts |= SHOWOPT_JOB_PROCS;
      continue;
    }

    /* Security */

    if (strcmp (argv[i], "-security") == 0) {
      showopts |= SHOWOPT_SECURITY;
      continue;
    }

    /* Threads - display thread information for the process */

    if (strcmp (argv[i], "-threads") == 0) {
      showopts |= SHOWOPT_JOB_THREADS;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is a job logical name */

    x = 1;
    sts = show_job_bylogical (h_output, h_error, argv[i], &showopts);
    if (sts != SS$_NORMAL) break;
  }

  if (x == 0) sts = show_job_bylogical (h_output, h_error, "OZ_THIS_JOB", &showopts);

  return (sts);
}

/************************************************************************/
/*									*/
/*  Display job info given logical name					*/
/*									*/
/************************************************************************/

static unsigned long show_job_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts)

{
  unsigned long sts;
  unsigned long h_job;

  sts = logname_getobj (logical, OZ_OBJTYPE_UNKNOWN, &h_job);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting job logical %s\n", sts, logical);
  else {
    sts = show_job_byhandle (h_output, h_error, h_job, showopts);
    oz_sys_handle_release (PSL$C_KERNEL, h_job);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Display info for a job given its handle				*/
/*									*/
/************************************************************************/

static unsigned long show_job_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_job, unsigned long *showopts)

{
  unsigned long index, sts;
  unsigned long h_lastproc, h_nextproc;

  char j_name[OZ_JOBNAME_MAX];
  signed long j_refcount;
  unsigned long j_processcount;
  void *j_objaddr;

  unsigned long items[] = { 
	JPI$M_FILL1,     sizeof j_refcount,     &j_refcount,     NULL, 
	JPI$M_FILL1, sizeof j_processcount, &j_processcount, NULL, 
	JPI$M_FILL1,         sizeof j_name,          j_name,         NULL, 
	JPI$M_FILL1,          sizeof j_objaddr,      &j_objaddr,      NULL };

  unsigned long p_item = { JPI$M_FILL1, sizeof h_nextproc, &h_nextproc, NULL };

  sts = oz_sys_handle_getinfo (h_job, sizeof items / sizeof *items, items, &index);

  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting job info [%u]\n", sts, index);
  else {
    if (!(*showopts & SHOWNHDR_JOB)) {
      *showopts |= SHOWNHDR_JOB;
      if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "          ");
      fprintf (h_output, "J    refc  processes  name\n");
    }
    if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "%8x: ", j_objaddr);
    fprintf (h_output, "j    %4d  %9u  %s\n", j_refcount, j_processcount, j_name);
    if (*showopts & SHOWOPT_SECURITY) show_secattr (h_output, h_job, JPI$M_FILL1, 4, "");
    if (*showopts & SHOWOPT_JOB_PROCS) {
      sts = oz_sys_handle_getinfo (h_job, 1, &p_item, &index);
      //      p_item.code = JPI$M_FILL1;
      while ((sts == SS$_NORMAL) && (h_nextproc != 0)) {
        h_lastproc = h_nextproc;
        sts = show_process_byhandle (h_output, h_error, h_nextproc, showopts);
        if (sts == SS$_NORMAL) sts = oz_sys_handle_getinfo (h_lastproc, 1, &p_item, &index);
        oz_sys_handle_release (PSL$C_KERNEL, h_lastproc);
      }
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting first/next process in job\n", sts);
    }
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	show logical name <logical_name> [-security]			*/
/*									*/
/************************************************************************/

static unsigned long int_show_logical_name (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i;
  unsigned long sholnmflg, sts;
  unsigned long h_logname, h_table;

  logical_name = NULL;
  sholnmflg    = 0;

  for (i = 0; i < argc; i ++) {

    /* - security */

    if (strcmp (argv[i], "-security") == 0) {
      sholnmflg |= SHOLNMFLG_SECURITY;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    logical_name = argv[i];
  }

  if (logical_name == NULL) {
    fprintf (h_error, "oz_cli: missing logical name\n");
    return (SS$_IVPARAM);
  }

  sts = sys$trnlnm (0, 0,  defaulttables,  PSL$C_USER,  &h_table);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u looking up default tables (%s)\n", sts, defaulttables);
  else {
    sts = sys$trnlnm (0, h_table,  logical_name,  PSL$C_USER,  &h_logname);
    if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u looking up logical %s\n", sts, logical_name);
    else {
      sts = show_logical_name (h_output, h_error, 0, sholnmflg, 1, h_logname);
      oz_sys_handle_release (PSL$C_KERNEL, h_logname);
    }
    oz_sys_handle_release (PSL$C_KERNEL, h_table);
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	show logical table <table_name>					*/
/*									*/
/************************************************************************/

static unsigned long int_show_logical_table (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *table_name;
  int i;
  unsigned long sholnmflg, sts;

  sholnmflg  = 0;
  table_name = defaulttables;

  for (i = 0; i < argc; i ++) {

    /* - security */

    if (strcmp (argv[i], "-security") == 0) {
      sholnmflg |= SHOLNMFLG_SECURITY;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    table_name = argv[i];
  }

  /* Dump out table and return status */

  sts = show_logical_table (h_output, h_error, 0, sholnmflg, 0, table_name, 0);
  return (sts);
}

/************************************************************************/
/*									*/
/*  Show info for logicals in a table					*/
/*									*/
/*    Input:								*/
/*									*/
/*	logical_name = table's name					*/
/*									*/
/*    Output:								*/
/*									*/
/*	show_logical_table = SS$_NORMAL : successful			*/
/*	                           else : error status			*/
/*									*/
/************************************************************************/

static unsigned long show_logical_table (unsigned long h_output, unsigned long h_error, int level, unsigned long sholnmflg, unsigned long logtblatr, const char *table_name, unsigned long h_table)

{
  char logical_name[OZ_LOGNAME_MAXNAMSZ], object_table_name[OZ_LOGNAME_MAXNAMSZ];
  unsigned long lognamatr, logvalatr, nvalues, i, sts;
  unsigned long h_logical;

  if (level > LNM$C_MAXDEPTH) {
    fprintf (h_error, "oz_cli: too many logical name levels\n");
    return (SS$_EXLNMQUOTA);
  }

  /* If given an handle to the table, get the table's name */

  if (logtblatr & 1/*OZ_LOGVALATR_OBJECT*/) {
    sts = sys$trnlnm (0, h_table,  object_table_name,  sizeof object_table_name,  &nvalues);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting logical table's name\n", sts);
      return (sts);
    }
    table_name = object_table_name;
  }

  /* Otherwise, we were given its name, so look it up in the directories */

  else {
    sts = sys$trnlnm (0, 0,  table_name,  PSL$C_USER,  &h_table);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u looking up table %s in the directories\n", sts, table_name);
      return (sts);
    }
  }

  /* Print out the name of the table */

  fprintf (h_output, "%*s%s:\n", level * 2, "", table_name);
  if (sholnmflg & SHOLNMFLG_SECURITY) show_secattr (h_output, h_table, JPI$M_FILL1, (level + 1) * 2, "");

  /* If it is a table, dump it out */

  if (lognamatr & LNM$M_TABLE) {
    h_logical = 0;
    while (1) {
      //      sts = oz_sys_logname_gettblent (h_table, &h_logical);			/* get logical name from table */
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u retrieving value from table %s\n", sts, table_name);
        break;
      }
      if (h_logical == 0) break;
      sts = show_logical_name (h_output, h_error, level + 1, sholnmflg, 0, h_logical); /* print out the logical */
      if (sts != SS$_NORMAL) break;						/* stop if error */
    }
    if (h_logical != 0) oz_sys_handle_release (PSL$C_KERNEL, h_logical);
  }

  /* Otherwise, it is a list of tables, so recurse */

  else {
    for (i = 0; i < nvalues; i ++) {
      sts = sys$trnlnm (0, h_table, i,  sizeof logical_name,  &logvalatr);
      if (sts == SS$_NORMAL) sts = show_logical_table (h_output, h_error, level + 1, sholnmflg, logvalatr, logical_name, h_logical);
      else fprintf (h_error, "oz_cli: error %u retrieving value %u from %s\n", sts, i, table_name);
#if 0
      if (logvalatr & OZ_LOGVALATR_OBJECT) oz_sys_handle_release (PSL$C_KERNEL, h_logical);
#endif
      if (sts != SS$_NORMAL) break;
    }
  }

  /* If we created the handle, release it */

#if 0
  if (!(logtblatr & OZ_LOGVALATR_OBJECT)) oz_sys_handle_release (PSL$C_KERNEL, h_table);
#endif

  return (sts);
}

/************************************************************************/
/*									*/
/*  Show info for a single logical name					*/
/*									*/
/*    Input:								*/
/*									*/
/*	table = 0 : don't print table name with logical name		*/
/*	        1 : print table name with logical name			*/
/*	h_logname = logical name handle					*/
/*									*/
/*    Output:								*/
/*									*/
/*	show_logical_name = SS$_NORMAL : successful			*/
/*	                          else : error status			*/
/*									*/
/************************************************************************/

static unsigned long show_logical_name (unsigned long h_output, unsigned long h_error, int level, unsigned long sholnmflg, int table, unsigned long h_logname)

{
  char name[OZ_LOGNAME_MAXNAMSZ], table_name[OZ_LOGNAME_MAXNAMSZ], value[256];
  unsigned long i, lognamatr, logvalatr, nvalues, sts;
  unsigned long h_table;
  unsigned long procmode;

  /* Get general info about logical name */

  sts = sys$trnlnm (0, h_logname,  name,  sizeof name,  &nvalues);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u getting logical name attributes\n", sts);
    return (sts);
  }

  /* Output stuff up to the = */

  if (table) {
    sts = sys$trnlnm (0, h_table,  table_name,  sizeof table_name,  NULL);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting logical name %s table name\n", sts, name);
      oz_sys_handle_release (PSL$C_KERNEL, h_table);
      return (sts);
    }
#if 0
    fprintf (h_output, "%*s%s%c%s", level * 2, "", table_name, OZ_LOGNAME_TABLECHR, name);
#endif
  } else {
    fprintf (h_output, "%*s%s", level * 2, "", name);
  }
  if (procmode == PSL$C_KERNEL) fprintf (h_output, " (kernel)");
  else if (procmode == PSL$C_USER) fprintf (h_output, " (user)");
  else fprintf (h_output, " (procmode %d)", procmode);
#if 0
  if (lognamatr & OZ_LOGNAMATR_NOSUPERSEDE) fprintf (h_output, " (nosupersede)");
  if (lognamatr & OZ_LOGNAMATR_NOOUTERMODE) fprintf (h_output, " (nooutermode)");
#endif
  if (lognamatr & LNM$M_TABLE)       fprintf (h_output, " (table)\n");
  else {
    fprintf (h_output, " =");

    /* Output the list of values */

    for (i = 0; i < nvalues; i ++) {
      sts = sys$trnlnm (0, h_logname, i,  sizeof value,  &logvalatr);
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u getting value %u of logical %s\n", sts, i, name);
        break;
      }
      fprintf (h_output, " '%s'", value);
#if 0
      if (logvalatr & OZ_LOGVALATR_OBJECT) fprintf (h_output, " (object)");
      else if (logvalatr & LNM$M_TERMINAL) fprintf (h_output, " (terminal)");
#endif
    }
    fprintf (h_output, "\n");
  }
  if (sholnmflg & SHOLNMFLG_SECURITY) show_secattr (h_output, h_logname, JPI$M_FILL1, (level + 1) * 2, "");

  oz_sys_handle_release (PSL$C_KERNEL, h_table);

  return (sts);
}

/************************************************************************/
/*									*/
/*	show mutex <logical_name>					*/
/*									*/
/************************************************************************/

static unsigned long int_show_mutex (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
#if 0
  char device_name[OZ_DEVUNIT_NAMESIZE];
  unsigned long rlen, sts;
  unsigned long h_iochan;
  unsigned long items[1];
  OZ_IO_mutex_getinfo1 mutex_getinfo1;

  /* There should only be exactly one argument - the logical name */

  if (argc != 1) {
    fprintf (h_error, "oz_cli: missing mutex iochan logical name\n");
    return (SS$_IVPARAM);
  }

  /* Convert the logical name to an I/O channel */

  sts = logname_getobj (argv[0], OZ_OBJTYPE_IOCHAN, &h_iochan);
  if (sts != SS$_NORMAL) return (sts);

  /* Get the corresponding mutex device name */

  items[0].code = JPI$M_FILL1;
  items[0].size = sizeof device_name;
  items[0].buff = device_name;
  items[0].rlen = NULL;
  sts = oz_sys_handle_getinfo (h_iochan, 1, items, NULL);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting device name of mutex %s\n", sts, argv[0]);
  else {

    /* Get the length of the mutex's name */

    memset (&mutex_getinfo1, 0, sizeof mutex_getinfo1);
    mutex_getinfo1.namerlen = &rlen;
    sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, OZ_IO_MUTEX_GETINFO1, sizeof mutex_getinfo1, &mutex_getinfo1);
    if (sts == SS$_NORMAL) {

      /* Now get everything else about the mutex */

      mutex_getinfo1.namesize = rlen;
      mutex_getinfo1.namebuff = malloc (rlen + 1);
      sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, OZ_IO_MUTEX_GETINFO1, sizeof mutex_getinfo1, &mutex_getinfo1);
      if (sts == SS$_NORMAL) {
        ((char *)(mutex_getinfo1.namebuff))[rlen] = 0;

        /* Print it all out */

        fprintf (h_output, "        device: %s\n", device_name);
        fprintf (h_output, "          name: %S\n", mutex_getinfo1.namebuff);
        fprintf (h_output, "       curmode: %s\n", lockmodes[mutex_getinfo1.curmode]);
        fprintf (h_output, "active readers: %d\n", mutex_getinfo1.active_readers);
        fprintf (h_output, "active writers: %d\n", mutex_getinfo1.active_writers);
        fprintf (h_output, " block readers: %d\n", mutex_getinfo1.block_readers);
        fprintf (h_output, " block writers: %d\n", mutex_getinfo1.block_writers);
      }
      free (mutex_getinfo1.namebuff);
    }
    if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting %s mutex info\n", sts, argv[0]);
  }
  oz_sys_handle_release (PSL$C_KERNEL, h_iochan);

  return (sts);
#endif
}

/************************************************************************/
/*									*/
/*	show process [<process_logical_name> ...]			*/
/*									*/
/*		-threads : show threads of the processes		*/
/*									*/
/************************************************************************/

static unsigned long int_show_process (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, x;
  unsigned long showopts, sts;

  x = 0;
  showopts = 0;

  for (i = 0; i < argc; i ++) {

    /* Objaddr - show objects kernel memory address */

    if (strcmp (argv[i], "-objaddr") == 0) {
      showopts |= SHOWOPT_OBJADDR;
      continue;
    }

    /* Security */

    if (strcmp (argv[i], "-security") == 0) {
      showopts |= SHOWOPT_SECURITY;
      continue;
    }

    /* Threads - display thread information for the process */

    if (strcmp (argv[i], "-threads") == 0) {
      showopts |= SHOWOPT_PROC_THREADS;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is a process logical name */

    x = 1;
    sts = show_process_bylogical (h_output, h_error, argv[i], &showopts);
    if (sts != SS$_NORMAL) break;
  }

  if (x == 0) sts = show_process_bylogical (h_output, h_error, LASTPROCLNM, &showopts);

  return (sts);
}

/************************************************************************/
/*									*/
/*  Display process info given logical name				*/
/*									*/
/************************************************************************/

static unsigned long show_process_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts)

{
  unsigned long sts;
  unsigned long h_process;

  sts = logname_getobj (logical, OZ_OBJTYPE_UNKNOWN, &h_process);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting process logical %s\n", sts, logical);
  else {
    sts = show_process_byhandle (h_output, h_error, h_process, showopts);
    oz_sys_handle_release (PSL$C_KERNEL, h_process);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Display info for a process given its handle				*/
/*									*/
/************************************************************************/

static unsigned long show_process_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_process, unsigned long *showopts)

{
  unsigned long index, sts;
  unsigned long h_lastthread, h_nextthread;

  char p_name[OZ_PROCESS_NAMESIZE];
  signed long p_refcount;
  unsigned long p_id;
  unsigned long p_threadcount;
  void *p_objaddr;

  unsigned long items[] = { 
        JPI$M_FILL1,    sizeof p_refcount,    &p_refcount,    NULL, 
        JPI$M_FILL1,          sizeof p_id,          &p_id,          NULL, 
        JPI$M_FILL1, sizeof p_threadcount, &p_threadcount, NULL, 
        JPI$M_FILL1,        sizeof p_name,         p_name,        NULL, 
	JPI$M_FILL1,             sizeof p_objaddr,     &p_objaddr,     NULL };

  unsigned long t_item = { JPI$M_FILL1, sizeof h_nextthread, &h_nextthread, NULL };

  sts = oz_sys_handle_getinfo (h_process, sizeof items / sizeof *items, items, &index);

  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting process info [%u]\n", sts, index);
  else {
    if (!(*showopts & SHOWNHDR_PROCESS)) {
      *showopts |= SHOWNHDR_PROCESS;
      if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "          ");
      fprintf (h_output, "P      refc  processid  threads  name\n");
    }
    if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "%8x: ", p_objaddr);
    fprintf (h_output, "p      %4d  %9u  %7u  %s\n", p_refcount, p_id, p_threadcount, p_name);
    if (*showopts & SHOWOPT_SECURITY) show_secattr (h_output, h_process, JPI$M_FILL1, 6, "");
    if (*showopts & SHOWOPT_PROC_THREADS) {
      sts = oz_sys_handle_getinfo (h_process, 1, &t_item, &index);
      //      t_item.code = JPI$M_FILL1;
      while ((sts == SS$_NORMAL) && (h_nextthread != 0)) {
        h_lastthread = h_nextthread;
        sts = show_thread_byhandle (h_output, h_error, h_nextthread, showopts);
        if (sts == SS$_NORMAL) sts = oz_sys_handle_getinfo (h_lastthread, 1, &t_item, &index);
        oz_sys_handle_release (PSL$C_KERNEL, h_lastthread);
      }
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting first/next thread in process\n", sts);
    }
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	show symbol [<symbol_name>]					*/
/*									*/
/************************************************************************/

static unsigned long int_show_symbol (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i;
  unsigned long sts, x;
  Script *script;
  Symbol *symbol;

  x = 0;
  for (i = 0; i < argc; i ++) {

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is a symbol name */

    symbol = lookup_symbol (argv[i], 0, &x);
    if (symbol == NULL) fprintf (h_output, "   %s (undefined)\n", argv[i]);
    else {
      fprintf (h_output, "  %s [%u]", argv[i], x);
      show_symbol (h_output, h_error, symbol);
    }
    x = 1;
  }

  if (x == 0) {
    script = scripts;
    symbol = symbols;
    while (1) {
      fprintf (h_output, "[%u]\n", x ++);
      for (; symbol != NULL; symbol = symbol -> next) {
        fprintf (h_output, "  %s", symbol -> name);
        show_symbol (h_output, h_error, symbol);
      }
      if (script == NULL) break;
      symbol = script -> symbols;
      script = script -> next;
    }
  }

  return (SS$_NORMAL);
}

static void show_symbol (unsigned long h_output, unsigned long h_error, Symbol *symbol)

{
  switch (symbol -> symtype) {
    case SYMTYPE_INTEGER: {
      if (symbol -> func != NULL) fprintf (h_output, " (integer) (function): %s\n", symbol -> svalue);
      else fprintf (h_output, " (integer) = %u\n", symbol -> ivalue);
      break;
    }
    case SYMTYPE_STRING: {
      if (symbol -> func != NULL) fprintf (h_output, " (string) (function): %s\n", symbol -> svalue);
      else fprintf (h_output, " (string) = %s\n", symbol -> svalue);
      break;
    }
    default: {
      fprintf (h_output, " (symtype %d)\n", symbol -> symtype);
      break;
    }
  }
}

#endif


/************************************************************************/
/*									*/
/*	show system							*/
/*									*/
/************************************************************************/

static unsigned long int_show_system (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i;
  unsigned long sts, showopts;

  showopts = 0;

  for (i = 0; i < argc; i ++) {

    /* Devices - display all devices in system */

    if (strcmp (argv[i], "-devices") == 0) {
      showopts |= SHOWOPT_SYSTEM_DEVICES;
      continue;
    }

    /* Iochans - display iochan information for each device */

    if (strcmp (argv[i], "-iochans") == 0) {
      showopts |= SHOWOPT_SYSTEM_IOCHANS;
      continue;
    }

    /* Jobs - display job information for each user */

    if (strcmp (argv[i], "-jobs") == 0) {
      showopts |= SHOWOPT_SYSTEM_JOBS;
      continue;
    }

    /* Objaddr - show objects kernel memory address */

    if (strcmp (argv[i], "-objaddr") == 0) {
      showopts |= SHOWOPT_OBJADDR;
      continue;
    }

    /* Processes - display process information for each job */

    if (strcmp (argv[i], "-processes") == 0) {
      showopts |= SHOWOPT_SYSTEM_PROCS;
      continue;
    }

    /* - security */

    if (strcmp (argv[i], "-security") == 0) {
      showopts |= SHOWOPT_SECURITY;
      continue;
    }

    /* Threads - display thread information for each process */

    if (strcmp (argv[i], "-threads") == 0) {
      showopts |= SHOWOPT_SYSTEM_THREADS;
      continue;
    }

    /* Users - display user information */

    if (strcmp (argv[i], "-users") == 0) {
      showopts |= SHOWOPT_SYSTEM_USERS;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }
  }

  sts = show_system (h_output, h_error, &showopts);
  return (sts);
}

#if 0

/************************************************************************/
/*									*/
/*  Display system information						*/
/*									*/
/************************************************************************/

static unsigned long show_system (unsigned long h_output, unsigned long h_error, unsigned long *showopts)
     // move to showsystem.c eventually
{
  char sepchar;
  unsigned long index, sts;
  unsigned long h_lastdev, h_lastuser, h_nextdev, h_nextuser;

  unsigned long long s_boottime;
  unsigned long s_phypagetotal;
  unsigned long s_phypagefree;
  unsigned long s_phypagel2size;
  unsigned long s_npptotal;
  unsigned long s_nppinuse;
  unsigned long s_npppeak;
  unsigned long s_pgptotal;
  unsigned long s_pgpinuse;
  unsigned long s_pgppeak;
  signed long s_cpucount;
  unsigned long s_cpusavail;
  unsigned long s_syspagetotal;
  unsigned long s_syspagefree;
  unsigned long s_usercount;
  unsigned long s_devicecount;
  unsigned long s_cachepages;

  unsigned long items[] = { 
	JPI$M_FILL1,      sizeof s_boottime,      &s_boottime,      NULL, 
	JPI$M_FILL1,  sizeof s_phypagetotal,  &s_phypagetotal,  NULL, 
	JPI$M_FILL1,   sizeof s_phypagefree,   &s_phypagefree,   NULL, 
	JPI$M_FILL1, sizeof s_phypagel2size, &s_phypagel2size, NULL, 
	JPI$M_FILL1,      sizeof s_npptotal,      &s_npptotal,      NULL, 
	JPI$M_FILL1,      sizeof s_nppinuse,      &s_nppinuse,      NULL, 
	JPI$M_FILL1,       sizeof s_npppeak,       &s_npppeak,       NULL, 
	JPI$M_FILL1,      sizeof s_pgptotal,      &s_pgptotal,      NULL, 
	JPI$M_FILL1,      sizeof s_pgpinuse,      &s_pgpinuse,      NULL, 
	JPI$M_FILL1,       sizeof s_pgppeak,       &s_pgppeak,       NULL, 
	JPI$M_FILL1,      sizeof s_cpucount,      &s_cpucount,      NULL, 
	JPI$M_FILL1,     sizeof s_cpusavail,     &s_cpusavail,     NULL, 
	JPI$M_FILL1,  sizeof s_syspagetotal,  &s_syspagetotal,  NULL, 
	JPI$M_FILL1,   sizeof s_syspagefree,   &s_syspagefree,   NULL, 
	JPI$M_FILL1,     sizeof s_usercount,     &s_usercount,     NULL, 
	JPI$M_FILL1,   sizeof s_devicecount,   &s_devicecount,   NULL, 
	JPI$M_FILL1,    sizeof s_cachepages,    &s_cachepages,    NULL };

  unsigned long d_item = { JPI$M_FILL1, sizeof h_nextdev, &h_nextdev, NULL };
  unsigned long u_item = { JPI$M_FILL1, sizeof h_nextuser, &h_nextuser, NULL };

  sts = oz_sys_handle_getinfo (0, sizeof items / sizeof *items, items, &index);

  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting system info [%u]\n", sts, index);
  else {
    fprintf (h_output, "                       boottime : %t\n", s_boottime);
    fprintf (h_output, "             physical page size : %u bytes\n", 1 << s_phypagel2size);
    fprintf (h_output, "           total physical pages : %u (%u MB)\n", s_phypagetotal, s_phypagetotal >> (20 - s_phypagel2size));
    fprintf (h_output, "        phy pages used by cache : %u (%u MB)\n", s_cachepages,   s_cachepages   >> (20 - s_phypagel2size));
    fprintf (h_output, "            free physical pages : %u (%u MB)\n", s_phypagefree,  s_phypagefree  >> (20 - s_phypagel2size));
    fprintf (h_output, "      non-paged pool total size : %u (%u KB)\n", s_npptotal, s_npptotal >> 10);
    fprintf (h_output, "          non-paged pool in use : %u (%u KB)\n", s_nppinuse, s_nppinuse >> 10);
    fprintf (h_output, "      non-paged pool peak usage : %u (%u KB)\n", s_npppeak,  s_npppeak  >> 10);
    fprintf (h_output, "          paged pool total size : %u (%u KB)\n", s_pgptotal, s_pgptotal >> 10);
    fprintf (h_output, "              paged pool in use : %u (%u KB)\n", s_pgpinuse, s_pgpinuse >> 10);
    fprintf (h_output, "          paged pool peak usage : %u (%u KB)\n", s_pgppeak,  s_pgppeak  >> 10);
    fprintf (h_output, "            max cpu's built for : %u\n", s_cpucount);
    fprintf (h_output, "        available cpu number(s) :");
    sepchar = ' ';
    for (index = 0; index < s_cpucount; index ++) {
      if (s_cpusavail & (1 << index)) {
         fprintf (h_output, "%c%u", sepchar, index);
         sepchar = ',';
      }
    }
    fprintf (h_output, "\n");
    fprintf (h_output, "   system pagetable entry total : %u (%u KB)\n", s_syspagetotal, s_syspagetotal << (s_phypagel2size - 10));
    fprintf (h_output, "  free system pagetable entries : %u (%u KB)\n", s_syspagefree,  s_syspagefree  << (s_phypagel2size - 10));
    fprintf (h_output, "      number of users logged in : %u\n", s_usercount);
    fprintf (h_output, "      number of devices defined : %u\n", s_devicecount);

    if (*showopts & SHOWOPT_SYSTEM_DEVICES) {
      sts = oz_sys_handle_getinfo (0, 1, &d_item, &index);
      //      d_item.code = JPI$M_FILL1;
      while ((sts == SS$_NORMAL) && (h_nextdev != 0)) {
        h_lastdev = h_nextdev;
        sts = show_device_byhandle (h_output, h_error, h_nextdev, showopts);
        if (sts == SS$_NORMAL) sts = oz_sys_handle_getinfo (h_lastdev, 1, &d_item, &index);
        oz_sys_handle_release (PSL$C_KERNEL, h_lastdev);
      }
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting first/next device in system\n", sts);
    }

    if ((sts == SS$_NORMAL) && (*showopts & SHOWOPT_SYSTEM_USERS)) {
      sts = oz_sys_handle_getinfo (0, 1, &u_item, &index);
      //      u_item.code = JPI$M_FILL1;
      while ((sts == SS$_NORMAL) && (h_nextuser != 0)) {
        h_lastuser = h_nextuser;
        sts = show_user_byhandle (h_output, h_error, h_nextuser, showopts);
        if (sts == SS$_NORMAL) sts = oz_sys_handle_getinfo (h_lastuser, 1, &u_item, &index);
        oz_sys_handle_release (PSL$C_KERNEL, h_lastuser);
      }
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting first/next user in system\n", sts);
    }
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*	show thread [<thread_logical_name> ...]				*/
/*									*/
/************************************************************************/

static unsigned long int_show_thread (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, security, usedup, x;
  unsigned long showopts, sts;
  unsigned long h_thread;
  unsigned long thread_id;

  showopts = 0;
  x = 0;

  for (i = 0; i < argc; i ++) {

    /* Id - thread id number */

    if (strcmp (argv[i], "-id") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing thread id number after -id\n");
        return (SS$_IVPARAM);
      }
      thread_id = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad thread id number %s after -id\n", argv[i]);
        return (SS$_BADPARAM);
      }
      sts = oz_sys_thread_getbyid (thread_id, &h_thread);
      if (sts != SS$_NORMAL) {
        fprintf (h_error, "oz_cli: error %u getting handle to thread id %u\n", sts, thread_id);
        return (sts);
      }
      x = 1;
      sts = show_thread_byhandle (h_output, h_error, h_thread, &showopts);
      oz_sys_handle_release (PSL$C_KERNEL, h_thread);
      if (sts != SS$_NORMAL) break;
      continue;
    }

    /* Objaddr - show objects kernel memory address */

    if (strcmp (argv[i], "-objaddr") == 0) {
      showopts |= SHOWOPT_OBJADDR;
      continue;
    }

    /* Security */

    if (strcmp (argv[i], "-security") == 0) {
      showopts |= SHOWOPT_SECURITY;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is a thread logical name */

    x = 1;
    sts = show_thread_bylogical (h_output, h_error, argv[i], &showopts);
    if (sts != SS$_NORMAL) break;
  }

  if (x == 0) sts = show_thread_bylogical (h_output, h_error, LASTHREADLNM, &showopts);

  return (sts);
}

/************************************************************************/
/*									*/
/*  Display thread info given logical name				*/
/*									*/
/************************************************************************/

static unsigned long show_thread_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts)

{
  unsigned long sts;
  unsigned long h_thread;

  sts = logname_getobj (logical, OZ_OBJTYPE_UNKNOWN, &h_thread);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting thread logical %s\n", sts, logical);
  else {
    sts = show_thread_byhandle (h_output, h_error, h_thread, showopts);
    oz_sys_handle_release (PSL$C_KERNEL, h_thread);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Display info for a thread given its handle				*/
/*									*/
/************************************************************************/

static unsigned long show_thread_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_thread, unsigned long *showopts)

{
  char state[16];
  unsigned long h_event;
  unsigned long t_id;
  unsigned long datelongs[OZ_DATELONG_ELEMENTS], index, len, sts;

  char t_name[OZ_THREAD_NAMESIZE];
  signed long t_refcount;
  unsigned long long t_tis_com, t_tis_ini, t_tis_run, t_tis_wev, t_tis_zom;
  unsigned char t_state;
  unsigned long t_basepri, t_curprio;
  void *t_objaddr;

  struct { unsigned long long bin;
           char str[16];
         } t_tis[OZ_THREAD_STATE_MAX];

  char e_name[OZ_EVENT_NAMESIZE];
  signed long e_value;

  static const unsigned long thread_wev_codes[] = { JPI$M_FILL1, JPI$M_FILL1, 
                                                     JPI$M_FILL1, JPI$M_FILL1 };

  unsigned long items[] = { 
	JPI$M_FILL1, sizeof t_refcount, &t_refcount, NULL, 
	JPI$_STATE,    sizeof t_state,    &t_state,    NULL, 
	JPI$M_FILL1,  sizeof t_tis[SCH$C_CUR].bin, &(t_tis[SCH$C_CUR].bin), NULL, 
	JPI$M_FILL1,  sizeof t_tis[SCH$C_COM].bin, &(t_tis[SCH$C_COM].bin), NULL, 
	JPI$M_FILL1,  sizeof t_tis[SCH$C_LEF].bin, &(t_tis[SCH$C_LEF].bin), NULL, 
#if 0
	JPI$M_FILL1,  sizeof t_tis[OZ_THREAD_STATE_ZOM].bin, &(t_tis[OZ_THREAD_STATE_ZOM].bin), NULL, 
	JPI$M_FILL1,  sizeof t_tis[OZ_THREAD_STATE_INI].bin, &(t_tis[OZ_THREAD_STATE_INI].bin), NULL, 
#endif
	JPI$_PID,       sizeof t_id,       &t_id,       NULL, 
	JPI$_PRCNAM,     sizeof t_name,      t_name,     NULL, 
	JPI$_PRI,  sizeof t_curprio,  &t_curprio,  NULL, 
	JPI$_PRIB,  sizeof t_basepri,  &t_basepri,  NULL, 
	JPI$M_FILL1,         sizeof t_objaddr,  &t_objaddr,  NULL };

  unsigned long thread_wev_item = { 0, sizeof h_event, &h_event, NULL };
  unsigned long event_items[] = {
	JPI$M_FILL1,  sizeof e_name,   e_name,  NULL, 
	JPI$M_FILL1, sizeof e_value, &e_value, NULL };

  sts = oz_sys_handle_getinfo (h_thread, sizeof items / sizeof *items, items, &index);

  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting thread info [%u]\n", sts, index);
  else {

    /* If we haven't shown the thread header, show it now */

    if (!(*showopts & SHOWNHDR_THREAD)) {
      *showopts |= SHOWNHDR_THREAD;
      if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "          ");
      fprintf (h_output, "T        refc  state       tis_run       tis_com       tis_wev       tis_zom  curprio/basepri  threadid  name\n");
    }

    /* Convert state number to equivalent string */

    switch (t_state) {
      case SCH$C_CUR: { strcpy (state, "RUN"); break; }
      case SCH$C_COM: { strcpy (state, "COM"); break; }
      case SCH$C_LEF: { strcpy (state, "WEV"); break; }
#if 0
      case OZ_THREAD_STATE_INI: { strcpy (state, "INI"); break; }
      case OZ_THREAD_STATE_ZOM: { strcpy (state, "ZOM"); break; }
#endif
      default: { snprintf (state, sizeof state, "%3d", t_state); };
    }

    /* Convert time-in-state values to nice strings that fit in 12 columns */

    for (index = 0; index <= SCH$C_CUR; index ++) {
      oz_sys_datebin_decode (t_tis[index].bin, datelongs);

      /* If there are any number of days, put them in at beginning */

      len = 0;
      if (datelongs[OZ_DATELONG_DAYNUMBER] > 0) {
        oz_hw_itoa (datelongs[OZ_DATELONG_DAYNUMBER], sizeof t_tis[index].str, t_tis[index].str);
        len = strlen (t_tis[index].str);
      }

      /* If we put in days and there is room for :hh, or if we didn't put in days, put hours in */

      if (((len != 0) && (len <= 9)) || (len == 0)) {
        if (len != 0) t_tis[index].str[len++] = '@';
        t_tis[index].str[len++] = (datelongs[OZ_DATELONG_SECOND] / 36000) + '0';
        datelongs[OZ_DATELONG_SECOND] %= 36000;
        t_tis[index].str[len++] = (datelongs[OZ_DATELONG_SECOND] /  3600) + '0';
        datelongs[OZ_DATELONG_SECOND] %=  3600;
      }

      /* If we put in hours and there is room for :mm, or if we didn't put in hours, put minutes in */

      if (((len != 0) && (len <= 9)) || (len == 0)) {
        if (len != 0) t_tis[index].str[len++] = ':';
        t_tis[index].str[len++] = (datelongs[OZ_DATELONG_SECOND] / 600) + '0';
        datelongs[OZ_DATELONG_SECOND] %= 600;
        t_tis[index].str[len++] = (datelongs[OZ_DATELONG_SECOND] /  60) + '0';
        datelongs[OZ_DATELONG_SECOND] %=  60;
      }

      /* If we put in minutes and there is room for :ss, or if we didn't put in minutes, put in seconds */
      /* Then follow it up with up to three digits of .fff if there is room                             */

      if (((len != 0) && (len <= 9)) || (len == 0)) {
        if (len != 0) t_tis[index].str[len++] = ':';
        if ((len != 0) || (datelongs[OZ_DATELONG_SECOND] >= 10)) {
          t_tis[index].str[len++] = (datelongs[OZ_DATELONG_SECOND] / 10) + '0';
          datelongs[OZ_DATELONG_SECOND] %= 10;
        }
        t_tis[index].str[len++] = datelongs[OZ_DATELONG_SECOND] + '0';
        if (len <= 10) {
          t_tis[index].str[len++] = '.';
          while ((len < 12) && (t_tis[index].str[len-4] != '.')) {
#if OZ_TIMER_RESOLUTION > 400000000
  error : code assumes OZ_TIMER_RESOLUTION * 10 fits in a unsigned long
#endif
            datelongs[OZ_DATELONG_FRACTION] *= 10;
            t_tis[index].str[len++] = (datelongs[OZ_DATELONG_FRACTION] / OZ_TIMER_RESOLUTION) + '0';
            datelongs[OZ_DATELONG_FRACTION] %= OZ_TIMER_RESOLUTION;
          }
        }
      }
      t_tis[index].str[len] = 0;
    }

    /* Display thread detail line */

    if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "%8x: ", t_objaddr);
    fprintf (h_output, "t        %4d  %5s  %-12s  %-12s  %-12s  %-12s  %7u/%-7u  %8u  %s\n", 
                         t_refcount, state, t_tis[SCH$C_CUR].str, 
                                            t_tis[SCH$C_COM].str, 
                                            t_tis[SCH$C_LEF].str, 
                                            t_tis[SCH$C_LEF].str, t_curprio, t_basepri, t_id, t_name);

    /* If state is WEV, display each event flag's name */

    if (t_state == SCH$C_LEF) {
      for (index = 0; index < sizeof thread_wev_codes / sizeof thread_wev_codes[0]; index ++) {
	//        thread_wev_item.code = thread_wev_codes[index];
        sts = oz_sys_handle_getinfo (h_thread, 1, &thread_wev_item, NULL);
        if (sts != SS$_NORMAL) {
          fprintf (h_error, "oz_cli: error %u getting wevent[%u] handle\n", sts, index);
          break;
        }
        if (h_event == 0) break;
        sts = oz_sys_handle_getinfo (h_event, sizeof event_items / sizeof *event_items, event_items, NULL);
        oz_sys_handle_release (PSL$C_KERNEL, h_event);
        if (sts != SS$_NORMAL) {
          fprintf (h_error, "oz_cli: error %u getting wevent[%u] info\n", sts, index);
          break;
        }
        fprintf (h_output, "%s %s", (index == 0) ? "                (WEV:" : ",", e_name);
        if (e_value != 0) fprintf (h_output, "=%d", e_value);
      }
      if (index != 0) fprintf (h_output, ")\n");
    }

    /* Maybe display security info */

    if (*showopts & SHOWOPT_SECURITY) {
      show_secattr (h_output, h_thread, JPI$M_FILL1, 16, "creates:");
      show_secattr (h_output, h_thread, JPI$M_FILL1,       16, "secattr:");
      show_thread_seckeys (h_output, h_thread);
    }
  }

  return (sts);
}

static void show_thread_seckeys (unsigned long h_output, unsigned long h_thread)

{
  char *secattrstr;
  unsigned char secattrbuf[SECATTRSIZE];
  unsigned long secattrlen, sts;

  unsigned long item = { JPI$M_FILL1, sizeof secattrbuf, secattrbuf, &secattrlen };

  sts = oz_sys_handle_getinfo (h_thread, 1, &item, NULL);
  if (sts == SS$_NORMAL) sts = oz_sys_seckeys_bin2str (secattrlen, secattrbuf, secmalloc, NULL, &secattrstr);
  if (sts != SS$_NORMAL) {
    fprintf (h_output, "        seckeys:(error %u)\n", sts);
  } else {
    fprintf (h_output, "        seckeys:(%s)\n", secattrstr);
    if (secattrstr != NULL) free (secattrstr);
  }
}

/************************************************************************/
/*									*/
/*  Show object's security attributes					*/
/*									*/
/*    Input:								*/
/*									*/
/*	h_output = output channel					*/
/*	h_object = object's handle					*/
/*	code     = JPI$M_FILL1?_SECATTR				*/
/*	prefix_w = prefix string width					*/
/*	prefix   = prefix string					*/
/*									*/
/*    Output:								*/
/*									*/
/*	secattrs written to output					*/
/*									*/
/************************************************************************/

static void show_secattr (unsigned long h_output, unsigned long h_object, unsigned long secattrcode, int prefix_w, const char *prefix)

{
  char *secattrstr;
  unsigned char secattrbuf[SECATTRSIZE];
  unsigned long secattrlen, sts;

  unsigned long item = { secattrcode, sizeof secattrbuf, secattrbuf, &secattrlen };

  sts = oz_sys_handle_getinfo (h_object, 1, &item, NULL);
  if (sts == SS$_NORMAL) sts = oz_sys_secattr_bin2str (secattrlen, secattrbuf, NULL, secmalloc, NULL, &secattrstr);
  if (sts != SS$_NORMAL) {
    fprintf (h_output, "%*.*s(error %u)\n", prefix_w, prefix_w, prefix, sts);
  } else {
    fprintf (h_output, "%*.*s(%s)\n", prefix_w, prefix_w, prefix, secattrstr);
    if (secattrstr != NULL) free (secattrstr);
  }
}

/* Alloc memory for security structs */

static void *secmalloc (void *dummy, unsigned long osize, void *obuff, unsigned long nsize)

{
  void *nbuff;

  nbuff = NULL;
  if (nsize != 0) {
    nbuff = malloc (nsize);
    memcpy (nbuff, obuff, osize);
  }
  if (obuff != NULL) free (obuff);
  return (nbuff);
}

/************************************************************************/
/*									*/
/*	show user [<user_logical_name> ...]				*/
/*									*/
/*		-jobs							*/
/*		-processes						*/
/*		-threads						*/
/*									*/
/************************************************************************/

static unsigned long int_show_user (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, x;
  unsigned long sts, showopts;

  x = 0;
  showopts = 0;

  for (i = 0; i < argc; i ++) {

    /* Jobs - display job information for the user */

    if (strcmp (argv[i], "-jobs") == 0) {
      showopts |= SHOWOPT_USER_JOBS;
      continue;
    }

    /* Objaddr - show objects kernel memory address */

    if (strcmp (argv[i], "-objaddr") == 0) {
      showopts |= SHOWOPT_OBJADDR;
      continue;
    }

    /* Processes - display process information for the job */

    if (strcmp (argv[i], "-processes") == 0) {
      showopts |= SHOWOPT_USER_PROCS;
      continue;
    }

    /* Security */

    if (strcmp (argv[i], "-security") == 0) {
      showopts |= SHOWOPT_SECURITY;
      continue;
    }

    /* Threads - display thread information for the process */

    if (strcmp (argv[i], "-threads") == 0) {
      showopts |= SHOWOPT_USER_THREADS;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is a user logical name */

    x = 1;
    sts = show_user_bylogical (h_output, h_error, argv[i], &showopts);
    if (sts != SS$_NORMAL) break;
  }

  if (x == 0) sts = show_user_bylogical (h_output, h_error, "OZ_THIS_USER", &showopts);

  return (sts);
}

/************************************************************************/
/*									*/
/*  Display user info given logical name				*/
/*									*/
/************************************************************************/

static unsigned long show_user_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts)

{
  unsigned long sts;
  unsigned long h_user;

  sts = logname_getobj (logical, OZ_OBJTYPE_UNKNOWN, &h_user);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting user logical %s\n", sts, logical);
  else {
    sts = show_user_byhandle (h_output, h_error, h_user, showopts);
    oz_sys_handle_release (PSL$C_KERNEL, h_user);
  }
  return (sts);
}

/************************************************************************/
/*									*/
/*  Display info for a user given its handle				*/
/*									*/
/************************************************************************/

static unsigned long show_user_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_user, unsigned long *showopts)

{
  unsigned long index, sts;
  unsigned long h_lastjob, h_nextjob;

  char u_name[OZ_USERNAME_MAX], u_quota[256];
  signed long u_refcount;
  unsigned long u_jobcount;
  void *u_objaddr;

  unsigned long items[] = { 
#if 0
	JPI$M_FILL1,  sizeof u_refcount, &u_refcount, NULL, 
	JPI$M_FILL1,  sizeof u_jobcount, &u_jobcount, NULL, 
#endif
	JPI$_USERNAME,      sizeof u_name,      u_name,     NULL, 
#if 0
	JPI$M_FILL1, sizeof u_quota,     u_quota,    NULL, 
	JPI$M_FILL1,        sizeof u_objaddr,  &u_objaddr,  NULL
#endif
	0, 0, 0, 0};

  unsigned long j_item = { JPI$M_FILL1, sizeof h_nextjob, &h_nextjob, NULL };

  sts = oz_sys_handle_getinfo (h_user, sizeof items / sizeof *items, items, &index);

  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting user info [%u]\n", sts, index);
  else {
    if (!(*showopts & SHOWNHDR_USER)) {
      *showopts |= SHOWNHDR_USER;
      if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "          ");
      fprintf (h_output, "U  refc  jobs  name\n");
    }
    if (*showopts & SHOWOPT_OBJADDR) fprintf (h_output, "%8x: ", u_objaddr);
    fprintf (h_output, "u  %4d  %4u  %4s\n", u_refcount, u_jobcount, u_name);
    fprintf (h_output, "    quota:(%s)\n", u_quota);
    if (*showopts & SHOWOPT_SECURITY) show_secattr (h_output, h_user, JPI$M_FILL1, 10, "  secattr:");
    if (*showopts & SHOWOPT_USER_JOBS) {
      sts = oz_sys_handle_getinfo (h_user, 1, &j_item, &index);
      //      j_item.code = JPI$M_FILL1;
      while ((sts == SS$_NORMAL) && (h_nextjob != 0)) {
        h_lastjob = h_nextjob;
        sts = show_job_byhandle (h_output, h_error, h_nextjob, showopts);
        if (sts == SS$_NORMAL) sts = oz_sys_handle_getinfo (h_lastjob, 1, &j_item, &index);
        oz_sys_handle_release (PSL$C_KERNEL, h_lastjob);
      }
      if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u getting first/next job for user\n", sts);
    }
  }

  return (sts);
}

/************************************************************************/
/*									*/
/*	show volume <device_name>					*/
/*									*/
/************************************************************************/

static char *quadmemsize (unsigned long long size, char *buff);

static unsigned long int_show_volume (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  char freebytesstr[32], totalbytesstr[32];
  int i;
  unsigned long h_iochan;
  //  OZ_IO_fs_getinfo3 fs_getinfo3;
  unsigned long clusterbytes, sts;
  unsigned long long freebytes, totalbytes;

#if 0
  for (i = 0; i < argc; i ++) {

    sts = sys$assign( argv[i], &h_iochan,PSL$C_KERNEL,0,0);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u assigning channel to %s\n", sts, argv[i]);
      return (sts);
    }

    memset (&fs_getinfo3, 0, sizeof fs_getinfo3);
    sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, OZ_IO_FS_GETINFO3, sizeof fs_getinfo3, &fs_getinfo3);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting info about %s\n", sts, argv[i]);
      return (sts);
    }

    clusterbytes = fs_getinfo3.clusterfactor * fs_getinfo3.blocksize;
    freebytes    = fs_getinfo3.clustersfree;
    freebytes   *= clusterbytes;
    totalbytes   = fs_getinfo3.clustertotal;
    totalbytes  *= clusterbytes;
    fprintf (h_output, "       Blocksize: %u bytes\n"
                                   "   Clusterfactor: %u blocks, %u bytes\n"
                                   "    Clustersfree: %u, %u blocks, %s\n"
                                   "    Clustertotal: %u, %u blocks, %s\n", 
                                   fs_getinfo3.blocksize, 
                                   fs_getinfo3.clusterfactor, clusterbytes, 
                                   fs_getinfo3.clustersfree,  fs_getinfo3.clustersfree  * fs_getinfo3.clusterfactor, quadmemsize (freebytes,  freebytesstr), 
                                   fs_getinfo3.clustertotal,  fs_getinfo3.clustertotal  * fs_getinfo3.clusterfactor, quadmemsize (totalbytes, totalbytesstr));
    if (fs_getinfo3.nincache != 0) {
      fprintf (h_output, "        In cache: %u page(s)\n"
                                     "           Dirty: %u page(s)\n", 
                                     fs_getinfo3.nincache, 
                                     fs_getinfo3.ndirties);
    }
    if (fs_getinfo3.avgwriterate != 0) {
      fprintf (h_output, "  Avg write rate: %u page(s)/sec\n", fs_getinfo3.avgwriterate);
    }
    if (fs_getinfo3.dirty_interval != 0) {
      fprintf (h_output, "  Dirty interval: %#t\n", fs_getinfo3.dirty_interval);
    }
    fprintf (h_output, "     Mount flags:");
    if (fs_getinfo3.mountflags == 0)  fprintf (h_output, " (none)\n");
    else {
      if (fs_getinfo3.mountflags & OZ_FS_MOUNTFLAG_NOCACHE)   fprintf (h_output, " -nocache");
      if (fs_getinfo3.mountflags & OZ_FS_MOUNTFLAG_READONLY)  fprintf (h_output, " -readonly");
      if (fs_getinfo3.mountflags & OZ_FS_MOUNTFLAG_WRITETHRU) fprintf (h_output, " -writethru");
      fprintf (h_output, "\n");
    }

    oz_sys_handle_release (PSL$C_KERNEL, h_iochan);
  }
#endif

  return (SS$_NORMAL);
}

static char *quadmemsize (unsigned long long size, char *buff)

{
  unsigned long long tenths;

  if (size >= ((unsigned long long)10) << 30) {
    tenths   = size & 0x3FFFFFFF;
    tenths  *= 10;
    tenths >>= 30;
    snprintf (buff, 32, "%u.%u Gigabytes", (unsigned long)(size >> 30), (unsigned long)tenths);
  } else if (size >= 100*1024*1024) {
    snprintf (buff, 32, "%u Megabytes", (unsigned long)(size >> 20));
  } else {
    snprintf (buff, 32, "%u Kilobytes", (unsigned long)(size >> 10));
  }
  return (buff);
}

/************************************************************************/
/*									*/
/*	suspend thread [<logical_name>]					*/
/*									*/
/************************************************************************/

static unsigned long int_suspend_thread (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i, usedup;
  unsigned long sts;
  unsigned long h_thread;
  unsigned long thread_id;

  logical_name = LASTHREADLNM;
  thread_id = 0;

  for (i = 0; i < argc; i ++) {

    /* Id - thread id number */

    if (strcmp (argv[i], "-id") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing thread id number after -id\n");
        return (SS$_IVPARAM);
      }
      thread_id = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad thread id number %s after -id\n", argv[i]);
        return (SS$_BADPARAM);
      }
      logical_name = NULL;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    logical_name = argv[i];
    thread_id    = 0;
  }

  /* Get handle to thread from logical name or id number */

  if (thread_id != 0) {
    sts = oz_sys_thread_getbyid (thread_id, &h_thread);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting handle to thread id %u\n", sts, thread_id);
      return (sts);
    }
  } else {
    sts = logname_getobj (logical_name, OZ_OBJTYPE_THREAD, &h_thread);
    if (sts != SS$_NORMAL) return (sts);
  }

  /* Suspend the thread */

  sts = sys$suspnd(h_thread,0,0);
  if ((sts != SS$_WASCLR) && (sts != SS$_WASSET)) {
    fprintf (h_error, "oz_cli: error %u suspending thread\n", sts);
  }

  /* Release the handle */

  oz_sys_handle_release (PSL$C_KERNEL, h_thread);

  /* Anyway, return composite status */

  return (sts);
}

/************************************************************************/
/*									*/
/*	wait event <logical_name> ...					*/
/*									*/
/************************************************************************/

static unsigned long int_wait_event (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i, nevents;
  unsigned long sts;
  unsigned long *h_events;

  nevents  = 0;
  h_events = malloc (argc * sizeof *h_events);

  for (i = 0; i < argc; i ++) {

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - it is a logical name - add corresponding event flag to list */

    logical_name = argv[i];
    sts = logname_getobj (logical_name, OZ_OBJTYPE_EVENT, h_events + nevents);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u looking up event logical %s\n", sts, logical_name);
      goto rtn;
    }
    nevents ++;
  }

  if (nevents == 0) {
    fprintf (h_error, "oz_cli: missing logical name\n");
    return (SS$_IVPARAM);
  }

  /* Wait for any one of them to be set (or for control-Y) */

  sts = wait_events (nevents, h_events);

  /* Release all handles and return final status */

rtn:
  while (nevents > 0) oz_sys_handle_release (PSL$C_KERNEL, h_events[--nevents]);
  free (h_events);
  return (sts);
}

/************************************************************************/
/*									*/
/*	wait mutex <logical_name>					*/
/*									*/
/************************************************************************/

static unsigned long int_wait_mutex (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
#if 0
  unsigned long sts;
  unsigned long h_iochan;

  if (argc != 1) {
    fprintf (h_error, "oz_cli: missing mutex iochan logical name\n");
    return (SS$_IVPARAM);
  }

  sts = logname_getobj (argv[0], OZ_OBJTYPE_IOCHAN, &h_iochan);
  if (sts != SS$_NORMAL) return (sts);

  sts = oz_sys_io (PSL$C_KERNEL, h_iochan, 0, OZ_IO_MUTEX_UNBLOCK, 0, NULL);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u waiting for mutex %s\n", sts, argv[0]);

  oz_sys_handle_release (PSL$C_KERNEL, h_iochan);

  return (sts);
#endif
}

/************************************************************************/
/*									*/
/*	wait thread [<logical_name>]					*/
/*									*/
/************************************************************************/

static unsigned long int_wait_thread (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  const char *logical_name;
  int i, usedup;
  unsigned long sts;
  unsigned long h_thread;
  unsigned long thread_id;

  logical_name = LASTHREADLNM;
  thread_id    = 0;

  for (i = 0; i < argc; i ++) {

    /* Id - thread id number */

    if (strcmp (argv[i], "-id") == 0) {
      if ((++ i >= argc) || (*argv[i] == '-')) {
        fprintf (h_error, "oz_cli: missing thread id number after -id\n");
        return (SS$_IVPARAM);
      }
      thread_id = oz_hw_atoi (argv[i], &usedup);
      if (argv[i][usedup] != 0) {
        fprintf (h_error, "oz_cli: bad thread id number %s after -id\n", argv[i]);
        return (SS$_BADPARAM);
      }
      logical_name = NULL;
      continue;
    }

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

    /* No option - this is the logical name */

    logical_name = argv[i];
    thread_id    = 0;
  }

  /* Get handle to thread from logical name.  If no logical name given, use last thread's handle. */

  if (thread_id != 0) {
    sts = oz_sys_thread_getbyid (thread_id, &h_thread);
    if (sts != SS$_NORMAL) {
      fprintf (h_error, "oz_cli: error %u getting handle to thread id %u\n", sts, thread_id);
      return (sts);
    }
  } else {
    sts = logname_getobj (logical_name, OZ_OBJTYPE_THREAD, &h_thread);
    if (sts != SS$_NORMAL) return (sts);
  }

  /* Wait for thread to exit (or for control-Y) */

  sts = wait_thread (h_error, h_thread);

  /* Release the handle */

  oz_sys_handle_release (PSL$C_KERNEL, h_thread);

  /* Anyway, return composite status */

  return (sts);
}

/************************************************************************/
/*									*/
/*  Wait for an thread to exit (also enable control-Y during wait)	*/
/*									*/
/************************************************************************/

static unsigned long wait_thread (unsigned long h_error, unsigned long h_thread)

{
  unsigned long exitst, sts, sts2;
  unsigned long h_exit;

  /* Get exit event flag associated with thread */

  sts = oz_sys_thread_getexitevent (h_thread, &h_exit);
  if (sts != SS$_NORMAL) {
    fprintf (h_error, "oz_cli: error %u getting thread exit event flag\n", sts);
    return (sts);
  }

  /* If handle is zero, it means the thread has exited (or it didn't have an exit event flag to begin with) */

  if (h_exit == 0) {
    sts = oz_sys_thread_getexitsts (h_thread, &exitst);
    if (sts == SS$_NORMAL) sts = exitst;
    else fprintf (h_error, "oz_cli: error %u checking for thread exited\n", sts);
    return (sts);
  }

  /* Resume the thread before attempting to wait for it */

  sts = sys$resume(h_thread,0);
  if ((sts != SS$_WASSET) && (sts != SS$_WASCLR)) {
    fprintf (h_error, "oz_cli: error %u resuming thread\n", sts);
    oz_sys_handle_release (PSL$C_KERNEL, h_exit);
    return (sts);
  }

  /* Wait for the thread to exit or for control-Y */

  do {
    sts = wait_events (1, &h_exit);				/* wait for it to exit */
    if (sts != SS$_NORMAL) break;				/* break out if error or control-Y */
    oz_sys_event_set (PSL$C_KERNEL, h_exit, 0, NULL);	/* clear the exit event flag before we check the status */
    sts = oz_sys_thread_getexitsts (h_thread, &exitst);		/* see if thread has exited by trying to get its status */
    if ((sts != SS$_NORMAL) && (sts != SS$_WASCLR)) fprintf (h_error, "oz_cli: error %u checking for thread exited\n", sts);
  } while (sts == SS$_WASCLR);				/* keep going if thread still running */
  if (sts == SS$_NORMAL) sts = exitst;				/* if thread has exited, return its exit status */
  else {
    sts2 = sys$suspnd(h_thread,0,0);			/* not exited, suspend it */
    if (sts2 == SS$_WASCLR) fprintf (h_error, "oz_cli: thread is now suspended (abort thread, resume thread, wait thread)\n");
    else if (sts2 == SS$_WASSET) fprintf (h_error, "oz_cli: thread was already suspended\n");
    else fprintf (h_error, "oz_cli: error %u suspending thread\n", sts2);
  }

  /* Release event flag handle and return status */

  oz_sys_handle_release (PSL$C_KERNEL, h_exit);
  return (sts);
}

/************************************************************************/
/*									*/
/*	wait until <datetime>						*/
/*									*/
/************************************************************************/

static unsigned long int_wait_until (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int rc;
  unsigned long sts;
  unsigned long long now;
  unsigned long long timer_waituntil;

  if (argc != 1) {
    fprintf (h_error, "oz_cli: missing datetime\n");
    return (SS$_IVPARAM);
  }

  sys$gettim(&now);
  //  now = oz_sys_datebin_tzconv (now, OZ_DATEBIN_TZCONV_UTC2LCL, 0);
  rc  = sys$bintim ( argv[0], &timer_waituntil);
  if (rc == 0) {
    fprintf (h_error, "oz_cli: invalid datetime %s\n", argv[0]);
    return (SS$_BADPARAM);
  }

  //  if (rc < 0) OZ_HW_DATEBIN_ADD (timer_waituntil, timer_waituntil, now);

  //  timer_waituntil = oz_sys_datebin_tzconv (timer_waituntil, OZ_DATEBIN_TZCONV_LCL2UTC, 0);
#if 0
  sts = oz_sys_io (PSL$C_KERNEL, h_timer, 0, OZ_IO_TIMER_WAITUNTIL, sizeof timer_waituntil, &timer_waituntil);
  if (sts != SS$_NORMAL) fprintf (h_error, "oz_cli: error %u waiting for timer %s\n", sts, argv[0]);
#endif

  return (sts);
}

/************************************************************************/
/*									*/
/*  Wait for an event flag to be set.  Enable control-Y during wait.	*/
/*									*/
/*    Input:								*/
/*									*/
/*	nevents  = number of events in h_events				*/
/*	h_events = event flag handle array				*/
/*									*/
/*    Output:								*/
/*									*/
/*	wait_events = SS$_NORMAL : event flag is now set		*/
/*	         SS$_ABORTEDBYCLI : control-Y was pressed		*/
/*	                    else : error status				*/
/*									*/
/************************************************************************/

static unsigned long wait_events (unsigned long nevents, unsigned long *h_events)

{
  signed long ef;
  unsigned long i;
  unsigned long sts;

  while (1) {
    sts = oz_sys_event_nwait (PSL$C_KERNEL, nevents, h_events, 0);	/* wait for given event flag or any ast */
    if ((sts != SS$_WASSET) && (sts != SS$_WASCLR) && (sts != SS$_NORMAL)) return (sts);
    for (i = 0; i < nevents; i ++) {					/* check the given event flags */
      oz_sys_event_inc (PSL$C_KERNEL, h_events[i], 0, &ef);
      if (ef > 0) return (SS$_NORMAL);					/* successful if any one of them is set */
    }
    if (ctrly_hit) return (SS$_ABORT);				/* see if ctrl-Y was pressed */
  }
}
#endif
