/* <rms$routines.h>
 *
 *	RMS routines from Starlet.
 */
#ifndef _RMS$ROUTINES_H
#define _RMS$ROUTINES_H
# ifdef __cplusplus
extern "C" {
# endif
	/* part of <starlet.h> */



unsigned long sys$close();


unsigned long sys$connect();


unsigned long sys$create();


unsigned long sys$delete();


unsigned long sys$disconnect();


unsigned long sys$display();


unsigned long sys$enter();


unsigned long sys$erase();


unsigned long sys$extend();


unsigned long sys$find();


unsigned long sys$flush();


unsigned long sys$free();


unsigned long sys$get();


unsigned long sys$modify();


unsigned long sys$nxtvol();


unsigned long sys$open();


unsigned long sys$parse();


unsigned long sys$put();


unsigned long sys$read();


unsigned long sys$release();


unsigned long sys$remove();


unsigned long sys$rename();


unsigned long sys$rewind();


unsigned long sys$rundwn();


unsigned long sys$search();


unsigned long sys$space();


unsigned long sys$truncate();


unsigned long sys$update();


unsigned long sys$wait();


unsigned long sys$write();

	


unsigned long sys$rmsrundwn(void *,int);	


unsigned long sys$setddir(const void *,unsigned short *,void *);


unsigned long sys$setdfprot(const unsigned short *,unsigned short *);

# ifdef __cplusplus
}
# endif
#endif	
