
/*
 *	strutil.c
 *	Miscellaneous string routines 
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
 *The author(s) may be contacted at:
 *
 *	Andrew Allison  	freevms@sympatico.ca
 *
 *				Andrew Allison
 *				50 Denlaw Road
 *				London, Ont
 *				Canada 
 *				N6G 3L4
 *
 *	Kevin Handy		Unknown
 *
 *	Christof Zeile		Unknown
 *
 *	Feb 18, 2004 - Andrew Allison
 *		Change malloc to calloc to initialize memory
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include <stdlib.h>
#include "strdef.h"
#include "libdef.h"
#include "str$routines.h"
//#include "stdint.h"

#define MAXSTR 		132000
#define MAXUINT16	65536
#define MAXCSTR16	65535
#define TRUE		1
#define FALSE		0


/************************************************/
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

/************************************************/
/*
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
/************************************************/
/************************************************/
/*	str$$iszero
 *
 * Description:
 *
 *	Boolean function to determine if a string is equal to zero
 *
 *	Returns True if string is equal to zero
 *		False if string is not equal to zero
 *	
 * Bugs:
 *
 * History
 *	2004	Jan 	Andrew Allison	Initial write
 */

int	str$$iszero (const struct dsc$descriptor_s *sd1)
{
int	i, c_not_zero;
char	*s1_ptr;
unsigned short s1_len;

	str$analyze_sdesc (sd1,&s1_len, &s1_ptr);
	c_not_zero = FALSE;
	for (i=0; i < s1_len; i++)
		if (s1_ptr[i] != '0')
			c_not_zero = TRUE;

	return c_not_zero;
}

/************************************************/
/*	str$$lzerotrim
 *
 * Description:
 *	Remove leading 0's from a descriptor
 *
 *	When passed a string descriptor remove all leading
 *	zeros from the string
 *
 * Bugs:
 *
 * History
 *	2004	Jan 	Andrew Allison	Initial write
 */

int	str$$lzerotrim (struct dsc$descriptor_s *sd1)
{
int	i,j,count;
char	*s1_ptr;
unsigned short s1_len;

	str$analyze_sdesc (sd1,&s1_len, &s1_ptr);
	i = 0;
	while ( (s1_ptr[i] == '0') && ( i < s1_len-1 ) ) // while leading zero
	{
		if (s1_ptr[i] == '0' )			// have leading zero
		{
			for (j = i; j < s1_len; j++)	// shuffle string
			{
				s1_ptr[j] = s1_ptr[j+1];
			}
			i=0;
			s1_len--;
			count--;
		}
		else
		{
			i++;
		}
	}

//	Resize descriptor if needed
	str$get1_dx (&s1_len, &*sd1);
	return count;
}

/***********************************************/
/*	str$$iszerotrim
 *
 * Description:
 *	If the string is zero set exponent to zero
 *
 * Bugs:
 *
 * History:
 *	2004	Jan 	Andrew Allison	Initial program creation
 */

int	str$$iszerotrim (struct dsc$descriptor_s *sd1, long *exp)
{
int	i, status, is_zero;
char	*s1_ptr;
unsigned short s1_len;

	is_zero = TRUE;
	status = str$analyze_sdesc (sd1,&s1_len, &s1_ptr);
	if ( s1_ptr != NULL )
	{
		for ( i = 0; i < s1_len; i++)
		{
			if (s1_ptr[i] != '0' )
			{	is_zero = FALSE;
			}
		}

		if ( is_zero )
		{	*exp = 0;
		}
	}
	return status;
}


/***********************************************/
/*	str$$rzerotrim
 *
 * Description:
 *	When passed a string descriptor remove 
 *	trailing 0 and incerement exponent
 *	i.e. Normallize the number
 *
 * Bugs:
 *
 * History:
 *	2004	Jan 	Andrew Allison	Initial program Creation
 *
 */

int	str$$rzerotrim (struct dsc$descriptor_s *sd1, long *exp)
{
int	i, status;
char	*s1_ptr;
unsigned short s1_len;

	status = str$analyze_sdesc (sd1,&s1_len, &s1_ptr);
	i = s1_len-1;
	while ( (s1_ptr[i] == '0') && (i > 0 ) )
	{
		if (s1_ptr[i] == '0' )
		{	s1_ptr[i] = ' ';
			(*exp)++;
			s1_len--;
		}
		i--;
	}

//	Resize descriptor
	status = str$get1_dx (&s1_len, sd1);
	str$analyze_sdesc (sd1,&s1_len, &s1_ptr);
	return status;
}


