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
#include "dirdef.h"
#include "scbdef.h"
#include "fatdef.h"
#include "utils2.h"

int clustersize;
int volumesize  = 90000; //71076708; //volume size in blocks
int maxfiles;
int indexfilesize;
int storagebitmapsize;
int bitmapfilesize;
int diffsizeindexf;
int diffsizebitmap;
char strmsg[30];
unsigned char *volnam = "TESTVOLUME";
unsigned char *owner  = "SYSTEM";

void do_checksum(unsigned char *line){
        unsigned short int checksum=0;
	unsigned data=0;
        unsigned short int *p;
	int i;
        p=(unsigned short int *) line;
	for(i=0;i<255;i++) {
		data=*p++;
                checksum=checksum+VMSWORD(data);
        }
        //line[511]=checksum/256;
        //line[510]=checksum-line[511]*256;
	*p = VMSWORD(checksum);
	//printf("%d %x %x\n", checksum, line[511], line[510]);
}

void create_ods2(FILE *fout)
{
unsigned char out_line[512];
int i;
struct HM2$ *pHM2$;
struct FH2$ *pFH2;
struct FI2$ *pFI2;
struct FM2$C_FORMAT2 *pFM2;

//cluster size...
    if (volumesize<=50000)
        clustersize=1;
    else {
        clustersize = roundup( ((double) volumesize)/(255 * 4096));
        if (clustersize<3) clustersize=3;
        }
    clustersize=17;
//maxfiles
    maxfiles=volumesize/((clustersize+1)*2);
    if (maxfiles>(volumesize/(clustersize+1))) maxfiles=volumesize/(clustersize+1);
//indefilesize
    indexfilesize=((4*clustersize)+roundup(((double) maxfiles)/4096)+16);
    diffsizeindexf=indexfilesize;
    indexfilesize=clustersize*roundup((double) indexfilesize/clustersize);
    diffsizeindexf=indexfilesize-diffsizeindexf;
//
        printf("Initialize device\n");
        printf("Volume Size %d Cluster %d Maxfiles %d\n", volumesize, clustersize, maxfiles);
	printf("Indexfile size %d\n", indexfilesize); 

//Cluster 1, 2 and 3

    //Boot Block: don't know what to do, just put all to 0
    for(i=0;i<512;i++) out_line[i]=0;
    write_blk(out_line, fout, "Boot block");

    //Home Block: construct
    pHM2$ = (struct HM2$ *) out_line;
    pHM2$->HM2$L_HOMELBN=1;  pHM2$->HM2$W_HOMEVBN=2;
    pHM2$->HM2$L_ALHOMELBN=clustersize*2;
    pHM2$->HM2$W_ALHOMEVBN=clustersize*2+1;
    pHM2$->HM2$L_ALTIDXLBN=clustersize*3;
    pHM2$->HM2$W_ALTIDXVBN=clustersize*3+1;
    pHM2$->HM2$W_STRUCLEV[1]=2; pHM2$->HM2$W_STRUCLEV[0]=1;
    pHM2$->HM2$W_CLUSTER=clustersize;
    pHM2$->HM2$W_IBMAPVBN=VMSWORD(clustersize*4+1); 
    pHM2$->HM2$L_IBMAPLBN=VMSLONG(clustersize*4);
    pHM2$->HM2$L_MAXFILES=(maxfiles);
    pHM2$->HM2$W_IBMAPSIZE=VMSWORD(roundup( ( (double) pHM2$->HM2$L_MAXFILES)/4096 ));
    /*pHM2$->HM2$W_RESFILES[2]=5;
    pHM2$->HM2$B_LRU_LIM= 30;
    pHM2$->HM2$B_WINDOW[1]=20;*/
    pHM2$->HM2$W_EXTEND[2]=clustersize;
    strcpy(pHM2$->HM2$T_STRUCNAME,"            ");
    strcpy(pHM2$->HM2$T_VOLNAME , volnam);
    strcpy(pHM2$->HM2$T_OWNERNAME, owner);
    strcpy(pHM2$->HM2$T_FORMAT,   "DECFILE11B  ");
    do_checksum(out_line);
    write_blk(out_line, fout, "first home Block\0"); //write first home block

    for(i=0;i<3*clustersize-2;i++){ //fill the rest of the cluster and the next two clusters
        pHM2$->HM2$L_HOMELBN=2+i;pHM2$->HM2$W_HOMEVBN=3+i;
        write_blk(out_line, fout, "\0");
    }

// Cluster 4
    //Backup Index File Header
    for(i=0;i<512;i++) out_line[i]=0; //clear all

	//index file header:
		// header area
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=1;pFH2->FH2$W_FID.FID$W_FID_SEQ=1;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"INDEXF.SYS;1");
        // map area
    pFM2 = (struct FM2$C_FORMAT2 *) (out_line+(2*pFH2->FH2$B_MPOFFSET));
    pFM2->FM2$V_COUNT2=indexfilesize-1+32768; //Add 32768 for format type
    pFM2->FM2$L_LBN2=0;
    pFH2->FH2$B_MAP_INUSE=2;
	//file attribs
    struct FAT$ *pFAT;
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=512;
    pFAT->FAT$L_HIBLK=VMSSWAP(indexfilesize-diffsizeindexf);
    pFAT->FAT$L_EFBLK=VMSSWAP(indexfilesize-diffsizeindexf+1); //not sure
    pFAT->FAT$W_MAXREC=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "Backup Indexfile header"); //write down index file header

    // (the rest of the cluster is not used)
    for(i=0;i<512;i++) out_line[i]=0; //clear all
    for(i=0;i<clustersize-1;i++) write_blk(out_line, fout, "\0");
    
