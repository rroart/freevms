/*
 * str.c
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
 *	Some versions of realloc are broken! Some don't like to be
 *	called hundreds of times.  The program may
 *	crash with a segmentation fault in such a case.
 *
 *	I probibly should be using dsc$descriptor instead of
 *	dsc$descriptor_s, but I prefer to have the type 'char*'
 *	instead of 'void*' which is the only difference.
 *
 *	Not worrying about the dsc$b_dtype field yet. Assumes it
 *	will always be type DSC$K_DTYPE_T (character coded text).
 *	Type V should mean size is in bits, and P means size is in
 *	digits (4 bit nibbles).
 *
 * History
 *
 *	Oct 10, 1996 - Kevin Handy
 *		Preliminary design. Spelling errors are
 *		not my fault! Someone must have snuck them in
 *		there when I wasn't looking.
 *
 *	Feb 4, 1997 - Kevin Handy
 *		Added a 'return STR$_ILLSTRCLA' so that compiling
 *		with '-Wall' won't display errors.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "descrip.h"
#include "strdef.h"
#include "str$routines.h"

/*************************************************************
 * str$add
 *
 *	Add two decimal strings of digits
 */
unsigned long str$add_not(const unsigned long* asign, const long* aexp, 
	const struct dsc$descriptor_s* adigits,
	const unsigned long* bsign, const long*bexp, 
	const struct dsc$descriptor_s* bdigits,
	unsigned long* csign, long* cexp, struct dsc$descriptor_s* cdigits)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}


/*************************************************************
 * str$compare_multi
 *
 */
long str$compare_multi_not(
	const struct dsc$descriptor_s* first_source_string,
	const struct dsc$descriptor_s* second_source_string,
	const long* flags_value,
	const long* foreign_language)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}


/*************************************************************
 * str$divide
 *
 */
unsigned long str$divide_not(
	const unsigned long* asign, const long* aexp, const struct dsc$descriptor_s* a_digits,
	const unsigned long* bsign, const long* bexp, const struct dsc$descriptor_s* b_digits,
	const long* total_digits, const long* round_truncate_indicator,
	unsigned long* csign, long* cexp, struct dsc$descriptor_s* cdigits)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}


/*************************************************************
 * str$match_wild
 *
 */
unsigned long str$match_wild_not(const struct dsc$descriptor_s* candidate_string,
	const struct dsc$descriptor_s* pattern_string)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}

/*************************************************************
 * str$mul
 *
 */
unsigned long str$mul_not(const unsigned long* asign, const long aexp,
	const struct dsc$descriptor_s* adigits,
	const unsigned long* bsign, const long* bexp,
	const struct dsc$descriptor_s* bdigits,
	unsigned long* csign, long* cexp,
	struct dsc$descriptor_s* cdigits)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}


/*************************************************************
 * str$recip
 *
 */
unsigned long str$recip_not(
	const unsigned long* asign, const long* aexp,
	const struct dsc$descriptor_s* adigits,
	const unsigned long* bsign, const long* bexp,
	const struct dsc$descriptor_s* bdigits,
	unsigned long* csign, long* cexp,
	struct dsc$descriptor_s* cdigits)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}

/*************************************************************
 * str$replace
 *
 */
unsigned long str$replace_not(struct dsc$descriptor_s* destinstaion_string,
	const struct dsc$descriptor_s* source_string,
	const long* start_position,
	const long* end_position,
	const struct dsc$descriptor_s* replacement_string)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}

/*************************************************************
 * str$round
 *
 */
unsigned long str$round_not(const long* places, unsigned long* flags,
	const unsigned long* asign, const long* aexp,
	const struct dsc$descriptor_s* adigits,
	const unsigned long* bsign, const long* bexp,
	const struct dsc$descriptor_s* bdigits,
	unsigned long* csign, long* cexp,
	struct dsc$descriptor_s* cdigits)
{
	/* Not yet coded */
	assert(0);
	return STR$_ILLSTRCLA;
}

