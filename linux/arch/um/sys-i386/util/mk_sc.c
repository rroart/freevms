#include <stdio.h>
#include <signal.h>
#include <linux/stddef.h>

#define OFFSET(name, field) \
  printf("#define " name "(sc) *((unsigned long *) &(((char *) (sc))[%d]))\n",\
	 offsetof(struct sigcontext, field))

int main(int argc, char **argv)
{
  OFFSET("SC_IP", eip);
  OFFSET("SC_SP", esp);
  OFFSET("SC_FS", fs);
  OFFSET("SC_GS", gs);
  OFFSET("SC_DS", ds);
  OFFSET("SC_ES", es);
  OFFSET("SC_SS", ss);
  OFFSET("SC_CS", cs);
  OFFSET("SC_EFLAGS", eflags);
  OFFSET("SC_EAX", eax);
  OFFSET("SC_EBX", ebx);
  OFFSET("SC_ECX", ecx);
  OFFSET("SC_EDX", edx);
  OFFSET("SC_EDI", edi);
  OFFSET("SC_ESI", esi);
  OFFSET("SC_EBP", ebp);
  OFFSET("SC_TRAPNO", trapno);
  OFFSET("SC_ERR", err);
  OFFSET("SC_CR2", cr2);
  return(0);
}
