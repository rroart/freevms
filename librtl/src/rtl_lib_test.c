/*
 *	rtl_lib_test
 *
 *	Copyright (C) 2003 Andrew Allison
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	The authors may be contacted at:
 *
 *	Andrew Allison		freevms@sympatico.ca
 *
 *				Andrew Allison
 *				50 Denlaw Road
 *				London, Ont
 *				Canada 
 *				N6G 3L4
 *
 * 	Kevin Handy		Unknown
 *
 *	Christof Zeile		Unknown
 *
 * rtltest.c
 *
 *	Test program for run time libraries functions
 *
 * Notes:
 *
 *	This program should be run using the Free-VMS packages
 *	'str$' functions, Vax/VMS's, and Alpha/VMS's to make
 *	sure they all happily work the same.
 *
 * History:
 *
 *
 *	Mar 16, 2004 - Andrew Allison
 *		Initial program creation
 *
 */
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "descrip.h"
#include "strdef.h"
#include "ssdef.h"
#include "rmsdef.h"
#include "str$routines.h"
#include "sys$routines.h"
#include "lib$routines.h"
#include <fabdef.h>

/***********************************************/
/*
 * Prototypes
 */

void	print_result_code(char *presult, unsigned long result_code,char *cont);
void 	print_desc_compare(	char *title,
				struct dsc$descriptor_s *s1,
				struct dsc$descriptor_s *s2,
				char *cont);
void print_lu_compare(char*title,unsigned long v1,unsigned long v2,char *cont);
void print_ls_compare	(char *title,long v1,long v2,char *cont);
void print_int_compare	(char *title,int v1,int v2,char *cont);

/*************************************************************/
/*
 *	local test function prototypes
 */

int testlibadawi       		(FILE *fptr, int *fstatus, char *cont);
int testlibaddx 		(FILE *fptr, int *fstatus, char *cont);
int testlibadd_times		(FILE *fptr, int *fstatus, char *cont);
int testlibanalyze_sdesc	(FILE *fptr, int *fstatus, char *cont);
int testlibanalyze_sdesc_64	(FILE *fptr, int *fstatus, char *cont);
int testlibasn_wth_mbx		(FILE *fptr, int *fstatus, char *cont);
int testlibast_in_prog		(FILE *fptr, int *fstatus, char *cont);
int testlibattach		(FILE *fptr, int *fstatus, char *cont);
int testlibbuild_nodespec	(FILE *fptr, int *fstatus, char *cont);
int testlibcallg		(FILE *fptr, int *fstatus, char *cont);
int testlibcallg_64		(FILE *fptr, int *fstatus, char *cont);
int testlibchar			(FILE *fptr, int *fstatus, char *cont);
int testlibcompare_nodename	(FILE *fptr, int *fstatus, char *cont);
int testlibcompress_nodename	(FILE *fptr, int *fstatus, char *cont);
int testlibconvert_date_string	(FILE *fptr, int *fstatus, char *cont);
int testlibcrc			(FILE *fptr, int *fstatus, char *cont);
int testlibcrc_table		(FILE *fptr, int *fstatus, char *cont);
int testlibcreate_dir		(FILE *fptr, int *fstatus, char *cont);
int testlibcreate_user_vm_zone	(FILE *fptr, int *fstatus, char *cont);
int testlibcreate_user_vm_zone_64	(FILE *fptr, int *fstatus, char *cont);
int testlibcreate_vm_zone	(FILE *fptr, int *fstatus, char *cont);
int testlibdate_time     	(FILE *fptr, int *fstatus, char *cont);
int testlibdelete_file         	(FILE *fptr, int *fstatus, char *cont);
int testlibdelete_logical      	(FILE *fptr, int *fstatus, char *cont);
int testlibdelete_symbol	(FILE *fptr, int *fstatus, char *cont);
int testlibdelete_vm_zone	(FILE *fptr, int *fstatus, char *cont);
int testlibdelete_vm_zone_64	(FILE *fptr, int *fstatus, char *cont);
int testlibdigit_sep		(FILE *fptr, int *fstatus, char *cont);
int testlibdisable_ctrl		(FILE *fptr, int *fstatus, char *cont);
int testlibdo_command		(FILE *fptr, int *fstatus, char *cont);
int testlibenable_ctrl		(FILE *fptr, int *fstatus, char *cont);
int testlibediv			(FILE *fptr, int *fstatus, char *cont);
int testlibestablish  		(FILE *fptr, int *fstatus, char *cont);
int testlibexpand_nodename	(FILE *fptr, int *fstatus, char *cont);
int testlibfid_to_name		(FILE *fptr, int *fstatus, char *cont);
int testlibfile_scan  		(FILE *fptr, int *fstatus, char *cont);
int testlibfile_scan_end	(FILE *fptr, int *fstatus, char *cont);
int testlibfind_file    	(FILE *fptr, int *fstatus, char *cont);
int testlibfind_file_end    	(FILE *fptr, int *fstatus, char *cont);
int testliblocc			(FILE *fptr, int *fstatus, char *cont);
int testlibmatchc		(FILE *fptr, int *fstatus, char *cont);
int testlibmovc3 		(FILE *fptr, int *fstatus, char *cont);
int testlibwait			(FILE *fptr, int *fstatus, char *cont);


int	get_fun_num (char *functionname);
void	call_test_function (int which_fun,char *fname, FILE *fptr,char *cont);

/*
 * Some local constants
 */
int flistsize;
#define scan1  "%s\n"
#define scan2  "%s%s\n"
#define scan3  "%s%s%s\n"
#define scan4  "%s%s%s%s\n"
#define scan5  "%s%s%s%s%s\n"
#define scan6  "%s%s%s%s%s%s\n"
#define scan7  "%s%s%s%s%s%s%s\n"
#define scan8  "%s%s%s%s%s%s%s%s\n"
#define scan9  "%s%s%s%s%s%s%s%s%s\n"
#define scan11 "%s%s%s%s%s%s%s%s%s%s%s\n"
#define scan17 "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"
#define scan19 "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"
#define scan20 "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"
#define scan21 "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"
#define scan23 "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"

#define FBUFSIZE  255
#define MAX_ARRAY 50
#define MAXSTRING 255
#define FNameSize 100


