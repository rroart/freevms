/*
	****************************************************************

		Copyright (c) 1992, Carnegie Mellon University

		All Rights Reserved

	Permission  is  hereby  granted   to  use,  copy,  modify,  and
	distribute  this software  provided  that the  above  copyright
	notice appears in  all copies and that  any distribution be for
	noncommercial purposes.

	Carnegie Mellon University disclaims all warranties with regard
	to this software.  In no event shall Carnegie Mellon University
	be liable for  any special, indirect,  or consequential damages
	or any damages whatsoever  resulting from loss of use, data, or
	profits  arising  out of  or in  connection  with  the  use  or
	performance of this software.

	****************************************************************
*/
#include stdio
#include acldef
#include descrip
#include psldef
#include armdef
#include chpdef

main()
{
    unsigned long objtyp, access, acmode, flags;
    struct dsc$descriptor objnam, usrnam;
    struct {
        unsigned short buflen;
	unsigned short itmcod;
	unsigned long bufadr;
	unsigned long retlenadr;
    } itmlst[4];

    objtyp = ACL$C_FILE;

    objnam.dsc$a_pointer = "SD1:[000000]CMU_IP.DIR";
    objnam.dsc$w_length = strlen(objnam.dsc$a_pointer);
    objnam.dsc$b_dtype = DSC$K_DTYPE_T;
    objnam.dsc$b_class = DSC$K_CLASS_S;

    usrnam.dsc$a_pointer = "SYNFUL";
    usrnam.dsc$w_length = strlen(usrnam.dsc$a_pointer);
    usrnam.dsc$b_dtype = DSC$K_DTYPE_T;
    usrnam.dsc$b_class = DSC$K_CLASS_S;

    access = ARM$M_WRITE;
    acmode = PSL$C_USER;
    flags = CHP$M_READ;

    itmlst[0].buflen = sizeof(access);
    itmlst[0].itmcod = CHP$_ACCESS;
    itmlst[0].bufadr = &access;
    itmlst[0].retlenadr = 0;
    itmlst[1].buflen = 0;
    itmlst[1].itmcod = 0;
    itmlst[1].bufadr = 0;
    itmlst[1].retlenadr = 0;

    printf("SYS$CHECK_ACCESS returns %x\n", 
        SYS$CHECK_ACCESS(&objtyp, &objnam, &usrnam, &itmlst));
}
