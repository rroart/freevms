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
 *   20-OCT-2003        Zakaria Yassine <zakaria@sympatico.ca>
 *      some bugs corrected. support write. add params: device-size (in blocks), volume-label
 *              Usage: init /dev/hdb4 VOLNAME 40000
 *
 * Report Bugs:
 *   To: FreeVMS project mailing list <http://freevms.nvg.org/>
 *       or directely to me.
 */

#include <stdio.h>
#include "initialize.h"

int clustersize;
int volumesize  = 90000; //71076708; //volume size in blocks
int maxfiles;
int indexfilesize;
int storagebitmapsize;
int bitmapfilesize;
int diffsizeindexf;
int diffsizebitmap;
char strmsg[30];
unsigned char volnam[20];
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
	*p = VMSWORD(checksum);
}

void create_ods2(FILE *fout, char *volname, int volsize)
{
unsigned char out_line[512];
int i;
struct _hm2 *pHM2$;
struct _fh2 *pFH2;
struct FM2_C_FORMAT2 *pFM2;
struct _fi2*pFI2;

//volume name and size
    strcpy(volnam, volname);
    volumesize=volsize;

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
        printf("Initialize device Version 0.2b\n");
        printf("Volume Size %d Cluster %d Maxfiles %d\n", volumesize, clustersize, maxfiles);
	printf("Indexfile size %d\n", indexfilesize); 

//Cluster 1, 2 and 3

    //Boot Block: don't know what to do, just put all to 0
    for(i=0;i<512;i++) out_line[i]=0;
    write_blk(out_line, fout, "Boot block");

    //Home Block: construct
    pHM2$ = (struct _hm2 *) out_line;
    pHM2$->hm2$l_homelbn=1;  
    pHM2$->hm2$w_homevbn=2;
    pHM2$->hm2$l_alhomelbn=clustersize*2;
    pHM2$->hm2$w_alhomevbn=clustersize*2+1;
    pHM2$->hm2$l_altidxlbn=clustersize*3;
    pHM2$->hm2$w_altidxvbn=clustersize*3+1;
    pHM2$->hm2$w_struclev=2*256+1;
    pHM2$->hm2$w_cluster=clustersize;
    pHM2$->hm2$w_ibmapvbn=VMSWORD(clustersize*4+1); 
    pHM2$->hm2$l_ibmaplbn=VMSLONG(clustersize*4);
    pHM2$->hm2$l_maxfiles=(maxfiles);
    pHM2$->hm2$w_ibmapsize=VMSWORD(roundup( ( (double) pHM2$->hm2$l_maxfiles)/4096 ));
    pHM2$->hm2$w_extend=clustersize;
    strcpy(pHM2$->hm2$t_strucname,"            ");
    strcpy(pHM2$->hm2$t_volname , volnam);
    strcpy(pHM2$->hm2$t_ownername, owner);
    strcpy(pHM2$->hm2$t_format,   "DECFILE11B  ");
    do_checksum(out_line);
    write_blk(out_line, fout, "first home Block\0"); //write first home block

    for(i=0;i<3*clustersize-2;i++){ //fill the rest of the cluster and the next two clusters
        pHM2$->hm2$l_homelbn=2+i;pHM2$->hm2$w_homevbn=3+i;
        write_blk(out_line, fout, "\0");
    }

// Cluster 4
    //Backup Index File Header
    for(i=0;i<512;i++) out_line[i]=0; //clear all

	//index file header:
		// header area
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=1;pFH2->fh2$w_fid.fid$w_seq=1;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"INDEXF.SYS;1");
        // map area
    pFM2 = (struct FM2_C_FORMAT2 *) (out_line+(2*pFH2->fh2$b_mpoffset));
    pFM2->fm2$w_word0=indexfilesize-1+32768; //Add 32768 for format type
    pFM2->fm2$v_count2=0;
    pFH2->fh2$b_map_inuse=2;
	//file attribs
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=512;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(indexfilesize-diffsizeindexf);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(indexfilesize-diffsizeindexf+1); //not sure
    pFH2->fh2$w_recattr.fat$w_maxrec=512;
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
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=VMSWORD(1);pFH2->fh2$w_fid.fid$w_seq=VMSWORD(1);pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"INDEXF.SYS;1");
        // map area
    pFM2 = (struct FM2_C_FORMAT2 *) (out_line+(2*pFH2->fh2$b_mpoffset));
    pFM2->fm2$w_word0=indexfilesize-1+32768; //VMSWORD((indexfilesize-1) + 32768); //Add 32768 for format type
    pFH2->fh2$b_map_inuse=2;
	//file attribs
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=VMSWORD(512);
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(indexfilesize);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(indexfilesize-diffsizeindexf+1); //not sure
    pFH2->fh2$l_highwater=VMSLONG(indexfilesize+1);
    pFH2->fh2$w_recattr.fat$w_maxrec=VMSWORD(512);
    do_checksum(out_line);
    write_blk(out_line, fout, "Index file header"); //write down index file header