/*
 *	This structure matches a character string
 *	to it's corresponding test function 
 *
 */
struct fliststruct{char *fname; int (*ptr_fun)(); }
flist[]={
{"lib$adawi",  			&testlibadawi},
{"lib$addx",			&testlibaddx},
{"lib$add_times",		&testlibadd_times},
{"lib$analyze_sdesc",		&testlibanalyze_sdesc},
{"lib$analyze_sdesc_64",	&testlibanalyze_sdesc_64},
#if 0
{"lib$asn_wth_mbx",		&testlibasn_wth_mbx},
{"lib$ast_in_prog",		&testlibast_in_prog},
{"lib$attach",			&testlibattach},
#endif
{"lib$build_nodespec",		&testlibbuild_nodespec},
#if 0
{"lib$callg",			&testlibcallg},
{"lib$callg_64",		&testlibcallg_64},
#endif
{"lib$char",			&testlibchar},
#if 0
{"lib$compare_nodename",	&testlibcompare_nodename},
{"lib$compress_nodename",	&testlibcompress_nodename},
#endif
{"lib$convert_date_string",	&testlibconvert_date_string},
#if 0
{"lib$crc",			&testlibcrc},
{"lib$crc_table",		&testlibcrc_table},
{"lib$create_dir",		&testlibcreate_dir},
{"lib$create_user_vm_zone",	&testlibcreate_user_vm_zone},
{"lib$create_user_vm_zone_64",	&testlibcreate_user_vm_zone_64},
{"lib$create_vm_zone",		&testlibcreate_vm_zone},
#endif
{"lib$date_time",		&testlibdate_time},
#if 0
{"lib$delete_file",		&testlibdelete_file},
{"lib$delete_logical",		&testlibdelete_logical},
{"lib$delete_symbol",		&testlibdelete_symbol},
{"lib$delete_vm_zone",		&testlibdelete_vm_zone},
{"lib$delete_vm_zone_64",	&testlibdelete_vm_zone_64},
#endif
{"lib$digit_sep",		&testlibdigit_sep},
#if 0
{"lib$disable_ctrl",		&testlibdisable_ctrl},
{"lib$do_command",		&testlibdo_command},
#endif
{"lib$ediv",			&testlibediv},
#if 0
{"lib$enable_ctrl",		&testlibenable_ctrl},
{"lib$establish",		&testlibestablish},
{"lib$expand_nodename",		&testlibexpand_nodename},
{"lib$fid_to_name",		&testlibfid_to_name},
{"lib$file_scan",		&testlibfile_scan},
{"lib$file_scan_end",		&testlibfile_scan_end},
{"lib$find_file",		&testlibfind_file},
{"lib$find_file_end",		&testlibfind_file},
#endif
{"lib$locc",			&testliblocc},
{"lib$matchc",			&testlibmatchc},
{"lib$movc3",			&testlibmovc3},
{"lib$wait",			&testlibwait}
};


/*************** MAIN *************************/

int main(void)
{

int	line_ctr;
int	which_fun,i,j;
char  	functionname[FNameSize],junk[10],cont;
FILE 	*fptr;

cont = 'P';
#if i386
	system ("clear");
#endif

	fptr = fopen ("test_lib_rtl.dat","r");
	if (fptr == NULL )
	{	printf ("Could not open data file test_lib_rtl.dat \n");
		return 0;
	}


	printf ("\n\n\n\t\t\tFreeVMS Runtime Library Tester\n\n\n");
	line_ctr = 0;

	printf ("Enter Function Name :");
	scanf  ("%s",functionname);

//	get rid of <CR>  some garbage left when changing input modes
	fgets(junk,2,stdin);

//	lower case input
	for (i=0; i < strlen(functionname); i++ )
	{	functionname[i] = tolower(functionname[i]);
	}


	which_fun = get_fun_num(functionname);

	if ( functionname[0] == '*' )
	{
		flistsize = sizeof(flist) / sizeof(flist[0]);
		for (j=0; j < flistsize; j++ )
		{
			strcpy (functionname,flist[j].fname);
			call_test_function(j,flist[j].fname,fptr,&cont);
		}
	}

	if (which_fun != 9999 ) // valid function name
	{
		call_test_function(which_fun,functionname,fptr,&cont);
	}
	else
	{	printf ("\n\n");
		printf ("Routine %s NOT FOUND \n",functionname);
		printf ("\n");
		printf ("Use a valid routine name  Example str$left \n");
		printf ("\n\n");
	}

	fclose (fptr);
printf ("\n\nEnd Program \n\n\n");
return 0;
}

/********************************************/

int	get_fun_num (char *functionname)
{
int	i,result;

	result = 9999;

	flistsize = sizeof(flist) / sizeof(flist[0]);
	for (i=0; i < flistsize; i++)
	{
		if ((strcmp (flist[i].fname,functionname)) == 0)
		{
			result = i;
		}
	}
	return result;
}


/********************/
void call_test_function (int which_fun,char *fname, FILE *fptr, char *cont)
{
char	fbuffer[FBUFSIZE];
char	*fstatus;


fstatus = fbuffer; // kluge are way into while statment
rewind (fptr);
while ( fstatus != NULL ) 
{
//	read in routine name
	fstatus = fgets (fbuffer,FBUFSIZE,fptr);
	if ( fstatus != NULL )
	{
		if ((strncmp(fbuffer,fname,strlen(fname)) == 0 ) &&
			( strlen(fbuffer)-1) == strlen (fname) )	
		{
#if i386
			system ("clear");
#endif
// 			read in comment
			fstatus = fgets (fbuffer,FBUFSIZE,fptr);
			printf ("Comment %s \n",fbuffer);
			flist[which_fun].ptr_fun(fptr,&*fstatus, cont) ;
			if ( *cont == 'P') 
			{	printf ("\n\n");
				printf ("Enter to continue or N for Nonstop :");
				scanf ("%c",cont);
				if ( *cont != 'N' )
					*cont = 'P';
			}
		} // if strcmp
		else	
		{	// ignore parameter line
			fstatus = fgets(fbuffer,FBUFSIZE,fptr);
			fstatus = fgets(fbuffer,FBUFSIZE,fptr);
		}
	} 		// if not NULL
} 	/* end while */

return;
}


/********************************************/