/************************************************/
/*	str$$ncompare
 *
 * Description:
 *	Compare two numeric strings
 *
 *	Return -1	String 1 < String 2
 *		0	String 1 = String 2
 *		1	String 1 > String 2
 *
 * Bugs:
 *
 * History
 *	2004	Jan	Andrew Allison
 *
 */

int str$$ncompare (	struct dsc$descriptor_s *sd1, 
			struct dsc$descriptor_s *sd2)
{
unsigned short	s1_len,  s2_len;
char		*s1_ptr, *s2_ptr;
int	 	min_len, max_len, i;

	str$$lzerotrim (sd1);
	str$$lzerotrim (sd2);

	str$analyze_sdesc (&*sd1,&s1_len, &s1_ptr);
	str$analyze_sdesc (&*sd2,&s2_len, &s2_ptr);

	min_len = ( s1_len < s2_len) ? s1_len : s2_len;
	max_len = ( s1_len > s2_len) ? s1_len : s2_len;

	if ( s1_len > s2_len )
		return  1;
	if ( s1_len < s2_len )
		return -1;

//	The string are of equal length
	for (i = 0; i < max_len; i++)
	{	if ( s1_ptr[i] > s2_ptr[i] )
			return  1;
		if ( s1_ptr[i] < s2_ptr[i] )
			return -1;
	}

	return 0;
}

/************************************************/
/*	str$$print_sd
 *
 * Description:
 *	Print out a string descriptor
 *
 *
 * Bugs:
 *
 * History
 *	2004	Jan	Andrew Allison	Initial program creation
 *
 */

void	str$$print_sd (const struct dsc$descriptor_s *sd1 )
{
unsigned short	s1_len;
char		*s1_ptr;
int		i, qmark;

qmark = '?';
str$analyze_sdesc (sd1,&s1_len, &s1_ptr);
if ( s1_len >= 65500 )
{	printf ("%.20s ... %20s",s1_ptr,&s1_ptr[s1_len-20]);
	return;
}
if ( ( s1_len == 0) && ( s1_ptr == NULL ))
{	printf ("null");
	return;
}

for (i = 0; i < s1_len; i++ )
{
	if (isprint (s1_ptr[i]) )
		putchar (s1_ptr[i]);
	else if ( s1_ptr[i] == '\t' )
		putchar ('\t');
	else
		putchar (qmark);
}
return;

}


/************************************************/
/*	str$$malloc_sd
 *
 * Description:
 *	Allocate space for a string descriptor and
 *	return a pointer to the structure
 *
 * Bugs:
 *
 * History
 *	2004	Jan	Andrew Allison	Initial Program Creation
 *
 */


void str$$malloc_sd(struct dsc$descriptor_s *temp_sd, char *string)
{
int i;
unsigned short temp_len;
char	maxstring [MAXCSTR16], temp_string[5];

if ( strcmp(string,"NULL") == 0 )
{
	temp_sd->dsc$w_length  = 0;
	temp_sd->dsc$b_class   = DSC$K_CLASS_D;
	temp_sd->dsc$b_dtype   = DSC$K_DTYPE_T;
	temp_sd->dsc$a_pointer = NULL;
}
// make the largest number possible 65535 9's
else if ( strcmp(string,"MAXCSTR16") == 0 )
{
	for (i=0; i < MAXCSTR16; i++)
		maxstring[i] = '9';
	temp_sd->dsc$w_length  = 0;
	temp_sd->dsc$b_class   = DSC$K_CLASS_D;
	temp_sd->dsc$b_dtype   = DSC$K_DTYPE_T;
	temp_sd->dsc$a_pointer = NULL;
	
	temp_len = MAXCSTR16;
	str$get1_dx (&temp_len, temp_sd);
	str$copy_r  (temp_sd, &temp_len, maxstring);

}
else if ( strcmp(string,"BLANK") == 0 )
{
	temp_sd->dsc$w_length  = 0;
	temp_sd->dsc$b_class   = DSC$K_CLASS_D;
	temp_sd->dsc$b_dtype   = DSC$K_DTYPE_T;
	temp_sd->dsc$a_pointer = NULL;

	temp_len = 1;
	temp_string[0] = ' ';
	str$get1_dx (&temp_len, temp_sd);
	str$copy_r  (temp_sd, &temp_len,temp_string);
}
else if ( strcmp(string,"TAB") == 0 )
{
	temp_sd->dsc$w_length  = 0;
	temp_sd->dsc$b_class   = DSC$K_CLASS_D;
	temp_sd->dsc$b_dtype   = DSC$K_DTYPE_T;
	temp_sd->dsc$a_pointer = NULL;

	temp_len = 1;
	temp_string[0] = '\t';
	str$get1_dx (&temp_len, temp_sd);
	str$copy_r  (temp_sd, &temp_len,temp_string);
}
else
{
/*	default action copy string value in */
	temp_sd->dsc$w_length  = 0;
	temp_sd->dsc$b_class   = DSC$K_CLASS_D;
	temp_sd->dsc$b_dtype   = DSC$K_DTYPE_T;
	temp_sd->dsc$a_pointer = NULL;

	temp_len = strlen (string);
	str$get1_dx (&temp_len, temp_sd);
	str$copy_r  (temp_sd, &temp_len,string);
	
}
return;
}

