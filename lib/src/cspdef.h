#ifndef cspdef_h
#define cspdef_h

CSP$K_MAX_FLWCTL               = 00000008
CSP$K_MAX_WAITING              = 00000032

CSP$M_CLEANUP                  = 00000001

CSP$V_CLEANUP                  = 00000000

CSP$_ABORT                     = 00000002
CSP$_BADCSD                    = 00000003
CSP$_DONE                      = 00000004
CSP$_LOCAL                     = 00000007                 
CSP$_REJECT                    = 00000006
CSP$_REPLY                     = 00000005

struct _csp {
  /* first? */
  unsigned char csp$b_waitcnt;
  unsigned char csp$b_rcvcsdcnt;
  /* 10 empty? */
  unsigned char csp$b_inited;
  unsigned char csp$b_status;
  unsigned char csp$b_rcvcwpscnt;
  /* more? */
}

#endif