void print_result_code (char *presult, unsigned long result_code,char *cont)
{
char	errmsg[80];
unsigned short int	errmsglen;
unsigned int	flags = 3;

if ( sys$$geterrno(presult) != result_code)
{
	sys$$getmsg(result_code,&errmsglen,errmsg,flags);
	printf ("FAILED - Unexpected Result Code \n");
	printf ("Expected %s result code \n",presult);
	printf ("Result code is %lu %s \n\n",result_code,errmsg);
	*cont = 'P';
}
else
{
	printf ("Result Code .....OK ");
	sys$$getmsg(result_code,&errmsglen,errmsg,flags);
	printf (" %lu %s \n",result_code,errmsg);
}
}

/********************************************/

void print_desc_compare (	char *title,
				struct dsc$descriptor_s *s1,
				struct dsc$descriptor_s *s2,
				char *cont)
{
if ( str$compare (s1,s2) != 0)
{	printf ("FAILED - %10s\n",title);
	*cont = 'P';
}
else
	printf ("%-10s ......OK \n",title);

}
/********************************************/

void print_lu_compare(char *title,unsigned long v1,unsigned long v2,char *cont)
{
if ( v1 != v2 )
{	printf ("FAILED - %10s\n",title);
	*cont = 'P';
}
else
	printf ("%-10s ......OK \n",title);

}
/********************************************/

void print_ls_compare (	char *title, long v1, long v2, char *cont)
{
if ( v1 != v2 )
{	printf ("FAILED - %10s\n",title);
	*cont = 'P';
}
else
	printf ("%-10s ......OK \n",title);

}
/********************************************/

void print_int_compare ( char *title, int v1, int v2, char *cont)
{
if ( v1 != v2 )
{	printf ("FAILED - %10s\n",title);
	*cont = 'P';
}
else
	printf ("%-10s ......OK \n",title);

}


/********************************************/

int	testlibadawi(FILE *fptr,int *fstatus, char *cont)
{

	printf ("Add with interlock not implemented \n");

	*cont = 'P';
	return 0;
}


/********************************************/

int	testlibaddx (FILE *fptr, int *fstatus, char *cont)
{

int	i;
unsigned long	result_code;
signed   long	array1_size, array2_size, array3_size, array_length;
unsigned long	array1[50], array2[50], array3[50];
char		pt1[10], pv1[10], presult[30];

//	First array of numbers
	*fstatus = fscanf (fptr,scan1,pv1);
	array1_size = atol (pv1);
	for ( i = array1_size-1; i >= 0; i-- )
		*fstatus = fscanf (fptr,"%lX",&array1[i]);

//	Second array of numbers
	*fstatus = fscanf (fptr,scan1,pv1);
	array2_size = atol (pv1);
	for ( i = array2_size-1; i >= 0; i-- )
		*fstatus = fscanf (fptr,"%lX",&array2[i]);

//	Array of sums
	*fstatus = fscanf (fptr,scan1,pv1);
	array3_size = atol (pv1);
	for ( i = array3_size-1; i >= 0; i-- )
		*fstatus = fscanf (fptr,"%lX",&array3[i]);

//	Size of array
	*fstatus = fscanf (fptr,scan2,pt1,pv1);
	array_length = atol (pv1);

//	Expected return status
	*fstatus = fscanf (fptr,scan1,presult);


	printf ("Testing LIB$ADDX \n");
	printf ("Add two arrays of long words \n\n\n");

	printf ("Input   Value 1     ");
	for ( i=array1_size-1; i >= 0; i-- )
			printf ("%.8lX ",array1[i]);
	printf ("\n");

	printf ("Input   Value 2     ");
	for ( i=array2_size-1; i >= 0; i-- )
			printf ("%.8lX ",array2[i]);
	printf ("\n");
	printf ("Input   Array Size  %ld \n",array_length);

	printf ("Input   Sum         ");
	for ( i=array3_size-1; i >= 0; i-- )
			printf ("%.8lX ",array3[i]);
	printf ("\n\n");

	result_code = lib$addx (array1, array2, array3, &array_length);

	print_result_code  (presult,result_code,cont);
	printf ("\n\n");

	printf ("Output  Value 1     ");
	for ( i=array1_size-1; i >= 0; i-- )
			printf ("%.8lX ",array1[i]);
	printf ("\n");

	printf ("Output  Value 2     ");
	for ( i=array2_size-1; i >= 0; i-- )
			printf ("%.8lX ",array2[i]);
	printf ("\n");

	printf ("Output  Array Size  %ld \n",array_length);
	printf ("\n");

	printf ("Output  Sum         ");
	for ( i=array3_size-1; i >= 0; i-- )
			printf ("%.8lX ",array3[i]);
	printf ("\n");

	return result_code;
}

/******************************************************/

int	testlibadd_times(FILE *fptr,int *fstatus, char *cont)

{
unsigned long result_code;
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10],presult[50];
char	time1[8], time2[8], time_result[8];

	*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

	printf ("Testing lib$add_times \n");
	printf ("Add two times \n\n\n");

	printf("Input Time   A   %s\n",pv1);
	printf("Input Time   B   %s\n",pv2);
	printf ("\n");

	result_code = lib$add_times(&time1,&time2,&time_result);

	print_result_code (presult, result_code,cont);

	printf ("\n\n");
	printf ("Output Time   A \n"); 
	printf ("Output Time   B \n"); 

	return (int) result_code;
}


/********************************************/

