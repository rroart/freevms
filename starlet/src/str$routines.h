/*
 * str$routines.h
 *
 *  Code for VAX STR$ routines
 *
 * Description:
 *
 *  This file contains various 'str$' functions equivalent
 *  to those available in Vax/VMS string library.
 *
 * Bugs:
 *
 *  Not compatible at the binary level.
 *
 *  No seperate "string zone" to allocate memory from,
 *  uses malloc/free instead.
 *
 * History
 *
 *  Oct 10, 1996 - Kevin Handy
 *      Preliminary design
 *
 *  Feb 7, 1997 - Christof Zeile
 *      Change 'short' to 'unsigned short' in several places.
 */

#ifndef STR_ROUTINES_H
#define STR_ROUTINES_H

#include <descrip.h>

#undef _buildz
#define _buildz(...) __VA_ARGS__, 0

#define DOSIGNAL(x) assert(x && 0)

#ifndef min
#define min(x,y) ((x)<(y) ? (x) : (y))
#define max(x,y) ((x)>(y) ? (x) : (y))
#endif

/*
 * Constants
 */
extern const unsigned long str$_facility;
extern const unsigned long str$_fatinterr;
extern const unsigned long str$_divby_zer;
extern const unsigned long str$_errfredyn;
extern const unsigned long str$_illstrcla;
extern const unsigned long str$_illstrpos;
extern const unsigned long str$_illstrspe;
extern const unsigned long str$_insvirmem;
extern const unsigned long str$_invdelim;
extern const unsigned long str$_match;
extern const unsigned long str$_negstrlen;
extern const unsigned long str$_noelem;
extern const unsigned long str$_nomatch;
extern const unsigned long str$_normal;
extern const unsigned long str$_strtoolon;
extern const unsigned long str$_stris_int;
extern const unsigned long str$_tru;
extern const unsigned long str$_wronumarg;

/*
 * Prototypes
 */
int str$add(const unsigned long* asign, const long* aexp, const struct dsc$descriptor_s* adigits, const unsigned long* bsign,
        const long* bexp, const struct dsc$descriptor_s* bdigits, unsigned long* csign, long* cexp,
        struct dsc$descriptor_s* cdigits);

short int str$analyze_sdesc(const struct dsc$descriptor_s* input_descriptor, unsigned short* word_integer_length,
        char** data_address);

unsigned long str$analyze_sdesc_64(const struct dsc$descriptor_s* descrip, unsigned long long *quad_integer_length,
        char **data_address, unsigned short *descriptor_type);

int str$append(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string);

int str$case_blind_compare(const struct dsc$descriptor_s* first_source_string, const struct dsc$descriptor_s* second_source_string);

int str$compare(const struct dsc$descriptor_s* first_source_string, const struct dsc$descriptor_s* second_source_string);

int str$compare_eql(const struct dsc$descriptor_s* first_source_string, const struct dsc$descriptor_s* second_source_string);

int str$compare_multi(const struct dsc$descriptor_s* first_source_string, const struct dsc$descriptor_s* second_source_string,
        const unsigned long* flags_value, const unsigned long* foreign_language);

int str$concat(struct dsc$descriptor_s* destination_string, ...);
#define str$concat(d, ...) str$concat(d, _buildz(__VA_ARGS__))

int str$copy_dx(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string);

int str$copy_r(struct dsc$descriptor_s* destination_string, const unsigned short* word_integer_source_length,
        const void* source_string_address);

int str$copy_r_64(struct dsc$descriptor_s* destination_string, const unsigned short* word_integer_source_length,
        const void *source_string_address);

int str$divide(const unsigned long* asign, const long* aexp, const struct dsc$descriptor_s* a_digits, const unsigned long* bsign,
        const long* bexp, const struct dsc$descriptor_s* b_digits, const long* total_digits,
        const unsigned long* round_truncate_indicator, unsigned long* csign, long* cexp, struct dsc$descriptor_s* cdigits);

int str$dupl_char(struct dsc$descriptor_s* destination_string, const long* repetition_count, const char* ascii_character);

