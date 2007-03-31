/*
 * Facility:	LIBCMU
 *
 * Abstract:	AF_INET address binary to/from ascii
 *
 * Modifications:
 *	 7-OCT-1993 mlo 1.0.1
 *		add hex recognision to inet_addr
 *	 6-OCT-1993 mlo 1.0.0
 *		original
 */	
#ifdef VAXC
#module INET "v1.0.1"
#endif

#ifndef __KERNEL__
#include <sys/types.h>
#include <sys/ctype.h>
#include <stdlib.h>
#else
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#define malloc(x) kmalloc(x, GFP_KERNEL)
#define cfree kfree
#define free kfree
#define strtoul simple_strtoul
#endif
#include <netinet/in.h>

#ifndef NULL
#define NULL (char *)'\0'
#endif

unsigned long inet_addr(str)
char *str;
{
int	i;
char	buf[4];
unsigned long *addr = buf;
char	*tmp, *cp, *dot, *cx;

    if (strlen(str) == 0)	/* return 0 if strlen is 0		*/
	return(0);

    /*
     * Make a copy of the input string
     */
    tmp = malloc(strlen(str));
    strcpy(tmp,str);

    cp  = tmp;			/* reset the pointer			*/
    *addr = 0x00000000;		/* zero the result			*/

    for (i=0; dot != NULL; i++) {

	if (i > 3)		/* more than 4 parts is an error	*/
	    goto ERROR;

	if ((dot = strchr(cp,'.')) != NULL)
	    *dot = '\0';

	if (strlen(cp)<1)	/* invalid part length error		*/
	    goto ERROR;

	/*
	 * Check for valid formatted number
	 */
	if ((*cp=='0')&&((*(cp+1)&20)=='x')) {	/* is hex?		*/
	    for (cx=(&cp[2]); *cx != '\0'; cx++)  /* yes			*/
		if (!isxdigit(*cx))
		    goto ERROR;
	}
	else {
	    for (cx=cp; *cx != '\0'; cx++)	/* not hex		*/
		if (!isdigit(*cx))
		    goto ERROR;
	}

	buf[i-1] = buf[3];	/* shift the bytes and convert the next */
	buf[3] = (char)strtoul(cp,0,0);
	cp = dot + 1;
    }

    free(tmp);
    return (*addr);		/* give back binary address		*/

ERROR:
    free(tmp);
    return(-1);
}

char *inet_ntoa(in)
struct in_addr in;
{
static char str[20];
   sprintf(str,"%d.%d.%d.%d",in.S_un.S_un_b.s_b1,in.S_un.S_un_b.s_b2,
			     in.S_un.S_un_b.s_b3,in.S_un.S_un_b.s_b4);
   return(str);
}
