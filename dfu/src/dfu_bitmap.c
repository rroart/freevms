// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified DFU source file, 2004.

/*
     DFU V2.2

     DFU_BITMAP.C

     This file contains the bitmap and quota file scanning routines.
     Uses ACPQIO to process these files.

     Called by REPORT, UNDELETE , VERIFY.

*/
#ifndef VAXC
#pragma message disable(GLOBALEXT)
#endif

#include "fibdef.h"
#include <ssdef.h>
#include <iodef.h>
#include <libdef.h>
#include <stdio.h>
#include <descrip.h>
#include "file_hdr.h"

#define globalvalue int
#define TRUE 1
#define FALSE 0
#define SYS$QIO sys$qiow
#define SYS$QIOW sys$qiow
#define SYS$ASSIGN sys$assign
#define SYS$DASSGN sys$dassgn
#define SYS$SEARCH sys$search
#define SYS$PARSE sys$parse
#define SYS$FAO sys$fao
#define SYS$ASCTIM sys$asctim
#define SYS$BINTIM sys$bintim
#define SYS$WAITFR sys$waitfr
#define SYS$GETJPIW sys$getjpiw
#define SYS$GETSYIW sys$getsyiw
#define SYS$GETDVIW sys$getdviw

typedef unsigned long Boolean;

typedef struct _usg { int owner, flag, rsize, asize, hdr;};

typedef struct _qb { int flags, uic_long, usage, permquota, 
                     overdraft, unused[3]; };

extern unsigned int ctrlc;
#if 0
extern unsigned int lbnstart, lbncount;
#else
unsigned int lbnstart, lbncount;
#endif
extern Boolean trigger;
extern char outbuf[255];

static struct { unsigned short iosb_1;
	        unsigned int length;
	        short spec; 
	      } iostat;

globalvalue DFU_EXPREG;

int read_bitmap(short chan, int xpages, Boolean locked, int *addr)

/* 
   Read BITMAP.SYS in dynamic space 
   Assumes 'chan' already opened by calling procedure 
*/

{ static struct fibdef bfib; /* File information block for bitmap */
  struct { int fiblen; struct fibdef *fibadr; } fibdescr;
  int func_code, status, bytes;

/* Fill in FIB for bitmap */

  fibdescr.fiblen = sizeof(bfib); /* Short fib */
  fibdescr.fibadr = &bfib;
  bfib.fib$w_fid[0] = 2;
  bfib.fib$w_fid[1] = 2;
  bfib.fib$w_fid[2] = 0;
  bfib.fib$w_did[0] = 0;
  bfib.fib$w_did[1] = 0;
  bfib.fib$w_did[2] = 0;
  bfib.fib$b_wsize = 7;
  if (locked == FALSE )
   bfib.fib$l_acctl = FIB$M_WRITE | FIB$M_WRITETHRU 
     | FIB$M_NORECORD | FIB$M_NOLOCK;
  else
   bfib.fib$l_acctl = FIB$M_NOLOCK;

/* Open bitmap. (Channel already opened in calling procedure)
   If the device is not write locked, we open it for write 
   to force a cache flush. */

  func_code = IO$_ACCESS | IO$M_ACCESS;
  status = SYS$QIOW(0, chan, func_code, &iostat, 0,0,&fibdescr,0,0,0,0,0);
  if ((status & 1) == 1) status = iostat.iosb_1;
  if ((status & 1) != 1) 
  { sprintf(outbuf,"%%DFU-E-BITMAP, Error opening bitmap,");
    put_disp();
    singlemsg(0,status); return(status);  
  }

/* Read in bitmap starting from VBN 2 (VBN 1 is Storage control block ) */
  bytes = 512*xpages;
  status = SYS$QIOW(0, chan, IO$_READVBLK, &iostat, 0,0, *addr, bytes,
             2,0,0,0);
  if ((status & 1) == 1) status = iostat.iosb_1;
  if ((status & 1) != 1)
  { sprintf(outbuf,"%%DFU-E-READERR, Error reading bitmap,");
    put_disp();
    singlemsg(0,status); 
  }
  return(status);
}

int scan_bitmap(short chan, int pages, Boolean locked, int *free, 
       int *large, int *blocks, int *large_lbn, int *table)
/* 
   Scan bitmap.sys. Count all the used and free bits to report
   largest contiguous free space, count of fragment, largest extent and LBN.
   Also create graph table. Called by REPORT.
*/