// index file bitmap
    for(i=0;i<512;i++) out_line[i]=0; //clear all
    int j = roundup(((double) maxfiles)/4096); //nbr of blocks in IFB
    out_line[0]=(unsigned char) 255;
    out_line[1]=(unsigned char) 1;   //files with number 1 to 9 reserved
    write_blk(out_line, fout, "Indexfile bitmap");    
    for(i=0;i<512;i++) out_line[i]=0;    //clear all
    for(i=0;i<j-1;i++) write_blk(out_line, fout, "\0");   //rest of bitmap index
    
//index file header: Yeah once again !!!
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=VMSWORD(1);pFH2->FH2$W_FID.FID$W_FID_SEQ=VMSWORD(1);pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"INDEXF.SYS;1");
        // map area
    pFM2 = (struct FM2$C_FORMAT2 *) (out_line+(2*pFH2->FH2$B_MPOFFSET));
    pFM2->FM2$V_COUNT2=indexfilesize-diffsizeindexf-1+16384; //VMSWORD((indexfilesize-1) + 32768); //Add 32768 for format type
    pFH2->FH2$B_MAP_INUSE=2;
	//file attribs
    //struct FAT$ *pFAT;
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=VMSWORD(512);
    pFAT->FAT$L_HIBLK=VMSSWAP(indexfilesize-diffsizeindexf);
    pFAT->FAT$L_EFBLK=VMSSWAP(indexfilesize-diffsizeindexf+1); //not sure
    pFH2->FH2$L_HIGHWATER=VMSSWAP(indexfilesize-diffsizeindexf+1);
    pFAT->FAT$W_MAXREC=VMSWORD(512);
    do_checksum(out_line);
    write_blk(out_line, fout, "Index file header"); //write down index file header

