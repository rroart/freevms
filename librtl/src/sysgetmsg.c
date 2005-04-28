
/*
 *	sysgetmsg.c
 *
 *	Copyright (C) 2003 Andrew Allison
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *The author may be contacted at freevms@sympatico.ca
 *
 *	Andrew Allison
 *	50 Denlaw Road
 *	London, Ont
 *	Canada 
 *	N6G 3L4
 *
 */

#include <stdio.h>
#include <string.h>
#include <ssdef.h>
#include <strdef.h>
#include <libdef.h>
#include <rmsdef.h>
#include <stdarg.h>

// just as a workaround until someone implements MSG utility (-Roar)

/* sys$getmsg msgid, msglen, bufadr, [flags], [outadr]
 *
 * msgid		Message id group by bits 3 - 27
 * msglen		Length of message returned by sys$getmsg
 * bufadr		Address to return message
 * flags - Optional	Flag components to be returned
 * outadr- Optional	byte	0 Reserved
 *				1 Count of FAO arguments
 *				2 User specified value in message, If any
 *				3 Reserved
 *
 */

int sys$$getmsg (unsigned int msgid, unsigned short int *msglen, char *fmt,...);
void	check_ss_codes ( int strmsgid, void *bufadr, void *smesg);
void	check_str_codes( int strmsgid, void *bufadr, void *smesg);
void	check_lib_codes( int libmsgid, void *bufadr, void *smesg);
void	check_rms_codes( int rmsmsgid, void *bufadr, void *smesg);

int sys$$getmsg(unsigned int msgid,unsigned short int *msglen,char *fmt,...)

// Optional parameters: unsigned int flags,unsigned char outadr[4] )

{
va_list ap;
char *lmesg,*smesg;
// Optional Parameters
unsigned int flags;

	va_start(ap,fmt);
	lmesg = fmt;

	strcpy (lmesg,"Unknown Error Code ");
	check_ss_codes ((int) msgid,lmesg,smesg);
	check_str_codes((int) msgid,lmesg,smesg);
	check_lib_codes((int) msgid,lmesg,smesg);
	check_rms_codes((int) msgid,lmesg,smesg);
	*msglen = strlen(lmesg);

	flags = va_arg(ap,unsigned int);
/*	printf ("We got more %d\n",flags);

	fmt++;
	if ( *fmt )
		printf ("We got more\n");
	else
		printf ("No more parameters\n");
*/
	va_end(ap);

	return (int) msgid;
}

/***********************************************/
void check_ss_codes(int strmsgid, void *bufadr, void *smsg)
{
switch ( strmsgid)
{
	case SS$_ACCVIO:	// 12
			strcpy (bufadr,"Access Violation");
			break;
	case SS$_INTOVF:	// 1148
			strcpy (bufadr,"Integer Overflow");
			break;
	case SS$_INTDIV:	// 1156
			strcpy (bufadr,"Integer Divide By Zero");
			break;
}
return;
}

/***********************************************/
void check_str_codes(int strmsgid, void *bufadr, void *smsg)
{
switch ( strmsgid)
{	case STR$_NORMAL:	// 1
			strcpy (bufadr,"Successfull");
			break;
	case STR$_FACILITY:   //	36
			strcpy (bufadr,"Error facility  ");
			break;
	case STR$_FATINTERR:  //	2392132
			strcpy (bufadr,"Internal consistency check");
			break;
	case STR$_DIVBY_ZER:  //	2392140
			strcpy (bufadr,"Divide by zero");
			break;
	case STR$_ILLSTRCLA:  //	2392148
			strcpy (bufadr,"Illegal String Class");
			break;
	case STR$_STRIS_INT:  //	2392156
			strcpy (bufadr,"String is Interlocked");
			break;
	case STR$_WRONUMARG:  //	2392164
			strcpy (bufadr,"Wrong number of arguments");
			break;
	case STR$_INSVIRMEM:  //	2392172
			strcpy (bufadr,"Insufficient Virtual Memory");
			break;
	case STR$_STRTOOLON:  //	2392180
			strcpy (bufadr,"String too long");
			break;
 	case STR$_ERRFREDYN:  //	2392188
			strcpy (bufadr,"Error Freeing dynamic string");
			break;
	case STR$_TRU:        //	2392576
			strcpy (bufadr,"String truncation");
			break;
	case STR$_NOMATCH:    //	2392584
			strcpy (bufadr,"No strings matched");
			break;
	case STR$_INVDELIM:   //	2392592
			strcpy (bufadr,"Invalid delimiter");
			break;
	case STR$_NOELEM:     //	2392600
			strcpy (bufadr,"No element");
			break;
	case STR$_NEGSTRLEN:   //	2393089
			strcpy (bufadr,"Negative string length");
			break;
	case STR$_ILLSTRPOS:   //	2393089
			strcpy (bufadr,"End position less than start");
			break;
	case STR$_ILLSTRSPE:  //	2393105
			strcpy (bufadr,"Illegal string specification");
			break;
	case STR$_MATCH:      //	2393113
			strcpy (bufadr,"More than one match");
			break;
	}
	return;
}

