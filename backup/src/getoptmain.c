/* This is the version of main() for POSIX.2 (getopt) style arguments.  */

char *version = "VMSBACKUP4.1";

#include <stdio.h>
#include <stdlib.h>
#include "vmsbackup.h"
#include "getopt.h"

void
usage (progname)
char *progname;
{
	fprintf (stderr, "\
Usage:  %s -{tx}[cdevwF][-b blocksize][-s setnumber][-f tapefile]\n",
		 progname);
}

extern int optind;
extern char *optarg;

int
main (argc, argv)
int argc;
char *argv[];
{
	char *progname;
	int c;

	progname = argv[0];
	if(argc < 2){
		usage(progname);
		exit(1);
	}

	gargv = argv;
	gargc = argc;

	cflag=dflag=eflag=sflag=tflag=vflag=wflag=xflag=0;
	flag_binary = 0;
	flag_full = 0;
	tapefile = NULL;

	while((c=getopt(argc,argv,"b:cdef:s:tvwxFVB")) != EOF)
		switch(c){
		case 'b':
			sscanf (optarg, "%d", &blocksize);
			break;
		case 'c':
			cflag++;
			break;
		case 'd':
			dflag++;
			break;
		case 'e':
			eflag++;
			break;
		case 'f':
			tapefile = optarg;
			break;
		case 's':
			sflag++;
			sscanf(optarg,"%d",&selset);
			break;
		case 't':
			tflag++;
			break;
		case 'v':
			vflag++;
			break;
		case 'w':
			wflag++;
			break;
		case 'x':
			xflag++;
			break;
		case 'F':
			/* I'd actually rather have this be --full, but at
			   the moment I don't feel like worrying about
			   infrastructure for parsing long arguments.  I
			   don't like the GNU getopt_long--the interface
			   is noreentrant and generally silly; and it might
			   be nice to have something which synergizes more
			   closely with the VMS options (that one is a bit
			   of a can of worms, perhaps, though) like parseargs
			   or whatever it is called.  */
			flag_full = 1;
			break;
		case 'V':
			printf ("VMSBACKUP version %s\n", version);
			exit (EXIT_FAILURE);
			break;
		case 'B':
			/* This of course should be --binary; see above
			   about long options.  */
			flag_binary = 1;
			break;
		case '?':
			usage(progname);
			exit(1);
			break;
		};
	goptind = optind;
	if(!tflag && !xflag) {
		usage(progname);
		exit(1);
	}
	vmsbackup ();
}

/* The following is code for non-VMS systems which isn't related to main()
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
    /* Not currently implemented, although it would be nice to.  */
    *asclength = 0;
    return 1;
}