int	testlibanalyze_sdesc (FILE *fptr, int *fstatus, char *cont)
{
	unsigned long result_code;
	char 	*s1_ptr;			/* Pointer to first string */
	char 	pt1[10], pv1[10];
	char	presult [20];
	struct	dsc$descriptor_s s1,sr;
	unsigned short s1_length;		/* Length of first string */
	int	in_length;

	*fstatus = fscanf (fptr,scan3,pt1,pv1,presult);

	if (strncmp (pt1,"cd",2) == 0)	str$$malloc_sd (&s1,pv1);
	if (strncmp (pt1,"cd",2) == 0)	str$$malloc_sd (&sr,pv1);

	if ( strncmp ("NULL",pv1,4) == 0 )
		in_length = 0;
	else if ( strncmp ("MAXCSTR16",pv1,9) == 0 )
		in_length = 65535;
	else
		in_length = strlen (pv1);


	printf ("Testing str$analyze_sdesc \n");
	printf ("Return a pointer to character string and it's length\n\n");

	printf ("Input   Value    ");	str$$print_sd (&s1);	printf ("\n");
	printf ("Input   Length   %d \n",in_length);
	printf ("\n");

	result_code = lib$analyze_sdesc(&s1, &s1_length, &s1_ptr);

	print_result_code  (presult,result_code,cont);
	print_int_compare  ("Type", DSC$K_DTYPE_T, s1.dsc$b_dtype, cont);
	print_int_compare  ("Class",DSC$K_CLASS_D, s1.dsc$b_class, cont);
	print_int_compare  ("Length",in_length,s1_length,cont);
	print_desc_compare ("String",&sr,&s1,&*cont);

	printf ("\n\n");
	printf ("Output  Type     %d\n", s1.dsc$b_dtype);
	printf ("Output  Class    %d\n", s1.dsc$b_class);
	printf ("Output  Length   %d\n", s1_length);
	printf ("Output  String   "); str$$print_sd(&s1); printf ("\n");

	str$free1_dx (&s1);
	str$free1_dx (&sr);

	return result_code;
}


/*************************************************/

int	testlibanalyze_sdesc_64 (FILE *fptr, int *fstatus, char *cont)
{
	int	input_length;
	unsigned long type;		/* Length of first string */
	unsigned long result_code;
	unsigned short s1_len;
	char 	*s1_ptr;			/* Pointer to first string */
	char 	pt1[10], pv1[10];
	char 	pt2[10], pv2[10];
	char	presult [20];
	struct	dsc$descriptor_s s1,sr;

	*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);
	if (strncmp (pt1,"cd",2) == 0)	str$$malloc_sd (&s1,pv1);
	if (strncmp (pt1,"cd",2) == 0)	str$$malloc_sd (&sr,pv1);
	type = atol (pv2);

	if ( strncmp (pv1,"NULL",4) == 0 )
		input_length = 0;
	else if ( strncmp ("MAXCSTR16",pv1,9) == 0 )
		input_length = 65535;
	else
		input_length = strlen(pv1);

	printf ("Testing str$analyze_sdesc_64 \n");
	printf ("Return a pointer to character string and it's length\n\n\n");
	printf ("Input   Value    "); str$$print_sd (&s1);	printf ("\n");
	printf ("\n");

	result_code = lib$analyze_sdesc_64 (&s1, &s1_len,&s1_ptr,&type);

	print_result_code  (presult,result_code,cont);
	print_int_compare  ("Type", DSC$K_DTYPE_T, s1.dsc$b_dtype, cont);
	print_int_compare  ("Class",DSC$K_CLASS_D, s1.dsc$b_class, cont);
	print_int_compare  ("Length",input_length,s1_len,cont);
	print_desc_compare ("String",&sr, &s1, cont);

	printf ("\n\n");
	printf ("Output  Type     %d\n", s1.dsc$b_dtype);
	printf ("Output  Class    %d\n", s1.dsc$b_class);
	printf ("Output  Length   %d\n", s1_len);
	printf ("Output  String   "); str$$print_sd(&s1); printf ("\n");
	printf ("Output  64 Bit   %ld \n",type);

	str$free1_dx (&s1);
	str$free1_dx (&sr);

	return result_code;
}

/************************************************/

#if 0
int	testlibasn_wth_mbx (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct dsc$descriptor_s device_name;
signed long message_size, buffer_quota;
unsigned short device_channel, mailbox_channel;

	printf ("LIB$ASN_WTH_MBX Not coded \n");
	*cont = 'P';

result_code = lib$asn_wth_mbx(&device_name,&message_size,&buffer_quota,&device_channel, &mailbox_channel);

return result_code;
}
#endif
/************************************************/

#if 0
int	testlibast_in_prog (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;

	printf ("LIB$AST_IN_PROG Not coded \n");
	*cont = 'P';

	result_code = lib$ast_in_prog();

return result_code;
}
/************************************************/

int	testlibattach (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
unsigned long process_id;

	printf ("LIB$ATTACH Not coded \n");
	*cont = 'P';

	result_code = lib$attach(&process_id);

return result_code;
}
#endif
/************************************************/


int	testlibchar (FILE *fptr, int *fstatus, char *cont)
{
unsigned long	result_code;
unsigned short	s1_len;
char		character, *s1_ptr;
char		pt1[30],pv1[30],presult[30];
struct	dsc$descriptor_s sd1;

*fstatus = fscanf (fptr,scan3,pt1,pv1,presult);

printf ("Testing lib$char \n");
printf ("Convert a single chartacter to a string descriptor \n");
printf ("\n");
printf ("Input  %c \n",pv1[0]);
printf ("\n\n");

character = pv1[0];
str$$malloc_sd (&sd1,"NULL");

result_code = lib$char (&sd1,&character);

str$analyze_sdesc (&sd1,&s1_len,&s1_ptr);

//print_result_compare (

printf ("Output Descriptor "); str$$print_sd(&sd1); printf ("\n");
printf ("Output Length     %d \n",s1_len);
printf ("Output String     %s \n",s1_ptr);

return result_code;
}

/************************************************/