{
  char size;
  register int i;
  int status, bmap_addr[2], count, offset,
   which, interval, entry, xpages ,j;
  unsigned int total_blocks, total_count, lbn, savebit, 
    maxlong, savelongword, largest, temp;
  int *ix; 

/* Allocate dynamic space */
  xpages = (pages + 4095) / 4096;
  status = SYS$EXPREG(xpages, &bmap_addr[0], 0, 0);
  if ((status & 1) != 1)
  { singlemsg(DFU_EXPREG,status); return(status);  
  }
  
  status = read_bitmap(chan, xpages, locked, &bmap_addr[0]);
/* Close it now */
  SYS$QIOW(0, chan, IO$_DEACCESS, 0,0,0,0,0,0,0,0,0);
  if ((status & 1) != 1) 
  { SYS$DELTVA(&bmap_addr, &bmap_addr, 0);
    return(status);  
  }
/* Now we have the bitmap read in, process it */

  total_blocks = 0;
  total_count = 0;
  lbn = 0;
  savebit = 0;
  savelongword = 0;
  largest = 0;
  count = 0; j = 0;  
  maxlong = (pages + 31) / 32 ;  /* Maximum longwords to scan */
  interval = maxlong / 70; 
  if (interval ==0) interval = 1;
  entry = 0; which = interval;
  ix = (int *) bmap_addr[0]; /* Start of array */
  i = 0;
  offset = 0;
  while (i < maxlong) /* Loop through the bitmap longword by longword */
  { size = 32 - offset;
    if (*ix == 0) status = 0; /* This longword fully used */
      else status = lib$ffs(&offset, &size, ix, &offset);
    if ((status &1) != 1) 
    { /* Adjust the graph table */
      if (i >= which) 
      { entry = interval - entry; entry = entry * 100;
        which += interval; j++;
/* Take care not to cross table bounds. This can only happen
   due to rounding errors on very small devices. */
        if (j <=70) 
        { *table = entry / interval;
          table++; 
        }
        entry = 0;
      }
      entry++;  
      offset = 0; ix++; i++; /* Next long word */
    }
    else
    { total_count++; savelongword = i;
      savebit = offset; 
      status = 0; 
      while (status != 1) 
      { size = 32 - offset; 
        if (*ix == -1) status = 0;
          else status = lib$ffc(&offset, &size, ix, &offset);
        if ((status & 1) !=1) /* We still have a contiguous free extent */
        { offset = 0; i++; ix++;
          if (i >= maxlong ) status = 1;
        }
      }
/* We have a free extent , calculate the various fields now */
      temp = i - savelongword;
      if ( i == 0)
        temp = offset - savebit; 
       else
      { temp--; savebit = 32 - savebit;
        temp = temp*32 + savebit + offset;     
      }
      total_blocks += temp;
      if (temp >= largest)
      { largest = temp; savebit = 32 - savebit;
        lbn = 32*savelongword + savebit;
      }
    }
  }
/* Report results to caller */
  SYS$DELTVA(&bmap_addr, &bmap_addr, 0);
  *free = total_count;
  *large = largest;
  *blocks = total_blocks;
  *large_lbn = lbn; 
  return(1);
}  

void new_bitmap(int *addr, struct header_area_struct *hdr, int cluster,
                int curvol, FILE *fp, int pass, int *lis, Boolean *flag,
                Boolean matoutput)
/* 
     1) Disassemble retrieval pointers into LBN's
     2) Build up new bitmap into the dynamic area
     3) Report multiple allocated blocks.
     If pass =1 the multiple allocated blocks are saved in a dynmic
	linked list pointed to by *lis (and *flag is set to true).
     The VERIFY_COMMAND procedure will call us a second time with pass=2
     to report the multiple allocated blocks to the user. This
     is done to ensure that all involved files will be reported
*/

