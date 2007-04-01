/*
 * Facility:	LIBCMU
 *
 * Abstract:	Network to/from host short and long conversions
 *
 * Modifications:
 *	16-SEP-1993 mlo 1.0.0
 *		original
 */
#ifdef VAXC
#module NTOH_HTON "v1.0.0"
#endif

#define ntohl kern_ntohl
#define ntohs kern_ntohs
#define htonl kern_htonl
#define htons kern_ntons

unsigned long int ntohl(x)
unsigned long int x;
{
    return(( ((x) >> 24)& 0x000000ff ) | ( ((x) >> 8) & 0x0000ff00 ) |
	   ( ((x) << 8) & 0x00ff0000 ) | ( ((x) << 24)& 0xff000000 ));
}

unsigned short int ntohs(x)
unsigned short int x;
{
    return((((x) >> 8) | ((x) << 8)) & 0xffff);
}

unsigned long int htonl(x)
unsigned long int x;
{
    return(( ((x) >> 24)& 0x000000ff ) | ( ((x) >> 8) & 0x0000ff00 ) |
	   ( ((x) << 8) & 0x00ff0000 ) | ( ((x) << 24)& 0xff000000 ));
}

unsigned short int htons(x)
unsigned short int x;
{
    return((((x) >> 8) | ((x) << 8)) & 0xffff);
}
