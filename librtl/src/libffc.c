#include<libdef.h>
#include<ssdef.h>

// borrowed from 2.6.14 bitops.h bitops.c

/**
 * find_first_zero_bit - find the first zero bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit-number of the first zero bit, not the number of the byte
 * containing a bit.
 */
static int find_first_zero_bit(const unsigned long *addr, unsigned size)
{
	int d0, d1, d2;
	int res;

	if (!size)
		return 0;
	/* This looks at memory. Mark it volatile to tell gcc not to move it around */
	__asm__ __volatile__(
		"movl $-1,%%eax\n\t"
		"xorl %%edx,%%edx\n\t"
		"repe; scasl\n\t"
		"je 1f\n\t"
		"xorl -4(%%edi),%%eax\n\t"
		"subl $4,%%edi\n\t"
		"bsfl %%eax,%%edx\n"
		"1:\tsubl %%ebx,%%edi\n\t"
		"shll $3,%%edi\n\t"
		"addl %%edi,%%edx"
		:"=d" (res), "=&c" (d0), "=&D" (d1), "=&a" (d2)
		:"1" ((size + 31) >> 5), "2" (addr), "d" (addr) : "memory");
	return res;
}
// -roart : had to to "b" -> "d" to compile pic

/**
 * find_next_zero_bit - find the first zero bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The maximum size to search
 */
static int find_next_zero_bit(const unsigned long *addr, int size, int offset)
{
	unsigned long * p = ((unsigned long *) addr) + (offset >> 5);
	int set = 0, bit = offset & 31, res;

	if (bit) {
		/*
		 * Look for zero in the first 32 bits.
		 */
		__asm__("bsfl %1,%0\n\t"
			"jne 1f\n\t"
			"movl $32, %0\n"
			"1:"
			: "=r" (set)
			: "r" (~(*p >> bit)));
		if (set < (32 - bit))
			return set + offset;
		set = 32 - bit;
		p++;
	}
	/*
	 * No zero yet, search remaining full bytes for a zero
	 */
	res = find_first_zero_bit (p, size - 32 * (p - (unsigned long *) addr));
	return (offset + set + res);
}

int lib$ffc(int * pos, char * size, int * base,int * ret) {
  int retval;
  if ((*size)<=32) {
    if ((*base)==0xffffffff) {
      *ret=*size;
      return LIB$_NOTFOU;
    }
#if 0
    retval=ffz(*base);
#endif
    retval = find_next_zero_bit (base, *size, *pos);

  } else {
    printf("ffc retval for > 32 bit size not yet implemented");
    return LIB$_NOTFOU;
    retval = find_next_zero_bit (base, *size, *pos);
  }
  if (retval>=(*size))
    return LIB$_NOTFOU;
  *ret=retval;
  return SS$_NORMAL;
}