{ struct _bm {int block[128];} *start, *bmap; /* Pointer to a bitmap block (128 longwords */
  struct _hd {unsigned short block[256]; } *head; /* pointer to header */
  struct ident_area_struct *id;
  struct mult { unsigned int l_start, l_end;
		struct mult *next;} *m_list;	        
  int i, l, k, ibit, iword, iblock, lbit, lword, lblock, 
    expon,offset,map_bytes, endj, size, lbnc1, lbne1, namelen, format, fidnum;
  Boolean report;
  char name[86];
  register int j;
  
  head = (struct _hd *) hdr; /*We can now view the header as 256 words */
  start = (struct _bm *) addr;
  i = hdr->idoffset;
  id = (struct ident_area_struct *) &(head->block[i]);
  offset = hdr->mpoffset; /* Start of map area */
  map_bytes = hdr->map_inuse;
  lbncount = 0;
  report = FALSE;
  expon = -1;
  l = 1; if (pass ==1) m_list = (struct mult *) *lis;

/* Now disassemble retrieval pointers into LBNSTART, LBNCOUNT */
  for (i = offset; i<(offset+map_bytes);)
  { disass_map_ptr(hdr,&i);
    if (lbncount !=0) 
/* Setup dynamic array. We try to set it up longword by longword ('j' loop).
   If multiple allocated blocks are discoverd we must check bit by bit
   to determine the exact LBN's ('k' loop) */
    { lbnstart = lbnstart/cluster;
      lbncount = lbncount/cluster;
      iblock = lbnstart/4096 + 1; 
      if ((lbnstart % 4096) == 0) iblock--; /* Block boundary */
/* End position of this pointer...*/
      endj = lbnstart + lbncount - 1;
      lblock = endj/4096 + 1;
      lbit = endj % 4096;
      lword = lbit/32 + 1;
      lbit = lbit % 32;
      j = lbnstart;
      while (j <= endj)
      { ibit = j % 4096;
        if (ibit == 0) iblock ++; /* Next block */
        iword = ibit/32 + 1;
        ibit = ibit % 32;
        if ((lword == iword) && (iblock == lblock))
          size = lbit - ibit + 1;
         else size = 32 - ibit; /*How many bits to parse in this longword */
/* Now check if these bits are free, if yes fill them up */
        bmap = start+(iblock-1); /* Pointer to correct block */
        if (lib$extzv(&ibit, &size, &bmap->block[iword-1]) == 0)
          lib$insv(&expon, &ibit, &size, &bmap->block[iword-1]);
         else /* We have multiple allocated blocks */
        { for (k = j; k <= j+size-1; k++)
          { if (lib$extzv(&ibit, &l ,&bmap->block[iword-1]) == 1)
            { if (report == FALSE)
              { report = TRUE;
                lbne1 = k*cluster;
                lbnc1 = lbne1 + cluster - 1;
              }
              else lbnc1=(k+1)*cluster - 1;
            }
           else
/* We now have a consecutive chunk of multiple allocated blocks, report them */
            if (report == TRUE)
            { report = FALSE; /* Don't report twice */
	      if (pass == 2) /* now report the file to the user */
              { memcpy(&name[0],id->filename,20);
                if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
                namelen = strindex(&name[0]," ",86); 
                fidnum = hdr->fid_overlay.fid_fields.fid_nmx << 16;
                fidnum += hdr->fid_overlay.fid_fields.fid_num;
                sprintf(outbuf,"%%DFU-E-MULTALLOC, file (%d,%d,%d) %.*s blocks LBN %d through %d multiple allocated",
                  fidnum, hdr->fid_overlay.fid_fields.fid_seq, curvol,
                  namelen, name, lbne1, lbnc1);
                put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
	      }
	       else /* Save these blocks */
	      { *flag = TRUE;
	        m_list->l_start = lbne1;
	        m_list->l_end = lbnc1;
		m_list->next = (struct mult *) malloc(sizeof(struct mult));
	        m_list = m_list->next;
	 	*lis = (int) m_list;
	      }
            }
            lib$insv(&l,&ibit,&l,&bmap->block[iword-1]);
            ibit++;
          } /* End k loop */
        }
        j+=size;
      } /* End j loop */
/* Check if we still have some unreported blocks left */
      if (report == TRUE)
      { report = FALSE; /* Don't report twice */
        if (pass == 2) /* now report the file to the user */
        { memcpy(&name[0],id->filename,20);
          if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
          namelen = strindex(&name[0]," ",86); 
          fidnum = hdr->fid_overlay.fid_fields.fid_nmx << 16;
          fidnum += hdr->fid_overlay.fid_fields.fid_num;
          sprintf(outbuf,"%%DFU-E-MULTALLOC, file (%d,%d,%d) %.*s blocks LBN %d through %d multiple allocated",
            fidnum, hdr->fid_overlay.fid_fields.fid_seq, curvol,
            namelen, name, lbne1, lbnc1);
          put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
        }
         else
        { *flag = TRUE;
          m_list->l_start = lbne1;
          m_list->l_end = lbnc1;
          m_list->next = (struct mult *) malloc(sizeof(struct mult));
	  m_list = m_list->next;
	  *lis = (int) m_list;
        }
      }
    }
  } /* End i loop */
}

void set_bitmap(int * addr, int cluster, int lbn_start, 
                int lbn_count, Boolean mbs)
