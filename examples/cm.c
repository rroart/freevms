#include<starlet.h>

extern struct _pcb * ctl$gl_pcb;

void * pcb=0;

getpcb(){
	pcb=ctl$gl_pcb;
}

main(){
	long arg=0;
	sys$cmkrnl(getpcb,&arg);
	printf("%x\n",pcb);
}
