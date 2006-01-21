// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <starlet.h>
#include <misc.h>

show_memory(){
  int sts;
  int i;
  extern int sch$gl_freecnt;
  extern int max_mapnr;
  extern int pagefile;
  int user_mapnr=0;
  int user_freecnt;
  int user_pagefile=0;

  sts = cliutl$getmem(&sch$gl_freecnt, &user_freecnt, 0);
  sts = cliutl$getmem(&max_mapnr, &user_mapnr, 0);
  sts = cliutl$getmem(&pagefile, &user_pagefile, 0);

  printf("\n");
  printf("              System Memory Resources on DD-MMM-2006 14:10:42.12\n");
  printf("\n");
  printf("Physical Memory Usage (pages):     Total        Free      In Use    Modified\n");
  printf("  Main Memory (%6dMB)           %5d        %5d      %5d       %5d\n",user_mapnr>>8,user_mapnr,user_freecnt,user_mapnr-user_freecnt,0);
  printf("\n");
#if 0
  printf("Extended File Cache  (Time of last reset:  1-DEC-2005 16:47:44.98)\n");
  printf(" Allocated (MBytes)              5.42    Maximum size (MBytes)            96.00\n");
  printf(" Free (MBytes)                   0.00    Minimum size (MBytes)             3.12\n");
  printf(" In use (MBytes)                 5.42    Percentage Read I/Os                68%\n");
  printf(" Read hit rate                     93%   Write hit rate                       0%\n");
  printf(" Read I/O count                658594    Write I/O count                 300398\n");
  printf(" Read hit count                618127    Write hit count                      0\n");
  printf(" Reads bypassing cache            305    Writes bypassing cache               0\n");
  printf(" Files cached open                578    Files cached closed                 58\n");
  printf(" Vols in Full XFC mode              0    Vols in VIOC Compatible mode         6\n");
  printf(" Vols in No Caching mode            0    Vols in Perm. No Caching mode        0\n");
  printf("\n");
  printf("Granularity Hint Regions (pages):  Total        Free      In Use    Released  \n");
  printf("  Execlet code region               1536           0         893         643\n");
  printf("  Execlet data region                296           0         296           0\n");
  printf("  S0/S1 Executive data region        672           0         672           0\n");
  printf("  Resident image code region        1024           0        1008          16\n");
  printf("\n");
  printf("Slot Usage (slots):                Total        Free    Resident     Swapped\n");
  printf("  Process Entry Slots                161         101          56           4\n");
  printf("  Balance Set Slots                  159         101          54           4\n");
  printf("\n");
#endif
#if 0
  printf("Dynamic Memory Usage:              Total        Free      In Use     Largest\n");
  printf("  Nonpaged Dynamic Memory (MB)      5.06        1.50        3.55        0.93\n");
  printf("  Bus Addressable Memory  (KB)    128.00      110.87       17.12      104.00\n");
  printf("  Paged Dynamic Memory    (MB)      2.17        0.91        1.26        0.91\n");
  printf("  Lock Manager Dyn Memory (MB)      2.74        1.28        1.45\n");
  printf("\n");
  printf("Buffer Object Usage (pages):                  In Use        Peak\n");
  printf("  32-bit System Space Windows (S0/S1)              7           9\n");
  printf("  64-bit System Space Windows (S2)                 0           0\n");
  printf("  Physical pages locked by buffer objects          5           6\n");
  printf("\n");
  printf("Memory Reservations (pages):       Group    Reserved      In Use        Type\n");
  printf("  Total (0 bytes reserved)                         0           0\n");
  printf("\n");
  printf("Write Bitmap (WBM) Memory Summary\n");
  printf("  Local bitmap count:     0     Local bitmap memory usage (bytes)       0.00\n");
  printf("  Master bitmap count:    0     Master bitmap memory usage (bytes)      0.00\n");
  printf("\n");
#endif
#if 0
  printf("Swap File Usage (4KB pages):                   Index        Free        Size\n");
  printf("  DISK$OBELIXSYS:[SYS0.SYSEXE]SWAPFILE.SYS                                      \N");
  printf("                                                   1        1024        1280\n");
  printf("\n");
#endif
  if (user_pagefile==0)
    goto skip;
  printf("Paging File Usage (4KB pages):                 Index        Free        Size\n");
  printf("  [vms$common.sysexe]pagefile.sys                                      \n");
#if 0
  printf("                                                 254       21660       25080\n");
  printf("  Total committed paging file usage:                                   12653\n");
#endif
  printf("\n");
 skip:
#if 0
  printf("Of the physical pages in use, 4825 pages are permanently allocated to FreeVMS.\n");
  printf("\n");
#endif
 fflush(stdout);

}