/* 
	Set the multiple allocated blocks in the new bitmap
        They were found in new_bitmap and entered in m_list

	V1.6 modify this routine so that we can set/clr bits
	in BITMAP.SYS in memory during a VERIFY/REBUILD
*/

{ struct _bm {int block[128];} *start, *bmap; /* Pointer to a bitmap block (128 longwords */
  int i, l, k, ibit, iword, iblock, lbit, lword, lblock , expon,
    offset, map_bytes, endj, size, lbnc1, lbne1, namelen, format, fidnum;
  register int j;
  
  start = (struct _bm *) addr;
  if (mbs) 
    expon = -1;
   else
    expon = 0;

  l = 1; 

  if (lbn_count !=0) 
  { lbn_count = lbn_count - lbn_start + 1;
    lbn_start = lbn_start/cluster;
    lbn_count = lbn_count/cluster;
    iblock = lbn_start/4096 + 1; 
    if ((lbn_start % 4096) == 0) iblock--; /* Block boundary */
/* End position of this pointer...*/
    endj = lbn_start + lbn_count - 1;
    lblock = endj/4096 + 1;
    lbit = endj % 4096;
    lword = lbit/32 + 1;
    lbit = lbit % 32;
    j = lbn_start;
    while (j <= endj)
    { ibit = j % 4096;
      if (ibit == 0) iblock ++; /* Next block */
      iword = ibit/32 + 1;
      ibit = ibit % 32;
      if ((lword == iword) && (iblock == lblock))
      size = lbit - ibit + 1;
       else size = 32 - ibit; /*How many bits to parse in this longword */
      bmap = start+(iblock-1); /* Pointer to correct block */
      lib$insv(&expon, &ibit, &size, &bmap->block[iword-1]);
      j+=size;
    } /* End j loop */
  } 
}

int rebuild_bitmap(struct header_area_struct *hdr, int *addr, int cluster,
                int curvol, Boolean check_only)
/*   Called by UNDELETE
     1) Disassemble retrieval pointers into LBN's
          If check_only is FALSE...
     2) rebuild the bitmap into the dynamic area
     3) Report multiple allocated blocks.
*/

{ struct _bm {int block[128];} *start, *bmap; /* Pointer to a bitmap block (128 longwords */
  struct _hd {unsigned short block[256]; } *head; /* pointer to header */
  struct ident_area_struct *id;
  int i, l, k, ibit, iword, iblock, lbit, lword, lblock, 
    expon, offset, map_bytes, endj, size, lbnc1, lbne1, namelen, 
    format, fidnum, status;
  Boolean report;
  char name[86];
  register int j;
  
  head = (struct _hd *) hdr; /*We can now view the header as 256 words */
  start = (struct _bm *) addr;
  i = hdr->idoffset;
  id = (struct ident_area_struct *) &(head->block[i]);
  offset = hdr->mpoffset; /* Start of map area */
  map_bytes = hdr->map_inuse;
  lbncount = 0;
  report = FALSE;
  expon = 0;
  l = 1; status = 1; /* Assume success */

/* Now disassemble retrieval pointers into LBNSTART, LBNCOUNT */
  for (i = offset; i<(offset+map_bytes);)
  { disass_map_ptr(hdr,&i);
    if (lbncount !=0) 
/* Setup dynamic array. We try to set it up longword by longword ('j' loop).
   If multiple allocated blocks are discoverd we must check bit by bit
   to determine the exact LBN's ('k' loop) */
    { lbnstart = lbnstart/cluster;
      lbncount = lbncount/cluster;
      iblock = lbnstart/4096 + 1; 
      if ((lbnstart % 4096) == 0) iblock--; /* Block boundary */
/* End position of this pointer...*/
      endj = lbnstart + lbncount - 1;
      lblock = endj/4096 + 1;
      lbit = endj % 4096;
      lword = lbit/32 + 1;
      lbit = lbit % 32;
      j = lbnstart;
      while (j <= endj)
      { ibit = j % 4096;
        if (ibit == 0) iblock ++; /* Next block */
        iword = ibit/32 + 1;
        ibit = ibit % 32;
        if ((lword == iword) && (iblock == lblock))
          size = lbit - ibit + 1;
         else size = 32 - ibit; /*How many bits to parse in this longword */
/* Now check if these bits are free, if yes fill them up */
        bmap = start+(iblock-1); /* Pointer to correct block */
        if (lib$extv(&ibit, &size, &bmap->block[iword-1]) == -1)
        {  if (!check_only) 
            lib$insv(&expon, &ibit, &size, &bmap->block[iword-1]);
        }
         else /* We have multiple allocated blocks */
        { if (check_only) 
          { i = 512 ; j = endj + 1; /* Stop further processing */
	    status = 0;
          }
         else
	  { status = 0;
            for (k = j; k <= j+size-1; k++)
            { if (lib$extv(&ibit, &l ,&bmap->block[iword-1]) == 0)
              { if (report == FALSE)
                { report = TRUE;
                  lbne1 = k*cluster;
                  lbnc1 = lbne1 + cluster - 1;
                }
                else lbnc1=(k+1)*cluster - 1;
              }
             else
/* We now have a consecutive chunk of multiple allocated blocks, report them */
              if (report == TRUE)
              { report = FALSE; /* Don't report twice */
                memcpy(&name[0],id->filename,20);
                if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
                namelen = strindex(&name[0]," ",86); 
                sprintf(outbuf,"%%DFU-E-MULTALLOC, file %.*s blocks LBN %d through %d already allocated",
                  namelen, name, lbne1, lbnc1);
                put_disp();
              }
              ibit++;
            } /* End k loop */
          }
        }
        j+=size;
      } /* End j loop */
/* Check if we still have some unreported blocks left */
      if (report == TRUE)
      { report = FALSE; /* Don't report twice */
        memcpy(&name[0],id->filename,20);
        if (name[19] != ' ') memcpy(&name[20],id->filenamext,66);
        namelen = strindex(&name[0]," ",86); 
        sprintf(outbuf,"%%DFU-E-MULTALLOC, file %.*s blocks LBN %d through %d already allocated",
          namelen, name, lbne1, lbnc1);
        put_disp();
      }
    }
  } /* End i loop */
  return(status);
}

