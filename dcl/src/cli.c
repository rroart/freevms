// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Ozone source file, 2001-2004  

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
#include<va_rangedef.h>

#include<ihddef.h>
#include<ihadef.h>
#include<syidef.h>

#include <string.h>

#include<dlfcn.h>
#include<linux/bitops.h>

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
#define OZ_FS_MAXFNLEN 42
#define OZ_TIMER_RESOLUTION 10000000
#define OZ_DATELONG_FRACTION 0
#define OZ_DATELONG_SECOND 1
#define OZ_DATELONG_ELEMENTS 3
#define OZ_THREAD_STATE_MAX 14
#define OZ_EVENT_NAMESIZE 32

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

static struct {
unsigned long curblock;
unsigned long curbyte;
} scriptinfo;

static int inscript=0;

static char * scriptbuffer;
static int scriptlen;
static int scriptpos;

static struct { 
  unsigned long size;                    /* data buffer size */
  void *buff;                    /* data buffer address */
  unsigned long trmsize;                 /* terminator buffer size */
  const void *trmbuff;           /* terminator buffer address */
  unsigned long *rlen;                   /* where to return length actually read */
  /* (doesn't include terminator) */
  unsigned long pmtsize;                 /* prompt string size */
  const void *pmtbuff;           /* prompt buffer address */
  unsigned long atblock;                /* position to this block before
				    reading (if non-zero) */
  unsigned long atbyte;                  /* position to this byte before 
				    reading (if .atblock non-zero) */
}  scriptread;

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
static void show_secattr (unsigned long h_output, unsigned long h_object, unsigned long secattrcode, int prefix_w, const char *prefix);
static void *secmalloc (void *dummy, unsigned long osize, void *obuff, unsigned long nsize);
static unsigned long show_user_bylogical (unsigned long h_output, unsigned long h_error, const char *logical, unsigned long *showopts);
static unsigned long show_user_byhandle (unsigned long h_output, unsigned long h_error, unsigned long h_user, unsigned long *showopts);
static unsigned long wait_events (unsigned long nevents, unsigned long *h_events);

/* Internal command declarations */