// header area 2.2 BITMAP.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    //pFH2->fh2$l_filechar[0]=128;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=2;pFH2->fh2$w_fid.fid$w_seq=2;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"BITMAP.SYS;1");
        // map area
    pFM2 = (struct FM2_C_FORMAT2 *) (out_line+(2*pFH2->fh2$b_mpoffset));
    storagebitmapsize=roundup( ( (double) (volumesize/clustersize))/4096);
    bitmapfilesize=clustersize*(roundup(((double) (storagebitmapsize+1))/clustersize));
    diffsizebitmap=bitmapfilesize-1-storagebitmapsize;
    pFM2->fm2$w_word0=bitmapfilesize-1+32768;
    pFM2->fm2$v_count2=(indexfilesize+clustersize);
    pFH2->fh2$b_map_inuse=2;
	//file attribs
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=512;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(bitmapfilesize);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(bitmapfilesize-diffsizebitmap+1);
    pFH2->fh2$l_highwater=VMSLONG(bitmapfilesize);
    pFH2->fh2$w_recattr.fat$w_maxrec=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "bitmap.sys header"); //write down bitmap file header

// header area 3.3 BADBLK.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=3;pFH2->fh2$w_fid.fid$w_seq=3;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"BADBLK.SYS;1");
        // no map area
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=512;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(0);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(1);
    pFH2->fh2$l_highwater=VMSSWAP(1);
    pFH2->fh2$w_recattr.fat$w_maxrec=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "badblk.sys header"); //write down file header

// header area 4.4 000000.DIR;1 
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
	pFH2->fh2$l_filechar=128+256*32; // directory flags
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=4;pFH2->fh2$w_fid.fid$w_seq=4;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"000000.DIR;1");
        // map area
    pFM2 = (struct FM2_C_FORMAT2 *) (out_line+(2*pFH2->fh2$b_mpoffset));
    pFM2->fm2$w_word0=(clustersize-1)+32768;
    pFM2->fm2$v_count2=indexfilesize;
    pFH2->fh2$b_map_inuse=2; //3;
    pFH2->fh2$w_recattr.fat$b_rtype=2;
    pFH2->fh2$w_recattr.fat$b_rattrib=8;
    pFH2->fh2$w_recattr.fat$w_rsize=512;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(clustersize);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(2);
    pFH2->fh2$l_highwater=VMSSWAP(clustersize);
    pFH2->fh2$w_recattr.fat$w_maxrec=512;
    pFH2->fh2$w_recattr.fat$w_ffbyte=216;
    do_checksum(out_line);
    write_blk(out_line, fout, "000000.dir header"); //write down file header

// header area 5.5 CORIMG.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=5;pFH2->fh2$w_fid.fid$w_seq=5;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"CORIMG.SYS;1");
        // no map area
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=512;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(0);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(1);
    pFH2->fh2$w_recattr.fat$w_maxrec=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "corimg.sys header"); //write down file header

// header area 6.6 VOLSET.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=6;pFH2->fh2$w_fid.fid$w_seq=6;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"VOLSET.SYS;1");
        // no map area
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=64;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(0);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(1);
    pFH2->fh2$w_recattr.fat$w_maxrec=64;
    do_checksum(out_line);
    write_blk(out_line, fout, "volset.sys header"); //write down file header

// header area 7.7 CONTIN.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=7;pFH2->fh2$w_fid.fid$w_seq=7;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"CONTIN.SYS;1");
        // no map area
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=512;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(0);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(1);
    pFH2->fh2$w_recattr.fat$w_maxrec=512;
    do_checksum(out_line);
    write_blk(out_line, fout, "contin.sys header"); //write down file header

// header area 8.8 BACKUP.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=8;pFH2->fh2$w_fid.fid$w_seq=8;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"BACKUP.SYS;1");
        // no map area
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=64;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(0);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(1);
    pFH2->fh2$w_recattr.fat$w_maxrec=64;
    do_checksum(out_line);
    write_blk(out_line, fout, "backup.sys header"); //write down file header

