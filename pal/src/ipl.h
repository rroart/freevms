#ifndef ipl_h
#define ipl_h

extern void splx(void);

extern void spl(unsigned char new);

extern int prespl(unsigned char new);

#endif
