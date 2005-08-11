/*
 * str.c
 *
 *	Code for VAX STR$$RESIZE routine
 *
 * Description:
 *
 *
 * Bugs:
 *
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Ziele
 *		Changed 'short' to 'unsigned short' in several places.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$$resize
 *
 *	Try to resize the destination, giving as close to the
 *	desired final size as possible.
 */
unsigned int str$$resize(struct dsc$descriptor_s* dest, unsigned short size)
{
	unsigned long result = STR$_NORMAL;
	int resize;

	/*
	 * Generate the proper memory to store the result in.
	 * Dependent on the type of string.
	 */
	switch(dest->dsc$b_class)
	{
	case DSC$K_CLASS_Z:
	case DSC$K_CLASS_S:
	case DSC$K_CLASS_SD:
	case DSC$K_CLASS_A:
	case DSC$K_CLASS_NCA:
		/*
		 * We can't change the size of this. Live with
		 * whatever we've got.
		 */
		if (dest->dsc$a_pointer == 0)
		{
			DOSIGNAL(STR$_FATINTERR);
			result = STR$_FATINTERR;
		}
		break;

	case DSC$K_CLASS_D:
		/*
		 * Try to allocate a different size
		 */
		result = str$get1_dx(&size, dest);
		break;

	case DSC$K_CLASS_VS:
		/*
		 * Better be pointing at something
		 */
                if (dest->dsc$a_pointer == 0)
                {
                        DOSIGNAL(STR$_FATINTERR);
                        result = STR$_FATINTERR;
                }
		resize = min(dest->dsc$w_length, size);
		*((unsigned short int*)dest->dsc$a_pointer) = resize;
		break;

	default:
		DOSIGNAL(STR$_ILLSTRCLA);
		result = STR$_ILLSTRCLA;
	}

	return result;
}


