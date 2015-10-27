/*
 * wccdef.h
 *
 *  Created on: Oct 18, 2015
 *      Author: guido
 */

#ifndef WCCDEF_H
#define WCCDEF_H

#include <descrip.h>
#include <fabdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <ifbdef.h>

/*
 * For file context info we use WCCDIR and WCCFILE structures...
 * Each WCCFILE structure contains one WCCDIR structure for file
 * context. Each level of directory has an additional WCCDIR record.
 * For example DKA200:[PNANKERVIS.F11]RMS.C is loosely stored as:-
 *   next          next
 *   WCCFILE  -->  WCCDIR  -->  WCCDIR
 *   RMS.C;        F11.DIR;1    PNANKERVIS.DIR;1
 *
 * WCCFILE is pointed to by fab->fab$l_nam->nam$l_wcc and if a
 * file is open also by ifi_table[fab->fab$w_ifi]  (so that close
 * can easily locate it).
 *
 * Most importantly WCCFILE contains a resulting filename field
 * which stores the resulting file spec. Each WCCDIR has a prelen
 * length to indicate how many characters in the specification
 * exist before the bit contributed by this WCCDIR. (ie to store
 * the device name and any previous directory name entries.)
 */

#define WCC_STATUS_INIT 1
#define WCC_STATUS_TMPDIR  2
#define WCC_STATUS_WILDCARD 4
#define WCC_STATUS_RECURSE 8
#define WCC_STATUS_TMPWCC  16

#define WCC_MAX_FILELEN 1024

/* Directory context */
struct _wccdir
{
    struct _wccdir *wcd_next;
    struct _wccdir *wcd_prev;
    UINT32 wcd_size;
    UINT32 wcd_status;
    UINT32 wcd_wcc;
    UINT32 wcd_prelen;
    UINT16 wcd_reslen;
    struct dsc$descriptor wcd_serdsc;
    struct _fiddef wcd_dirid;
    char wcd_sernam[1]; /* Must be last in structure */
};

/* File context */
struct _wccfile
{
    struct _fabdef *wcf_fab;
    UINT32 wcf_status;
    struct _fibdef wcf_fib;
    char wcf_result[WCC_MAX_FILELEN];
    void *xab;
    struct _wccdir wcf_wcd; /* Must be last..... (dynamic length). */
};

#endif /* WCCDEF_H */
