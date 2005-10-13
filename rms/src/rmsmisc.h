// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thronæs.

/*      For file context info we use WCCDIR and WCCFILE structures...
        Each WCCFILE structure contains one WCCDIR structure for file
        context. Each level of directory has an additional WCCDIR record.
        For example DKA200:[PNANKERVIS.F11]RMS.C is loosley stored as:-
                        next         next
                WCCFILE  -->  WCCDIR  -->  WCCDIR
                RMS.C;       F11.DIR;1    PNANKERVIS.DIR;1

        WCCFILE is pointed to by fab->fab$l_nam->nam$l_wcc and if a
        file is open also by ifi_table[fab->fab$w_ifi]  (so that close
        can easily locate it).

        Most importantly WCCFILE contains a resulting filename field
        which stores the resulting file spec. Each WCCDIR has a prelen
        length to indicate how many characters in the specification
        exist before the bit contributed by this WCCDIR. (ie to store
        the device name and any previous directory name entries.)  */


#define STATUS_INIT 1
#define STATUS_TMPDIR  2
#define STATUS_WILDCARD 4

struct WCCDIR {
    struct WCCDIR *wcd_next;
    struct WCCDIR *wcd_prev;
    int wcd_status;
    int wcd_wcc;
    int wcd_prelen;
    unsigned short wcd_reslen;
    struct dsc$descriptor wcd_serdsc;
    struct _fiddef wcd_dirid;
    char wcd_sernam[1];         /* Must be last in structure */
};                              /* Directory context */


#define STATUS_RECURSE 8
#define STATUS_TMPWCC  16

#define MAX_FILELEN 1024

struct WCCFILE {
    struct _fabdef *wcf_fab;
    int wcf_status;
    struct _fibdef wcf_fib;
    char wcf_result[MAX_FILELEN];
    unsigned long xab;
    struct WCCDIR wcf_wcd;      /* Must be last..... (dynamic length). */
};                              /* File context */

#define IFI_MAX 64
extern struct WCCFILE *ifi_table[];
extern struct _ifbdef *ifb_table[];

// RMS reserves event flags 27-30, and maybe 31
// have seen something about flag 29 being used
// pick 30 for xqp; set in access.c

#define RMS_EF 29
