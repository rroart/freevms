/*
 * LIB$MOVC3
 *
 *
 *	Source item is moved to destination
 *
 * History
 *
 *	Mar 28, 2005 - Andrew Allison
 *		Initial procedure creation
 *
 * Bugs
 *
 */

#include "lib$routines.h"
#include <string.h>

void lib$movc3(const unsigned short *size,const void *source,void *destination)
{
//	use memmove allows source and destination to overlap
	memmove (destination, source, *size);

	return;
}