// header area 2.2 BITMAP.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    //pFH2->FH2$L_FILECHAR[0]=128;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=2;pFH2->FH2$W_FID.FID$W_FID_SEQ=2;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"BITMAP.SYS;1");
        // map area
    pFM2 = (struct FM2$C_FORMAT2 *) (out_line+(2*pFH2->FH2$B_MPOFFSET));
    storagebitmapsize=roundup( ( (double) (volumesize/clustersize))/4096);
    bitmapfilesize=clustersize*(roundup(((double) (storagebitmapsize+1))/clustersize));
    diffsizebitmap=bitmapfilesize-1-storagebitmapsize;
    pFM2->FM2$V_COUNT2=bitmapfilesize-1+32768;
    pFM2->FM2$L_LBN2=(indexfilesize+clustersize);
    pFH2->FH2$B_MAP_INUSE=2;
	//file attribs
    //struct FAT$ *pFAT;
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=512;
    pFAT->FAT$L_HIBLK=VMSSWAP(bitmapfilesize);
    pFAT->FAT$L_EFBLK=VMSSWAP(indexfilesize+clustersize+bitmapfilesize-diffsizebitmap+1);
    pFH2->FH2$L_HIGHWATER=VMSSWAP(indexfilesize+clustersize+bitmapfilesize-diffsizebitmap+1);
    pFAT->FAT$W_MAXREC=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "bitmap.sys header"); //write down bitmap file header

// header area 3.3 BADBLK.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=3;pFH2->FH2$W_FID.FID$W_FID_SEQ=3;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"BADBLK.SYS;1");
        // no map area
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=512;
    pFAT->FAT$L_HIBLK=VMSSWAP(0);
    pFAT->FAT$L_EFBLK=VMSSWAP(1);
    pFH2->FH2$L_HIGHWATER=VMSSWAP(1);
    pFAT->FAT$W_MAXREC=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "badblk.sys header"); //write down file header

// header area 4.4 000000.DIR;1 
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
	pFH2->FH2$L_FILECHAR[0]=128;pFH2->FH2$L_FILECHAR[1]=32; // directory flags
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=4;pFH2->FH2$W_FID.FID$W_FID_SEQ=4;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"000000.DIR;1");
        // map area
    pFM2 = (struct FM2$C_FORMAT2 *) (out_line+(2*pFH2->FH2$B_MPOFFSET));
    //pFM2->FM2$V_COUNT2=16384; 
    pFM2->FM2$V_COUNT2=(indexfilesize+2)+32768;
    pFM2->FM2$L_LBN2=0; //indexfilesize;
    pFH2->FH2$B_MAP_INUSE=2; //3;
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=2;
    pFAT->FAT$B_RATTRIB=8;
    pFAT->FAT$W_RSIZE=512;
    pFAT->FAT$L_HIBLK=VMSSWAP(clustersize+indexfilesize+1);
    pFAT->FAT$L_EFBLK=VMSSWAP(indexfilesize+1);
    pFH2->FH2$L_HIGHWATER=VMSSWAP(indexfilesize+2);
    pFAT->FAT$W_MAXREC=512;
    pFAT->FAT$W_FFBYTE=216;
    do_checksum(out_line);
    write_blk(out_line, fout, "000000.dir header"); //write down file header

// header area 5.5 CORIMG.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=5;pFH2->FH2$W_FID.FID$W_FID_SEQ=5;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"CORIMG.SYS;1");
        // no map area
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=512;
    pFAT->FAT$L_HIBLK=VMSSWAP(0);
    pFAT->FAT$L_EFBLK=VMSSWAP(1);
    pFAT->FAT$W_MAXREC=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "corimg.sys header"); //write down file header

// header area 6.6 VOLSET.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=6;pFH2->FH2$W_FID.FID$W_FID_SEQ=6;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"VOLSET.SYS;1");
        // no map area
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=64;
    pFAT->FAT$L_HIBLK=VMSSWAP(0);
    pFAT->FAT$L_EFBLK=VMSSWAP(1);
    pFAT->FAT$W_MAXREC=64;
    do_checksum(out_line);
    write_blk(out_line, fout, "volset.sys header"); //write down file header

