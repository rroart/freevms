// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <otsdef.h>
#include <descrip.h>
#include <libdef.h>
#include <ssdef.h>
#include <starlet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// lots of missing checks, bugs and endianness here
// but at least the basic functionality is in place

int ots$cvt_l_tb (void * varying_input_value,void * fixed_length_resultant_string ,int number_of_digits ,int input_value_size) {
  struct dsc$descriptor * dsc = fixed_length_resultant_string;
  int max = dsc->dsc$w_length;
  if (number_of_digits)
    max = number_of_digits;
  if (max > dsc->dsc$w_length) {
    memset (dsc->dsc$a_pointer, '*', dsc->dsc$w_length);
    return  OTS$_OUTCONERR;
  }
  if (input_value_size == 0)
    input_value_size = 4;
  int val = * (int *) varying_input_value;
  int bits = 8 * input_value_size;
  long long high = 1 << (bits - 1);
  char * pos = dsc->dsc$a_pointer;
  for ( ; max && bits ; pos++, max--, bits--, val <<= 1) {
    char byte = '0';
    if (val & high)
      byte = '1';
    *pos = byte;
  }
  return SS$_NORMAL;
}

int ots$cvt_l_ti (void * varying_input_value ,void * fixed_length_resultant_string ,int number_of_digits ,int input_value_size ,int flags_value) {
  struct dsc$descriptor * dsc = fixed_length_resultant_string;
  int max = dsc->dsc$w_length;
  if (number_of_digits)
    max = number_of_digits;
  if (max > dsc->dsc$w_length) {
    memset (dsc->dsc$a_pointer, '*', dsc->dsc$w_length);
    return  OTS$_OUTCONERR;
  }
  if (input_value_size == 0)
    input_value_size = 4;
  long long val = 0;
  memcpy(&val, varying_input_value, input_value_size);
  snprintf(dsc->dsc$a_pointer, max, "%d", val);
  return SS$_NORMAL;
}

#if 0
int ots$cvt_l_tl (int * longword_integer_value ,void * fixed_length_resultant_string) {

}
#endif

int ots$cvt_l_to (void * varying_input_value ,void * fixed_length_resultant_string ,int number_of_digits ,int input_value_size) {
  struct dsc$descriptor * dsc = fixed_length_resultant_string;
  int max = dsc->dsc$w_length;
  if (number_of_digits)
    max = number_of_digits;
  if (max > dsc->dsc$w_length) {
    memset (dsc->dsc$a_pointer, '*', dsc->dsc$w_length);
    return  OTS$_OUTCONERR;
  }
  if (input_value_size == 0)
    input_value_size = 4;
  long long val = 0;
  memcpy(&val, varying_input_value, input_value_size);
  snprintf(dsc->dsc$a_pointer, max, "%o", val);
  return SS$_NORMAL;
} 

int ots$cvt_l_tu (void * varying_input_value ,void * fixed_length_resultant_string ,int number_of_digits ,int input_value_size) {
  struct dsc$descriptor * dsc = fixed_length_resultant_string;
  int max = dsc->dsc$w_length;
  if (number_of_digits)
    max = number_of_digits;
  if (max > dsc->dsc$w_length) {
    memset (dsc->dsc$a_pointer, '*', dsc->dsc$w_length);
    return  OTS$_OUTCONERR;
  }
  if (input_value_size == 0)
    input_value_size = 4;
  unsigned long long val = 0;
  memcpy(&val, varying_input_value, input_value_size);
  snprintf(dsc->dsc$a_pointer, max, "%d", val);
  return SS$_NORMAL;
}

int ots$cvt_l_tz (void * varying_input_value ,void * fixed_length_resultant_string ,int number_of_digits ,int input_value_size) {
  struct dsc$descriptor * dsc = fixed_length_resultant_string;
  int max = dsc->dsc$w_length;
  if (number_of_digits)
    max = number_of_digits;
  if (max > dsc->dsc$w_length) {
    memset (dsc->dsc$a_pointer, '*', dsc->dsc$w_length);
    return  OTS$_OUTCONERR;
  }
  if (input_value_size == 0)
    input_value_size = 4;
  long long val = 0;
  memcpy(&val, varying_input_value, input_value_size);
  snprintf(dsc->dsc$a_pointer, max, "%x", val);
  return SS$_NORMAL;
}

int ots$cvt_tb_l (void * fixed_or_dynamic_input_string ,void * varying_output_value ,int output_value_size ,int flags_value) {
  struct dsc$descriptor * dsc = fixed_or_dynamic_input_string;
  if (output_value_size == 0)
    output_value_size = 4;
  switch (output_value_size) {
  case 1:
  case 2:
  case 4:
  case 8:
    break;
  default:
    return OTS$_INPCONERR;
  }
  long long int retval = strtoll(dsc->dsc$a_pointer, 0, 2);
  memcpy (varying_output_value, &retval, output_value_size);
  return SS$_NORMAL;
}

int ots$cvt_ti_l (void * fixed_or_dynamic_input_string ,void * varying_output_value ,int output_value_size ,int flags_value) {
  struct dsc$descriptor * dsc = fixed_or_dynamic_input_string;
  if (output_value_size == 0)
    output_value_size = 4;
  switch (output_value_size) {
  case 1:
  case 2:
  case 4:
  case 8:
    break;
  default:
    return OTS$_INPCONERR;
  }
  long long int retval = strtoll(dsc->dsc$a_pointer, 0, 10);
  memcpy (varying_output_value, &retval, output_value_size);
  return SS$_NORMAL;
}

int ots$cvt_to_l (void * fixed_or_dynamic_input_string ,void * varying_output_value ,int output_value_size ,int flags_value) {
  struct dsc$descriptor * dsc = fixed_or_dynamic_input_string;
  if (output_value_size == 0)
    output_value_size = 4;
  switch (output_value_size) {
  case 1:
  case 2:
  case 4:
  case 8:
    break;
  default:
    return OTS$_INPCONERR;
  }
  long long int retval = strtoll(dsc->dsc$a_pointer, 0, 8);
  memcpy (varying_output_value, &retval, output_value_size);
  return SS$_NORMAL;
}

int ots$cvt_tu_l (void * fixed_or_dynamic_input_string ,void * varying_output_value ,int output_value_size ,int flags_value) {
  struct dsc$descriptor * dsc = fixed_or_dynamic_input_string;
  if (output_value_size == 0)
    output_value_size = 4;
  switch (output_value_size) {
  case 1:
  case 2:
  case 4:
  case 8:
    break;
  default:
    return OTS$_INPCONERR;
  }
  unsigned long long int retval = strtoll(dsc->dsc$a_pointer, 0, 10);
  memcpy (varying_output_value, &retval, output_value_size);
  return SS$_NORMAL;
}

int ots$cvt_tz_l (void * fixed_or_dynamic_input_string ,void * varying_output_value ,int output_value_size ,int flags_value) {
  struct dsc$descriptor * dsc = fixed_or_dynamic_input_string;
  if (output_value_size == 0)
    output_value_size = 4;
  switch (output_value_size) {
  case 1:
  case 2:
  case 4:
  case 8:
    break;
  default:
    return OTS$_INPCONERR;
  }
  long long int retval = strtoll(dsc->dsc$a_pointer, 0, 16);
  memcpy (varying_output_value, &retval, output_value_size);
  return SS$_NORMAL;
}