/***********************************************/
/*	str$$copy_fill
 *
 * Description:
 *	Copies over text from source to dest.
 *
 * Bugs:
 *
 * History
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *
 *	Mar 10, 2004 - Andrew Allison
 *		Added code to skip memcopy if source was NULL
 */
unsigned long str$$copy_fill(char *dest_ptr, unsigned short dest_length,
	const char *source_ptr, unsigned short source_length, char fill)
{
unsigned short max_copy;
int fill_loop;


	/*
	 * Copy over however much can.
	 */
	max_copy = min(dest_length, source_length);
	if (  source_ptr !=  NULL )
	{
		memcpy(dest_ptr, source_ptr, max_copy);
	}
	/*
	 * Fill in the rest
	 */
	if ( max_copy < dest_length )
		for (fill_loop = max_copy; fill_loop < dest_length; fill_loop++)
		{
			dest_ptr[fill_loop] = fill;
		}

	/*
	 * Figure out what to return
	 */
	if (source_length > dest_length)
	{
		return STR$_TRU;
	}
	else
	{
		return STR$_NORMAL;
	}

}

/***********************************************/
/*	str$$resize
 *
 * Description:
 *	Try to resize the destination, giving as close to the
 *	desired final size as possible.
 *
 * Bugs:
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design.
 *
 *	Feb 7, 1997 - Christof Ziele
 *		Changed 'short' to 'unsigned short' in several places.
 *
 *	Jan 15, 2004 - Andrew Allison
 *		Changed unsigned short to int to quiet compiler complaints
 */

unsigned int str$$resize(struct dsc$descriptor_s* dest, unsigned short size)
{
	unsigned long result = STR$_NORMAL;
	unsigned short	usize;
	int resize;

	usize = (unsigned short) size;
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
		result = str$get1_dx(&usize, dest);
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
		resize = min(dest->dsc$w_length, usize);
		*((unsigned short int*)dest->dsc$a_pointer) = resize;
		break;

	default:
		DOSIGNAL(STR$_ILLSTRCLA);
		result = STR$_ILLSTRCLA;
	}

	return result;
}


/************************************************/
/*	str$$is_string_class
 *
 *	This function is used to determine if the descriptor
 *	passed in is actually a viable string for this set of
 *	functions.
 *
 *	Code for VAX STR$$IS_STRING_CLASS routine
 *
 * Description:
 *
 * Bugs:
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy	Preliminary design.
 *
 */
unsigned long str$$is_string_class(const struct dsc$descriptor_s* test_string)
{
	/*
	 * Did we get passed anything?
	 */
	if (test_string == NULL)
	{
		return STR$_FATINTERR;
	}

	/*
	 * Make sure it is a atring class we can handle
	 */
	switch (test_string->dsc$b_class)
	{
	case DSC$K_CLASS_Z:
	case DSC$K_CLASS_S:
	case DSC$K_CLASS_SD:
	case DSC$K_CLASS_VS:
	case DSC$K_CLASS_D:
	case DSC$K_CLASS_A:
	case DSC$K_CLASS_NCA:
		break;

	default:
		return STR$_ILLSTRCLA;
	}

	return STR$_NORMAL;
}

/************************************************/
