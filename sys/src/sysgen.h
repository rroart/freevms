#ifndef __SYSGEN_LOADED
#define __SYSGEN_LOADED 1

/* Will change name to sysparam.h */
 
/*Parameter Name            Current    Default     Min.     Max.     Unit  Dynamic */

#define ACP_BASEPRIO                    8   /*        8         4        31 Priority   D */
#define ACP_DATACHECK                   2   /*        2         0        99 Bit-mask   D */
#define ACP_DINDXCACHE                 42   /*       26         2     65535 Pages      D */
#define ACP_DIRCACHE                  168   /*       22         4     65535 Pages      D */
#define ACP_EXTCACHE                   64   /*       64         0      2000 Extents    D */
#define ACP_EXTLIMIT                  100   /*      100         0      1000 Percent/10 D */
#define ACP_FIDCACHE                   64   /*       64         0       100 File-Ids   D */
#define ACP_HDRCACHE                  168   /*       36         8     65535 Pages      D */
#define ACP_MAPCACHE                   10   /*        9         2     65535 Pages      D */
#define ACP_MAXREAD                    32   /*       32         1        64 Blocks     D */
#define ACP_MULTIPLE                    0   /*        0         0         1 Boolean    D */
#define ACP_QUOCACHE                  160   /*       64         0      2337 Users      D */
#define ACP_REBLDSYSD                   1  /*         1         0         1 Boolean     */
#define ACP_SHARE                       1  /*         1         0         1 Boolean     */
#define ACP_SWAPFLGS                   14   /*       15         0        15 Bit-mask   D */
#define ACP_SYSACC                     16   /*        8         0     65535 Directorie D */
#define ACP_WINDOW                      7   /*        7         1       255 Pointers   D */
#define ACP_WORKSET                     0   /*        0         0     65535 Pages      D */
#define ACP_WRITEBACK                   1   /*        1         0         1 Boolean    D */
#define ACP_XQP_RES                     1  /*         1         0         1 Boolean     */
#define ALLOCLASS                       1  /*         0         0       255 Pure-numbe  */
#define AWSMIN                         50   /*       50         0        -1 Pages      D */
#define AWSTIME                        20   /*       20         1        -1 10Ms       D */
#define BALSETCNT                      74  /*        16         4      8192 Slots       */
#define BJOBLIM                        16   /*       16         0      1024 Jobs       D */
#define BORROWLIM                      94   /*      300         0        -1 Pages      D */
#define BUGCHECKFATAL                   0   /*        0         0         1 Boolean    D */
#define BUGREBOOT                       1   /*        1         0         1 Boolean    D */
#define CHECK_CLUSTER                   0  /*         0         0         1 Boolean     */
#define CLASS_PROT                      0   /*        0         0         1 Boolean    D */
#define CLISYMTBL                     512   /*      512        20      1024 Pages      D */
#define CLUSTER_CREDITS                10  /*        10        10       128 Credits     */
#define CRD_CONTROL                     7  /*         7         0        15 Bit-mask    */
#define CWCREPRC_ENABLE                 1   /*        1         0         1 Bit-mask   D */
#define DEADLOCK_WAIT                  10   /*       10         0        -1 Seconds    D */
#define DEFMBXBUFQUO                 1056   /*     1056       256     64000 Bytes      D */
#define DEFMBXMXMSG                   256   /*      256        64     64000 Bytes      D */
#define DEFPRI                          4   /*        4         1        31 Priority   D */
#define DEFQUEPRI                     100   /*      100         0       255 Priority   D */
#define DISK_QUORUM     "                "  /*   "    "    "    "    "ZZZZ" Ascii        */
#define DISMOUMSG                       0   /*        0         0         1 Boolean    D */
#define DORMANTWAIT                     2   /*        2         0     65535 Seconds    D */
#define DUMPBUG                         1  /*         1         0         1 Boolean     */
#define DUMPSTYLE                       2   /*        2         0        -1 Bitmask    D */
#define ERLBUFFERPAGES                  2  /*         2         2        32 Pages       */
#define ERRORLOGBUFFERS                14  /*         4         2        64 Buffers     */
#define EXPECTED_VOTES                  1  /*         1         1       127 Votes       */
#define EXTRACPU                     1000   /*     1000         0        -1 10Ms       D */
#define FREEGOAL                     5120   /*      200        16        -1 Pages      D */
#define FREELIM                        94  /*        32        16        -1 Pages       */
#define GBLPAGES                   119506  /*     15000       512   4194176 Pages       */
#define GBLPAGFIL                    9096  /*      1024       128   4194176 Pages       */
#define GBLSECTIONS                   530  /*       250        60      4095 Sections    */
#define GROWLIM                        94   /*       63         0        -1 Pages      D */
#define IEEE_ADDRESS                    0   /*        0         0        -1 Pure-numbe D */
#define IEEE_ADDRESSH                   0   /*        0         0     65535 Pure-numbe D */
#define IJOBLIM                        64   /*       64         1      8192 Jobs       D */
#define INTSTKPAGES                    12  /*         6         4     65535 Pages       */
#define KFILSTCNT                      16  /*         4         2       255 Slots       */
#define LAMAPREGS                       0  /*         0         0       255 Mapregs     */
#define LGI_BRK_DISUSER                 0   /*        0         0         1 Boolean    D */
#define LGI_BRK_LIM                     5   /*        5         1       255 Failures   D */
#define LGI_BRK_TERM                    1   /*        1         0         1 Boolean    D */
#define LGI_BRK_TMO                   300   /*      300         0   5184000 Seconds    D */
#define LGI_CALLOUTS                    0   /*        0         0       255 Count      D */
#define LGI_HID_TIM                   300   /*      300         01261440000 Seconds    D */
#define LGI_PWD_TMO                    30   /*       30         0       255 Seconds    D */
#define LGI_RETRY_LIM                   3   /*        3         0       255 Tries      D */
#define LGI_RETRY_TMO                  20   /*       20         0       255 Seconds    D */
#define LNMPHASHTBL                   128  /*       128         1      8192 Entries     */
#define LNMSHASHTBL                   128  /*       128         1      8192 Entries     */
#define LOAD_PWD_POLICY                 0   /*        0         0         1 Boolean    D */
#define LOCKDIRWT                       1  /*         0         0       255 Pure-numbe  */
#define LOCKIDTBL                    2374  /*      3840      1792  16776959 Entries     */
#define LONGWAIT                       30   /*       30         0     65535 Seconds    D */
#define MAXBUF                       7696   /*     7696      1700     64000 Bytes      D */
#define MAXPROCESSCNT                 160  /*        32        12      8192 Processes   */
#define MAXQUEPRI                     100   /*      100         0       255 Priority   D */
#define MAXSYSGROUP                     8   /*        8         1     32768 UIC Group  D */
#define MINWSCNT                       20  /*        20        10        -1 Pages       */
#define MMG_CTLFLAGS                    3   /*        3         0       255 Bit-mask   D */
#define MOUNTMSG                        0   /*        0         0         1 Boolean    D */
#define MPW_HILIMIT                  2946  /*       500         0        -1 Pages       */
#define MPW_IOLIMIT                     4  /*         4         1       127 I/O         */
#define MPW_LOLIMIT                   120  /*        32         0        -1 Pages       */
#define MPW_LOWAITLIMIT              2826   /*      380         0        -1 Pages      D */
#define MPW_THRESH                    200   /*      200         0        -1 Pages      D */
#define MPW_WAITLIMIT                3066   /*      620         0        -1 Pages      D */
#define MPW_WRTCLUSTER                120  /*       120        16       120 Pages       */
#define MSCP_BUFFER                   128  /*       128        16        -1 Pages       */
#define MSCP_CMD_TMO                    0   /*        0         02147483647 Seconds    D */
#define MSCP_CREDITS                    8  /*         8         2       128 I/O         */
#define MSCP_LOAD                       1  /*         0         0     16384 Coded-valu  */
#define MSCP_SERVE_ALL                  1  /*         4         0        15 Bit-Encode  */
#define MULTIPROCESSING                 3  /*         3         0         4 Coded-valu  */
#define MULTITHREAD                     1  /*         1         0         1 Kthreads    */
#define MVTIMEOUT                    3600   /*     3600         1     64000 Seconds    D */
#define NET_CALLOUTS                    0   /*        0         0       255 Count      D */
#define NISCS_CONV_BOOT                 0  /*         0         0         1 Boolean     */
#define NISCS_LAN_OVRHD                18  /*        18         0       256 Bytes       */
#define NISCS_LOAD_PEA0                 1  /*         0         0         1 Boolean     */
#define NISCS_MAX_PKTSZ              1498  /*      1498      1080      8192 Bytes       */
#define NISCS_PORT_SERV                 0  /*         0         0         3 Bit-encode  */
#define NJOBLIM                        16   /*       16         0      1024 Jobs       D */
#define NPAGEDYN                  1876480  /*    860000     16384        -1 Bytes       */
#define NPAGEVIR                  9384960  /*   4300000     16384        -1 Bytes       */
#define PAGEDYN                   1357312  /*    214100     14336        -1 Bytes       */
#define PAGFILCNT                       4  /*         4         4        63 Files       */
#define PAMAXPORT                      32   /*       32         0       223 Port-numbe D */
#define PANOPOLL                        0   /*        0         0         1 Boolean    D */
#define PANUMPOLL                      16   /*       16         1       223 Ports      D */
#define PAPOLLINTERVAL                  5   /*        5         1     32767 Seconds    D */
#define PAPOOLINTERVAL                 15   /*       15         1     32767 Seconds    D */
#define PASANITY                        1   /*        1         0         1 Boolean    D */
#define PASTDGBUF                      16  /*         4         1        16 Buffers     */
#define PASTIMOUT                       5   /*        5         1        99 Seconds    D */
#define PFCDEFAULT                     64   /*       32         0       127 Pages      D */
#define PFRATH                        120   /*      120         0        -1 Flts/10Sec D */
#define PFRATL                          0   /*        0         0        -1 Flts/10Sec D */
#define PQL_DASTLM                     24   /*       24         4     32767 Ast        D */
#define PQL_DBIOLM                     18   /*       18         4     32767 I/O        D */
#define PQL_DBYTLM                   8192   /*     8192      1024        -1 Bytes      D */
#define PQL_DCPULM                      0   /*        0         0        -1 10Ms       D */
#define PQL_DDIOLM                     18   /*       18         4     32767 I/O        D */
#define PQL_DENQLM                    128   /*      128         4     32767 Locks      D */
#define PQL_DFILLM                     16   /*       16         2     32767 Files      D */
#define PQL_DJTQUOTA                 1024   /*     1024         0        -1 Bytes      D */
#define PQL_DPGFLQUOTA              32768   /*     8192       512        -1 Pages      D */
#define PQL_DPRCLM                      8   /*        8         0     32767 Processes  D */
#define PQL_DTQELM                      8   /*        8         0     32767 Timers     D */
#define PQL_DWSDEFAULT                241  /*       100        60        -1 Pages       */
#define PQL_DWSEXTENT               10200   /*      400        60        -1 Pages      D */
#define PQL_DWSQUOTA                  482   /*      200        60        -1 Pages      D */
#define PQL_MASTLM                    100   /*        4         4     32767 Ast        D */
#define PQL_MBIOLM                    100   /*        4         4     32767 I/O        D */
#define PQL_MBYTLM                  48000   /*     1024      1024        -1 Bytes      D */
#define PQL_MCPULM                      0   /*        0         0        -1 10Ms       D */
#define PQL_MDIOLM                    100   /*        4         4     32767 I/O        D */
#define PQL_MENQLM                    200   /*       30         4     32767 Locks      D */
#define PQL_MFILLM                    100   /*        2         2     32767 Files      D */
#define PQL_MJTQUOTA                    0   /*        0         0        -1 Bytes      D */
#define PQL_MPGFLQUOTA              10200   /*      512       512        -1 Pages      D */
#define PQL_MPRCLM                      8   /*        0         0     32767 Processes  D */
#define PQL_MTQELM                      0   /*        0         0     32767 Timers     D */
#define PQL_MWSDEFAULT                512  /*        60        60        -1 Pages       */
#define PQL_MWSEXTENT               10200   /*       60        60        -1 Pages      D */
#define PQL_MWSQUOTA                 1024   /*       60        60        -1 Pages      D */
#define PRCPOLINTERVAL                 30   /*       30         1     32767 Seconds    D */
#define PROCSECTCNT                    64  /*        64         5      1024 Sections    */
#define QDSKINTERVAL                   10  /*         3         1     32767 Seconds     */
#define QDSKVOTES                       1  /*         1         0       127 Votes       */
#define QUANTUM                        20   /*       20         2     32767 10Ms       D */
#define REALTIME_SPTS                   0  /*         0         0        -1 Pages       */
#define RECNXINTERVAL                  20   /*       20         1     32767 Seconds    D */
#define RESHASHTBL                   2048  /*        64         1  16777216 Entries     */
#define RJOBLIM                        16   /*       16         0     65535 Jobs       D */
#define RMS_DFLRL                       0   /*        0         0     32767 Bytes      D */
#define RMS_DFMBC                      16   /*       16         1       127 Blocks     D */
#define RMS_DFMBFHSH                    0   /*        0         0       127 Obsolete   D */
#define RMS_DFMBFIDX                    0   /*        0         0       127 Buffers    D */
#define RMS_DFMBFREL                    0   /*        0         0       127 Buffers    D */
#define RMS_DFMBFSDK                    0   /*        0         0       127 Buffers    D */
#define RMS_DFMBFSMT                    0   /*        0         0       127 Buffers    D */
#define RMS_DFMBFSUR                    0   /*        0         0       127 Buffers    D */
#define RMS_DFNBC                       8   /*        8         1       127 Blocks     D */
#define RMS_EXTEND_SIZE                 0   /*        0         0     65535 Blocks     D */
#define RMS_FILEPROT                64000  /*     64000         0     65535 Prot-mask   */
#define RMS_HEURISTIC                   0   /*        0         0         1 Boolean    D */
#define RMS_PROLOGUE                    0   /*        0         0         3 Prolog-Lvl D */
#define SAVEDUMP                        0  /*         0         0         1 Boolean     */
#define SCSBUFFCNT                    512  /*        50         0     32767 Entries     */
#define SCSCONNCNT                      7  /*        40         2     32767 Entries     */
#define SCSFLOWCUSH                     1   /*        1         0        16 Credits    D */
#define SCSMAXDG                      576  /*       576        28       985 Bytes       */
#define SCSMAXMSG                     216  /*       216        60       985 Bytes       */
#define SCSNODE                   "MARIN1"  /*   "    "    "    "    "ZZZZ" Ascii        */
#define SCSRESPCNT                    300  /*       300         0     32767 Entries     */
#define SCSSYSTEMID                 56881  /*         0         0        -1 Pure-numbe  */
#define SCSSYSTEMIDH                    0  /*         0         0        -1 Pure-numbe  */
#define SECURITY_POLICY                 7  /*         7         0        -1 Bit-mask    */
#define SETTIME                         0  /*         0         0         1 Boolean     */
#define SHADOWING                       0  /*         0         0         3 Coded-valu  */
#define SHADOW_MAX_COPY                 4   /*        4         0       200 Threads    D */
#define SHADOW_MAX_UNIT               100  /*       100        10     10000 S. sets     */
#define SHADOW_MBR_TMO                120   /*      120         1     65535 Seconds    D */
#define SHADOW_SYS_DISK                 0   /*        0         0        -1 Bit_Code   D */
#define SHADOW_SYS_TMO                120   /*      120         1     65535 Seconds    D */
#define SHADOW_SYS_UNIT                 0  /*         0         0      9999 Unit        */
#define SHADOW_SYS_WAIT               256   /*      256         1     65535 Seconds    D */
#define SMP_CPUS                       -1  /*        -1         0        -1 CPU bitmas  */
#define SMP_LNGSPINWAIT           3000000  /*   3000000         1   8388607 10 usec.    */
#define SMP_SANITY_CNT                300  /*       300         1        -1 10ms.       */
#define SMP_SPINWAIT               100000  /*    100000         1   8388607 10 usec.    */
#define SPTREQ                       8000  /*      3900      3000        -1 Pages       */
#define STARTUP_P1                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define STARTUP_P2                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define STARTUP_P3                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define STARTUP_P4                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define STARTUP_P5                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define STARTUP_P6                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define STARTUP_P7                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define STARTUP_P8                  "    "  /*   "    "    "    "    "zzzz" Ascii        */
#define SWPFILCNT                       2  /*         2         0        63 Files       */
#define SWPOUTPGCNT                   500   /*      288         0        -1 Pages      D */
#define SYSMWCNT                     3077  /*       508        48     32767 Pages       */
#define TAILORED                        0  /*         0         0         1 Boolean     */
#define TAPE_ALLOCLASS                  0  /*         0         0       255 Pure-numbe  */
#define TAPE_MVTIMEOUT                600   /*      600         1     64000 Seconds    D */
#define TIMEPROMPTWAIT              65535  /*     65535         0     65535 uFortnight  */
#define TIMVCFAIL                    1600   /*     1600       100     65535 10Ms       D */
#define TMSCP_LOAD                      1  /*         0         0         3 Coded-valu  */
#define TMSCP_SERVE_ALL                 2  /*         0         0        15 Bit-Encode  */
#define TTY_ALTALARM                   64  /*        64         0     65535 Bytes       */
#define TTY_ALTYPAHD                  200  /*       200         0     32767 Bytes       */
#define TTY_AUTOCHAR                    7   /*        7         0       255 Character  D */
#define TTY_BUF                        80  /*        80         0       511 Characters  */
#define TTY_CLASSNAME                 "TT"  /*     "TT"      "AA"      "ZZ" Ascii        */
#define TTY_DEFCHAR             402657952  /* 402657952         0        -1 Bit-Encode  */
#define TTY_DEFCHAR2                 4098  /*      4098         0        -1 Bit-Encode  */
#define TTY_DIALTYPE                    0  /*         0         0       255 Bit-Encode  */
#define TTY_DMASIZE                    64   /*       64         0     65535 Bytes      D */
#if 0
// not yet
#define TTY_PARITY                     24  /*        24         0       255 Special     */
#endif
#define TTY_RSPEED                      0  /*         0         0        17 Special     */
#define TTY_SCANDELTA            10000000  /*  10000000    100000        -1 100Ns       */
#define TTY_SILOTIME                    8  /*         8         0       255 Ms          */
#define TTY_SPEED                      15  /*        15         1        17 Special     */
#define TTY_TIMEOUT                   900   /*      900         0        -1 Seconds    D */
#define TTY_TYPAHDSZ                   78  /*        78         0     32767 Bytes       */
#define UAFALTERNATE                    0  /*         0         0         1 Boolean     */
#define UDABURSTRATE                    0  /*         0         0        31 Longwords   */
#define USER3                           0  /*         0         0        -1             */
#define USER4                           0  /*         0         0        -1             */
#define USERD1                          0   /*        0         0        -1            D */
#define USERD2                          0   /*        0         0        -1            D */
#define VAXCLUSTER                      2  /*         1         0         2 Coded-valu  */
#define VECTOR_MARGIN                 100   /*      100         1        -1 Pure-numbe D */
#define VECTOR_PROC                     1  /*         1         0         3 Coded-valu  */
#define VIRTUALPAGECNT              50000  /*     12032       512   4194304 Pages       */
#define VOTES                           1  /*         1         0       127 Votes       */
#define WBM_MSG_INT                    10   /*       10        10        -1 msec       D */
#define WBM_MSG_LOWER                   5   /*        5         0        -1 Messages   D */
#define WBM_MSG_UPPER                  10   /*       10         0        -1 Messages   D */
#define WBM_OPCOM_LVL                   1   /*        1         0         2 mode       D */
#define WINDOW_SYSTEM                   1   /*        0         0         2 Pure-numbe D */
#define WSDEC                         250   /*      250         0        -1 Pages      D */
#define WSINC                         150   /*      150         0        -1 Pages      D */
#define WSMAX                       10200  /*      1024        60   1048576 Pages       */
#define WS_OPA0                         0   /*        0         0         1 Boolean    D */
#define XFMAXRATE                     236   /*      236         0       255 Special    D */

#define CHANNELCNT 1024

#endif /* __SYSGEN_LOADED */
