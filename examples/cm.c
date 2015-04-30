#include <starlet.h>
#include <stdio.h>

extern struct _pcb * ctl$gl_pcb;

void * pcb=0;

getpcb()
{
#ifdef __i386__
    pcb=ctl$gl_pcb;
#endif
}

main()
{
#if 0
#ifdef __i386__
    long arg=0;
    sys$cmkrnl(getpcb,&arg);
    printf("%x\n",pcb);
#endif
#endif
}
