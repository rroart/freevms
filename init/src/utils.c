/*
 *      This is part of ODS2 initialize utility written by Zakaria Yassine,
 *      email address:  zakaria@sympatico.ca
 *
 *      This work is based on ODS2 specifications (see file ods2.doc). As
 *      part of the FreeVMS project, (see http://freevms.free.fr/indexGB.html),
 *      this work is under GNU GPL license.
 *
 * Modifications:
 *
 *   30-SEP-2003        Zakaria Yassine <zakaria@sympatico.ca>
 *      Initiale version. Device size hardcoded, geometry not set, and many
 *      other things :-)        Usage: init /dev/hdb4
 *
 *   20-OCT-2003	Zakaria Yassine <zakaria@sympatico.ca>
 *	some bugs corrected. support write. add params: device-size (in blocks), volume-label
 *		Usage: init /dev/hdb4 VOLNAME 40000
 *
 * Report Bugs:
 *   To: FreeVMS project mailing list <http://freevms.nvg.org/>
 *       or directely to me.
 */

#include <stdio.h>
#include "initialize.h"

int curVBN=0;

void read_blk(unsigned char *blk, FILE *fin)
{
    int i;
    for(i=0; i<512; i++) blk[i]=fgetc(fin);
}

unsigned int roundup(double y)   //instead of ceil?
{
    unsigned int x;
    x=y;
    if(x<y) x++;
    return(x);
}

void write_blk(unsigned char *blk, FILE *fout, char * strmsg)
{
    int i;
    curVBN++;
#ifdef DEBUG
    if (strlen(strmsg)!=0) printf("Writing %s at %d\n", strmsg, curVBN);
#endif
    for(i=0; i<512; i++) fputc(blk[i],fout);
}

int getClusterSizehomeblock(unsigned char *in_HM2)
{
    struct _hm2 *pHM2;
    pHM2 = (struct _hm2 *) in_HM2;
    return pHM2->hm2$w_cluster;
}

int getIBMAPSIZE(unsigned char *in_HM2)
{
    struct _hm2 *pHM2;
    pHM2 = (struct _hm2 *) in_HM2;
    return pHM2->hm2$w_ibmapsize;
}