int	testlibbuild_nodespec (FILE *fptr, int *fstatus, char *cont)
{
unsigned long result_code;
unsigned short len;
struct dsc$descriptor_s primary,nodespec,acs,second;
char	pt1[30],pv1[30],presult[30];
char	pt2[30],pv2[30];
char	pt3[30],pv3[30];
char	pt4[30],pv4[30];
char	pt5[30],pv5[30];

*fstatus = fscanf (fptr,scan11,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,presult);

str$$malloc_sd (&primary , pv1);
str$$malloc_sd (&nodespec, pv2);
str$$malloc_sd (&acs,      pv3);
str$$malloc_sd (&second,   pv4);
len = 0;

printf ("Input  Primary     "); str$$print_sd(&primary);   printf ("\n");
printf ("Input  Node Spec   "); str$$print_sd(&nodespec);  printf ("\n");
printf ("Input  ACS         "); str$$print_sd(&acs);       printf ("\n");
printf ("Input  Secondary   "); str$$print_sd(&second);    printf ("\n");
printf ("Input  Length      %d \n",len);
printf ("\n");

printf ("                    primary  nodespec  acs  second  length)\n");
if      ( strncmp (pv3,"0",1) == 0 )
{	printf("lib$build_nodespec (&primary,&nodespec,0,&second,&length)\n");
	result_code = lib$build_nodespec (&primary,&nodespec,0,&second,&len);
}
else if ( strncmp (pv4,"0",1) == 0 )
{	printf("lib$build_nodespec (&primary,&nodespec,&acs,0,&length)\n");
	result_code = lib$build_nodespec (&primary,&nodespec,&acs,0,&len);
}
else if ( strncmp (pv5,"0",1) == 0 )
{	printf ("lib$build_nodespec (&primary,&nodespec,&acs,&second,0)\n");
	result_code = lib$build_nodespec (&primary,&nodespec,&acs,&second,0);
}
else if (( strncmp (pv3,"0",1) == 0 ) && ( strncmp (pv4,"0",1) == 0 ))
{	printf ("lib$build_nodespec (&primary,&nodespec,0,0,&length)\n");
	result_code = lib$build_nodespec (&primary,&nodespec,0,0,&len);
}
else if (( strncmp (pv3,"0",1) == 0 ) && ( strncmp (pv5,"0",1) == 0 ))
{	printf(" lib$build_nodespec (&primary,&nodespec,&acs,&second,&len\n)");
	result_code = lib$build_nodespec (&primary,&nodespec,&acs,&second,&len);
}
else if (( strncmp (pv4,"0",1) == 0 ) && ( strncmp (pv5,"0",1) == 0 ))
{	printf(" lib$build_nodespec (&primary,&nodespec,&acs,0,0)\n");
	result_code = lib$build_nodespec (&primary,&nodespec,&acs,0,0);
}
else
{	printf ("lib$build_nodespec(&primary,&nodespec,&acs,&second,&len)\n");
	result_code = lib$build_nodespec (&primary,&nodespec,&acs,&second,&len);
}
printf ("\n");

print_result_code  (presult,result_code,cont);
printf ("\n\n");

printf ("Output Node Spec   "); str$$print_sd(&nodespec);  printf ("\n");
printf ("\n");
printf ("Output Primary     "); str$$print_sd(&primary);   printf ("\n");
printf ("Output ACS         "); str$$print_sd(&acs);       printf ("\n");
printf ("Output Secondary   "); str$$print_sd(&second);    printf ("\n");
printf ("Output Length      %d \n",len);

str$free1_dx (&primary);
str$free1_dx (&nodespec);
str$free1_dx (&acs);
str$free1_dx (&second);

return result_code;
}


/************************************************/

#if 0
int	testlibcompress_nodename (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct	dsc$descriptor_s name, compressed_name;
unsigned short	name_len;

	printf ("Test LIB$COMPRESS_NODENAME Not Implemented \n");
	*cont = 'P';

	str$$malloc_sd (&name,"NULL");
	str$$malloc_sd (&compressed_name,"NULL");

	result_code = lib$compress_nodename(&name, &compressed_name,&name_len);

	str$free1_dx (&name);
	str$free1_dx (&compressed_name);

return result_code;
}
#endif


/************************************************/


#if 0
int	testlibcompare_nodename (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct	dsc$descriptor_s name1, name2;
unsigned long	name_len;

	printf ("Test LIB$COMPRESS_NODENAME Not Implemented \n");
	*cont = 'P';

	str$$malloc_sd (&name1,"NULL");
	str$$malloc_sd (&name2,"NULL");

	result_code = lib$compare_nodename(&name1, &name2, &name_len);

	str$free1_dx (&name1);
	str$free1_dx (&name2);

return result_code;
}
#endif

/************************************************/


int	testlibcallg (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
char	arglist[255];

strcpy (arglist,"ls -l");

//result_code = lib$callg (arglist, testlibcallg(fptr,fstatus,cont));

return result_code;
}

/************************************************/


int	testlibcallg_64 (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;

	printf ("Test LIB$CALLG_64 Not Implemented \n");
	*cont = 'P';

//	result_code = lib$callg_64 ();


return result_code;
}

/************************************************/


int	testlibconvert_date_string (FILE *fptr, int *fstatus, char *cont)
{
char	pv1[10],pv2[10],pv3[10],pv4[10],pv5[10],pv6[10],presult[30];
unsigned long	result_code;
unsigned long long date_time;
struct dsc$descriptor_s date_string;
unsigned long 	user_context, flags, defaulted_fields;
unsigned short	defaults;

printf ("Testing lib$convert_date_string \n");
printf ("\n\n");

*fstatus = fscanf (fptr,scan7,pv1,pv2,pv3,pv4,pv5,pv6,presult);
str$$malloc_sd (&date_string,"NULL");

printf ("Input Date             "); str$$print_sd (&date_string); printf("\n");
printf ("Input date_time        %llu \n",date_time);
printf ("Input User Context     %lu  \n",user_context);
printf ("Input Flags            %lu  \n",flags);
printf ("Input Defaults         %u   \n",defaults);
printf ("Input Defaulted Fields %lu  \n",defaulted_fields);	
printf ("\n\n");

result_code = lib$convert_date_string(&date_string,&date_time,0,0,0,0);

print_result_code (presult, result_code, cont);

printf ("Output Date             "); str$$print_sd (&date_string); printf("\n");
printf ("Output date_time        %llu \n",date_time);
printf ("Output User Context     %lu  \n",user_context);
printf ("Output Flags            %lu  \n",flags);
printf ("Output Defaults         %u   \n",defaults);
printf ("Output Defaulted Fields %lu  \n",defaulted_fields);	
printf ("\n\n");

str$free1_dx (&date_string);

return result_code;
}

/************************************************/


