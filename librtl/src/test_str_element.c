/*
 * test_str_element.c
 *
 *	Test program for str$element function
 *
 * Notes:
 *
 *	This program should be run using the Free-VMS packages
 *	'str$' function str$element, Vax/VMS's, and Alpha/VMS's to make
 *	sure they all happily work the same.
 *
 * History:
 *
 *	Feb 7, 1997 - Christof Zeile
 *	Initial version, based on Kevin Handy's teststr.c
 *	Function testanal is Kevin Handy's version, unchanged
 *
 *	Feb 10, 1997 - Kevin Handy
 *		Change SS$_NORMAL to STR$_NORMAL so I don't need
 *		to copy SSDEF.H across.
 *		Use '%ld' instead of '%d' several places to quiet gcc.
 *
 *	Mar 23, 2005 - Andrew Allison
 *		Added casts to pass values to anacond as unsigned long
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "descrip.h"
#include "strdef.h"
#include <string.h>
#include "str$routines.h"

/* #include <ssdef.h> */

/*
 * Prototypes
 */
void testanal(char* Title, struct dsc$descriptor_s* strptr);
void anacond(unsigned long condcode);

/*
 * Some local constants
 */
$DESCRIPTOR(GlobalStrElem, "**First(2)*Second(3)***Third(6)*VeryLongOne**");
$DESCRIPTOR(GlobalStrDelimOk,"*");
$DESCRIPTOR(GlobalStrDelimNull,"");
$DESCRIPTOR(GlobalStrDelimLong,"**");


int main()
{
	long elemnumb;
	long retrnval;
	struct dsc$descriptor_s Test1;
	struct dsc$descriptor_s Test3;

	/*
	 * Lets prove we started
	 */
	printf("Starting tests . . .\n");

	Test1.dsc$w_length = 0;
	Test1.dsc$b_class = DSC$K_CLASS_D;
	Test1.dsc$b_dtype = DSC$K_DTYPE_T;
	Test1.dsc$a_pointer = NULL;

	Test3.dsc$w_length = 8;
	Test3.dsc$b_class = DSC$K_CLASS_S;
	Test3.dsc$b_dtype = DSC$K_DTYPE_T;
	Test3.dsc$a_pointer = (char*)malloc(20);

	/*
	 * Testing with correct delimiter string for various element numbers
	 * Start with negative element number and end with too high number,
	 * just to see what happens (check error messages).
         */
	for (elemnumb=-1;  elemnumb<12;  elemnumb++)
	{
	    printf("\n-----------------\n");
	    printf("\nelement number %ld\n",elemnumb);
	    retrnval=str$element(&Test1,&elemnumb,&GlobalStrDelimOk,&GlobalStrElem);
	    printf("return value %ld : ",retrnval);
	    anacond((unsigned long) retrnval);
	    printf("\nelement number %ld\n",elemnumb);
	    testanal("str$element, delimiter ok",&Test1);
	}
	/*
	 * Testing with empty delimiter string
	 */
	printf("\n=================\n");
	elemnumb=2;
	printf("\nelement number %ld\n",elemnumb);
	retrnval=str$element(&Test1,&elemnumb,&GlobalStrDelimNull,&GlobalStrElem);
	printf("return value %ld : ",retrnval);
	anacond((unsigned long) retrnval);
	printf("\nelement number %ld\n",elemnumb);
	testanal("str$element, delimiter null",&Test1);
	/*
	 * Testing with 2-character delimiter string
	 */
	printf("\n=================\n");
	elemnumb=2;
	printf("\nelement number %ld\n",elemnumb);
	retrnval=str$element(&Test1,&elemnumb,&GlobalStrDelimLong,&GlobalStrElem);
	printf("return value %ld : ",retrnval);
	anacond((unsigned long) retrnval);
	printf("\nelement number %ld\n",elemnumb);
	testanal("str$element, delimiter long",&Test1);
	/*
	 * Testing truncation of destination string
	 */
	printf("\n=================\n");
	elemnumb=7;
	printf("\nelement number %ld\n",elemnumb);
	retrnval=str$element(&Test3,&elemnumb,&GlobalStrDelimOk,&GlobalStrElem);
	printf("return value %ld : ",retrnval);
	anacond((unsigned long) retrnval);
	printf("\nelement number %ld\n",elemnumb);
	testanal("str$element, delimiter ok, dest string short",&Test3);
	return EXIT_SUCCESS;
}

void testanal(char* Title, struct dsc$descriptor_s* strptr)
{
	char* s1_ptr;           /* Pointer to first string */
	unsigned short s1_length;        /* Length of first string */

	printf("Analyzing '%s':\n", Title);

	str$analyze_sdesc(strptr, &s1_length, &s1_ptr);

	printf("  Type: %d\n", strptr->dsc$b_dtype);
	printf("  Class: %d\n", strptr->dsc$b_class);
	printf("  Length: %d\n", s1_length);
	if (s1_ptr != NULL)
	{
		printf("  Value: '%*.*s'\n", s1_length, s1_length, s1_ptr);
	}
	printf("\n");
}

void anacond(unsigned long condcode)
{
	switch (condcode)
	{
	    case STR$_NORMAL:
	    	printf("SS$_NORMAL\n");
		break;
	    case STR$_INVDELIM:
	    	printf("STR$_INVDELIM\n");
		break;
	    case STR$_NOELEM:
	    	printf("STR$_NOELEM\n");
		break;
	    case STR$_TRU:
	    	printf("STR$_TRU\n");
		break;
	    default:
	    	printf("Unknown condition code");
	}
}