static unsigned long int_allocate_device      (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_change_password      (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_logical_name  (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_logical_table (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_create_symbol        (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_deallocate_device    (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_delete_logical_name  (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_delete_logical_table (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_echo                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_exit                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_logout                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_goto                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_help                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_if                   (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_script               (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_set_default          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_set_prompt          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_set_process          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_set_working_set          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_stop          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_datetime        (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_device          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_default         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_working_set         (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_job             (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_logical_name    (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_symbol          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);
static unsigned long int_show_system          (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[]);

static Command intcmd[] = {
	0, "allocate device",      int_allocate_device,      NULL, "[-user] [-job] [-process] [-thread] <device_name>", 
	0, "change password",      int_change_password,      NULL, "[<old_password> [<new_password>]]", 
	0, "create logical name",  int_create_logical_name,  NULL, "[-kernel] [-nooutermode] [-nosupersede] <logical_name> [-copy <logical_name>] [-link <logical_name>] [-object <handle>] [-terminal] [-value <value>] <value> ...", 
	0, "define",  int_create_logical_name,  NULL, "<logical_name>", 
	0, "create logical table", int_create_logical_table, NULL, "[-kernel] [-nooutermode] [-nosupersede] <table_name>", 
	0, "create /name_table",  int_create_logical_table,  NULL, "<table_name>", 
	0, "create symbol",        int_create_symbol,        NULL, "[-global] [-integer] [-level <n>] [-string] <name> <value> ...",
	0, "deallocate device",    int_deallocate_device,    NULL, "<device_name>", 
	0, "delete logical name",  int_delete_logical_name,  NULL, "<logical_name>", 
	0, "delete logical table", int_delete_logical_table, NULL, "<table_name>", 
	1, "echo",                 int_echo,                 NULL, "<string> ...", 
	0, "logout",               int_logout,                 NULL, "[<status>]", 
	0, "exit"  ,               int_exit,                 NULL, "[<status>]", 
	0, "goto",                 int_goto,                 NULL, "<label>",
	1, "help",                 int_help,                 NULL, "", 
	0, "if",                   int_if,                   NULL, "<integervalue> <statement ...>", 
	0, "@",               int_script,               NULL, "<script_name> [<args> ...]", 
	0, "set default",          int_set_default,          NULL, "<directory>", 
	0, "set prompt",           int_set_prompt,           NULL, "<prompt>", 
	0, "set process",          int_set_process,          NULL, "<name>", 
	0, "set working_set",      int_set_working_set,      NULL, "", 
	0, "show time",            int_show_datetime,        NULL, "", 
	0, "show devices",         int_show_device,          NULL, "[<device_logical_name> ...] [-iochans] [-objaddr] [-security]", 
	0, "show default",         int_show_default,         NULL, "", 
	0, "show working_set",     int_show_working_set,     NULL, "", 
	0, "show logical",    int_show_logical_name,    NULL, "<logical_name> [-security]", 
	0, "show symbol",          int_show_symbol,          NULL, "[<symbol_name> ...]", 
	0, "show system",          int_show_system,          NULL, "[-devices] [-iochans] [-job] [-processes] [-security] [-threads]", 
	0, "stop",                 int_stop,                 NULL, "[/id <pid>] <name>", 
	// the following are really not internal
	// waiting for dcltables.exe
	0, "_mount", 1, 0, "/vms$common/sysexe/mount",
	0, "_directory", 1, 0, "/vms$common/sysexe/directory",
	0, "_copy", 1, 0, "/vms$common/sysexe/copy",
	0, "_export", 1, 0, "/vms$common/sysexe/export",
	0, "_import", 1, 0, "/vms$common/sysexe/import",
	0, "_delete", 1, 0, "/vms$common/sysexe/delete",
	0, "_difference", 1, 0, "/vms$common/sysexe/difference",
	0, "_extend", 1, 0, "/vms$common/sysexe/extend",
	0, "_search", 1, 0, "/vms$common/sysexe/search",
	0, "_type", 1, 0, "/vms$common/sysexe/type",
	0, "_init", 1, 0, "/vms$common/sysexe/init",
	0, "_create /directory", 1, 0, "/vms$common/sysexe/create",
	0, "_edt", 1, 0, "/vms$common/sysexe/edt",
	0, "_dfu", 1, 0, "/vms$common/sysexe/dfu",
	0, "mount", 1, 0, "SYS$SYSTEM:mount.exe",
	0, "directory", 1, 0, "SYS$SYSTEM:directory.exe",
	0, "copy", 1, 0, "SYS$SYSTEM:copy.exe",
	0, "export", 1, 0, "SYS$SYSTEM:export.exe",
	0, "import", 1, 0, "SYS$SYSTEM:import.exe",
	0, "delete", 1, 0, "SYS$SYSTEM:delete.exe",
	0, "difference", 1, 0, "SYS$SYSTEM:difference.exe",
	0, "extend", 1, 0, "SYS$SYSTEM:extend.exe",
	0, "search", 1, 0, "SYS$SYSTEM:search.exe",
	0, "type", 1, 0, "SYS$SYSTEM:type.exe",
	0, "init", 1, 0, "SYS$SYSTEM:init.exe",
	0, "create /directory", 1, 0, "SYS$SYSTEM:create.exe",
	0, "edt", 1, 0, "SYS$SYSTEM:edt.exe",
	0, "dfu", 1, 0, "SYS$SYSTEM:dfu.exe",
	0, NULL, NULL, NULL, NULL };


const char oz_s_logname_defaulttables[] = "DEFAULT";

const char oz_sys_copyright[] = "C";

int check_vms_mm() {
  int retlenaddr;
  int mem=0;
  struct item_list_3 lst[14], syilst[2];
  syilst[0].buflen=4;
  syilst[0].item_code=SYI$_LASTFLD;
  syilst[0].bufaddr=&mem;
  syilst[0].retlenaddr=&retlenaddr;
  syilst[1].buflen=0;
  syilst[1].item_code=0;

  int sts=sys$getsyi(0,0,0,syilst,0,0,0);

  return mem;
}

int vms_mm;

static char prompt[32]="$ ";

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

  vms_mm = check_vms_mm();

  int cli_table=0;
  if (vms_mm==0) {
    cli_table=cli$cli("/vms$common/sysexe/all.cld");
  }

  /* Put argc/argv's in symbols oz_arg0... Set symbol oz_nargs to the number of values. */

  setscriptsyms (argc, (const char **)argv);

  /* Process command line arguments */

  pn = "oz_cli";
  if (argc > 0) pn = argv[0];

  for (i = 1; i < argc; i ++) {

    /* -exec <rest_of_line> : execute the single rest_of_line command */

    if (strcmp (argv[i], "-exec") == 0) {
      argc -= i;
      argv += i;
      break;
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
  def_func ("oz_field",       func_field,       SYMTYPE_STRING,      "field = oz_field (index, separator, string)");
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
  def_func ("oz_trim",        func_trim,        SYMTYPE_STRING,    "trimmed = oz_trim (string)");
  def_func ("oz_uppercase",   func_uppercase,   SYMTYPE_STRING,  "uppercase = oz_uppercase (string)");
  def_func ("oz_verify",      func_verify,      SYMTYPE_INTEGER, "oldverify = oz_verify (-1:nc 0:off 1:on)");

  /* Create an event flag to use for I/O, and one for control-Y */

  sts = lib$get_ef (&h_event);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "cli: error %u creating event flag\n", sts);
    return (sts);
  }

  sts = lib$get_ef (&h_event_ctrly);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_error, "cli: error %u creating event flag\n", sts);
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

  memset (&scriptread, 0, sizeof scriptread);
  scriptread.size = sizeof cmdbuf - 1;
  scriptread.buff = cmdbuf;
  scriptread.rlen = &cmdlen;
  scriptread.trmsize = 1;
  scriptread.trmbuff = "\n";
  scriptread.pmtsize = 7;
  scriptread.pmtbuff = "oz_cli>";

  memset (&scriptinfo, 0, sizeof scriptinfo);

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
      scriptread.atblock = 0;
      scriptread.atbyte  = 0;
    }

    /* Read command line from input file (but first get current file position) */

    sts = SS$_NORMAL;

    if (inscript) {
      int start;
#if 0
      sts = sys$qio (h_event, h_s_input, IO$_SENSECHAR, 0, 0, 0, &scriptinfo, sizeof scriptinfo );
#endif
      scriptinfo.curblock=1;
      scriptinfo.curbyte=scriptpos;

      if (sts != SS$_NORMAL) scriptinfo.curblock = 0;
      if ((sts == SS$_NORMAL) || (sts == SS$_ILLIOFUNC)) {
	if (scriptread.atblock != 0) {
	  scriptinfo.curblock = scriptread.atblock;
	  scriptinfo.curbyte  = scriptread.atbyte;
	}
#if 0
	sts = sys$qiow (h_event, h_s_input, IO$_READLBLK, 0, 0, 0, &scriptread, sizeof scriptread );
#endif

	scriptread.atblock = 1;
	scriptpos=scriptread.atbyte;

	start=scriptpos;
	while(scriptpos<scriptlen && scriptbuffer[scriptpos]!='\n')
	  scriptpos++;
	scriptpos++;
	bcopy(&scriptbuffer[start],cmdbuf,scriptpos-start);
	cmdbuf[scriptpos-start]=0;
	sts=SS$_NORMAL;
	if(scriptpos>scriptlen)
	  sts=SS$_ENDOFFILE;
	scriptread.atbyte=scriptpos;
      }
#if 0
      // why???
      scriptread.atblock = 0;
      scriptread.atbyte  = 0;
#endif
    }

    /* If read error, close this script and resume next outer level */

    if (sts != SS$_NORMAL) {
      if (sts != SS$_ENDOFFILE) {					/* error message if not end-of-file */
        fprintf (h_s_error, "oz_cli: error %u reading from input\n", sts);
        symbol -> ivalue = sts;						/* ... and set the error status */
      }
      exit_script ();							/* in any case, pop script level */
      if ((sts == SS$_ENDOFFILE) && exited && (sys$qio (h_event, h_s_input, IO$_SENSEMODE, 0, 0, NULL, 0, 0, 0, 0, 0, 0) == SS$_NORMAL)) {
        fprintf (h_s_error, "oz_cli: use 'exit' command to log out\n"); /* don't allow end-of-file to exit interactive session */
        exited = 0;
      }
    }

    /* Otherwise, process the command line */

    else {
      if (inscript==0) {
	char c=0;
	int idx=0;
	fprintf(stdout,"%s",prompt);
	fflush(stdout);
	//scanf("%s",cmdbuf);
	//programmed with buffer overflow possibility
	bzero(cmdbuf,CMDSIZ);
	// was: read(0,cmdbuf,CMDSIZ);
      again:
	read(1,&cmdbuf[idx],1);
	c=cmdbuf[idx++];
	fprintf(stdout,"%c",c);
	fflush(stdout);
	if (c!='\n' && c!='\r') goto again;
      }

      cmdlen=strlen(cmdbuf)-1;
      //      bcopy(p,cmdbuf,cmdlen);
      //      free(p);
      cmdbuf[cmdlen] = 0;						/* null terminate command line */
      p = proclabels (cmdbuf);						/* process any labels that are present */
      if (skiplabel[0] == 0) {						/* ignore line if skipping to a particular label */
	if (verify) fprintf (h_s_output, "%s\n", cmdbuf);	/* ok, echo if verifying turned on */
	if (vms_mm==0) {
	  struct dsc$descriptor d;
	  d.dsc$a_pointer=cmdbuf;
	  d.dsc$w_length=cmdlen;
	  sts = cli$dcl_parse(&d,cli_table,0,0,0);
	  if (sts&1)
	    sts = cli$dispatch(0);
	  static struct _cdu ** cur_cdu=0x3f000000;
	  *cur_cdu=0;
	  if (sts&1)
	    goto dontexec;
	}
	sts = execute (p);						/* execute the command line */
      dontexec:
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
  sys$qio (0, h_s_input, IO$_WRITELBLK, 0, 0, 0, &console_write, sizeof console_write, 0, 0, 0, 0);
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
    sts = sys$qio (NULL, h_s_console, IO$_SETMODE, 0, ctrly_ast, NULL, &ctrly, sizeof ctrly, 0, 0, 0, 0);
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
    sts = sys$qio (0, h_s_console, IO$_SETMODE, NULL, ctrlt_ast, 0, &ctrlt, sizeof ctrlt, 0, 0, 0, 0);
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

  //  sts = oz_sys_handle_getinfo (0, 1, &u_item, &index);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_console, "oz_cli: error %u getting user info\n", sts);
    goto cleanup;
  }
  sys$gettim(&now);
  fprintf (h_s_console, "\noz_cli  %.22t  (%s)\n", now, u_quota);

  /* Get handle to the first process in this job */

  //  sts = oz_sys_handle_getinfo (0, 1, &p_item, &index);
  if (sts != SS$_NORMAL) {
    fprintf (h_s_console, "oz_cli: error %u getting first process handle\n", sts);
    goto cleanup;
  }

  /* Repeat as long as we find processes in this job */

  while (h_nextproc != 0) {
    h_lastproc = h_nextproc;
    h_nextproc = 0;

    /* Find out process */

    //    sts = oz_sys_handle_getinfo (h_lastproc, sizeof p_items / sizeof *p_items, p_items, &index);
    if (sts != SS$_NORMAL) {
      fprintf (h_s_console, "oz_cli: error %u getting process info\n", sts);
      goto cleanup;
    }

    /* Repeat as long as we find threads in the process */

    while (h_nextthread != 0) {
      h_lastthread = h_nextthread;
      h_nextthread = 0;

      /* Find out about thread */

      // sts = oz_sys_handle_getinfo (h_lastthread, sizeof t_items / sizeof *t_items, t_items, &index);
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

      h_lastthread = 0;
    }
    h_lastproc = 0;
  }

cleanup:
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
    h_s_input = script -> h_input;			/* get next outer script handle */
    if (h_s_output != script -> h_output) {		/* restore its output file */
      h_s_output = script -> h_output;
    }
    if (h_s_error  != script -> h_error)  {		/* restore its error file */
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
  inscript=0;
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

      label -> blkoffs = scriptinfo.curblock;
      label -> bytoffs = scriptinfo.curbyte;

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

  /* Return completion status */

  return (sts);

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

  // use getjpi sometime
  //  sts = oz_sys_process_getbyid (processid, &h_process);
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
    if (vms_mm) {
    } else {
      if (*p=='_') p++;
    }
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
    if (sts == SS$_NORMAL) {
      sts = sys$trnlnm (0, h_logname, 0,  NULL,  objtype);	/* get handle to object the logical points to */
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
  }
  return (sts);
}

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

  // not yet sts = sys$alloc (devname, 0, objtype);
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

  //  sts = oz_sys_password_change (oldpw, newpw);
  // use $get/setuai when it gets implemented
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
  //  sts = oz_sys_io (PSL$C_KERNEL, h_s_console, 0, IO$_READLBLK, sizeof console_read, &console_read);
  if (sts == SS$_NORMAL) buffer[buflen] = 0;
  memset (&console_write, 0, sizeof console_write);
  console_write.size = 1;
  console_write.buff = "\n";
  //oz_sys_io (PSL$C_KERNEL, h_s_console, 0, IO$_WRITELBLK, sizeof console_write, &console_write);
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

  struct dsc$descriptor mytabnam, mynam;
  struct item_list_3 itm[2];

  if (argc>1 && 0==strncmp(argv[0],"/table",strlen(argv[0]))) {
  } else {
    return 0;
  }

  mynam.dsc$w_length=strlen(argv[2]);
  mynam.dsc$a_pointer=argv[2];
  mytabnam.dsc$w_length=strlen(argv[1]);
  mytabnam.dsc$a_pointer=argv[1];


  itm[0].item_code=LNM$_STRING;
  itm[0].buflen=strlen(argv[3]);
  itm[0].bufaddr=argv[3];
  bzero(&itm[1],sizeof(struct item_list_3));

  sts=sys$crelnm(0,&mytabnam,&mynam,0,itm);

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
  int i, kernel;
  unsigned long sts;

  struct dsc$descriptor mypartab, mytabnam;

  if (argc>1 && 0==strncmp(argv[0],"/parent_table",strlen(argv[0]))) {
    mypartab.dsc$w_length=strlen(argv[1]);
    mypartab.dsc$a_pointer=argv[1];
    mytabnam.dsc$w_length=strlen(argv[2]);
    mytabnam.dsc$a_pointer=argv[2];
  } else {
    mypartab.dsc$w_length=strlen("LNM$SYSTEM_DIRECTORY");
    mypartab.dsc$a_pointer="LNM$SYSTEM_DIRECTORY";
    mytabnam.dsc$w_length=strlen(argv[0]);
    mytabnam.dsc$a_pointer=argv[0];
  }

  kernel = 0;

  for (i = 0; i < argc; i ++) {

    /* Unknown option */

    if (*argv[i] == '-') {
      fprintf (h_error, "oz_cli: unknown option %s\n", argv[i]);
      return (SS$_IVPARAM);
    }

  }

#if 0

  /* Make sure we got a logical name in there somewhere */

  if (crelogtblpar.table_name == NULL) {
    fprintf (h_error, "oz_cli: missing table name\n");
    return (SS$_IVPARAM);
  }

#endif

  /* Create the table and return status */

  sts = sys$crelnt(0,0,0,0,0,&mytabnam,&mypartab,0);

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

  //  sts = sys$dalloc (argv[0]);
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
    }
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

static unsigned long int_logout (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

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

  if (label != NULL) {
    scriptread.atblock = label -> blkoffs;
    scriptread.atbyte  = label -> bytoffs;
  }

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

  if (strcasecmp (argv[0], "run") == 0 || strcasecmp (argv[0], "mcr") == 0 || strcasecmp (argv[0], "creprc") == 0 || dummy) {
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
    printf("%DCL-W-IVVERB, unrecognized command verb - check validity and spelling\n");
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
  struct dsc$descriptor aname;
  struct dsc$descriptor dflnam;
  struct file * f;
  struct _ihd * hdrbuf;
  struct _iha * active;
  struct _va_range inadr;
  void (*func)();
  int len = strlen(argv[0]);
  //  if (strcasecmp (argv[-1], "creprc") == 0) goto do_creprc;
  if (0==strncmp(".exe2",argv[0]+len-5,5)) goto do_dl;
  if (strncmp(".exe",argv[0]+len-4,4)) goto do_fork;

  aname.dsc$w_length=len-4;
  aname.dsc$a_pointer=argv[0];
  dflnam.dsc$w_length=len;
  dflnam.dsc$a_pointer=argv[0];

  hdrbuf=malloc(512);
  memset(hdrbuf, 0, 512);

  sts=sys$imgact(&aname,&dflnam,hdrbuf,0,0,0,0,0);
  printf("imgact got sts %x\n",sts);

  if (sts!=SS$_NORMAL) return sts;

  active=(unsigned long)hdrbuf+hdrbuf->ihd$w_activoff;

#if 0
  // can't do this, for some reason it causes pagefault
  char * str = argv[0];
  str[len-4]=0;
#endif

  func=active->iha$l_tfradr1;

  printf("entering image? %x\n",func);
  func(argc,argv++);
  printf("after image\n");

  sys$rundwn();

  return SS$_NORMAL;

 do_dl:
  {}
  void * handle = dlopen(argv[0],RTLD_NOW);
  if (handle==0) {
    printf("dlopen: %s\n",dlerror());
    return 0;
  }
  dlerror(); // clear error
  int(*fn)() = dlsym(handle,"main");
  int error=dlerror();
  if (error) {
    printf("dlsym: %s\n",error);
    dlclose(handle);
    return 0;
  }
  fn(argc,argv++);
  
  dlclose(handle);

  return SS$_NORMAL;

 do_fork:

  if (fork()==0) {
    execv(image,argv);
  }

  return SS$_NORMAL;

 do_creprc:
  {
    unsigned int pidadr;
    struct dsc$descriptor image,prcnam;
    image.dsc$w_length=strlen(argv[0]);
    image.dsc$a_pointer=argv[0];
    prcnam.dsc$a_pointer=basename(argv[0]);
    prcnam.dsc$w_length=strlen(prcnam.dsc$a_pointer);
    sys$creprc(&pidadr, &image, stdin, stdout, stderr, 0, 0, &prcnam, 27 , 0, 0, 0);

  }
  return SS$_NORMAL;

  sts = setup_runopts (h_error, runopts);
  if (sts != SS$_NORMAL) goto rtn;

  /* Spawn the image */
  posix_spawn();

  sts = sys$creprc /* was: spawn */ (&runopts -> h_job, 
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
    //    if (runopts -> orphan) oz_sys_thread_orphan (runopts -> h_thread);
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


/************************************************************************/
/*									*/
/*	script <script_name> [<args> ...]				*/
/*									*/
/************************************************************************/

static unsigned long int_script (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  unsigned long sts;
  unsigned long h_script;
  //  OZ_IO_fs_open fs_open;
  int fd;
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

  fd = open(argv[0],0);
  if (fd==0)
    return 0;

  inscript=fd;

  scriptbuffer=malloc(10000);

  scriptlen=read(fd,scriptbuffer,10000);

  scriptpos=0;

  close(fd);

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
}

static unsigned long int_set_prompt (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  unsigned long sts;

  /* We should have exactly one argument - the prompt name */

  if (argc == 0) {
    bcopy("$ ",prompt,2);
    prompt[2]=0;
  } else {
    bcopy(argv[0],prompt,strlen(argv[0]));
    prompt[strlen(argv[0])]=0;
  }

  return SS$_NORMAL;
}

static unsigned long int_set_process (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  unsigned long sts;

  /* We should have exactly one argument - the process name */

  set_process(argc,argv);

  return SS$_NORMAL;
}


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

  struct dsc$descriptor newdir;
  newdir.dsc$w_length=strlen(argv[0]);
  newdir.dsc$a_pointer=argv[0];
  sys$setddir(&newdir,0,0);
  //chdir(argv[0]);
  return SS$_NORMAL;
}

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
  unsigned short len;

  struct dsc$descriptor newdir;
  newdir.dsc$w_length=80;
  newdir.dsc$a_pointer=buf;
  sys$setddir(0,&len,&newdir);
  buf[len]=0;
  //getcwd(buf,80);

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

  sts = show_logical(argc, argv);

  return sts;
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

static unsigned long int_stop (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i, usedup;
  unsigned long sts;
  unsigned long h_thread;
  unsigned long thread_id;

  thread_id = 0;

  if (strcmp (argv[0], "/id") == 0) {
    unsigned int pid=strtol(argv[1],0,16);
    sys$forcex(&pid,0,42);
  } else {
    struct dsc$descriptor nam;
    nam.dsc$w_length=strlen(argv[0]);
    nam.dsc$a_pointer=argv[0];
    sys$forcex(0,&nam,84);
  }
  return SS$_NORMAL;
}

static unsigned long int_set_working_set (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i;
  unsigned long sts, showopts;

  showopts = 0;

  sts = set_working_set (argc , argv);
  return (sts);
}


static unsigned long int_show_working_set (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])

{
  int i;
  unsigned long sts, showopts;

  showopts = 0;

  sts = show_working_set (argc , argv);
  return (sts);
}

static unsigned long int_exit                 (unsigned long h_input, unsigned long h_output, unsigned long h_error, char *name, void *dummy, int argc, const char *argv[])
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
