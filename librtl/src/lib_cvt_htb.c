/*
 * lib_cvt_htb.c
 *
 *	Code for VAX LIB$CVT_HTB routine
 *
 * Description:
 *
 *	Converts an ascii hexidecimal string representation to
 *	binary.
 *
 * Bugs:
 *
 *
 * History
 *
 *	March 30, 1996 - Kevin Handy
 *		Preliminary design.
 *
 */

#include <ctype.h>
#include "libdef.h"
#include "lib$routines.h"

/*************************************************************
 * xtoi
 *
 *	Convert one hex digit to a number
 *
 * Bugs:
 *	Assumes that it is actually givin a hex digit.
 */
static int xtoi(char ch)
{
	/*
	 * 0-9
	 */
	if ((ch >= '0') && (ch <= '9'))
	{
		return(ch - '0');
	}

	/*
	 * a-f
	 */
	if ((ch >= 'a') && (ch <= 'f'))
	{
		return ch - 'a' + 10;
	}

	/*
	 * A-F
	 */
	return ch - 'A' + 10;
}

/*************************************************************
 * str$trim
 *
 */
unsigned long lib$cvt_htb(int byte_count, const char* source_string,
	long* result)
{
	int Loop;			/* Loop for characters */
	long answer = 0;		/* Result returned back */
	unsigned long status = 1;	/* Exit status */

	/*
	 * Loop through all the characters passed
	 */
	for (Loop = 0; Loop < byte_count; Loop++)
	{
		/*
		 * Make sure it is a valid character
		 */
		if (isxdigit(source_string[Loop]))
		{
			/*
			 * Add one more digit to result
			 */
			answer = answer * 10 + xtoi(source_string[Loop]);
		}
		else
		{
			/*
			 * Bad character
			 */
			status = 0;
		}
	}

	/*
	 * Return result
	 */
	*result = answer;
	return status;
}