int compare_bitmap(short chan, int *addr, int pages, Boolean locked, 
                   int cluster, FILE *fp, Boolean rebuild, Boolean matoutput)
/*
   Compare the bitmap build up in new_bitmap with the real BITMAP.SYS.
   The new bitmap has bits set for each used cluster, whereas BITMAP.SYS
   has such bits clear. So we XOR each longword ; this should result in -1.
   If not we have incorrectly marked free or allocated clusters which
   we report back to the user. V1.6 Modified the code to rebuild
   and rewrite the bitmap.
*/

{ struct _bm {int block[128];} *start, *bmap; /* Pointer to a bitmap block (128 longwords */
  int i, l, k, status, xpages, bmap_addr[2], bounds, maxblocks, chk;
  int blkss, blkse, blkas, blkae, bytes;
  register int j;
  Boolean reports, reportc, trigger;

/* Allocate dynamic space */
  sprintf(outbuf,"%%DFU-I-CHKBITMAP, Checking BITMAP.SYS...");
  put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
  maxblocks = pages * cluster;
  xpages = (pages + 4095) / 4096;
  status = SYS$EXPREG(xpages, &bmap_addr[0], 0, 0);
  if ((status & 1) != 1)
  { singlemsg(DFU_EXPREG,status); return(status);  
  }
  
  trigger = FALSE;
  status = read_bitmap(chan, xpages, locked, &bmap_addr[0]);
  if ((status & 1) != 1) 
  { SYS$DELTVA(&bmap_addr, &bmap_addr, 0);
    SYS$QIOW(0, chan, IO$_DEACCESS, 0,0,0,0,0,0,0,0,0);
    return(status);  
  }
/* Close it now if no rebuild is needed */
  if (!rebuild) SYS$QIOW(0, chan, IO$_DEACCESS, 0,0,0,0,0,0,0,0,0);

/* Now we have the bitmap read in, process it */
  start = (struct _bm *) addr; 
  bmap = (struct _bm *) bmap_addr[0];
  l = 1;
  reports = FALSE; reportc = FALSE;
  for (i = 0; i < xpages; i++)
  { bounds = (i * 4096) * cluster - 32*cluster;
    for (j = 0; j <= 127; j++)
    { bounds += 32*cluster;
      if (bounds <= maxblocks) /* Do not cross array bounds */
      { chk = ((start+i)->block[j] ^ (bmap+i)->block[j]);
        if (chk != -1) /* Found some blocks...*/
        { for (k=0; k <= 31; k++)
          { bounds += cluster;
            if (bounds <= maxblocks) /* Do not cross array bounds */
            { chk = lib$extzv(&k, &l, &(bmap+i)->block[j]) +
                   lib$extzv(&k, &l, &(start+i)->block[j]);
              if (chk == 2) /* Incorrectly set bits */
              { if (reports == FALSE) 
                { reports = TRUE;
                  blkss = ((i * 4096) + j*32 + k) * cluster;
                  blkse = blkss + cluster - 1;
                }
                else 
                  blkse = ((i * 4096) + j*32 + k + 1) * cluster - 1;
              }
              else /* Report these blocks */
              { if (reports == TRUE)
                { reports = FALSE; trigger = TRUE;
                  sprintf(outbuf,"%%DFU-E-ALLOCSET, blocks LBN %d through %d incorrectly marked free",
                    blkss, blkse);
                  put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
                  if (rebuild) 
                    set_bitmap((int *) bmap,cluster,blkss,blkse,FALSE);
                }
              }
              if (chk == 0) /* Incorrectly clear bits */
              { if (reportc == FALSE) 
                { reportc = TRUE;
                  blkas = ((i * 4096) + j*32 + k) * cluster;
                  blkae = blkas + cluster - 1;
                }
                else 
                  blkae = ((i * 4096) + j*32 + k + 1) * cluster - 1;
              }
              else /* Report these blocks */
              { if (reportc == TRUE)
                { reportc = FALSE; trigger = TRUE;
                  sprintf(outbuf,"%%DFU-E-ALLOCCLR, blocks LBN %d through %d incorrectly marked allocated",
                    blkas, blkae);
                  put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
                  if (rebuild) 
                    set_bitmap( (int *) bmap,cluster,blkas,blkae,TRUE);
                }
              }
            }
          } /* end k loop */
        }
      } 
    } /* end j loop */
  } /* end i loop */
/* Check if we have unreported blocks left */
  if (reports == TRUE)
    { reports == FALSE; trigger = TRUE;
      sprintf(outbuf,"%%DFU-E-ALLOCSET, blocks LBN %d through %d incorrectly marked free",
                blkss, blkse);
      put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
      if (rebuild) 
        set_bitmap((int *) bmap,cluster,blkss,blkse,FALSE);
    }
  if (reportc == TRUE)
    { reportc == FALSE; trigger = TRUE;
      sprintf(outbuf,"%%DFU-E-ALLOCCLR, blocks LBN %d through %d incorrectly marked allocated",
                blkas, blkae);
      put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
      if (rebuild) 
        set_bitmap((int *) bmap,cluster,blkas,blkae,TRUE);
    }

  if ((rebuild) && (trigger))
  { /* Now rewrite the bitmap */
    bytes = 512*xpages;
    status = SYS$QIOW(0, chan, IO$_WRITEVBLK, &iostat, 0,0, bmap_addr[0], 
             bytes,2,0,0,0);
    if ((status & 1) == 1) status = iostat.iosb_1;
    if ((status & 1) != 1)
    { sprintf(outbuf,"%%DFU-E-RBDBMAP, Error rewriting BITMAP.SYS,");
      put_disp();
      singlemsg(0,status);
    }
     else
    { sprintf(outbuf,"%%DFU-S-RBDBITMAP, BITMAP.SYS succesfully rebuild");
      put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
    }
  }
  if (rebuild) SYS$QIOW(0, chan, IO$_DEACCESS, 0,0,0,0,0,0,0,0,0);

  SYS$DELTVA(&bmap_addr, &bmap_addr, 0);
  return(1);
}