/***********************************/

void	check_lib_codes(int libmsgid, void *bufadr, void *smesg)
{
switch ( libmsgid)
{ 
case	LIB$_FACILITY:
			strcpy (bufadr,"Facility Error");
			break;
case	LIB$_NORMAL:
			strcpy (bufadr,"Normal");
			break;
case	LIB$_STRTRU:
			strcpy (bufadr,"String truncation");
			break;
case	LIB$_ONEENTQUE:
			strcpy (bufadr,"An entry was added");
			break;
case	LIB$_KEYALRINS:
			strcpy (bufadr,"Key already inserted in tree");
			break;
case	LIB$_ERRROUCAL:
			strcpy (bufadr,"Error routine called");
			break;
case	LIB$_RESTART:
			strcpy (bufadr,"Restart");
			break;
case	LIB$_BADSTA:
			strcpy (bufadr,"Bad stack");
			break;
case	LIB$_FATERRLIB:
			strcpy (bufadr,"Fatal error in library");
			break;
case	LIB$_INSVIRMEM:
			strcpy (bufadr,"Insufficient virtual memory");
			break;
case	LIB$_INPSTRTRU:
			strcpy (bufadr,"Input string truncation");
			break;
case	LIB$_INVSTRDES:
			strcpy (bufadr,"Invalid string descriptor");
			break;
case	LIB$_INTLOGERR:
			strcpy (bufadr,"Internal logic error");
			break;
case	LIB$_INVARG:
			strcpy (bufadr,"Invalid argument");
			break;
case	LIB$_AMBKEY:
			strcpy (bufadr,"Ambiguous keyword");
			break;
case	LIB$_UNRKEY:
			strcpy (bufadr,"Unrecognized keyword,field,class name");
			break;
case	LIB$_PUSSTAOVE:
			strcpy (bufadr,"Pushdown stack overflow");
			break;
case	LIB$_USEFLORES:
			strcpy (bufadr,"Use of floating reserved operand");
			break;
case	LIB$_SIGNO_ARG:
			strcpy (bufadr,"Signal with no arguments");
			break;
case	LIB$_BADBLOADR:
			strcpy (bufadr,"Bad block address");
			break;
case	LIB$_BADBLOSIZ:
			strcpy (bufadr,"Bad block size");
			break;
case	LIB$_NOTFOU:
			strcpy (bufadr,"Not found");
			break;
case	LIB$_ATTCONSTO:
			strcpy (bufadr,"Attempt to continue from stop");
			break;
case	LIB$_SYNTAXERR:
			strcpy (bufadr,"Syntax error");
			break;
case	LIB$_INVTYPE:
			strcpy (bufadr,"Invalid type");
			break;
case	LIB$_INSEF:
			strcpy (bufadr,"Insufficient event flag");
			break;
case	LIB$_EF_ALRFRE:
			strcpy (bufadr,"Event flag already free");
			break;
case	LIB$_EF_ALRRES:
			strcpy (bufadr,"Event flag already reserver");
			break;
case	LIB$_EF_RESSYS:
			strcpy (bufadr,"Event flag reserved to system");
			break;
case	LIB$_INSLUN:
			strcpy (bufadr,"Insufficient logical unit numbers");
			break;
case	LIB$_LUNALRFRE:
			strcpy (bufadr,"Logical unit already free");
			break;
case	LIB$_LUNRESSYS:
			strcpy (bufadr,"Logical unit reserved to system");
			break;
case	LIB$_SCRBUFOVF:
			strcpy (bufadr,"Screen buffer overflow");
			break;
case	LIB$_INVSCRPOS:
			strcpy (bufadr,"Invalid screen position");
			break;
case	LIB$_SECINTFAI:
			strcpy (bufadr,"Seconday interlock failure in queue");
			break;
case	LIB$_INVCHA:
			strcpy (bufadr,"Invalid character");
			break;
case	LIB$_QUEWASEMP:
			strcpy (bufadr,"Queue was empty");
			break;
case	LIB$_STRIS_INT:
			strcpy (bufadr,"String is interlocked");
			break;
case	LIB$_KEYNOTFOU:
			strcpy (bufadr,"Key not found");
			break;
case	LIB$_INVCVT:
			strcpy (bufadr,"Invalid conversion");
			break;
case	LIB$_INVDTYDSC:
			strcpy (bufadr,"Invalid data type in descriptor");
			break;
case	LIB$_INVCLADSC:
			strcpy (bufadr,"Invalid class in descriptor");
			break;
case	LIB$_INVCLADTY:
			strcpy (bufadr,"Invalid class data type combination");
			break;
case	LIB$_INVNBDS:
			strcpy (bufadr,"Invalid numeric byte data string");
			break;
case	LIB$_DESSTROVF:
			strcpy (bufadr,"Destination string overflow");
			break;
case	LIB$_INTOVF:
			strcpy (bufadr,"Integer overflow");
			break;
case	LIB$_DECOVF:
			strcpy (bufadr,"Decimal overflow");
			break;
case	LIB$_FLTOVF:
			strcpy (bufadr,"Floating overflow");
			break;
case	LIB$_FLTUND:
			strcpy (bufadr,"Floating underflow");
			break;
case	LIB$_ROPRAND:
			strcpy (bufadr,"Reserverd operand fault");
			break;
case	LIB$_WRONUMARG:
			strcpy (bufadr,"Wrong number of arguments");
			break;
case	LIB$_NOSUCHSYM:
			strcpy (bufadr,"no such symbol");
			break;
case	LIB$_INSCLIMEM:
			strcpy (bufadr,"Insufficient CLI memory");
			break;
case	LIB$_AMBSYMDEF:
			strcpy (bufadr,"Ambiguous symbol definition");
			break;
case	LIB$_NOCLI:
			strcpy (bufadr,"no CLI present to perform function");
			break;
case	LIB$_UNECLIERR:
			strcpy (bufadr,"Unexpected CLI error");
			break;
case	LIB$_INVSYMNAM:
			strcpy (bufadr,"Invalid symbol name");
			break;
case	LIB$_ATTREQREF:
			strcpy (bufadr,"Attach request refused");
			break;
case	LIB$_INVFILSPE:
			strcpy (bufadr,"Invalid file specification");
			break;
case	LIB$_INVXAB:
			strcpy (bufadr,"LIB$_INVXAB");
			break;
case	LIB$_NO_STRACT:
			strcpy (bufadr,"LIB$_NO_STRACT");
			break;
case	LIB$_BADTAGVAL:
			strcpy (bufadr,"Bad boundry tag value");
			break;
case	LIB$_INVOPEZON:
			strcpy (bufadr,"Invalid operation for zone");
			break;
case	LIB$_PAGLIMEXC:
			strcpy (bufadr,"Page limit exceeded for zone");
			break;
case	LIB$_NOTIMP:
			strcpy (bufadr,"Not implemented");
			break;
case	LIB$_BADZONE:
			strcpy (bufadr,"Bad zone");
			break;
case	LIB$_IVTIME:
			strcpy (bufadr,"Invalid time passed in or computed");
			break;
case	LIB$_ONEDELTIM:
			strcpy (bufadr,"At least one delta time required");
			break;
case	LIB$_NEGTIM:
			strcpy (bufadr,"Negative time");
			break;
case	LIB$_INVARGORD:
			strcpy (bufadr,"Invalid argument order");
			break;
case	LIB$_ABSTIMREQ:
			strcpy (bufadr,"absolute time requires");
			break;
case	LIB$_DELTIMREQ:
			strcpy (bufadr,"Delta time required");
			break;
case	LIB$_INVOPER:
			strcpy (bufadr,"Invalid operation");
			break;
case	LIB$_REENTRANCY:
			strcpy (bufadr,"Reentrancy detected");
			break;
case	LIB$_UNRFORCOD:
			strcpy (bufadr,"Unrecognized format code");
			break;
case	LIB$_ILLINISTR:
			strcpy (bufadr,"Illegal initialization string");
			break;
case	LIB$_NUMELEMENTS:
			strcpy (bufadr,"Number of elements incorrect");
			break;
case	LIB$_ELETOOLON:
			strcpy (bufadr,"Element too long max 255 characters");
			break;
case	LIB$_ILLCOMPONENT:
			strcpy (bufadr,"Illegal initialization component");
			break;
case	LIB$_AMBDATTIM:
			strcpy (bufadr,"Ambiguous date-time");
			break;
case	LIB$_INCDATTIM:
			strcpy (bufadr,"Incomplete date-time");
			break;
case	LIB$_ILLFORMAT:
			strcpy (bufadr,"Illegal format");
			break;
case	LIB$_UNSUPVER:
			strcpy (bufadr,"LIB$_UNSUPVER");
			break;
case	LIB$_BADFORMAT:
			strcpy (bufadr,"Format version mismatch");
			break;
case	LIB$_OUTSTRTRU:
			strcpy (bufadr,"Output string truncated");
			break;
case	LIB$_FILFAIMAT:
			strcpy (bufadr,"Filed failed to match selection criteria");
			break;
case	LIB$_NEGANS:
			strcpy (bufadr,"LIB$_NEGANS");
			break;
case	LIB$_QUIPRO:
			strcpy (bufadr,"LIB$_QUIPRO");
			break;
case	LIB$_QUICONACT:
			strcpy (bufadr,"LIB$_QUICONACT");
			break;
case	LIB$_ENGLUSED:
			strcpy (bufadr,"Englished used could not determine");
			break;
case	LIB$_DEFFORUSE:	
			strcpy (bufadr,"Default format used");
			break;
case	LIB$_MEMALRFRE:
			strcpy (bufadr,"Memory already free");
			break;
case	LIB$_BADCCC:
			strcpy (bufadr,"Illegal compilation code");
			break;
case	LIB$_EOMERROR:
			strcpy (bufadr,"Compilation errors in module");
			break;
case	LIB$_EOMFATAL:
			strcpy (bufadr,"Fatal errors in module");
			break;
case	LIB$_EOMWARN:
			strcpy (bufadr,"Warnings in module");
			break;
case	LIB$_GSDTYP:
			strcpy (bufadr,"Illegal GSD record type");
			break;
case	LIB$_ILLFMLCNT:
			strcpy (bufadr,"Minimum argument count exceeds maximum");
			break;
case	LIB$_ILLMODNAM:
			strcpy (bufadr,"Illegal module name");
			break;
case	LIB$_ILLPSCLEN:
			strcpy (bufadr,"Illegal length in psect ");
			break;
case	LIB$_ILLRECLEN:
			strcpy (bufadr,"Illegal record length");
			break;
case	LIB$_ILLRECLN2:
			strcpy (bufadr,"Illegal record length 2");
			break;
case	LIB$_ILLRECTYP:
			strcpy (bufadr,"Illegal record type");
			break;
case	LIB$_ILLRECTY2:
			strcpy (bufadr,"Illegal record type 2");
			break;
case	LIB$_ILLSYMLEN:
			strcpy (bufadr,"Illegal symbol length");
			break;
case	LIB$_NOEOM:
			strcpy (bufadr,"No end of module");
			break;
case	LIB$_RECTOOSML:
			strcpy (bufadr,"Record too small");
			break;
case	LIB$_SEQUENCE:
			strcpy (bufadr,"Illegal record sequence in moduel");
			break;
case	LIB$_SEQUENCE2:
			strcpy (bufadr,"LIB$_SEQUENCE2");
			break;
case	LIB$_STRLVL:
			strcpy (bufadr,"Object structure level unsupported");
			break;
case	LIB$_NOWILD:
			strcpy (bufadr,"No wildcard permitted");
			break;
case	LIB$_ACTIMAGE:
			strcpy (bufadr,"LIB$_ACTIMAGE");
			break;
case	LIB$_OPENIN:
			strcpy (bufadr,"Error opening input file");
			break;
case	LIB$_CLOSEIN:
			strcpy (bufadr,"Error closeing input file");
			break;
case	LIB$_READERR : 
			strcpy (bufadr,"Read error");
			break;
}	//end switch
return;
}	// end getlibmsg

