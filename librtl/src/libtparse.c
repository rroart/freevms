#include <descrip.h>
#include <libdef.h>
#include <misc.h>
#include <ssdef.h>
#include <tpadef.h>

// or rather use diutpa.b36 and diutpamac.r36?

/*
  NOTE:
  The following lines was changed from word to long, since the assembler
  did not seem to handle word. Might still not be 64-bit clean.
  
  OWN TPA$KEY : WORD INITIAL (TPA$KEYST0 - TPA$KEY0);
  NOT: OWN TPA$TARGET : WORD ALIGN (0) INITIAL
  NOT: OWN TPA$SUBEXP : WORD ALIGN(0) INITIAL (SUBNAME - TPA$SUBEXP-2);
*/

#define TPA$M_CODEFLAG          256
#define TPA$M_EXTRAFLAG         512
#define TPA$M_LASTFLAG          1024
#define TPA$M_EXTFLAG           2048
#define TPA$M_TRANFLAG          4096
#define TPA$M_MASKFLAG          8192
#define TPA$M_ADDRFLAG          16384
#define TPA$M_ACTFLAG           32768
#define TPA$M_PARMFLAG          65536

#define SPACE 32
#define TERMINATOR 255
#define TAB 9

#define SPACES 0
#define NUMERIC 1
#define ALPHANUMERIC 2
#define SYMBOL 3

#define TPA$_KEYWORD            256
#define TPA$_EXIT               -1
#define TPA$_FAIL               -2

#define TPA$_NODE_ACS           487  
#define TPA$_NODE_PRIMARY       488  
#define TPA$_NODE               489  
#define TPA$_FILESPEC           490  
#define TPA$_UIC                491  
#define TPA$_IDENT              492  
#define TPA$_ANY                493  
#define TPA$_ALPHA              494  
#define TPA$_DIGIT              495  
#define TPA$_STRING             496  
#define TPA$_SYMBOL             497  
#define TPA$_BLANK              498  
#define TPA$_DECIMAL            499  
#define TPA$_OCTAL              500  
#define TPA$_HEX                501  
#define TPA$_LAMBDA             502  
#define TPA$_EOS                503  
#define TPA$_SUBEXPR            504  

#define $NODE_ACS (0xff & TPA$_NODE_ACS)
#define $NODE_PRIMARY (0xff & TPA$_NODE_PRIMARY)
#define $NODE (0xff & TPA$_NODE)
#define $FILESPEC (0xff & TPA$_FILESPEC)
#define $UIC (0xff & TPA$_UIC)
#define $IDENT (0xff & TPA$_IDENT)
#define $ANY (0xff & TPA$_ANY)
#define $ALPHA (0xff & TPA$_ALPHA)
#define $DIGIT (0xff & TPA$_DIGIT)
#define $TSTRING (0xff & TPA$_STRING)
#define $SYMBOL (0xff & TPA$_SYMBOL)
#define $BLANK (0xff & TPA$_BLANK)
#define $DECIMAL (0xff & TPA$_DECIMAL)
#define $OCTAL (0xff & TPA$_OCTAL)
#define $HEX (0xff & TPA$_HEX)
#define $LAMBDA (0xff & TPA$_LAMBDA)
#define $EOS (0xff & TPA$_EOS)
#define $SUBEXPR (0xff & TPA$_SUBEXPR)
#define HIGH_ASCII 255
#define KEYWORD 256
#define HIGH_KEYWORD 475
#define LOW_SPECIAL $ANY
#define HIGH_SPECIAL $SUBEXPR

static int inline
is_alphabetic(char * s) {
  char c=*s;
  return ((c>='A' && c<='Z') || (c>='a' && c<='z'));
}

static int inline
is_numeric(char * s) {
  char c=*s;
  return (c>='0' && c<='9');
}

static int inline
is_symbol(char * s) {
  char c=*s;
  return (c=='_' || c=='$');
}

static int inline
is_space(char * s) {
  char c=*s;
  return (c==SPACE || c==TAB);
}

