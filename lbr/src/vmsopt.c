/*
 * VMS Command Line Parsing
 * FreeVMS Project
 *
 * (C) 1999 by Noah Paul
 * To be distributed only under the GNU GPL. No warranty.
 *
 * Expanded a bit by Roar Thronæs
 */

#include <string.h>
#include "vmsopt.h"

/*
 * Parse VMS Options.
 *
 * !WARNING! This mangles argc and argv's option arguments (beginning with /),
 *	     but others are safe.
 *
 */
int vmsopt_parse( int argc, char **argv )
{
	extern
	char 	*vms_optstr,
		*vms_optarg;	
	char	*ptr;
	int i;

	
	for( i = 1; i < argc; i++ )
	{

		if( argv[i][0] == 0 )
			continue;

		if( argv[i][0] != '/' ) {
			vms_optstr = (char*) malloc ( strlen(argv[i]) + 1);
			strcpy( vms_optstr, argv[i] );
//			vms_optstr++;
			argv[i][0]=0;
			vms_optarg = NULL;
			return(VMSOPT_ARG_PL);
		}
			
		if( strchr( argv[i], ':' ) == NULL )
		{
			vms_optstr = (char*) malloc ( strlen(argv[i]) + 1 );
			strcpy( vms_optstr, argv[i] );
			vms_optstr++; 
			argv[i][0] = 0;
			vms_optarg = NULL;
			return(VMSOPT_NOARG);
		}		

		vms_optarg = (char*) malloc( strlen(strchr( argv[i], ':' ))  + 1 );
		vms_optstr = (char*) malloc( ( strlen(argv[i] -
					     strlen(strchr( argv[i], ':')) )) );
		strcpy( vms_optarg, strchr( argv[i], ':' ) );
		vms_optarg++; 
		ptr = strchr(argv[i],':');		 
		ptr[0] = 0;  
		strcpy( vms_optstr, argv[i] );
		argv[i][0] = 0;
		vms_optstr++;
		return(VMSOPT_ARG_CO);
	}

	return(VMSOPT_DONE);
}

