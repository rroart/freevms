/* Direct.h v1.3    Definitions for directory access routines */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.
*/

unsigned direct(struct _vcb *vcb,struct dsc$descriptor *fibdsc,
                struct dsc$descriptor *filedsc,unsigned short *reslen,
                struct dsc$descriptor *resdsc, struct _atrdef * atrp, unsigned action, struct _irp * i);