void add_usage(struct _usg *table, int owner, int r_size, int a_size, int hdrs)
/* 
   Build a disk usage table based on identifiers/uic's.
*/

{ struct _usg *usage_table;
  int i,j; 
  Boolean found;

  found = FALSE;
  i = 1;
  usage_table = table;
  while (!found)
  { if (usage_table->flag == 0)
    { usage_table->flag = 1;
      found = TRUE;
      usage_table->rsize = 0;
      usage_table->asize = 0;
      usage_table->hdr = 0;
      usage_table->owner = owner;
    }
    else 
    if (usage_table->owner == owner) 
      found = TRUE;
     else
      if (i < 750) 
      { i++; usage_table++;
      }
      else 
       found = TRUE;
  }
  usage_table->hdr += hdrs;
  usage_table->asize += a_size;
  usage_table->rsize += r_size;
}

void add_quota(short chan, int uic, int hdrs, int size)
/* 
   Add the diskquota for an undeleted file
*/

{ static struct fibdef qfib; /* File information block for quota file */
  static struct _qb qblock;
  struct { int fiblen; struct fibdef *fibadr; } fibdescr;
  struct { int qlen; struct _qb *qadr; } qb_descr;
  int len, status;
  Boolean matquota;

  matquota = TRUE;
/* Fill in FIB for quota file */

  fibdescr.fiblen = sizeof(qfib); /* Short fib */
  fibdescr.fibadr = &qfib;
  len = 32;
  qfib.fib$w_exctl = FIB$C_EXA_QUOTA;
  qfib.fib$l_exsz = 0;
  qb_descr.qlen = sizeof(qblock);
  qb_descr.qadr = &qblock;
  qblock.uic_long = uic;

/* Check if quota is enabled */
  status = SYS$QIOW(0, chan, IO$_ACPCONTROL, &iostat, 0, 0,
    &fibdescr, &qb_descr, &len, &qb_descr, 0 , 0);
  if ((status &1) !=1) matquota = FALSE;
  if (iostat.iosb_1 == SS$_QFNOTACT) matquota = FALSE;

  if (matquota)
  { qfib.fib$w_exctl = FIB$C_MOD_QUOTA;
    qfib.fib$l_exsz = FIB$M_MOD_USE;
    sprintf(outbuf,"%%DFU-I-ADDQUOTA, updating diskquota...");
    put_disp();
    qblock.usage += (hdrs + size);
    status = SYS$QIOW(0, chan, IO$_ACPCONTROL, &iostat, 0, 0,
      &fibdescr, &qb_descr, &len, &qb_descr, 0 , 0);
  }
}

