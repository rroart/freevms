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
 * Report Bugs:
 *   To: FreeVMS project mailing list <http://freevms.nvg.org/>
 *       or directely to me.
 */

#include <stdio.h>

#include "fiddef.h"
#include "fi2def.h"
#include "fh2def.h"
#include "fm2def.h"
#include "hm2def.h"
#include "fatdef.h"

int curVBN=0;

void read_blk(unsigned char *blk, FILE *fin){
    int i;
    for(i=0;i<512;i++) blk[i]=fgetc(fin);
}

unsigned int roundup(double y){  //instead of ceil?
    unsigned int x;
    x=y;
    if(x<y) x++;
    return(x);
}

void write_blk(unsigned char *blk, FILE *fout, char * strmsg){
    int i;
    curVBN++;
#ifdef DEBUG
    if (strlen(strmsg)!=0) printf("Writing %s at %d\n", strmsg, curVBN);
#endif
    for(i=0;i<512;i++) fputc(blk[i],fout);
}

int getClusterSizehomeblock(unsigned char *in_HM2){
    struct HM2$ *pHM2;
    pHM2 = (struct HM2$ *) in_HM2;
    return pHM2->HM2$W_CLUSTER;
}

int getIBMAPSIZE(unsigned char *in_HM2){
    struct HM2$ *pHM2;
    pHM2 = (struct HM2$ *) in_HM2;
    return pHM2->HM2$W_IBMAPSIZE;
}

void dump_homeblock(unsigned char *in_HM2){
    struct HM2$ *pHM2;
    pHM2 = (struct HM2$ *) in_HM2;
    //printf("Cluster Factor %i\n", pHM2->HM2$W_CLUSTER);
    printf("Structure level: %x, %x\n", pHM2->HM2$W_STRUCLEV[1], pHM2->HM2$W_STRUCLEV[0]);
    int i = 0;
	// printf("--");
  	//for (i=496;i<506;i++) { printf("%c", in_HM2[i]);}
	// printf("\n");
}

void dump_fileid(struct FID$ in_FID){

/*    printf("File identification:           %i\n", pFID->FID$B_FID_RVN);
    printf("File identification:           %i\n", pFID->FID$W_FID_SEQ);
    printf("File identification:           %i\n", pFID->FID$W_FID_NUM);
    printf("File identification:           %i\n", (pFID->FID$B_FID_NMX * 65536)+pFID->FID$W_FID_NUM);
*/
    printf("File identification:           (%i,%i,%i)\n", (in_FID.FID$B_FID_NMX * 65536)+in_FID.FID$W_FID_NUM,in_FID.FID$W_FID_SEQ,in_FID.FID$B_FID_RVN);
}

void dump_identarea(unsigned char *in_identarea)
{
    struct FI2$ *pFI2;
    pFI2 = (struct FI2$ *) in_identarea;
    printf("File name:       %10s=\n", pFI2->FI2$T_FILENAME);
}

void dump_fileheader(unsigned char *in_block){
    struct FH2$ *pFH2;
	pFH2 = (struct FH2$ *) in_block;

       /*	printf("Identification area offset:    %i\n", pFH2->FH2$B_IDOFFSET);
	printf("Map area offset:               %i\n", pFH2->FH2$B_MPOFFSET);
	printf("Access control area offset:    %i\n", pFH2->FH2$B_ACOFFSET);
	printf("Reserved area offset:          %i\n", pFH2->FH2$B_RSOFFSET);
    printf("Extension segment number:      %i\n", pFH2->FH2$W_SEG_NUM);
    printf("Structure level and version:   %i, %i\n", pFH2->FH2$W_STRUCLEV[1], pFH2->FH2$W_STRUCLEV[0]);
    printf("file attr %i %i %i %i\n",pFH2->FH2$L_FILECHAR[0],pFH2->FH2$L_FILECHAR[1],pFH2->FH2$L_FILECHAR[2],pFH2->FH2$L_FILECHAR[3]);
 */
    struct FAT$ *pFAT;
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
	unsigned short int * mp = (unsigned short int *) in_block+(pFH2->FH2$B_MPOFFSET);
	unsigned short int phylen, phyblk;
                switch ((*mp) >> 14) {
                    case 0:
                        phylen = 0;
                        mp++;
                        break;
                    case 1:
                        phylen = ((*mp) & 0377) + 1;
                        phyblk = (((*mp) & 037400) << 8) | (mp[1]);
                        mp += 2;puts("mp1");
                        break;
                    case 2:
                        phylen = ((*mp) & 037777) + 1;
                        phyblk = ((mp[2]) << 16) | (mp[1]);
                        mp += 3;puts("mp2");
                        break;
                    case 3:
                        phylen = (((*mp) & 037777) << 16) + (mp[1]) + 1;
                        phyblk = ((mp[3]) << 16) | (mp[2]);
                        mp += 4;puts("mp3");
                } 
	printf("phylen %d phyblk %d\n", phylen, phyblk);
	/*
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+0]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+1]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+2]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+3]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+4]);
    printf("%02X\n ",in_block[(2*pFH2->FH2$B_MPOFFSET)+5]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+6]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+7]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+8]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+9]);
    printf("%02X ", in_block[(2*pFH2->FH2$B_MPOFFSET)+10]);
    printf("%02X\n ",in_block[(2*pFH2->FH2$B_MPOFFSET)+11]);
    /*
    printf("--%d",pFAT->FAT$B_RTYPE);
    printf("--%d",pFAT->FAT$B_RATTRIB);
    printf("--%d",pFAT->FAT$W_RSIZE);
    printf("--%d",pFAT->FAT$L_HIBLK1);
    printf("--%d",pFAT->FAT$L_HIBLK2);
    printf("--%d",pFAT->FAT$L_EFBLK1);
    printf("--%d",pFAT->FAT$L_EFBLK2);
    printf("--%d",pFAT->FAT$W_FFBYTE);
    printf("--%d",pFAT->FAT$B_BKTSIZE);
    printf("--%d",pFAT->FAT$B_VFCSIZE);
    printf("--%d",pFAT->FAT$W_MAXREC);
    printf("--%d",pFAT->FAT$W_DEFEXT);
    printf("--%d",pFAT->FAT$W_GBC);
    printf("--%d--",pFAT->FAT$W_VERSIONS);

    printf("\n");  */
    dump_fileid(pFH2->FH2$W_FID);
    dump_fileid(pFH2->FH2$W_EXT_FID);
    dump_fileid(pFH2->FH2$W_BACKLINK);
    dump_identarea(in_block+2*pFH2->FH2$B_IDOFFSET); 
    return;
}

