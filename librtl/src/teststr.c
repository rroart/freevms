/*
 * teststr.c
 *
 *	Test program for str functions
 *
 * Notes:
 *
 *	This program should be run using the Free-VMS packages
 *	'str$' functions, Vax/VMS's, and Alpha/VMS's to make
 *	sure they all happily work the same.
 *
 * History:
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Initial version
 *
 *	Feb 4, 1997 - Kevin Handy
 *		Lose variable 'Tets2', change '%d' to '%ld' in several
 *		places to lose '-Wall' messages.
 *		Include 'stdlib.h'
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *		Add 'include <strdef.h>'
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*
 * Prototypes
 */
void testanal(char* Title, struct dsc$descriptor_s* strptr);

/*
 * Some local constants
 */
$DESCRIPTOR(GlobalString1, "This is global1");
$DESCRIPTOR(GlobalString2, "This is GlObAl2 . . . Longer");
$DESCRIPTOR(GlobalString3, "This has spaces             ");
$DESCRIPTOR(GlobalString4, "This has spaces");

int main()
{
	struct dsc$descriptor_s Test1;
	struct dsc$descriptor_s Test3;
	unsigned short ten = 10;
	long seventeen = 17;
	long eight = 8;
	char TextChar = 'R';
	short answer1;

	/*
	 * Lets prove we started
	 */
	printf("Starting tests . . .\n");

	/*
	 * See if $DESCRIPTOR works
	 */
	testanal("GlobalString1", &GlobalString1);
	testanal("GlobalString2", &GlobalString2);

	/*
	 * Let's try str$get1_dx
	 */
	Test1.dsc$w_length = 0;
	Test1.dsc$b_class = DSC$K_CLASS_D;
	Test1.dsc$b_dtype = DSC$K_DTYPE_T;
	Test1.dsc$a_pointer = NULL;

	testanal("Pre Test1", &GlobalString1);
	printf("get1dx = %ld\n", str$get1_dx(&ten, &Test1));
	memcpy(Test1.dsc$a_pointer, "qwertyuiop", 10);	/* Force out garbage */
	testanal("get1_dx test", &Test1);

	/*
	 * Let's try an append while we have a dynamic string
	 */
	printf("str$append = %ld\n", str$append(&Test1, &GlobalString1));
	testanal("appended", &Test1);

	/*
	 * Now test dupl_char
	 */
	printf("dupl_char = %ld\n",
		str$dupl_char(&Test1, &seventeen, &TextChar));
	testanal("dupl", &Test1);

	printf("left = %ld\n",
		str$left(&Test1, &GlobalString2, &eight));
	testanal("left", &Test1);

	printf("right = %ld\n",
		str$right(&Test1, &GlobalString2, &eight));
	testanal("right", &Test1);

	printf("upcase = %ld\n",
		str$upcase(&Test1, &GlobalString2));
	testanal("upcase", &Test1);

	printf("trim = %ld",
		str$trim(&Test1, &GlobalString3, &answer1));
	printf(" (%d)\n", answer1);
	testanal("trim", &Test1);

	printf("pos_extr = %ld",
		str$pos_extr(&Test1, &GlobalString2, &eight, &eight));
	testanal("pos_extr", &Test1);

	/*
	 * Free1_dx
	 */
	printf("free1dx = %ld\n", str$free1_dx(&Test1));
	testanal("free1_dx test", &Test1);

	/*
	 * Comparisons
	 */
	testanal("Global1", &GlobalString1);
	testanal("Global2", &GlobalString2);

	printf("str$compare(1,1) = %ld\n", 
		str$compare(&GlobalString1, &GlobalString1));
	printf("str$compare(1,2) = %ld\n",
		str$compare(&GlobalString1, &GlobalString2));
	printf("str$compare(2,1) = %ld\n",
		str$compare(&GlobalString2, &GlobalString1));
	printf("str$compare(2,2) = %ld\n",
		str$compare(&GlobalString2, &GlobalString2));
	printf("str$compare(3,4) = %ld\n",
		str$compare(&GlobalString3, &GlobalString4));
	printf("str$compare(4,3) = %ld\n",
		str$compare(&GlobalString4, &GlobalString3));
	printf("str$compare(2,4) = %ld\n",
		str$compare(&GlobalString2, &GlobalString4));

	printf("\n");

	printf("str$compare_eql(1,1) = %ld\n",
		str$compare_eql(&GlobalString1, &GlobalString1));
	printf("str$compare_eql(1,2) = %ld\n",
		str$compare_eql(&GlobalString1, &GlobalString2));
	printf("str$compare_eql(2,1) = %ld\n",
		str$compare_eql(&GlobalString2, &GlobalString1));
	printf("str$compare_eql(2,2) = %ld\n",
		str$compare_eql(&GlobalString2, &GlobalString2));
	printf("\n");


	printf("str$case_blind_compare(1,1) = %ld\n",
		str$case_blind_compare(&GlobalString1, &GlobalString1));
	printf("str$case_blind_compare(1,2) = %ld\n",
		str$case_blind_compare(&GlobalString1, &GlobalString2));
	printf("str$case_blind_compare(2,1) = %ld\n",
		str$case_blind_compare(&GlobalString2, &GlobalString1));
	printf("str$case_blind_compare(2,2) = %ld\n",
		str$case_blind_compare(&GlobalString2, &GlobalString2));
	printf("\n");

	printf("Copy_r = %ld\n", str$copy_r(&Test1, &ten, &"A Test String"));
	testanal("Test1", &Test1);

	Test3.dsc$w_length = 20;
	Test3.dsc$b_class = DSC$K_CLASS_S;
	Test3.dsc$b_dtype = DSC$K_DTYPE_T;
	Test3.dsc$a_pointer = (char*)malloc(20);
	printf("Copy_r = %ld\n", str$copy_r(&Test3, &ten, &"A Test String"));
	testanal("Test3", &Test3);
	testanal("Global3", &GlobalString2);

	printf("copy_dx = %ld\n", str$copy_dx(&Test3, &GlobalString2));
	testanal("Test3", &Test3);

	return EXIT_SUCCESS;
}

void testanal(char* Title, struct dsc$descriptor_s* strptr)
{
	char* s1_ptr;			/* Pointer to first string */
	unsigned short s1_length;	/* Length of first string */

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