void report_usage(struct _usg *table, short chan, FILE *fp, 
                  Boolean matoutput)
/* 
   Report the disk usage (and quota usage)
*/

{ struct _usg *usage_table;
  int i, status, outlen, len; 
  Boolean matquota;
  static char ident[30];
  static struct fibdef qfib; /* File information block for quota file */
  static struct _qb qblock;
  struct { int fiblen; struct fibdef *fibadr; } fibdescr;
  struct { int qlen; struct _qb *qadr; } qb_descr;
  $DESCRIPTOR(id_descr , ident);
  $DESCRIPTOR(out_descr , outbuf);
  $DESCRIPTOR(ctrstr,"!%I");
  $DESCRIPTOR(ctrstr2,"!30AS!9UL/!9<!UL!>!6UL  !9UL/!9<!UL!>");
  $DESCRIPTOR(ctrstr3,"!30AS!9UL/!9<!UL!>!6UL");
 
/* Fill in FIB for quota file */

  fibdescr.fiblen = sizeof(qfib); /* Short fib */
  fibdescr.fibadr = &qfib;
  qfib.fib$w_exctl = FIB$C_EXA_QUOTA;
  qfib.fib$l_exsz = 0;
  qb_descr.qlen = sizeof(qblock);
  qb_descr.qadr = &qblock;
  len = 0;
  qblock.uic_long=0;
  matquota = TRUE;

/* Check if quota is enabled */
  status = SYS$QIOW(0, chan, IO$_ACPCONTROL, &iostat, 0, 0,
    &fibdescr, &qb_descr, &len, &qb_descr, 0 , 0);
  if ((status &1) !=1) matquota = FALSE;
  if (iostat.iosb_1 == SS$_QFNOTACT) matquota = FALSE;

  outlen = 0;
  if(matquota==TRUE) 
  { sprintf(outbuf,
      "\n      ***** Disk Usage Table (from INDEXF.SYS and QUOTA.SYS) *****"); 
    put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
    sprintf(outbuf," Identifier/UIC                   Used/Allocated   Headers   Quota Used/Perm");
    put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
  }
   else
  { sprintf(outbuf,"\n        ***** Disk Usage Table (from INDEXF.SYS) *****");
    put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
    sprintf(outbuf," Identifier/UIC                   Used/Allocated   Headers");
    put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
  }
  sprintf(outbuf," ---------------------------------------------------------------------------");
  put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
  i = 1;
  usage_table = table;
  while ((i < 750) && (usage_table->flag == 1))
  { status = SYS$FAO(&ctrstr, &id_descr.dsc$w_length, &id_descr,
      usage_table->owner);
    if ((status &1) != 1) singlemsg(0,status);
    if (matquota == TRUE) /* Get the actual quota */
    { qblock.uic_long = usage_table->owner;
      status = SYS$QIOW(0, chan, IO$_ACPCONTROL, &iostat, 0, 0,
       &fibdescr, &qb_descr, &len, &qb_descr, 0 , 0);
      if (((status &1) !=1) || (iostat.iosb_1 == SS$_NODISKQUOTA))
      { qblock.usage = 0;
        qblock.permquota = 0;
      }
      status = SYS$FAO(&ctrstr2, &outlen, &out_descr, &id_descr,
        usage_table->rsize, usage_table->asize, usage_table->hdr,
        qblock.usage, qblock.permquota);
      if ((status &1) != 1) singlemsg(0,status);
    }
    else
    {  status = SYS$FAO(&ctrstr3, &outlen, &out_descr, &id_descr,
        usage_table->rsize, usage_table->asize, usage_table->hdr);
      if ((status &1) != 1) singlemsg(0,status);
    }
    if (matoutput) fprintf(fp,"%*s\n",outlen,outbuf);
    outbuf[outlen] = '\0';
    put_disp();
    id_descr.dsc$w_length = 30;
    i++; usage_table++;
  }
}