void	check_rms_codes(int rmsmsgid, void *bufadr, void *smesg)
{

switch (rmsmsgid)
{
case	RMS$_RTB : 
			strcpy (bufadr,"Record too large for buffer");
			break;
case	RMS$_EOF: 
			strcpy (bufadr,"End of file");
			break;
case	RMS$_FNF : 
			strcpy (bufadr,"File not found");
			break;
case	RMS$_NMF : 
			strcpy (bufadr,"No more files");
			break;
case	RMS$_WCC : 
			strcpy (bufadr,"Invalid wild card contect");
			break;
case	RMS$_BUG : 
			strcpy (bufadr,"Fatal process deleted");
			break;
case	RMS$_DIR: 
			strcpy (bufadr,"Error in directory name");
			break;
case	RMS$_ESS : 
			strcpy (bufadr,"Expand string area too small");
			break;
case	RMS$_FNM : 
			strcpy (bufadr,"Error in file name");
			break;
case	RMS$_IFI : 
			strcpy (bufadr,"Invalid internal file identifier");
			break;
case	RMS$_NAM : 
			strcpy (bufadr,"Invalid name");
			break;
case	RMS$_RSS : 
			strcpy (bufadr,"Invalid resultant string size");
			break;
case	RMS$_RSZ : 
			strcpy (bufadr,"Invalid record size");
			break;
case	RMS$_WLD : 
			strcpy (bufadr,"Read error");
			break;
}

}