#if 0
int	testlibcrc (FILE *fptr, int *fstatus, char *cont)
{
char	pv1[50], pv2[50], presult[30];
signed	long crc_table[16], initial_crc;
char *stream;
unsigned long result_code;
unsigned long polynomial;		// bitmask indicating which polynomial
int	i;

polynomial = 0xFFFF;

*fstatus = fscanf (fptr,scan3,pv1,pv2,presult);
polynomial = atol (pv1);
stream = pv1;

printf ("Input    polynomial %.16lX \n",polynomial );
printf ("Input    stream     %s \n",pv1);

// construct table
lib$crc_table (&polynomial, crc_table);

for ( i = 0; i < 16; i++ )
{	if ( i % 4 == 0)
		printf ("\nInput  ");
	printf ("%.16lX ",crc_table[i]);
}
printf ("\n");

// do crc check
result_code = lib$crc (crc_table, &initial_crc, stream);

printf ("Output  Result  %ld \n",result_code);

for ( i = 0; i < 16; i++ )
{	if ( i % 4 == 0)
		printf ("\nOutput ");
	printf ("%.16lX ",crc_table[i]);
}
printf ("\n");

return 0;
}

/************************************************/


int	testlibcrc_table (FILE *fptr, int *fstatus, char *cont)
{
char	pv1[50], presult[30];
signed	long crc_table[16];
unsigned long polynomial;		// bitmask indicating which polynomial
int	i;

polynomial = 0xFFFF;

*fstatus = fscanf (fptr,scan2,pv1,presult);
polynomial = atol (pv1);
for ( i = 0; i < 16; i++ )
{	if ( (i % 4) == 0)
		printf ("\nInput  ");
	printf ("%.16lX ",crc_table[i]);
}
printf ("\n");

printf ("Input    polynomial %.16lX \n",polynomial );

// construct table
lib$crc_table (&polynomial, crc_table);

for ( i = 0; i < 16; i++ )
{	if ( (i % 4) == 0)
		printf ("\nOutput ");
	printf ("%.16lX ",crc_table[i]);
}
printf ("\n");

return 0;
}


/************************************************/

int	testlibcreate_dir (FILE *fptr, int *fstatus, char *cont)
{
char pv1[10],pv2[10],pv3[10],pv4[10],pv5[10],pv6[10],pv7[10],presult[30];
unsigned long result;
struct dsc$descriptor_s device;
unsigned long owner;
unsigned short enable, value, maxium, relative;
unsigned long initial;

*fstatus = fscanf (fptr,scan8,pv1,pv2,pv3,pv4,pv5,pv6,pv7,presult);

str$$malloc_sd (&device,pv1);
owner      = atol (pv2);
enable     = atol (pv3);
value      = atol (pv4);
maxium     = atol (pv5);
relative   = atol (pv6);
initial    = atol (pv7);

printf ("Input     Device             %s \n",pv1);
printf ("Input     Owner              %s \n",pv2);
printf ("Input     Protection Enable  %s \n",pv3);
printf ("Input     Protection Value   %s \n",pv4);
printf ("Input     Maxium             %s \n",pv5);
printf ("Input     Relative           %s \n",pv6);
printf ("Input     Initial            %s \n",pv7);
printf ("\n\n");

result=lib$create_dir(&device,&owner,&enable,&value,&maxium,&relative,&initial);

printf ("Output    Device             "); str$$print_sd(&device); printf ("\n");
printf ("Output    Owner              %ld \n",owner);
printf ("Output    Protection Enable  %d  \n",enable);
printf ("Output    Protection Value   %d  \n",value);
printf ("Output    Maxium             %d  \n",maxium);
printf ("Output    Relative           %d  \n",relative);
printf ("Output    Initial            %ld \n",initial);

str$free1_dx (&device);

return result;
}


/************************************************/

int testlibcreate_user_vm_zone	(FILE *fptr, int *fstatus, char *cont)
{
unsigned long result;
unsigned long id;


printf ("Not implemented \n");

printf ("Input     Zone ID %ld \n",id);
printf ("\n\n");

result = lib$create_user_vm_zone (&id,0,0,0,0,0);

printf ("Output    Zone ID %ld \n",id);

return 0;
}


/************************************************/

int testlibcreate_user_vm_zone_64(FILE *fptr, int *fstatus, char *cont)
{
unsigned long result;
unsigned long id;


printf ("Not implemented \n");

printf ("Input     Zone ID %ld \n",id);
printf ("\n\n");

result = lib$create_user_vm_zone_64 (&id,0,0,0,0,0);

printf ("Output    Zone ID %ld \n",id);

return 0;
}
/************************************************/

int testlibcreate_vm_zone (FILE *fptr, int *fstatus, char *cont)
{
unsigned long result;
unsigned long id;


printf ("lib$create_vm_zone  Not implemented \n");

printf ("Input     Zone ID %ld \n",id);
printf ("\n\n");

result = lib$create_vm_zone (&id,0,0,0,0,0,0,0,0,0,0,0,0);

printf ("Output    Zone ID %ld \n",id);

return 0;
}
#endif
/************************************************/

int	testlibwait (FILE *fptr, int *fstatus, char *cont)
{

	printf ("Test LIB$WAIT Not Implemented \n");
	*cont = 'P';

return 0;
}


/************************************************/

int	testlibdate_time (FILE *fptr, int *fstatus, char *cont)
{
unsigned short date_time_size = 23;
unsigned long  result;
struct dsc$descriptor_s date_time_desc;
char	presult[30];

	printf ("Test LIB$DATE_TIME \n");
	
	*fstatus = fscanf (fptr,scan1,presult);
	
	printf ("Input  None \n");
	printf ("\n\n");

	str$$malloc_sd (&date_time_desc,"NULL");
	str$get1_dx (&date_time_size, &date_time_desc);

	result = lib$date_time( &date_time_desc );

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf("Output DateTime ");str$$print_sd(&date_time_desc);printf ("\n");

return 0;
}


/************************************************/

#if 0
int	testlibdelete_file (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result;
struct	dsc$descriptor_s file_name;
char	temp_file_name[255], presult[30];

	
	printf ("Test LIB$DELETE_FILE \n");
	printf ("\n\n");	

	*fstatus = fscanf (fptr,scan2,temp_file_name,presult);

	printf ("Input  File Name %s \n",temp_file_name);
	printf ("\n\n");

	str$$malloc_sd (&file_name,temp_file_name);

	result = lib$delete_file( &file_name,0,0,0,0,0,0,0,0);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf ("Output File Name "); str$$print_sd (&file_name);printf ("\n");
	printf ("\n\n");


return 0;
}

/************************************************/

