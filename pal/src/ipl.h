#ifndef ipl_h
#define ipl_h

/* Current API:
   pushpsl/poppsl: equivalent of push and pop on the stack
   savipl: not yet in use: about the same as the macro?
   setipl: about the same as macro. plain "mtpr".
   chm: chmx-ish. should be done after it has startet the exe$-routine?
   regtrap: equivalent of some of what it does when intr or exception starts
   intr_blocked: in beginning of sw (and hw?) interrupts: return if wrong
                 ipl level
   do_sw_int: something for REI; do pending software interrupts
   myrei: my version of REI? before each return?

   Forget:
   *spl*: bsd-ish
*/   

/* For things done to PSL etc on exception or interrupt */

#define REG_INTR 0
#define REG_CHMX 1
#define REG_OTHER 2

extern void splret(void);

extern char spl(unsigned char new);

extern inline void savipl(void);

/* no smp yet */
inline void setipl(unsigned char i);

//extern int prespl(unsigned char new);

#endif