// header area 7.7 CONTIN.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=7;pFH2->FH2$W_FID.FID$W_FID_SEQ=7;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"CONTIN.SYS;1");
        // no map area
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=512;
    pFAT->FAT$L_HIBLK=VMSSWAP(0);
    pFAT->FAT$L_EFBLK=VMSSWAP(1);
    pFAT->FAT$W_MAXREC=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "contin.sys header"); //write down file header

// header area 8.8 BACKUP.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=8;pFH2->FH2$W_FID.FID$W_FID_SEQ=8;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"BACKUP.SYS;1");
        // no map area
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=64;
    pFAT->FAT$L_HIBLK=VMSSWAP(0);
    pFAT->FAT$L_EFBLK=VMSSWAP(1);
    pFAT->FAT$W_MAXREC=64;
    do_checksum(out_line);
    write_blk(out_line, fout, "backup.sys header"); //write down file header

// header area 9.9 BADLOG.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct FH2$ *) out_line;
	pFH2->FH2$B_IDOFFSET=40;
	pFH2->FH2$B_MPOFFSET=100;
	pFH2->FH2$B_ACOFFSET=255;
	pFH2->FH2$B_RSOFFSET=255;
	pFH2->FH2$W_SEG_NUM=0;
    pFH2->FH2$W_STRUCLEV[1]=2; pFH2->FH2$W_STRUCLEV[0]=1;
    pFH2->FH2$W_FID.FID$W_FID_NUM=9;pFH2->FH2$W_FID.FID$W_FID_SEQ=9;pFH2->FH2$W_FID.FID$B_FID_RVN=0;
    pFH2->FH2$W_BACKLINK.FID$W_FID_NUM=4;pFH2->FH2$W_BACKLINK.FID$W_FID_SEQ=4;pFH2->FH2$W_BACKLINK.FID$B_FID_RVN=0;
        // identification area
    pFI2 = (struct FI2$ *) (out_line+2*pFH2->FH2$B_IDOFFSET);
    strcpy(pFI2->FI2$T_FILENAME,"BADLOG.SYS;1");
        // no map area
    pFAT = (struct FAT$ *) pFH2->FH2$W_RECATTR;
    pFAT->FAT$B_RTYPE=1;
    pFAT->FAT$W_RSIZE=16;
    pFAT->FAT$L_HIBLK=VMSSWAP(0);
    pFAT->FAT$L_EFBLK=VMSSWAP(1);
    pFAT->FAT$W_MAXREC=16;
    do_checksum(out_line);
    write_blk(out_line, fout, "badlog.sys header"); //write down file header

    for(i=0;i<512;i++) out_line[i]=0; //clear all

    for(i=0;i<7;i++) write_blk(out_line, fout, ""); // complete 16 file headers 
    for(i=0;i<diffsizeindexf;i++) write_blk(out_line, fout, ""); // write until next cluster !
    
// All Above IS actually the INDEXF.SYS. Now:
// create 000000.DIR, files are in alphabetical order
    int pos=0;
    struct DIR$ *pDIR;
    for(i=0;i<512;i++) out_line[i]=0; //clear all

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"000000.DIR"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=4; pDIR->DIR$W_FID.FID$W_FID_SEQ=4;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"BACKUP.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=8; pDIR->DIR$W_FID.FID$W_FID_SEQ=8;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"BADBLK.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=3; pDIR->DIR$W_FID.FID$W_FID_SEQ=3;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"BADLOG.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=9; pDIR->DIR$W_FID.FID$W_FID_SEQ=9;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"BITMAP.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=2; pDIR->DIR$W_FID.FID$W_FID_SEQ=2;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"CONTIN.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=7; pDIR->DIR$W_FID.FID$W_FID_SEQ=7;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"CORIMG.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=5; pDIR->DIR$W_FID.FID$W_FID_SEQ=5;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"INDEXF.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=1; pDIR->DIR$W_FID.FID$W_FID_SEQ=1;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    pDIR = (struct DIR$ *) (out_line+pos);
    pDIR->DIR$W_SIZE=22; pos=pos+24;
    strcpy(pDIR->DIR$T_NAME,"VOLSET.SYS"); pDIR->DIR$B_NAMECOUNT=10;
    pDIR->DIR$W_FID.FID$W_FID_NUM=6; pDIR->DIR$W_FID.FID$W_FID_SEQ=6;
    pDIR->DIR$W_VERLIMIT=1; pDIR->DIR$B_FLAGS=0; pDIR->DIR$W_VERSION=1;
    pDIR->DIR$W_FID.FID$B_FID_RVN=0; pDIR->DIR$W_FID.FID$B_FID_NMX=0;

    out_line[pos]=255; out_line[pos+1]=255; // (-1) word 
    
    write_blk(out_line, fout, "directory file"); //write directory file - only one record, but one cluster is used
    for(i=0;i<512;i++) out_line[i]=0; //clear all
    for(i=0;i<clustersize-1;i++) write_blk(out_line, fout, ""); 
    
