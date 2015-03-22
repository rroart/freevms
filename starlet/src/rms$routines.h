/* <rms$routines.h>
 *
 *  RMS routines from Starlet.
 */
#ifndef _RMS$ROUTINES_H
#define _RMS$ROUTINES_H
# ifdef __cplusplus
extern "C"
{
# endif
    /* part of <starlet.h> */

    unsigned long sys$close(void);

    unsigned long sys$connect(void);

    unsigned long sys$create(void);

    unsigned long sys$delete(void);

    unsigned long sys$disconnect(void);

    unsigned long sys$display(void);

    unsigned long sys$enter(void);

    unsigned long sys$erase(void);

    unsigned long sys$extend(void);

    unsigned long sys$find(void);

    unsigned long sys$flush(void);

    unsigned long sys$free(void);

    unsigned long sys$get(void);

    unsigned long sys$modify(void);

    unsigned long sys$nxtvol(void);

    unsigned long sys$open(void);

    unsigned long sys$parse(void);

    unsigned long sys$put(void);

    unsigned long sys$read(void);

    unsigned long sys$release(void);

    unsigned long sys$remove(void);

    unsigned long sys$rename(void);

    unsigned long sys$rewind(void);

    unsigned long sys$rundwn(void);

    unsigned long sys$search(void);

    unsigned long sys$space(void);

    unsigned long sys$truncate(void);

    unsigned long sys$update(void);

    unsigned long sys$wait(void);

    unsigned long sys$write(void);

    unsigned long sys$rmsrundwn(void *, int);

    unsigned long sys$setddir(const void *, unsigned short *, void *);

    unsigned long sys$setdfprot(const unsigned short *, unsigned short *);

# ifdef __cplusplus
}
# endif
#endif