int	testlibdelete_logical (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result;
struct	dsc$descriptor_s logical_name, table_name;
char    temp_logical_name[256], temp_table_name[256], presult[30];

	printf ("Test LIB$DELETE_LOGICAL \n");
	printf ("\n\n");
	
	*fstatus= fscanf (fptr,scan3,temp_logical_name,temp_table_name,presult);

	printf ("Input  Logical Name %s \n",temp_logical_name);
	printf ("Input  Table   Name %s \n",temp_table_name);
	printf ("\n\n");
	

	str$$malloc_sd (&logical_name,temp_logical_name);

	if (temp_table_name == NULL )
		result = lib$delete_logical ( &logical_name,0);
	else
	{	str$$malloc_sd (&table_name,temp_table_name);
		result = lib$delete_logical ( &logical_name,&table_name);
	}

	print_result_code (presult,result,cont);

	printf ("\n\n");

	printf ("Output Logical  "); str$$print_sd(&logical_name);printf ("\n");
	printf ("Output Table    "); str$$print_sd(&table_name);  printf ("\n");

return 0;
}

/************************************************/

int	testlibdelete_symbol (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result;
struct	dsc$descriptor_s symbol_name;
char	pv1[256],presult[30];

	printf ("Test LIB$DELETE_LOGICAL \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan2,pv1,presult);

	printf ("Input  Symbol name %s \n",pv1);
	printf ("\n\n");

	str$$malloc_sd (&symbol_name,pv1);

	result = lib$delete_symbol ( &symbol_name,0);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf("Output Symbol name ");str$$print_sd(&symbol_name);printf ("\n");
	printf ("\n\n");

return 0;
}

/************************************************/

int	testlibdelete_vm_zone (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result, zone_id;
char	pv1[256],presult[30];

	printf ("Test LIB$DELETE_VM_ZONE \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan2,pv1,presult);

	printf ("Input   Zone Name %s \n",pv1);
	printf ("\n\n");

	zone_id = atol (pv1);

	result = lib$delete_vm_zone (&zone_id);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf("Output   Zone name %lu \n",zone_id);
	printf ("\n\n");

return 0;
}

/************************************************/

int	testlibdelete_vm_zone_64 (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result, zone_id;
char	pv1[256],presult[30];

	printf ("Test LIB$DELETE_VM_ZONE_64 \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan2,pv1,presult);

	zone_id = atol (pv1);

	printf ("Input   Zone Number %s \n",pv1);
	printf ("\n\n");


	result = lib$delete_vm_zone_64 ( &zone_id);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf("Output   Zone Number %lu \n",zone_id);
	printf ("\n\n");

return 0;
}
#endif

/************************************************/

int	testlibdigit_sep (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result;
unsigned short	length;
char	pv1[256],pv2[255],presult[30];
struct dsc$descriptor_s digit_sep;

	printf ("Test LIB$DIGIT_SEP \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan3,pv1,pv2,presult);

	str$$malloc_sd (&digit_sep, pv1);
	length = atol (pv2);

	printf ("Input  Descriptor %s \n",pv1);
	printf ("Input  Length     %s \n",pv2);
	printf ("\n\n");

	if ( length == 0 )
		result = lib$digit_sep (&digit_sep,0);
	else
		result = lib$digit_sep (&digit_sep, &length);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf ("Output  Descriptor "); str$$print_sd (&digit_sep);printf("\n");
	printf ("Output  Length     %d \n",length);
	printf ("\n\n");

return 0;
}

/************************************************/

#if 0
int	testlibdisable_ctrl (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result,mask,oldmask;
char	pv1[256],pv2[255],presult[30];

	printf ("Test LIB$DISABLE_CTRL \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan3,pv1,pv2,presult);

	mask    = (unsigned long) atol (pv1);
	oldmask = (unsigned long) atol (pv2);

	printf ("Input      Mask %s \n",pv1);
	printf ("Input  Old Mask %s \n",pv2);
	printf ("\n\n");

	if (  oldmask == 0 )
		result = lib$disable_ctrl (&mask,0);
	else
		result = lib$disable_ctrl (&mask, &oldmask);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf ("Output      Mask   %lu \n",mask);
	printf ("Output  Old Mask   %lu \n",oldmask);
	printf ("\n\n");

return 0;
}

/************************************************/

int	testlibdo_command (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result;
char	pv1[256],presult[30];
struct dsc$descriptor_s command;

	printf ("Test LIB$DO_COMMAND \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan2,pv1,presult);

	str$$malloc_sd (&command,pv1);

	printf ("Input  Command  %s \n",pv1);
	printf ("\n\n");

	result = lib$do_command (&command);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf ("Output  Command   \n");
	printf ("\n\n");

return 0;
}

/************************************************/
#endif
 
int	testlibediv (FILE *fptr, int *fstatus, char *cont)
{
char	pv1[256],pv2[255],pv3[255],pv4[255],presult[30];
unsigned long result_code;
long 	divisor, quotient, remainder;
long long dividend;

	printf ("Test LIB$EDIV \n");
	printf ("\n\n");

	*fstatus = fscanf (fptr,scan5,pv1,pv2,pv3,pv4,presult);

	divisor   = atol  (pv1);
	dividend  = atoll (pv2);
	quotient  = atol  (pv3);
	remainder = atol  (pv4);

	printf ("Input Dvidend     %lli \n",dividend);
	printf ("Input Divisor /   %li  \n",divisor);
	printf ("Input Quotient    %li  \n",quotient);
	printf ("Input Remainder   %li  \n",remainder);
	printf ("\n\n");

	quotient  = 0;
	remainder = 0;

	result_code = lib$ediv (&divisor, &dividend, &quotient, &remainder);

	print_result_code (presult,result_code,cont);
	printf ("\n\n");

	printf ("Output Dvidend    %lli \n",dividend);
	printf ("Output Divisor /  %li  \n",divisor);
	printf ("Output Quotient   %li  \n",quotient);
	printf ("Output Remainder  %li  \n",remainder);

	return 0;
}
#if 0
int	testlibenable_ctrl (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result,mask,oldmask;
char	pv1[256],pv2[255],presult[30];

	printf ("Test LIB$ENABLE_CTRL \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan3,pv1,pv2,presult);

	mask    = (unsigned long) atol (pv1);
	oldmask = (unsigned long) atol (pv2);

	printf ("Input      Mask %s \n",pv1);
	printf ("Input  Old Mask %s \n",pv2);
	printf ("\n\n");

	if (  oldmask == 0 )
		result = lib$enable_ctrl (&mask,0);
	else
		result = lib$enable_ctrl (&mask, &oldmask);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf ("Output      Mask   %lu \n",mask);
	printf ("Output  Old Mask   %lu \n",oldmask);
	printf ("\n\n");

return 0;
}

/**************************************/

int	testlibestablish (FILE *fptr, int *fstatus, char *cont)
{
unsigned long  result;
char	presult[30];

	printf ("Test LIB$ESTABLISH \n");
	printf ("\n\n");
	
	*fstatus = fscanf (fptr,scan1,presult);

	printf ("Input        \n");
	printf ("\n\n");

//	result = lib$establish (void);

	print_result_code (presult,result,cont);
	printf ("\n\n");

	printf ("Output          \n");
	printf ("\n\n");

return 0;
}
#endif

/************************************************/

#if 0
int	testlibexpand_nodename (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct	dsc$descriptor_s name, expanded_name;
unsigned short	name_len;

	printf ("Test LIB$EXPAND_NODENAME Not Implemented \n");
	*cont = 'P';

	result_code = lib$expand_nodename(&name, &expanded_name,&name_len);

return result_code;
}
#endif

/************************************************/


#if 0
int	testlibfid_to_name (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct	dsc$descriptor_s device_name, filespec;
unsigned short	file_id, filespec_length, directory_id;
unsigned long	acp_status;

	printf ("Test LIB$FID_TO_NAME   \n");
	*cont = 'P';

	result_code = lib$fid_to_name (&device_name, &file_id, &filespec,
				&filespec_length, &directory_id, &acp_status);

return result_code;
}


/************************************************/

#define FAB _fabdef

int	testlibfile_scan (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct FAB nam_block;
unsigned long context;

	printf ("Test LIB$FILE_SCAN   \n");
	*cont = 'P';

	result_code = lib$file_scan (&nam_block,0, 0, &context);

return result_code;
}



/************************************************/


int	testlibfile_scan_end (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct FAB nam_block;
unsigned long context;

	printf ("Test LIB$FILE_SCAN   \n");
	*cont = 'P';

	result_code = lib$file_scan_end (&nam_block, &context);

return result_code;
}



/************************************************/


int	testlibfind_file (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
unsigned long context, status, flags;
struct dsc$descriptor_s	file_spec, result_file_spec, default_file_spec;
struct dsc$descriptor_s related_file_spec;

	printf ("Test LIB$FIND_FILE   \n");
	*cont = 'P';

	result_code = lib$find_file (&file_spec, &result_file_spec,&context,
				 &default_file_spec, &related_file_spec,
				 &status, &flags );

return result_code;
}




/************************************************/


int	testlibfind_file_end (FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
unsigned long context;

	printf ("Test LIB$FIND_FILE_END   \n");
	*cont = 'P';

	result_code = lib$find_file_end (&context);

return result_code;
}
#endif

/************************************************/

int testliblocc			(FILE *fptr, int *fstatus, char *cont)
{
int	result_code;
struct	dsc$descriptor_s char_string, source_string;
char	pt1[10],pt2[10];
char	pv1[10],pv2[10],presult[30];

	printf ("Testing LIB$LOCC \n");
	printf ("\n\n");

	*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

	str$$malloc_sd (&char_string, pv1);
	str$$malloc_sd (&source_string, pv2);

	printf ("Input  Character ");str$$print_sd(&char_string); printf ("\n");
	printf ("Input  Source    ");str$$print_sd(&source_string);printf ("\n");
	printf ("\n");

	result_code = lib$locc (&char_string, &source_string);

	if ( result_code != atoi (presult ) )
	{	*cont = 'P';
	}

	printf ("Output Result %d \n",result_code);
	printf ("\n");
	printf ("Output Character ");str$$print_sd(&char_string);  printf ("\n");
	printf ("Output Source    ");str$$print_sd(&source_string);printf ("\n");
	printf ("\n");

return result_code;
}

/************************************************/

int testlibmatchc		(FILE *fptr, int *fstatus, char *cont)
{
unsigned long	result_code;
struct	dsc$descriptor_s char_string, source_string;
char	pt1[10],pt2[10];
char	pv1[10],pv2[10],presult[30];

	printf ("Testing LIB$MATCHC \n");
	printf ("\n\n");

	*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

	str$$malloc_sd (&char_string, pv1);
	str$$malloc_sd (&source_string, pv2);

	printf ("Input  Character ");str$$print_sd(&char_string); printf ("\n");
	printf ("Input  Source    ");str$$print_sd(&source_string);printf ("\n");
	printf ("\n");

	result_code = lib$matchc (&char_string, &source_string);

	print_lu_compare("INDEX",(unsigned long)atoi(presult),result_code,cont);

	printf ("\n\n");
	if ( result_code != atoi (presult) )
	{	*cont = 'P';
	}

	printf ("Output Result %ld \n",result_code);
	printf ("\n");
	printf ("Output Character ");str$$print_sd(&char_string);  printf ("\n");
	printf ("Output Source    ");str$$print_sd(&source_string);printf ("\n");
	printf ("\n");

return result_code;
}


/************************************************/

int	testlibmovc3 		(FILE *fptr, int *fstatus, char *cont)
{
char	pt1[10],pt2[10], pt3[10];
char	pv1[30],pv2[30], pv3[30];
unsigned short length;

	printf ("Testing LIB$MOVC3 \n");
	printf ("\n\n");

	*fstatus = fscanf (fptr,scan6,pt1,pv1,pt2,pv2,pt3,pv3);

	printf ("Input  Count       "); printf ("%30s \n",pv1);
	printf ("Input  Source      "); printf ("%30s \n",pv2);
	printf ("Input  Destination "); printf ("%30s \n",pv3);
	printf ("\n");

	length = atoi (pv3);
	lib$movc3 (&length,pv1, pv2);

	printf ("\n");
	printf ("Output Count       "); printf ("%30s \n",pv1);
	printf ("Output Source      "); printf ("%30s \n",pv2);
	printf ("Output Destination "); printf ("%30s \n",pv3);
	printf ("\n");

return 0;
}