// create BITMAP.SYS
    //write storage control block
    struct SCB$ *pSCB;
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pSCB = (struct SCB$ *) out_line;
	pSCB->SCB$W_STRUCLEV[1]=2; pSCB->SCB$W_STRUCLEV[0]=1;
	pSCB->SCB$W_CLUSTER=clustersize;
	pSCB->SCB$L_VOLSIZE=volumesize;
	pSCB->SCB$L_BLKSIZE=16065;	
	pSCB->SCB$L_SECTORS=63;	
	pSCB->SCB$L_TRACKS=255;
	pSCB->SCB$L_CYLINDER=4998;
	/*SCB$L_STATUS;	SCB$L_STATUS2;
	SCB$W_WRITECNT;	SCB$T_VOLOCKNAME[12];	SCB$Q_MOUNTTIME[4];
	SCB$W_CHECKSUM[2]; */
    write_blk(out_line, fout, "bitmap.sys SCB"); //write storage control block, some missing information...
    
    //storage bitmap 
    
    // nbr of clusters on volume = volumesize/clustersize, at worse we loose clustersize-1 blocks
    int nbrcluster=volumesize/clustersize; 
    
    // nbr of blocks in storage bitmap = roundup( ( (double) (volumesize/clustersize))/4096);
    //storagebitmapsize=roundup( ( (double) (volumesize/clustersize))/4096);
    for(i=0;i<512;i++) out_line[i]=255; //make all 512 first clusters free
    
    //(indexfilesize/clustersize+bitmapfilesize/clustersize+clustersize) clusters will be used. 
    // indexf.sys + bitmap.sys + 000000.dir
    //It cannot be more that 512, and don't ask me why...
    int usedclusters=(indexfilesize/clustersize)+(bitmapfilesize/clustersize)+clustersize;
    j=0;
    while(j<usedclusters){
        out_line[j/8] = out_line[j/8] << 1;
        j++;
    }
    write_blk(out_line, fout, "storage bitmap");

    for(i=0;i<512;i++) out_line[i]=255; //make rest of clusters free, except last one
    for(i=0;i<storagebitmapsize-2;i++) write_blk(out_line, fout, "");

    j=(storagebitmapsize*4096)-nbrcluster;  //last block, some cluster bits are unusable
    i=0;
    while(i<j){
        out_line[511-(i/8)] = out_line[511-(i/8)] >> 1;
        i++;
    }
    write_blk(out_line, fout, "");
    
    for(i=0;i<diffsizebitmap;i++) write_blk(out_line, fout, ""); // write until next cluster !
    
    for(i=0;i<512;i++) out_line[i]=0;
    printf("Clearing %d blocks\n",volumesize-(usedclusters*clustersize));  
    for(i=0;i<volumesize-(usedclusters*clustersize);i++) write_blk(out_line, fout, "");
}