int str$element(struct dsc$descriptor_s* destination_string, const long* element_number,
        const struct dsc$descriptor_s* delimiter_string, const struct dsc$descriptor_s* source_string);

int str$find_first_in_set(const struct dsc$descriptor_s* source_string, const struct dsc$descriptor_s* set_of_characters);

int str$find_first_not_in_set(const struct dsc$descriptor_s* source_string, const struct dsc$descriptor_s* set_of_characters);

int str$find_first_substring(const struct dsc$descriptor_s* source_string, long* index, long* substring_index,
        struct dsc$descriptor_s *sub, ...);
#define str$find_first_substring(s, i, si, sub, ...) str$find_first_substring(s, i, si, sub, _buildz(__VA_ARGS__))

int str$free1_dx(struct dsc$descriptor_s* string_descriptor);

int str$get1_dx(unsigned short* word_integer_length, struct dsc$descriptor_s* character_string);

int str$get1_dx_64(unsigned short* word_integer_length, struct dsc$descriptor_s* character_string);

int str$left(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string, const long* end_position);

int str$len_extr(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string,
        const long* start_position, const long* longword_integer_length);

int str$match_wild(const struct dsc$descriptor_s* candidate_string, const struct dsc$descriptor_s* pattern_string);

int str$mul(const unsigned long* asign, const long* aexp, const struct dsc$descriptor_s* adigits, const unsigned long* bsign,
        const long* bexp, const struct dsc$descriptor_s* bdigits, unsigned long* csign, long* cexp,
        struct dsc$descriptor_s* cdigits);

unsigned int str$position(const struct dsc$descriptor_s* source_string, const struct dsc$descriptor_s* substring,
        const long* start_position);

int str$pos_extr(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* cource_string,
        const long* start_position, const long* end_position);

int str$prefix(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string);

int str$recip(const unsigned long* asign, const long* aexp, const struct dsc$descriptor_s* adigits, const unsigned long* bsign,
        const long* bexp, const struct dsc$descriptor_s* bdigits, unsigned long* csign, long* cexp,
        struct dsc$descriptor_s* cdigits);

int str$replace(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string,
        const long* start_position, const long* end_position, const struct dsc$descriptor_s* replacement_string);

int str$right(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string,
        const long* start_position);

int str$round(const long* places, const unsigned long* flags, const unsigned long* asign, const long* aexp,
        const struct dsc$descriptor_s* adigits,
#if 0
// where does this come from?
        const unsigned long* bsign, const long* bexp,
        const struct dsc$descriptor_s* bdigits,
#endif
        unsigned long* csign, long* cexp, struct dsc$descriptor_s* cdigits);

int str$sub(const unsigned long *asign, const long *aexp, const struct dsc$descriptor_s *adigits, const unsigned long *bsign,
        const long *bexp, const struct dsc$descriptor_s *bdigits, unsigned long *csign, long *cexp,
        struct dsc$descriptor_s *cdigits);

int str$translate(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string,
        const struct dsc$descriptor_s* translation_string, const struct dsc$descriptor_s* match_string);

int str$trim(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string,
        unsigned short* resultant_length);

int str$upcase(struct dsc$descriptor_s* destination_string, const struct dsc$descriptor_s* source_string);

/*
 * Some special functions that are not really part of str$,
 * but are used internally be them
 */
int str$$is_string_class(const struct dsc$descriptor_s* test_string);
int str$$iszero(const struct dsc$descriptor_s *sd1);
int str$$lzerotrim(struct dsc$descriptor_s *sd1);
int str$$iszerotrim(struct dsc$descriptor_s *sd1, long *exp);
int str$$rzerotrim(struct dsc$descriptor_s *sd1, long *exp);
int str$$ncompare(struct dsc$descriptor_s *sd1, struct dsc$descriptor_s *sd2);

int str$$resize(struct dsc$descriptor_s* dest, unsigned short size);
int str$$copy_fill(char* dest_ptr, unsigned short dest_length, const char* source_ptr, unsigned short source_length, char fill);
void str$$malloc_sd(struct dsc$descriptor_s *temp_sd, char *string);
void str$$print_sd(const struct dsc$descriptor_s *sd1);

#endif /* STR_ROUTINES_H */
