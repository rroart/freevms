/* This is the version of main() for DCL-style option parsing.  */

/* FIXME: Probably should be using init_cli from the VERB distribution
   so that we can be defined as a foreign command as well as by SET
   COMMAND.  */

#include <stdio.h>
#include <stdlib.h>
#include <descrip.h>
#include <string.h>
#include "vmsbackup.h"
#include "sysdep.h"

/* FIXME: Is there a header file for any of the following?  */
extern int cli$get_value();
extern int cli$present();

extern int sys$asctim ();

int
main (argc, argv)
int argc;
char *argv[];
{
	static $DESCRIPTOR (file1,"P1");
	static $DESCRIPTOR (file2,"P2");
	/* Is the /SAVE_SET qualifier specified with P1 and P2,
	   respectively.  */
	int p1_saveset = 0;
	int p2_saveset = 0;
	/* Was P2 specified?  */
	int p2_specified;

	static $DESCRIPTOR (q_saveset, "SAVE_SET");
	static $DESCRIPTOR (q_list, "LIST");
	static $DESCRIPTOR (q_full, "FULL");
	static $DESCRIPTOR (q_brief, "BRIEF");
	static struct dsc$descriptor result =
		{0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};

	/* We don't yet have a mechanism to select only some files from
	   a save set.  */
	gargv = NULL;
	gargc = 0;
	goptind = 0;

	cflag=dflag=eflag=sflag=tflag=vflag=wflag=xflag=0;
	flag_full = 0;
	if (cli$get_value (&file1, &result) & 1) {
		if (cli$present (&q_saveset) & 1) {
			p1_saveset = 1;
		}
		tapefile = malloc (result.dsc$w_length + 1);
		if (tapefile == NULL) {
			fprintf (stderr, "out of memory!\n");
			exit (EXIT_FAILURE);
		}
		strncpy (tapefile, result.dsc$a_pointer, result.dsc$w_length);
		tapefile[result.dsc$w_length] = '\0';
	}
	if (cli$get_value (&file2, &result) & 1) {
		if (cli$present (&q_saveset) & 1) {
			p2_saveset = 1;
		}
		if (result.dsc$w_length != 2
		    || result.dsc$a_pointer[0] != '['
		    || result.dsc$a_pointer[1] != ']') {
			fprintf (stderr,
				 "error: extraction only supported to []\n");
			exit (EXIT_FAILURE);
		}
		p2_specified = 1;
	}
	if (cli$present (&q_list) & 1) {
		tflag = 1;
	}
	if (cli$present (&q_full) & 1) {
		flag_full = 1;
	}
	if (cli$present (&q_brief) & 1) {
		/* /BRIEF overrides /FULL.  */
		flag_full = 0;
	}

	/* Note that our processing of the parameters, /SAVE_SET, etc.
	   differs somewhat from BACKUP.  In some cases this perhaps
	   should be changed, but in other cases I'm not sure the
	   way BACKUP does it is very good.  For example, did the
	   user who typed "BACKUP [-]TESTB.BCK []" really mean to
	   copy the saveset intact?  Seems to me that an error is more
	   useful in that case.  */

	/* If P1 or P2 refers to a tape device, we should be setting
	   p1_saveset or p2_saveset.  But this is not yet implemented
	   (FIXME).  */
	if (!p1_saveset) {
		fprintf (stderr, "error: must specify /SAVE_SET\n");
		exit (EXIT_FAILURE);
	}
	if (p2_saveset) {
		fprintf (stderr, "error: creating savesets not implemented\n");
		exit (EXIT_FAILURE);
	}
	xflag = !tflag;
	if (tflag && p2_specified) {
		fprintf
		  (stderr,
		   "error: must supply two parameters when extracting\n");
		exit (EXIT_FAILURE);
	}
	if (xflag && !p2_specified) {
		fprintf
		  (stderr,
		   "error: must supply one parameter with /LIST\n");
		exit (EXIT_FAILURE);
	}
	vmsbackup ();
}

/* The following is code for VMS systems which isn't related to main()
   or option parsing.  It should perhaps be part of a separate file
   (depending, of course, on things like whether anyone ever feels like
   separating the two concepts).  */

/* Given an 8-byte VMS-format date (little-endian) in SRCTIME, put an
   ASCII representation in *ASCBUFFER and put the length in *ASCLENGTH.
   ASCBUFFER must be big enough for 23 characters.
   Returns: condition code.  */

int
time_vms_to_asc (asclength, ascbuffer, srctime)
    short *asclength;
    char *ascbuffer;
    void *srctime;
{
    struct dsc$descriptor buffer;

    buffer.dsc$w_length = 23;
    buffer.dsc$b_dtype = DSC$K_DTYPE_T;
    buffer.dsc$b_class = DSC$K_CLASS_S;
    buffer.dsc$a_pointer = ascbuffer;
    return sys$asctim (asclength, &buffer, srctime, 0);
}
