/* 
 * Copyright (C) 2000, 2001  Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef _UM_UNISTD_H_
#define _UM_UNISTD_H_

#include "/usr/include/asm/unistd.h"

#undef __NR_$testtest          
#undef __NR_$setpri		
#undef __NR_$crelnt            
#undef __NR_$setprn            
#undef __NR_$dclast            
#undef __NR_$waitfr            
#undef __NR_$wflor             
#undef __NR_$wfland            
#undef __NR_$clref             
#undef __NR_$setime           
#undef __NR_$setimr           
#undef __NR_$cantim           
#undef __NR_$numtim            /* really in exec mode */
#undef __NR_$gettim           
#undef __NR_$hiber            
#undef __NR_$wake             
#undef __NR_$schdwk           
#undef __NR_$canwak           
#undef __NR_$suspnd           
#undef __NR_$resume           
#undef __NR_$exit             
#undef __NR_$forcex            
#undef __NR_$setrwm           
#undef __NR_$delprc           
#undef __NR_$readef           
#undef __NR_$setef            
#undef __NR_$synch            
#undef __NR_$enq              
#undef __NR_$deq              
#undef __NR_$assign          
#undef __NR_$dassgn          
#undef __NR_$qio             
#undef __NR_$qiow            
#undef __NR_$getlki          
#undef __NR_$getlkiw         
#undef __NR_$enqw            
#undef __NR_$crelnm          
#undef __NR_$trnlnm          
#undef __NR_$dellnm          

#undef __NR_$close 
#undef __NR_$connect 
#undef __NR_$create 
#undef __NR_$delete 
#undef __NR_$disconnect 
#undef __NR_$display 
#undef __NR_$enter 
#undef __NR_$erase 
#undef __NR_$extend 
#undef __NR_$find 
#undef __NR_$flush 
#undef __NR_$free 
#undef __NR_$get 
#undef __NR_$modify 
#undef __NR_$nxtvol 
#undef __NR_$open 
#undef __NR_$parse 
#undef __NR_$put 
#undef __NR_$read 
#undef __NR_$release 
#undef __NR_$remove 
#undef __NR_$rename 
#undef __NR_$rewind 
#undef __NR_$search 
#undef __NR_$space 
#undef __NR_$truncate 
#undef __NR_$update 
#undef __NR_$wait 
#undef __NR_$write 
#undef __NR_$filescan 
#undef __NR_$setddir 
#undef __NR_$setdfprot 
#undef __NR_$ssvexc 
#undef __NR_$rmsrundwn 

#undef __NR_$asctim            
#undef __NR_$bintim            

#define __NR_$testtest         238
#define __NR_$setpri	239
#define __NR_$crelnt        240   
#define __NR_$setprn           241
#define __NR_$dclast           242
#define __NR_$waitfr           243
#define __NR_$wflor            245
#define __NR_$wfland           246
#define __NR_$clref            247
#define __NR_$setime          248
#define __NR_$setimr          249
#define __NR_$cantim          250
#define __NR_$numtim       251
#define __NR_$gettim          252
#define __NR_$hiber           253
#define __NR_$wake            254
#define __NR_$schdwk          255
#define __NR_$canwak          256
#define __NR_$suspnd          257
#define __NR_$resume          258
#define __NR_$exit            259
#define __NR_$forcex 260
#define __NR_$setrwm    261      
#define __NR_$delprc       262   
#define __NR_$readef          263
#define __NR_$setef           264
#define __NR_$synch           265
#define __NR_$enq             266
#define __NR_$deq             303

#define __NR_$close 267
#define __NR_$connect 268
#define __NR_$create 269
#define __NR_$delete 270
#define __NR_$disconnect 271 
#define __NR_$display 272
#define __NR_$enter 273
#define __NR_$erase 274
#define __NR_$extend 275
#define __NR_$find 276
#define __NR_$flush 277
#define __NR_$free 278
#define __NR_$get 279
#define __NR_$modify 280
#define __NR_$nxtvol 281
#define __NR_$open 282
#define __NR_$parse 283
#define __NR_$put 284
#define __NR_$read 285
#define __NR_$release 286
#define __NR_$remove 287
#define __NR_$rename 288
#define __NR_$rewind 289
#define __NR_$search 290
#define __NR_$space 291
#define __NR_$truncate 292
#define __NR_$update 293
#define __NR_$wait 294
#define __NR_$write 295
#define __NR_$filescan 296
#define __NR_$setddir 297
#define __NR_$setdfprot 298
#define __NR_$ssvexc 299
#define __NR_$rmsrundwn 300

#define __NR_$asctim       301    
#define __NR_$bintim          302 

#define __NR_$assign 306
#define __NR_$dassgn 304
#define __NR_$qio 305
#define __NR_$qiow 307
#define __NR_$getlki 308
#define __NR_$getlkiw 309
#define __NR_$enqw 310
#define __NR_$crelnm 311
#define __NR_$trnlnm 312
#define __NR_$dellnm 313

#endif

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