// header area 9.9 BADLOG.SYS;1
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pFH2 = (struct _fh2 *) out_line;
	pFH2->fh2$b_idoffset=40;
	pFH2->fh2$b_mpoffset=100;
	pFH2->fh2$b_acoffset=255;
	pFH2->fh2$b_rsoffset=255;
	pFH2->fh2$w_seg_num=0;
    pFH2->fh2$w_struclev=2*256+1;
    pFH2->fh2$w_fid.fid$w_num=9;pFH2->fh2$w_fid.fid$w_seq=9;pFH2->fh2$w_fid.fid$b_rvn=0;
    pFH2->fh2$w_backlink.fid$w_num=4;pFH2->fh2$w_backlink.fid$w_seq=4;pFH2->fh2$w_backlink.fid$b_rvn=0;
        // identification area
    pFI2 = (struct _fi2*) (out_line+2*pFH2->fh2$b_idoffset);
    strcpy(pFI2->fi2$t_filename,"BADLOG.SYS;1");
        // no map area
    pFH2->fh2$w_recattr.fat$b_rtype=1;
    pFH2->fh2$w_recattr.fat$w_rsize=16;
    pFH2->fh2$w_recattr.fat$l_hiblk=VMSSWAP(0);
    pFH2->fh2$w_recattr.fat$l_efblk=VMSSWAP(1);
    pFH2->fh2$w_recattr.fat$w_maxrec=16;
    do_checksum(out_line);
    write_blk(out_line, fout, "badlog.sys header"); //write down file header

    for(i=0;i<512;i++) out_line[i]=0; //clear all

    for(i=0;i<7;i++) write_blk(out_line, fout, ""); // complete 16 file headers 
    for(i=0;i<diffsizeindexf;i++) write_blk(out_line, fout, ""); // write until next cluster !
    
// All Above IS actually the INDEXF.SYS. Now:
// create 000000.DIR, files are in alphabetical order
    int pos=0;
    struct _dir  *pDIR;
    struct _dir1 *pDIR1;
    for(i=0;i<512;i++) out_line[i]=0; //clear all

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"000000.DIR"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=4; pDIR1->dir$fid.fid$w_seq=4;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"BACKUP.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=8; pDIR1->dir$fid.fid$w_seq=8;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"BADBLK.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=3; pDIR1->dir$fid.fid$w_seq=3;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"BADLOG.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=9; pDIR1->dir$fid.fid$w_seq=9;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"BITMAP.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=2; pDIR1->dir$fid.fid$w_seq=2;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"CONTIN.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=7; pDIR1->dir$fid.fid$w_seq=7;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"CORIMG.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=5; pDIR1->dir$fid.fid$w_seq=5;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"INDEXF.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=1; pDIR1->dir$fid.fid$w_seq=1;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    pDIR = (struct _dir *) (out_line+pos);
    pDIR1 = (struct _dir1 *) (out_line+pos+16);
    pDIR->dir$w_size=22; pos=pos+24;
    strcpy(pDIR->dir$t_name,"VOLSET.SYS"); pDIR->dir$b_namecount=10;
    pDIR1->dir$fid.fid$w_num=6; pDIR1->dir$fid.fid$w_seq=6;
    pDIR->dir$w_verlimit=1; pDIR->dir$b_flags=0; pDIR1->dir$w_version=1;
    pDIR1->dir$fid.fid$b_rvn=0; pDIR1->dir$fid.fid$b_nmx=0;

    out_line[pos]=255; out_line[pos+1]=255; // (-1) word 
    
    write_blk(out_line, fout, "directory file"); //write directory file - only one record, but one cluster is used
    for(i=0;i<512;i++) out_line[i]=0; //clear all
    for(i=0;i<clustersize-1;i++) write_blk(out_line, fout, ""); 
    
// create BITMAP.SYS
    //write storage control block
    struct _scbdef *pSCB;
    for(i=0;i<512;i++) out_line[i]=0; //clear all
	pSCB = (struct _scbdef *) out_line;
	pSCB->scb$w_struclev=2*256+1;
	pSCB->scb$w_cluster=clustersize;
	pSCB->scb$l_volsize=volumesize;
	pSCB->scb$l_blksize=16065;	
	pSCB->scb$l_sectors=63;	
	pSCB->scb$l_tracks=255;
	pSCB->scb$l_cylinders=4998;
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
    int usedclusters=(indexfilesize/clustersize)+(bitmapfilesize/clustersize)+1;
    j=0;
    while(j<usedclusters){
        out_line[j/8] = out_line[j/8] << 1;
        j++;
    }
    if (storagebitmapsize > 1){
    	write_blk(out_line, fout, "storage bitmap");

	   for(i=0;i<512;i++) out_line[i]=255; //make rest of clusters free, except last one
 	   for(i=0;i<storagebitmapsize-2;i++) write_blk(out_line, fout, "");
	}
    j=(storagebitmapsize*4096)-nbrcluster;  //last block, some cluster bits are unusable
    i=0;
    while(i<j){
        out_line[511-(i/8)] = out_line[511-(i/8)] >> 1;
        i++;
    }
    if (storagebitmapsize > 1)
	write_blk(out_line, fout, "");
    else
	write_blk(out_line, fout, "storage bitmap");
    for(i=0;i<512;i++) out_line[i]=0;
    for(i=0;i<diffsizebitmap;i++) write_blk(out_line, fout, ""); // write until next cluster !
    
    for(i=0;i<512;i++) out_line[i]=0;
    printf("Clearing %d blocks\n",volumesize-(usedclusters*clustersize));  
    for(i=0;i<volumesize-(usedclusters*clustersize);i++) write_blk(out_line, fout, "");
}


