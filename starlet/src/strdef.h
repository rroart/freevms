/*
 * strdef.h
 *
 *	Code for VAX STR$ routines
 *
 * Description:
 *
 *	This file contains various 'str$' functions equivalent
 *	to those available in Vax/VMS string library.
 *
 * Bugs:
 *
 *	Not compatible at the binary level.
 *
 *	No seperate "string zone" to allocate memory from,
 *	uses malloc/free instead.
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 */


/*
 * String error messages
 */
#define STR$_FACILITY   36
#define STR$_FATINTERR  2392132
#define STR$_DIVBY_ZER  2392140
#define STR$_ILLSTRCLA  2392148
#define STR$_STRIS_INT  2392156
#define STR$_WRONUMARG  2392164
#define STR$_INSVIRMEM  2392172
#define STR$_STRTOOLON  2392180
#define STR$_ERRFREDYN  2392188

#define STR$_TRU        2392576
#define STR$_NOMATCH    2392584
#define STR$_INVDELIM   2392592
#define STR$_NOELEM     2392600

#define STR$_NEGSTRLEN  2393089
#define STR$_ILLSTRPOS  2393097
#define STR$_ILLSTRSPE  2393105
#define STR$_MATCH      2393113
#define STR$_NORMAL     1      


