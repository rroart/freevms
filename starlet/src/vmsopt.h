#ifndef __FREE_VMS_VMSOPT_VMSOPT_H
#define __FREE_VMS_VMSOPT_VMSOPT_H

	#define VMSOPT_DONE		-1 
	#define VMSOPT_NOARG		1
	#define VMSOPT_ARG_EQ		2 
	#define VMSOPT_ARG_CO		3 
	#define VMSOPT_ARG_PL		4 

	char 	*vms_optstr;
	char	*vms_optarg;
#endif