static int
getstring(long * string, int type);

#define next_state(state, type) \
	    if (type&TPA$M_EXTRAFLAG) \
	      state+=4; \
	    if (type&TPA$M_EXTFLAG) \
	      state+=2; \
	    if (type&TPA$M_ACTFLAG) \
	      state+=4; \
	    if (type&TPA$M_ADDRFLAG) \
	      state+=4; \
	    if (type&TPA$M_MASKFLAG) \
	      state+=4; \
	    if (type&TPA$M_TRANFLAG) \
	      state+=2;

static int upcase(char c) {
  if (c>='A' || c<='Z')
    return c|0x20;
  else
    return c;
}

int char_count;

int
lib$tparse(void *argument_block ,void *state_table , void *key_table) {
  struct _tpadef * tpa = argument_block; // 64-bit later
  int * start_state = state_table;
  static int action_value;
  char * state, * curpos;
  int type, keylength;
  char * key; 
  if (tpa->tpa$l_count < TPA$K_COUNT0)
    return SS$_INSFARG;
  tpa->tpa$v_ambig=0;
  state=start_state;
  action_value=0;

  while (1) {

    int ifval = 0;

    curpos = state;
    type = *(unsigned short*)state; // tpa$type
    state+=2;
    if (type&TPA$M_EXTRAFLAG) {
      type|=(*state)<<16; // tpa$flags2
      state++;
    }
    if (tpa->tpa$v_blanks==0 && (type&0x1ff)!=TPA$_LAMBDA) {
      getstring(&tpa->tpa$l_stringcnt, SPACES);
      tpa->tpa$l_stringcnt-=char_count;
      tpa->tpa$l_stringptr+=char_count;
    }
    char_count = 0;
    tpa->tpa$l_tokenptr=tpa->tpa$l_stringptr;

    keylength = 0;
    key = 0;

    if ((type&0x1ff)<TPA$_LAMBDA && tpa->tpa$l_stringcnt==0)
      ifval = 0;
    else
      switch (type&0x1ff) {
      case 0 ... HIGH_ASCII:
	if (upcase((type&0xff)) == upcase(*(char*)tpa->tpa$l_stringptr)) { // check
	  tpa->tpa$l_char=*(char*)tpa->tpa$l_stringptr;
	  char_count = 1;
	  ifval = char_count;
	} else
	  ifval = 0;
	break;

      case KEYWORD ... HIGH_KEYWORD:
	if (tpa->tpa$v_ambig==0 && getstring(&tpa->tpa$l_stringcnt, SYMBOL)) {
	  //curpos+=4; // %upval, 8 for 64-bit?
	  // keytable should have been short, see NOTE
	  int * keytable = key_table;
	  keylength = keytable[type&0xff];
	  key = key_table + keylength; // tpa_key gives tpa$keyst
	  if (0==strncasecmp(tpa->tpa$l_tokenptr, key, char_count)) { // check
	    const char myterm = TERMINATOR;
	    if (key[char_count]==myterm || (tpa->tpa$b_mcount != 0 && char_count >= tpa->tpa$b_mcount))
	      ifval = 1;
	    else
	      if (tpa->tpa$v_abbrfm)
		ifval = 1;
	      else 
		if (type&TPA$M_LASTFLAG)
		  ifval = 1;
		else {
		  if (tpa->tpa$v_abbrev) {
		    char * offset;
		    int temp_length;
		    char temp_key;
		    int temp_type;
		    // does not understand this
		    offset = curpos + ((type&TPA$M_PARMFLAG) + (type&TPA$M_ADDRFLAG) + (type&TPA$M_MASKFLAG) + (type&TPA$M_ACTFLAG) + 2)*4; // upval
		    // if abbrev it should just check next in line?
		    temp_key = memchr(key+char_count, TERMINATOR, 65535);
		    temp_key++;

		    if (0==strncasecmp(tpa->tpa$l_tokenptr, temp_key, char_count)) {
		      tpa->tpa$v_ambig=1;
		      ifval = 0;
		    } else 
		      ifval = 1;
		  }
		}
	  }
	}
	break;
      default:
	switch (type&0xff) {
	case $LAMBDA:
	  ifval = 1;
	  break;
	case $EOS:
	  ifval = tpa->tpa$l_stringcnt == 0;
	  break;
	case $ANY:
	  tpa->tpa$l_char = *(char*)tpa->tpa$l_stringptr;
	  char_count = 1;
	  ifval = 1;
	  break;
	case $ALPHA:
	  if (is_alphabetic(tpa->tpa$l_stringptr)) {
	    tpa->tpa$l_char = *(char*)tpa->tpa$l_stringptr;
	    char_count = 1;
	    ifval = 1;
	  } else
	    ifval = 0;
	  break;
	case $DIGIT:
	  if (is_alphabetic(tpa->tpa$l_stringptr)) {
	    tpa->tpa$l_char = *(char*)tpa->tpa$l_stringptr;
	    char_count = 1;
	    ifval = 1;
	  } else
	    ifval = 0;
	  break;
	case $TSTRING:
	  ifval = getstring(&tpa->tpa$l_stringcnt, ALPHANUMERIC);
	  break;
	case $SYMBOL:
	  ifval = getstring(&tpa->tpa$l_stringcnt, SYMBOL);
	  break;
	case $BLANK:
	  ifval = getstring(&tpa->tpa$l_stringcnt, SPACES);
	  break;
	case $DECIMAL:
	  if (getstring(&tpa->tpa$l_stringcnt, NUMERIC)) {
	    char s[char_count+1];
	    memcpy(s,tpa->tpa$l_tokenptr,char_count);
	    s[char_count]=0;
	    char *end;
	    tpa->tpa$l_number=strtol(s,&end,10);
#if 0
	    ifval = 1 & ((tpa->tpa$l_stringptr+char_count)==end);
#endif
	    ifval = 1 & ((s+char_count)==end);
	  } else
	    ifval = 0;
	  break;
	case $OCTAL:
	  if (getstring(&tpa->tpa$l_stringcnt, NUMERIC)) {
	    char s[char_count+1];
	    memcpy(s,tpa->tpa$l_tokenptr,char_count);
	    s[char_count]=0;
	    char *end;
	    tpa->tpa$l_number=strtol(s,&end,8);
#if 0
	    ifval = 1 & ((tpa->tpa$l_stringptr+char_count)==end);
#endif
	    ifval = 1 & ((s+char_count)==end);
	  } else
	    ifval = 0;
	  break;
	case $HEX:
	  if (getstring(&tpa->tpa$l_stringcnt, NUMERIC)) {
	    char s[char_count+1];
	    memcpy(s,tpa->tpa$l_tokenptr,char_count);
	    s[char_count]=0;
	    char *end;
	    tpa->tpa$l_number=strtol(s,&end,16);
#if 0
	    ifval = 1 & ((tpa->tpa$l_stringptr+char_count)==end);
#endif
	    ifval = 1 & ((s+char_count)==end);
	  } else
	    ifval = 0;
	  break;
	case $SUBEXPR:
	  {
	    int savecount;
	    char * savepointer;
	    savecount = tpa->tpa$l_stringcnt;
	    savepointer = tpa->tpa$l_stringptr;
	    int sts = lib$tparse(argument_block,state + 2 + (*(signed short *) state),key_table);
	    if (sts&1) {
	      char_count = savecount - tpa->tpa$l_stringcnt;
	      tpa->tpa$l_tokenptr = savepointer;
	      tpa->tpa$l_stringcnt = savecount;
	      tpa->tpa$l_stringptr = savepointer;
	      ifval = 1;
	    } else {
	      tpa->tpa$l_stringcnt = savecount;
	      tpa->tpa$l_stringptr = savepointer;
	      ifval = 0;
	    }
	  }
	  break;
	default:
	  return LIB$_INVTYPE;
	}
      }

    if (ifval) { // 1
      tpa->tpa$l_stringcnt -= char_count;
      tpa->tpa$l_stringptr += char_count;
      tpa->tpa$l_tokencnt = char_count;
      if (type&TPA$M_EXTFLAG)
	state+=2;
      if (type&TPA$M_PARMFLAG) {
	tpa->tpa$l_param=*(int*)state; // tpa$param
	state+=4; // upval
      } 
      if (type&TPA$M_ACTFLAG) {
	int (*action_routine)(void *);
	action_routine = state + 4 + *(signed int*)state; // tpa$action
	action_value = action_routine(argument_block);
	ifval = action_value;
	state+=4; // upval
      } else
	ifval = 1;
      if (ifval) { // 2
	if (type&TPA$M_ADDRFLAG) {
	  long * address, mask;
	  address = state + 4 + *(signed int*)state; // tpa$addr // 64-bit unclean
	  state += 4; // upval
	  if (type&TPA$M_MASKFLAG) {
	    mask = *(int*)state; // 64-bit unclean
	    state += 4; // upval
	    *address|=mask;
	  } else {
	    if ((type&TPA$M_CODEFLAG)==0)
	      *(char*)address=tpa->tpa$b_char;
	    else {
	      switch (type&0xff) {
	      case $ANY:
	      case $ALPHA:
	      case $DIGIT:
		*(char*)address=tpa->tpa$b_char;
		break;
	      case $DECIMAL:
	      case $OCTAL:
	      case $HEX:
		*address=tpa->tpa$l_number;
		break;
	      default:
		*address=tpa->tpa$l_tokencnt;
		*(address+1)=tpa->tpa$l_tokenptr;
	      }
	    } 
	  }
	}
	tpa->tpa$v_ambig=0;
	if (type&TPA$M_TRANFLAG) {
	  if (*(signed short *)state==TPA$_EXIT)
	    return 1;
	  if (*(signed short *)state==TPA$_FAIL)
	    return LIB$_SYNTAXERR;
	  state+=2+(*(signed short*)state); // word -> long, upval, see NOTE
	} else {
	  while ((type&TPA$M_LASTFLAG)==0) {
	    type = *(unsigned short*)state; // tpa$type
	    state+=2;
	    if (type&TPA$M_EXTRAFLAG) {
	      type|=(*state)<<16; // tpa$flags2
	      state++;
	    }
	    next_state(state, type);
	  }
	}
      } else { // transition reject
	tpa->tpa$l_stringcnt += char_count;
	tpa->tpa$l_stringptr -= char_count;
	// plus state change not understood
	next_state(state, type);
	if (type&TPA$M_LASTFLAG) {
	  getstring(&tpa->tpa$l_stringcnt, SYMBOL);
	  tpa->tpa$l_tokencnt = char_count;
	  if (tpa->tpa$l_tokencnt == 0 && tpa->tpa$l_stringcnt !=0)
	    tpa->tpa$l_tokencnt ++;
	  if (action_value == 0 || (action_value&1))
	    return LIB$_SYNTAXERR;
	  else
	    return action_value;
	}
	ifval = 1;
      }
    } else {
      // state stuff not understood
      next_state(state, type);
      if (type&TPA$M_LASTFLAG) {
	getstring(&tpa->tpa$l_stringcnt, SYMBOL);
	tpa->tpa$l_tokencnt = char_count;
	if (tpa->tpa$l_tokencnt == 0 && tpa->tpa$l_stringcnt !=0)
	  tpa->tpa$l_tokencnt ++;
	if (action_value == 0 || (action_value&1))
	  return LIB$_SYNTAXERR;
	else
	  return action_value;
      }
    }
  }
}

static int
getstring(long * string, int type) {
  char_count = 0;
  while (*(int*)string > char_count && ((type >= SPACES && is_space(string[1]+char_count)) || (type >= NUMERIC && is_numeric(string[1]+char_count)) || (type >= ALPHANUMERIC && is_alphabetic(string[1]+char_count)) || (type == SYMBOL && is_symbol(string[1]+char_count))))
    char_count++;
  return char_count>0;
}

