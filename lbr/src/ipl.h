#ifndef ipl_h
#define ipl_h

extern void splx(unsigned char old);

extern unsigned char spl(unsigned char new);

extern int prespl(unsigned char new);

#endif
