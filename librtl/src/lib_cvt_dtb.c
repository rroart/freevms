/*
 * lib_cvt_dtb.c
 *
 *	Code for VAX LIB$CVT_DTB routine
 *
 * Description:
 *
 *	Converts an ascii decimal string representation to
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
 * str$trim
 *
 */
unsigned long lib$cvt_dtb(int byte_count, const char* source_string,
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
		if (isdigit(source_string[Loop]))
		{
			/*
			 * Add one more digit to result
			 */
			answer = answer * 10 + source_string[Loop] - '0';
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