void check_usage(struct _usg *table, short chan, FILE *fp, Boolean reb,
                 Boolean matoutput)
/* 
   Compare the usage calculated in the usage table
   with the diskquota (if enabled).
   Report mismatches, and repair them if rebuild is requested.
   This routine is called by VERIFY
*/

{ struct _usg *usage_table;
  int i, status, len; 
  unsigned int exp_quota;
  Boolean matquota, trigger, reb_done;
  static char ident[30];
  static struct fibdef qfib; /* File information block for quota file */
  struct { int fiblen; struct fibdef *fibadr; } fibdescr;
  static struct _qb qblock;
  struct { int qlen; struct _qb *qadr; } qb_descr;
  $DESCRIPTOR(id_descr , ident);
  $DESCRIPTOR(ctrstr,"!%I");
 
/* Fill in FIB for quota file */

  fibdescr.fiblen = sizeof(qfib); /* Short fib */
  fibdescr.fibadr = &qfib;
  qfib.fib$w_exctl = FIB$C_EXA_QUOTA;
  qfib.fib$l_exsz = 0;
  qb_descr.qlen = sizeof(qblock);
  qb_descr.qadr = &qblock;
  qblock.uic_long=0;
  matquota = TRUE;
  i = 1;
  len = 32;
/* Check if quota is enabled */
  status = SYS$QIOW(0, chan, IO$_ACPCONTROL, &iostat, 0, 0,
    &fibdescr, &qb_descr, &len, &qb_descr, 0 , 0);
  if ((status &1) !=1) matquota = FALSE;
  if (iostat.iosb_1 == SS$_QFNOTACT) matquota = FALSE;
  reb_done = FALSE;
  if(matquota==TRUE) 
  { usage_table = table;
    sprintf(outbuf,"%%DFU-I-CHKQUOTA, Checking QUOTA.SYS...");
    put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
    while ((i < 750) && (usage_table->flag == 1) && (ctrlc != 1) )
    { trigger = FALSE;
      qblock.uic_long = usage_table->owner;
      status = SYS$QIOW(0, chan, IO$_ACPCONTROL, &iostat, 0, 0,
       &fibdescr, &qb_descr, &len, &qb_descr, 0 , 0);
      if (((status &1) !=1) || (iostat.iosb_1 == SS$_NODISKQUOTA))
      { qblock.usage = 0;
        qblock.permquota = 0;
      }
       else trigger = TRUE;
      exp_quota = usage_table->asize + usage_table->hdr;
      if (exp_quota != qblock.usage)
      { SYS$FAO(&ctrstr, &id_descr.dsc$w_length, &id_descr,
         usage_table->owner);
        sprintf(outbuf,"%%DFU-W-QUOTAERR, %.*s has %d blocks used, QUOTA indicates %d blocks"
         ,id_descr.dsc$w_length, ident, exp_quota, qblock.usage);
        put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
        id_descr.dsc$w_length = 30;
        if ((reb) && (trigger)) /* rebuild if there was an entry for this UIC*/
        { qfib.fib$w_exctl = FIB$C_MOD_QUOTA;
          qfib.fib$l_exsz = FIB$M_MOD_USE;
          qblock.usage = exp_quota;
          status = SYS$QIOW(0, chan, IO$_ACPCONTROL, &iostat, 0, 0,
            &fibdescr, &qb_descr, &len, &qb_descr, 0 , 0);
          reb_done = TRUE;
          qfib.fib$w_exctl = FIB$C_EXA_QUOTA;
          qfib.fib$l_exsz = 0;
        }
      }
      i++; usage_table++;
    }
  }
  if ((reb) && (reb_done))
  { sprintf(outbuf,"%%DFU-S-REBQUOTA, QUOTA.SYS succesfully rebuild");
    put_disp(); if (matoutput) fprintf(fp,"%s\n",outbuf);
  }
}
