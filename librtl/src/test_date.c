/*
 * This program is used to debug the date and time functions
 *
 * History:
 *
 *	02/12/1998 - Kevin Handy
 *		Original version
 */
#include <stdio.h>
#include <stdlib.h>

#include "descrip.h"
#include "lib$routines.h"

int main()
{
	struct dsc$descriptor_s return_date;
	char date_buffer[64];
	unsigned long status;

	return_date.dsc$w_length = sizeof(date_buffer);
	return_date.dsc$b_dtype = DSC$K_DTYPE_T;
	return_date.dsc$b_class = DSC$K_CLASS_S;
	return_date.dsc$a_pointer = date_buffer;

	status = sys$asctim(&return_date.dsc$w_length, &return_date, 0, 0);

	printf("Todays date: Status %ld: Text: %-32.32s\n",
		status, date_buffer);

	return EXIT_SUCCESS;
}
