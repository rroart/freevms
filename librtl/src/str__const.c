/*
 * str.c
 *
 *	Code for VAX STR$ routines
 *
 * Description:
 *
 *	This file contains various constants that are externally
 *	accessable from STR$ routines.
 *
 * Bugs:
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design.
 */

#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * Error values
 */
const unsigned long str$_faility = STR$_FACILITY;
const unsigned long str$_divby_zer = STR$_DIVBY_ZER;
const unsigned long str$_errfredyn = STR$_ERRFREDYN;
const unsigned long str$_fatinterr = STR$_FATINTERR;
const unsigned long str$_illstrcla = STR$_ILLSTRCLA;
const unsigned long str$_illstrpos = STR$_ILLSTRPOS;
const unsigned long str$_illstrspe = STR$_ILLSTRSPE;
const unsigned long str$_insvirmem = STR$_INSVIRMEM;
const unsigned long str$_invdelim = STR$_INVDELIM;
const unsigned long str$_match = STR$_MATCH;
const unsigned long str$_negstrlen = STR$_NEGSTRLEN;
const unsigned long str$_noelem = STR$_NOELEM;
const unsigned long str$_nomatch = STR$_NOMATCH;
const unsigned long str$_normal = STR$_NORMAL;
const unsigned long str$_stris_int = STR$_STRIS_INT;
const unsigned long str$_strtoolon = STR$_STRTOOLON;
const unsigned long str$_tru = STR$_TRU;
const unsigned long str$_wronumarg = STR$_WRONUMARG;

