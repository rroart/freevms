/*
 *	rtltest
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
 *	Oct 10, 1996 - Kevin Handy
 *		Initial version
 *
 *	Feb 4, 1997 - Kevin Handy
 *		Lose variable 'Tets2', change '%d' to '%ld' in several
 *		places to lose '-Wall' messages.
 *		Include 'stdlib.h'
 *
 *	Feb 7, 1997 - Christof Zeile
 *		Change 'short' to 'unsigned short' in several places.
 *		Add 'include <strdef.h>'
 *
 *	Sep 13, 2002 - Andrew Allison
 *		Added a crude menu system for selecting tests
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
#include "str$routines.h"
#include "sys$routines.h"
#include "lib$routines.h"

/***********************************************/
/*
 * Prototypes
 */

void 	testanal	(char* Title, struct dsc$descriptor_s* strptr);
int  	testsysasctim	(void);
int  	test_array	(void);
void 	test_compare	(void);
void 	test_analyze	(void);
int  	test_analyze2	(void);
void	print_result_code (char *presult, unsigned long result_code,char *cont);
void 	print_desc_compare (	char *title,
				struct dsc$descriptor_s *s1,
				struct dsc$descriptor_s *s2,
				char *cont);
void print_lu_compare(char *title,unsigned long v1,unsigned long v2,char *cont);
void print_ls_compare	(char *title,long v1,long v2,char *cont);
void print_int_compare	(char *title,int v1,int v2,char *cont);

/*************************************************************/
/*
 *	local test function prototypes
 */

int teststradd        		(FILE *fptr,int *fstatus, char *cont);
int teststranalyze 		(FILE *fptr,int *fstatus, char *cont);
int teststranalyze_64 		(FILE *fptr,int *fstatus, char *cont);
int teststrappend     		(FILE *fptr,int *fstatus, char *cont);
int teststrcase_blind_compare	(FILE *fptr,int *fstatus, char *cont);
int teststrcompare	  	(FILE *fptr,int *fstatus, char *cont);
int teststrcompare_eql		(FILE *fptr,int *fstatus, char *cont);
int teststrcompare_multi	(FILE *fptr,int *fstatus, char *cont);
int teststrconcat		(FILE *fptr,int *fstatus, char *cont);
int teststrcopy_dx		(FILE *fptr,int *fstatus, char *cont);
int teststrcopy_fill 		(FILE *fptr,int *fstatus, char *cont);
int teststrcopy_r		(FILE *fptr,int *fstatus, char *cont);
int teststrcopy_r_64		(FILE *fptr,int *fstatus, char *cont);
int teststrdiv		  	(FILE *fptr,int *fstatus, char *cont);
int teststrdupl_char		(FILE *fptr,int *fstatus, char *cont);
int teststrelement		(FILE *fptr,int *fstatus, char *cont);
int teststrfind_first_in_set	(FILE *fptr,int *fstatus, char *cont);
int teststrfind_first_not_in_set(FILE *fptr,int *fstatus, char *cont);
int teststrfind_first_substring	(FILE *fptr,int *fstatus, char *cont);
int teststrfree1_dx     	(FILE *fptr,int *fstatus, char *cont);
int teststrget1_dx     		(FILE *fptr,int *fstatus, char *cont);
int teststrget1_dx_64  		(FILE *fptr,int *fstatus, char *cont);
int teststrleft		  	(FILE *fptr,int *fstatus, char *cont);
int teststrlen_extr	  	(FILE *fptr,int *fstatus, char *cont);
int teststrmatch_wild		(FILE *fptr,int *fstatus, char *cont);
int teststrmul        		(FILE *fptr,int *fstatus, char *cont);
int teststrposition		(FILE *fptr,int *fstatus, char *cont);
int teststrpos_extr		(FILE *fptr,int *fstatus, char *cont);
int teststrprefix		(FILE *fptr,int *fstatus, char *cont);
int teststrrecip 		(FILE *fptr,int *fstatus, char *cont);
int teststrreplace 		(FILE *fptr,int *fstatus, char *cont);
int teststrright	  	(FILE *fptr,int *fstatus, char *cont);
int teststrround	  	(FILE *fptr,int *fstatus, char *cont);
int teststrsub        		(FILE *fptr,int *fstatus, char *cont);
int teststrtranslate   		(FILE *fptr,int *fstatus, char *cont);
int teststrtrim   		(FILE *fptr,int *fstatus, char *cont);
int teststrupcase   		(FILE *fptr,int *fstatus, char *cont);

int teststriszero		(FILE *fptr,int *fstatus, char *cont);
int teststriszerotrim 		(FILE *fptr,int *fstatus, char *cont);
int teststrlzerotrim  		(FILE *fptr,int *fstatus, char *cont);
int teststrrzerotrim  		(FILE *fptr,int *fstatus, char *cont);
int teststrncompare		(FILE *fptr,int *fstatus, char *cont);

void testdate(void);
int tempf (void);

int	get_fun_num (char *functionname);
void call_test_function (int which_fun,char *fname,char *cont, FILE *fptr);

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
{"str$add",   			&teststradd},
{"str$analyze_sdesc",		&teststranalyze},
{"str$analyze_sdesc_64",	&teststranalyze_64},
{"str$append",			&teststrappend},
{"str$case_blind_compare",	&teststrcase_blind_compare},
{"str$compare",			&teststrcompare},
{"str$compare_eql",		&teststrcompare_eql},
{"str$compare_multi",		&teststrcompare_multi},
{"str$concat",			&teststrconcat},
{"str$copy_dx",			&teststrcopy_dx},
{"str$$copy_fill",		&teststrcopy_fill},
{"str$copy_r",			&teststrcopy_r},
{"str$copy_r_64",		&teststrcopy_r_64},
{"str$divide",			&teststrdiv},
{"str$dupl_char",		&teststrdupl_char},
{"str$element",			&teststrelement},
{"str$find_first_in_set",	&teststrfind_first_in_set},
{"str$find_first_not_in_set",	&teststrfind_first_not_in_set},
{"str$find_first_substring",	&teststrfind_first_substring},
{"str$free1_dx",		&teststrfree1_dx},
{"str$get1_dx",			&teststrget1_dx},
{"str$get1_dx_64",		&teststrget1_dx_64},
{"str$iszero",    		&teststriszero},
{"str$iszerotrim",    		&teststriszerotrim},
{"str$left",			&teststrleft},
{"str$len_extr",		&teststrlen_extr},
{"str$$lzerotrim", 		&teststrlzerotrim},
{"str$match_wild",		&teststrmatch_wild},
{"str$mul",			&teststrmul},
{"str$ncompare",		&teststrncompare},
{"str$pos_extr",		&teststrpos_extr},
{"str$position",		&teststrposition},
{"str$prefix",			&teststrprefix},
{"str$recip",			&teststrrecip},
{"str$replace",			&teststrreplace},
{"str$right",			&teststrright},
{"str$round",			&teststrround},
{"str$$rzerotrim", 		&teststrrzerotrim},
{"str$sub",			&teststrsub},
{"str$translate",		&teststrtranslate},
{"str$trim",			&teststrtrim},
{"str$upcase",			&teststrupcase},
#if 0
{"sys$asctim",			&testsysasctim}
#endif
};


/*************** MAIN *************************/

int main(void)
{

int	line_ctr;
int	which_fun,i,j;
char  	functionname[FNameSize];
FILE 	*fptr;
char	cont[5];

#if i386
	system ("clear");
#endif

	fptr = fopen ("test_str_rtl.dat","r");
	if (fptr == NULL )
	{	printf ("Could not open data file test_str_rtl.dat \n");
		return 0;
	}


	printf ("\n\n\n\t\t\tFreeVMS Runtime Library Tester\n\n\n");
	line_ctr = 0;
	strcpy (cont,"P");

	printf ("Enter Function Name :");
	scanf  ("%s",functionname);

//	get rid of <CR>  some garbage left when changing input modes
	fgets(cont,2,stdin);
	strcpy (cont,"P");

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
			call_test_function(j,flist[j].fname,cont,fptr);
		}
	}

	if (which_fun != 9999 ) // valid function name
	{
		call_test_function(which_fun,functionname,cont,fptr);
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
void call_test_function (int which_fun,char *fname,char *cont, FILE *fptr)
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
			if((strcmp(cont,"P")==0)||(strcmp(cont,"p")==0))
			{	printf ("\n\n");
				printf ("Enter to continue or N for Nonstop :");
				fgets(cont,2,stdin);
					
				if (strncmp (cont,"N",1) == 0)
					strcpy (cont,"C");
				else
					strcpy (cont,"P");
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
	strcpy (cont,"P");
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
	strcpy (cont,"P");
}
else
	printf ("%-10s ......OK \n",title);

}
/********************************************/

void print_lu_compare (char *title,unsigned long v1,unsigned long v2,char *cont)
{
if ( v1 != v2 )
{	printf ("FAILED - %10s\n",title);
	strcpy (cont,"P");
}
else
	printf ("%-10s ......OK \n",title);

}
/********************************************/

void print_ls_compare (	char *title, long v1, long v2, char *cont)
{
if ( v1 != v2 )
{	printf ("FAILED - %10s\n",title);
	strcpy (cont,"P");
}
else
	printf ("%-10s ......OK \n",title);

}
/********************************************/

void print_int_compare ( char *title, int v1, int v2, char *cont)
{
if ( v1 != v2 )
{	printf ("FAILED - %10s\n",title);
	strcpy (cont,"P");
}
else
	printf ("%-10s ......OK \n",title);

}


/********************************************/

int	teststradd(FILE *fptr,int *fstatus, char *cont)
{
unsigned long result_code;
struct dsc$descriptor_s s1,s2,s3,s4;
unsigned long asign,bsign,csign,rsign;
signed long	aexp,bexp,cexp,rexp;
char 	pt1[10], pv1[50];
char 	pt2[10], pv2[50];
char 	pt3[10], pv3[50];
char 	pt4[10], pv4[50];
char 	pt5[10], pv5[50];
char 	pt6[10], pv6[50];
char 	pt7[10], pv7[50];
char 	pt8[10], pv8[50];
char 	pt9[10], pv9[50];
char	presult [30];

strcpy (pv3,"\n");
strcpy (pv6,"\n");
strcpy (pv9,"\n");

*fstatus = fscanf (fptr,scan19,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,pt6,pv6,pt7,pv7,pt8,pv8,pt9,pv9,presult);

if (strncmp (pt3,"cd",2) == 0) str$$malloc_sd (&s1,pv3);
if (strncmp (pt6,"cd",2) == 0) str$$malloc_sd (&s2,pv6);
if (strncmp (pt9,"cd",2) == 0) str$$malloc_sd (&s4,pv9);
strcpy (pv9,"NULL");
if (strncmp (pt9,"cd",2) == 0) str$$malloc_sd (&s3,pv9);
asign = atol (pv1);
bsign = atol (pv4);
csign = atol (pv7);
rsign = csign;

aexp = atol(pv2);
bexp = atol(pv5);
cexp = atol(pv8);
rexp = cexp;

printf ("Testing str$add \n");
printf ("A + B = C\n\n\n");
printf ("Input A sign %lu value ",asign);str$$print_sd(&s1);printf(" exp %ld\n",aexp);
printf ("Input B sign %lu value ",bsign);str$$print_sd(&s2);printf(" exp %ld\n",bexp);
printf ("Input C sign %lu value ",csign);str$$print_sd(&s3);printf(" exp %ld\n",cexp);
printf ("\n");

result_code=str$add(&asign,&aexp,&s1,&bsign,&bexp,&s2,&csign,&cexp,&s3);

print_result_code  (presult,result_code,cont);
print_ls_compare   ("Exponent",cexp,rexp,cont);
print_lu_compare   ("Sign",csign,rsign,cont);
print_desc_compare ("Digits",&s3,&s4,cont);

printf ("\n\n");
printf ("Output A sign %lu value ",asign);str$$print_sd(&s1);printf(" exp %ld\n",aexp);
printf ("Output B sign %lu value ",bsign);str$$print_sd(&s2);printf(" exp %ld\n",bexp);
printf ("Output C sign %lu value ",csign);str$$print_sd(&s3);printf(" exp %ld\n",cexp);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&s3);
str$free1_dx (&s4);

return result_code;
}


/********************************************/

int	teststranalyze (FILE *fptr, int *fstatus, char *cont)
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

#define MAXCSTR16     65535
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

	result_code = str$analyze_sdesc(&s1, &s1_length, &s1_ptr);

	print_result_code  (presult,result_code,cont);
	print_int_compare  ("Type", DSC$K_DTYPE_T, s1.dsc$b_dtype, cont);
	print_int_compare  ("Class",DSC$K_CLASS_D, s1.dsc$b_class, cont);
	print_int_compare  ("Length",in_length,s1_length,cont);
	print_desc_compare ("String",&sr,&s1,cont);

	printf ("\n\n");
	printf ("Output  Type     %d\n", s1.dsc$b_dtype);
	printf ("Output  Class    %d\n", s1.dsc$b_class);
	printf ("Output  Length   %d\n", s1_length);
	printf ("Output  String   "); str$$print_sd(&s1); printf ("\n");

	str$free1_dx (&s1);
	str$free1_dx (&sr);

	return result_code;
}


/********************************************/

int	teststranalyze_64 (FILE *fptr, int *fstatus, char *cont)
{
	int	input_length;
	unsigned short type;		/* Length of first string */
	unsigned long result_code;
	unsigned short s1_len;
	unsigned long long s1_long_len;
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

	result_code = str$analyze_sdesc_64 (&s1, &s1_long_len,&s1_ptr,&type);
	s1_len = (unsigned short) s1_long_len;

	print_result_code  (presult,result_code,cont);
	print_int_compare  ("Type", DSC$K_DTYPE_T, s1.dsc$b_dtype, cont);
	print_int_compare  ("Class",DSC$K_CLASS_D, s1.dsc$b_class, cont);
	print_int_compare  ("Length",input_length,s1_len,cont);
	print_desc_compare ("String",&sr,&s1,cont);

	printf ("\n\n");
	printf ("Output  Type     %d\n", s1.dsc$b_dtype);
	printf ("Output  Class    %d\n", s1.dsc$b_class);
	printf ("Output  Length   %d\n", s1_len);
	printf ("Output  String   "); str$$print_sd(&s1); printf ("\n");
	printf ("Output  64 Bit   %d \n",type);

	str$free1_dx (&s1);
	str$free1_dx (&sr);

	return result_code;
}

/******************************************************/

int	teststrappend(FILE *fptr,int *fstatus, char *cont)

{
unsigned long result_code;
unsigned short s1_len;
struct dsc$descriptor_s s1,s2, sr1,sr2;
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char	presult [MAXSTRING];
char	combo[MAXCSTR16], *s1_ptr;
int	combo_length, i;


	*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

	if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
	if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);

// concatenate the inputs
	for (i=0; i < MAXCSTR16; i++)
		combo[i] = '\0';

	if ( strncmp (pv1,"NULL",4) == 0 )
		;
	else if ( strncmp (pv1,"MAXCSTR16",9) == 0 )
	{
		for (i=0; i < MAXCSTR16; i++)
			combo[i] = '9';
	}
	else
		strcat (combo,pv1);

	if (strncmp (pv1,"MAXCSTR16",9) == 0 )
		strcpy (combo,"MAXCSTR16");
	else if  (strncmp (pv2,"NULL",4) == 0)
		;
	else
		strcat (combo,pv2);

// setup the saved result descriptor
	if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr1,combo);
	if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&sr2,pv2);

//	add up the lengths of the inputs
	if ( strncmp (pv1,"NULL",4) == 0 )
		combo_length = 0;
	else if ( strncmp (pv1,"MAXCSTR16",9) == 0 )
		combo_length = MAXCSTR16;
	else
		combo_length = strlen(pv1);

	if ( strncmp (pv2,"NULL",4) == 0 )
		combo_length += 0;
	else if ( strncmp (pv1,"MAXCSTR16",9) == 0 )
		combo_length = MAXCSTR16;
	else
		combo_length += strlen(pv2);


	printf ("Testing str$append \n");
	printf ("Append a string to another \n\n\n");

	printf("Input String A   %s\n",pv1);
	printf("Input String B   %s\n",pv2);
	printf ("\n");

	result_code = str$append(&s1,&s2);

	print_result_code (presult, result_code,cont);
	print_desc_compare("String 1",&sr1,&s1,cont);
	print_desc_compare("String 2",&sr2,&s2,cont);

	str$analyze_sdesc (&s1,&s1_len,&s1_ptr);
	print_int_compare ("Length",(int) s1_len,combo_length,cont);

	printf ("\n\n");
	printf ("Output String A  "); str$$print_sd(&s1); printf ("\n");
	printf ("Output String B  "); str$$print_sd(&s2); printf ("\n");

	str$free1_dx (&s1);
	str$free1_dx (&s2);
	str$free1_dx (&sr1);
	str$free1_dx (&sr2);

	return (int) result_code;
}


/*************************************************/

int	teststrcase_blind_compare(FILE *fptr, int *fstatus, char *cont)
{
char 	pt1[MAXSTRING],pv1[MAXSTRING];
char	pt2[MAXSTRING],pv2[MAXSTRING];
char	presult[MAXSTRING];
struct	dsc$descriptor_s sd1,sd2,sr1,sr2;
signed long	expect_result,result_code;


*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);
expect_result = atol (presult);

if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sd1,pv1);
if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sr1,pv1);
if (strncmp (pt2,"cd",2) == 0) str$$malloc_sd (&sd2,pv2);
if (strncmp (pt2,"cd",2) == 0) str$$malloc_sd (&sr2,pv2);

printf ("Testing str$case_blind_compare \n");
printf ("Compare strings ignoring case \n\n\n");

printf ("Input String 1      %s \n",pv1);
printf ("Input String 2      %s \n",pv2);
printf ("\n");

result_code = str$case_blind_compare(&sd1, &sd2);

print_int_compare ("Result       ",(int) expect_result,(int)result_code,cont);
print_desc_compare("String 1     ",&sd1,&sr1,cont);
print_desc_compare("String 2     ",&sd2,&sr2,cont);

printf("\n\n");
printf ("Output Result Code  %ld \n",result_code);
printf ("Output String 1     "); str$$print_sd (&sd1); printf ("\n");
printf ("Output String 2     "); str$$print_sd (&sd2); printf ("\n");

str$free1_dx (&sd1);
str$free1_dx (&sr1);
str$free1_dx (&sd2);
str$free1_dx (&sr2);

return result_code;
}

/*********************************************************/

int	teststrcompare(FILE *fptr,int *fstatus, char *cont)
{
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char			presult [30];
signed long		expect_result,result_code;
struct dsc$descriptor_s s1,s2,sr1,sr2;

*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr1,pv1);
if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&sr2,pv2);
expect_result = atol (presult);

printf ("Testing str$compare \n");
printf ("Compare 2 strings\n\n\n");

printf ("Input A          %s \n",pv1);
printf ("Input B          %s \n",pv2);
printf ("\n\n");

result_code = str$compare(&s1,&s2);

print_int_compare  ("Results ",result_code,expect_result,cont);
print_desc_compare ("String 1",&sr1,&s1,cont);
print_desc_compare ("String 2",&sr2,&s2,cont);


printf ("\n\n");
printf ("Output Result    %ld \n",result_code);
printf ("Output A         "); str$$print_sd (&s1); printf ("\n");
printf ("Output B         "); str$$print_sd (&s2); printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&sr1);
str$free1_dx (&s2);
str$free1_dx (&sr2);

return result_code;
}

/*********************************************************/

int	teststrcompare_eql(FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		expect_result, result_code;
struct dsc$descriptor_s s1,s2,sr1,sr2;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char			presult [30];

*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr2,pv2);
expect_result = atol (presult);

printf ("Testing str$compare_eql \n");
printf ("Compare 2 strings for equality\n\n\n");

printf ("Input A          %s \n",pv1);
printf ("Input B          %s \n",pv2);
printf ("\n\n");

result_code = str$compare_eql(&s1,&s2);

print_int_compare  ("Result",(int) expect_result,(signed int) result_code,cont);
print_desc_compare ("String 1",&sr1, &s1, cont);
print_desc_compare ("String 2",&sr2, &s2, cont);

printf ("\n\n");
printf ("Output Result    %ld \n",result_code);
printf ("Output A         "); str$$print_sd (&s1); printf ("\n");
printf ("Output B         "); str$$print_sd (&s2); printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&sr1);
str$free1_dx (&s2);
str$free1_dx (&sr2);

return result_code;
}


/*********************************************************/


int	teststrcompare_multi(FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[MAXSTRING], pv1[MAXSTRING];
char 		pt2[MAXSTRING], pv2[MAXSTRING];
char 		pt3[MAXSTRING], pv3[MAXSTRING];
char 		pt4[MAXSTRING], pv4[MAXSTRING];
char		presult[MAXSTRING];
struct		dsc$descriptor_s sd1,sd2;
struct		dsc$descriptor_s sr1,sr2;	// saved result
unsigned long	flags, language,result;
char		*s1_ptr, *s2_ptr;		/* Pointer to strings */
unsigned short	s1_len, s2_len;			/* Length of strings */

flags = 0;
language = 0;

*fstatus = fscanf (fptr,scan9,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,presult);

if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sd1,pv1);
if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sr1,pv1);

if (strncmp (pt2,"cd",2) == 0) str$$malloc_sd (&sd2,pv2);
if (strncmp (pt2,"cd",2) == 0) str$$malloc_sd (&sr2,pv2);

flags    = atol ( pv3 );
language = atol ( pv4 );

str$analyze_sdesc(&sd1, &s1_len, &s1_ptr);
str$analyze_sdesc(&sd2, &s2_len, &s2_ptr);

printf ("Testing  str$concat_multi \n");
printf ("Compare two strings using multinational character set \n\n\n");

printf ("Input String    %s \n",pv1);
printf ("Input String    %s \n",pv2);
printf ("Case flag       %lu  ",flags);
switch ( flags ) 
{ 
case 0:	printf ("Case Sensitive\n");
	break;
case 1:	printf ("Ignore Case \n");
	break;
default:printf ("Invalid Flag  \n");
}

printf ("Language        %lu  ",language);
switch ( language ) 
{ 
case 1:	printf ("Multinational\n");
	break;
case 2:	printf ("Danish\n");
	break;
case 3:	printf ("Finish / Swedish\n");
	break;
case 4:	printf ("German\n");
	break;
case 5:	printf ("Norwegian\n");
	break;
case 6:	printf ("Spanish\n");
	break;
default:printf ("Invalid Language selection\n");
}

result = str$compare_multi(&sd1,&sd2,&flags,&language);

printf ("\n\n");
print_result_code (presult,result,cont);
print_desc_compare("String 1",&sd1,&sr1,cont);
print_desc_compare("String 2",&sd2,&sr2,cont);

printf ("\n\n");
printf ("Output String 1 "); str$$print_sd(&sd1); printf("\n");
printf ("Output String 2 "); str$$print_sd(&sd2); printf("\n");
printf ("Output Flag     %lu\n", flags);
printf ("Output Language %lu\n", language);
printf ("\n");

str$free1_dx(&sd1);
str$free1_dx(&sr1);
str$free1_dx(&sd2);
str$free1_dx(&sr2);

return result;
}


/******************************************************/

int	teststrconcat(FILE *fptr,int *fstatus, char *cont)

{
char 		pt[MAX_ARRAY][20],pv[MAX_ARRAY][20],presult[MAX_ARRAY];
struct		dsc$descriptor_s sd[MAX_ARRAY];
int		i, num;
unsigned long	result;
char		*sptr[MAX_ARRAY];		/* Pointer to first string */
unsigned short	slen[MAX_ARRAY];		/* Length of first string */



for (i=0; i< MAX_ARRAY; i++)
{	strcpy (pv[i],"");
	strcpy (pt[i],"");
}

*fstatus = fscanf (fptr,"%d",&num);	// number of arguments to read

for (i =0; i < num; i++)
{
	*fstatus = fscanf (fptr,"%s%s",pt[i],pv[i]);
	if (strncmp (pt[i],"cd",2) == 0)
	{
		str$$malloc_sd (&sd[i],pv[i]);
		str$analyze_sdesc(&sd[i], &slen[i], &sptr[i]);
	}
}
*fstatus = fscanf (fptr,scan1,presult);


printf ("Testing  str$concat \n");
printf ("Concatenate two or more strings \n\n\n");

switch (num)
{
case 2:	
	printf ("Input Destin     %s \n",pv[0]);
	printf ("Input String 1   %s \n",pv[1]);
	printf ("\n");

	result=str$concat(&sd[0],&sd[1],0);

	printf ("\n\n");
	print_result_code (presult,result,cont);

	printf ("\n\n");
	printf ("Output Destin    "); str$$print_sd(&sd[0]); printf ("\n");
	printf ("Output String 1  "); str$$print_sd(&sd[1]); printf ("\n");
	break;
	printf ("\n\n");
case 3:	 
	printf ("Input Destin     %s \n",pv[0]);
	printf ("Input String 1   %s \n",pv[1]);
	printf ("Input String 2   %s \n",pv[2]);
	printf ("\n\n");

	result = str$concat(&sd[0],&sd[1],&sd[2],0);

	print_result_code (presult,result,cont);
	printf ("Output Destin    "); str$$print_sd(&sd[0]); printf ("\n");
	printf ("Output String 1  "); str$$print_sd(&sd[1]); printf ("\n");
	printf ("Output String 2  "); str$$print_sd(&sd[2]); printf ("\n");

	break;
case 4:	 
	printf ("Input Destin     %s \n",pv[0]);
	printf ("Input String 1   %s \n",pv[1]);
	printf ("Input String 2   %s \n",pv[2]);
	printf ("Input String 3   %s \n",pv[3]);
	printf ("\n\n");

	result = str$concat(&sd[0],&sd[1],&sd[2],0);

	print_result_code (presult,result,cont);
	printf ("Output Destin    "); str$$print_sd(&sd[0]); printf ("\n");
	printf ("Output String 1  "); str$$print_sd(&sd[1]); printf ("\n");
	printf ("Output String 2  "); str$$print_sd(&sd[2]); printf ("\n");
	printf ("Output String 3  "); str$$print_sd(&sd[3]); printf ("\n");

	break;

default:
	printf ("rtltest - str$concat - NOT CODED\n");
}

for (i =0; i < num; i++)
{
	if (strncmp (pt[i],"cd",2) == 0)
		str$free1_dx (&sd[i]);
	
}

return result;
}

/*********************************************************/

int	teststrcopy_dx (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result_code;
struct dsc$descriptor_s s1,s2, sr;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char			presult [30];

*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr,pv2);

printf ("Testing str$copy_dx \n");
printf ("Compare 2 descriptors - strings\n\n\n");
printf ("Input Destination   %s \n",pv1);
printf ("Input Source        %s \n",pv2);
printf ("\n");

result_code = str$copy_dx(&s1,&s2);

printf ("\n\n");
print_result_code (presult,result_code,cont);
print_desc_compare ("String ",&s1,&sr,cont);

printf ("\n\n");
printf ("Output Destination  "); str$$print_sd (&s1); printf ("\n");
printf ("Output Source       "); str$$print_sd (&s2); printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&s2);

return result_code;
}

/*********************************************************/

int	teststrcopy_fill (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result_code;
char			*source, *destin;
unsigned int		slen,dlen;
int			fillchar;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char 			pt3[10], pv3[10];
char 			pt4[10], pv4[10];
char 			pt5[10], pv5[10];
char			presult [30];

source = calloc (100,1);
destin = calloc (100,1);

*fstatus = fscanf (fptr,scan11,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,presult);

if ( strncmp(pv1,"NULL",4) == 0 )
{	free (destin);
	destin = NULL;
	dlen = 0;
}
else
{	strcpy (destin,pv1);
	dlen = atol (pv2);
}


if ( strncmp(pv3,"NULL",4) == 0 )
{	free (source);
	source = NULL;
	slen = 0;
}
else
{	strcpy (source,pv3);
	slen = atol (pv4);
}

fillchar = pv5[0];

printf ("Testing str$copy_fill \n");
printf ("Copy a source to a destination string with fill character\n\n\n");

printf ("Input Destination         %s \n",pv1);
printf ("Input Destination length  %d \n",dlen);
printf ("Input Source              %s \n",pv3);
printf ("Input Source length       %d \n",slen);
printf ("Input Fill character      %c \n",fillchar);
printf ("\n");

result_code = str$$copy_fill(destin,dlen,source,slen,fillchar);

print_result_code (presult,result_code,cont);

printf ("\n\n");
printf ("Output Destination         %s \n",destin);
printf ("Output Destination length  %d \n",dlen);
printf ("Output Source              %s \n",source);
printf ("Output Source length       %d \n",slen);
printf ("Output Fill character      %c \n",fillchar);

free (source);
free (destin);

return result_code;
}

/*********************************************************/

int	teststrcopy_r (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 	result_code;
unsigned short	len,s1_len;
struct dsc$descriptor_s s1;
char			string[30],*s1_ptr;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char 			pt3[10], pv3[10];
char			presult [30];

*fstatus = fscanf (fptr,scan7,pt1,pv1,pt2,pv2,pt3,pv3,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);

len = (unsigned short) atol (pv2);
if ( strncmp(pv3,"NULL",4) == 0 )
	string[0] = '0';
else
	strcpy (string,pv3);

printf ("Testing str$copy_r \n");
printf ("Copy a string into a descriptor \n\n");

str$analyze_sdesc (&s1,&s1_len,&s1_ptr);
printf ("Input Descriptor        %s \n",s1_ptr);
printf ("Input Descriptor length %d \n",s1_len);
printf ("Input Descriptor string %s \n",s1_ptr);
printf ("Input String Length     %d \n",len);
printf ("Input String            %s \n",string);
printf ("\n");

result_code = str$copy_r(&s1,&len,string);
str$analyze_sdesc (&s1,&s1_len,&s1_ptr);

print_result_code (presult,result_code,cont);
print_int_compare ("Length ",len,s1_len,cont);

printf ("\n");
printf ("Output Descriptor        "); str$$print_sd (&s1); printf ("\n");
printf ("Output Descriptor length %d \n",s1_len);
printf ("Output Descriptor string %s \n",s1_ptr);
printf ("Output Len               %s \n",pv2);
printf ("Output String            %s \n",pv3);


str$free1_dx (&s1);

return result_code;
}


/*********************************************************/

int	teststrcopy_r_64 (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 	result_code;
unsigned short	len,s1_len;
struct dsc$descriptor_s s1;
char			string[30], *s1_ptr;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char 			pt3[10], pv3[10];
char			presult [30];

*fstatus = fscanf (fptr,scan7,pt1,pv1,pt2,pv2,pt3,pv3,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);

len = (unsigned short) atol (pv2);
if ( strncmp(pv3,"NULL",4) == 0 )
	string[0] = '0';
else
	strcpy (string,pv3);

printf ("Testing str$copy_r_64 \n");
printf ("Copy a string into a 64 bit descriptor \n\n\n");

str$analyze_sdesc (&s1,&s1_len,&s1_ptr);
printf ("Input Descriptor        %s \n",s1_ptr);
printf ("Input Descriptor length %d \n",s1_len);
printf ("Input Descriptor string %s \n",s1_ptr);
printf ("Input String Length     %d \n",len);
printf ("Input String            %s \n",string);
printf ("\n\n");

result_code = str$copy_r_64(&s1,&len,string);

print_result_code (presult,result_code,cont);
print_int_compare ("Length ",len,s1_len,cont);

str$analyze_sdesc (&s1,&s1_len,&s1_ptr);
printf ("\n\n");
printf ("Output Descriptor        "); str$$print_sd (&s1); printf ("\n");
printf ("Output Descriptor length %d \n",s1_len);
printf ("Output Descriptor string %s \n",s1_ptr);
printf ("Output Len               %s \n",pv2);
printf ("Output String            %s \n",pv3);


str$free1_dx (&s1);

return result_code;
}

/*********************************************************/

int	teststrdiv(FILE *fptr,int *fstatus, char *cont)
{
unsigned long result_code;
struct dsc$descriptor_s s1,s2,s3,s4;
unsigned long asign,bsign,csign,rsign,rt;
signed long	aexp,bexp,cexp,rexp,td;
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char 	pt3[10], pv3[10];
char 	pt4[10], pv4[10];
char 	pt5[10], pv5[10];
char 	pt6[10], pv6[10];
char 	pt7[10], pv7[10];
char 	pt8[10], pv8[10];
char 	pt9[10], pv9[10];
char 	pt10[10], pv10[10];
char 	pt11[10], pv11[10];
char	presult [30];

strcpy (pv3,"\n");
strcpy (pv6,"\n");
strcpy (pv9,"\n");

*fstatus = fscanf (fptr,scan23,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,pt6,pv6,pt7,pv7,pt8,pv8,pt9,pv9,pt10,pv10,pt11,pv11,presult);

asign = atol (pv1);
aexp  = atol (pv2);
if (strncmp  (pt3,"cd",2) == 0) str$$malloc_sd (&s1,pv3);

bsign = atol (pv4);
bexp  = atol (pv5);
if (strncmp  (pt6,"cd",2) == 0) str$$malloc_sd (&s2,pv6);

td    = atol (pv7);
rt    = atol (pv8);

csign = atol (pv9);
cexp  = atol (pv10);
if (strncmp  (pt11,"cd",2) == 0) str$$malloc_sd (&s3,pv11);
if (strncmp  (pt11,"cd",2) == 0) str$$malloc_sd (&s4,pv11);

rsign = csign;
rexp  = cexp;

printf ("C = A / B \n\n");
printf ("Sign 0 Positive 1 Negative \n");
printf ("Input A  sign %lu value ",asign); str$$print_sd(&s1);printf(" exp %ld\n",aexp);
printf ("Input B  sign %lu value ",bsign); str$$print_sd(&s2);printf(" exp %ld\n",bexp);
printf ("Input C  sign %lu value ",csign); str$$print_sd(&s3);printf(" exp %ld\n",cexp);
printf ("Total digits  %ld Mask  %lu ",td,rt);
switch (rt) {
case	0:	printf ("Truncate \n");
		break;
case	1:	printf ("Round \n");
		break;
default:	printf ("Invalid Flag\n");
		break;
}

printf ("\n");

result_code=str$divide(&asign,&aexp,&s1,&bsign,&bexp,&s2,&td,&rt,&csign,&cexp,&s3);

print_result_code  (presult,result_code,cont);
print_ls_compare   ("Exponent",cexp,rexp,cont);
print_lu_compare   ("Sign",csign,rsign,cont);
print_desc_compare ("Numeric",&s3,&s4,cont);

printf ("\n\n");
printf ("OUTPUT A sign %lu value ",asign);str$$print_sd(&s1);printf(" exp %ld\n",aexp);
printf ("OUTPUT B sign %lu value ",bsign);str$$print_sd(&s2);printf(" exp %ld\n",bexp);
printf ("OUTPUT C sign %lu value ",csign);str$$print_sd(&s3);printf(" exp %ld\n",cexp);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&s3);
str$free1_dx (&s4);

return result_code;
}

/*********************************************************/

int	teststrdupl_char (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[20],pv1[20],presult[30];
char 		pt2[20],pv2[20];
char 		pt3[20],pv3[20];
struct		dsc$descriptor_s sd1;
unsigned long	result;
signed long	rep;
char		dupchar;


*fstatus = fscanf (fptr,scan7,pt1,pv1,pt2,pv2,pt3,pv3,presult);

if (strncmp (pt1,"cd",2) == 0)
	str$$malloc_sd (&sd1,pv1);

if ( pv3[0] == '0' )
	dupchar = ' ';
else
	dupchar = pv3[0];

if ( pv2[0] == '0' )
	rep = 1;
else
	rep	= atol (pv2);

printf ("Testing  str$dupl_char \n");
printf ("Duplcicat char n times \n\n\n");

printf ("Input Descriptor  %s \n",pv1);
printf ("Input Char        %s \n",pv3);
printf ("Input Repetion    %s \n",pv2);
printf ("\n\n");

result = str$dupl_char(&sd1,&rep,&dupchar);

print_result_code (presult,result,cont);
printf ("\n\n");

printf ("Output Descriptor ["); str$$print_sd (&sd1); printf ("]\n");
if (  dupchar == ' ')
	printf ("Output Char        Blank \n");
else
	printf ("Output Char        %c  \n",dupchar);
printf ("Output Repetion    %ld \n",rep);
printf ("\n\n");
printf ("\n\n");


return result;
}


/*********************************************************/

int	teststrelement (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[20],pv1[20],presult[30];
char 		pt2[20],pv2[20];
char 		pt3[20],pv3[20];
char 		pt4[20],pv4[20];
long		element_number;
unsigned long	result_code;
struct dsc$descriptor_s		destin, delimiter, source;

*fstatus = fscanf (fptr,scan9,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,presult);

if (strncmp (pt1,"cd",2) == 0)
	str$$malloc_sd (&destin,pv1);

element_number = pv2[0] - '0';

if (strncmp (pt3,"cd",2) == 0)
	str$$malloc_sd (&delimiter,pv3);

if (strncmp (pt4,"cd",2) == 0)
	str$$malloc_sd (&source,pv4);

printf ("Input Descriptor Destin    %s \n",pv1);
printf ("Input Element Number       %s \n",pv2);
printf ("Input Descriptor Delimiter %s \n",pv3);
printf ("Input Descriptor Source    %s \n",pv4);
printf ("\n\n");


result_code = str$element (&destin, &element_number, &delimiter, &source);

print_result_code (presult,result_code,cont);
printf ("\n\n");

printf ("Output Descriptor Destin    "); str$$print_sd (&destin); printf ("\n");
printf ("Output Element Number       %ld \n",element_number);
printf ("Output Descriptor Delimiter ");str$$print_sd(&delimiter);printf ("\n");
printf ("Output Descriptor Source    "); str$$print_sd(&source); printf ("\n");
printf ("\n\n");

result_code = 0;

str$free1_dx (&destin);
str$free1_dx (&delimiter);
str$free1_dx (&source);

return result_code;
}

/*********************************************************/

int	teststrfind_first_in_set (FILE *fptr,int *fstatus, char *cont)
{
long	 		expect_result,result_position;
struct dsc$descriptor_s s1,s2;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char			result [10];
*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,result);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
expect_result = atol (result);

printf ("Testing str$find_first_in_set \n");
printf ("Returns position of first character matched\n\n\n");

printf ("Input Expected        %s \n",result);
printf ("Input Source          %s \n",pv1);
printf ("Input Search Set      %s \n",pv2);
printf ("\n");

result_position = str$find_first_in_set(&s1,&s2);

print_int_compare ("Position",expect_result,result_position,cont);

printf ("\n\n");
printf ("Output Position       %ld \n",result_position);
printf ("Output Source         "); str$$print_sd (&s1); printf ("\n");
printf ("Output Search Set     "); str$$print_sd (&s2); printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&s2);

return SS$_NORMAL;
}


/*********************************************************/

int	teststrfind_first_not_in_set (FILE *fptr,int *fstatus, char *cont)
{
long	 		expect_result,result_position;
struct dsc$descriptor_s s1,s2;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char			result [10];
*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,result);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
expect_result = atol (result);

printf ("Testing str$find_first_not_in_set \n");
printf ("Returns position of first character not matched\n\n\n");

printf ("Input Expected        %s \n",result);
printf ("Input Source          %s \n",pv1);
printf ("Input Search Set      %s \n",pv2);
printf ("\n");

result_position = str$find_first_not_in_set(&s1,&s2);

print_int_compare ("Position",expect_result,result_position,cont);

printf ("\n\n");
if ( result_position == 0 )
	printf ("Output is 0 all match \n");
printf ("Output Position       %ld \n",result_position);
printf ("Output Source         "); str$$print_sd (&s1); printf ("\n");
printf ("Output Search Set     "); str$$print_sd (&s2); printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&s2);

return SS$_NORMAL;

}

/**********************************************/


int teststrfind_first_substring (FILE *fptr,int *fstatus, char *cont)
{
char 		pt[MAX_ARRAY][20],pv[MAX_ARRAY][20],presult[MAX_ARRAY];
struct		dsc$descriptor_s sd[MAX_ARRAY];
int		i, num;
unsigned long	result, expect_result;
signed long	index, subindex, saveindex, savesubindex;
char		*sptr[MAX_ARRAY];		/* Pointer to first string */
unsigned short	slen[MAX_ARRAY];		/* Length of first string */


for (i=0; i< MAX_ARRAY; i++)
{	strcpy (pv[i],"");
	strcpy (pt[i],"");
}

*fstatus = fscanf (fptr,"%d",&num);	// number of arguments to read

for (i =0; i < num; i++)
{
	*fstatus = fscanf (fptr,"%s%s",pt[i],pv[i]);
	if (strncmp (pt[i],"cd",2) == 0)
	{
		str$$malloc_sd (&sd[i],pv[i]);
		str$analyze_sdesc(&sd[i], &slen[i], &sptr[i]);
	}
}
*fstatus = fscanf (fptr,scan1,presult);
expect_result = atol (presult);

printf ("Testing  str$find_first_substring \n");
printf ("Multiple substring are tested \n\n\n");

index = 0;
subindex = 0;
switch (num)
{
case 4:	
	saveindex    = atol (pv[1]);
	savesubindex = atol (pv[2]);
	printf ("Input String     %s \n",pv[0]);
	printf ("Input Index      %s \n",pv[1]);
	printf ("Input Subindex   %s \n",pv[2]);
	printf ("Input Substring  %s \n",pv[3]);
	printf ("\n");

	result=str$find_first_substring(&sd[0],&index,&subindex,&sd[3],0);

	print_lu_compare ("Return",expect_result,result,cont);
	print_ls_compare ("Index",index,saveindex,cont);
	print_ls_compare ("SubIndex",subindex,savesubindex,cont);
	printf ("\n");

	printf ("Output String     ");str$$print_sd (&sd[0]); printf ("\n");
	printf ("Output Index      %ld \n",index);
	printf ("Output Subindex   %ld \n",subindex);
	printf ("Output Substring  ");str$$print_sd (&sd[3]);printf ("\n");
	printf ("\n");

	break;
case 5:	 
	saveindex    = atol (pv[1]);
	savesubindex = atol (pv[2]);
	printf ("Input String    %s \n",pv[0]);
	printf ("Input Index     %s \n",pv[1]);
	printf ("Input Subindex  %s \n",pv[2]);
	printf ("Input Substring %s \n",pv[3]);
	printf ("Input Substring %s \n",pv[4]);
	printf ("\n\n");

     result = str$find_first_substring(&sd[0],&index,&subindex,&sd[3],&sd[4],0);

	print_lu_compare ("Result",expect_result,result,cont);
	print_ls_compare ("Index",index,saveindex,cont);
	print_ls_compare ("SubIndex",subindex,savesubindex,cont);
	printf ("\n");

	printf ("Output String    "); str$$print_sd(&sd[0]); printf ("\n");
	printf ("Output Index     %ld \n",index);
	printf ("Output Subindex  %ld \n",subindex);
	printf ("Output Substring "); str$$print_sd(&sd[3]); printf("\n");
	printf ("Output Substring "); str$$print_sd(&sd[4]); printf("\n");
	printf ("\n\n");

	break;
case 6: 
	saveindex    = atol (pv[1]);
	savesubindex = atol (pv[2]);
	printf ("Input String    %s \n",pv[0]);
	printf ("Input Index     %s \n",pv[1]);
	printf ("Input Subindex  %s \n",pv[2]);
	printf ("Input Substring %s \n",pv[3]);
	printf ("Input Substring %s \n",pv[4]);
	printf ("Input Substring %s \n",pv[5]);
	printf ("\n");

	result = str$find_first_substring(&sd[0],&index,&subindex,&sd[3],&sd[4],&sd[5],0);

	print_lu_compare ("Result",expect_result,result,cont);
	print_ls_compare ("Index",index,saveindex,cont);
	print_ls_compare ("SubIndex",subindex,savesubindex,cont);
	printf ("\n");

	printf ("Output String    "); str$$print_sd(&sd[0]); printf("\n");
	printf ("Output Index     %ld \n",index);
	printf ("Output Subindex  %ld \n",subindex);
	printf ("Output Substring "); str$$print_sd(&sd[3]); printf("\n");
	printf ("Output Substring "); str$$print_sd(&sd[4]); printf("\n");
	printf ("Output Substring "); str$$print_sd(&sd[5]); printf("\n");
	printf ("\n");
	break;
default:
	printf ("rtltest - str$find_first_substring - NOT CODED\n");
}

for (i =0; i < num; i++)
{	if (strncmp (pt[i],"cd",2) == 0)
	{	str$free1_dx (&sd[i]);
	}
}

return result;
}


/**********************************************/


int teststrfree1_dx     (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[6],pv1[50],presult[50], *s1_ptr;
unsigned short 	s1_len;
struct		dsc$descriptor_s sd1;
unsigned long	result_code;


*fstatus = fscanf (fptr,scan3,pt1,pv1,presult);

if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sd1,pv1);

printf ("Testing str$free1_dx \n");
printf ("Free memory and set length to 0 \n");
printf ("\n\n");

printf ("Input String %s \n",pv1);
printf ("\n");

result_code = str$free1_dx (&sd1);

print_result_code (presult,result_code,cont);

printf ("\n\n");
str$analyze_sdesc (&sd1,&s1_len, &s1_ptr);
printf ("Output String ["); str$$print_sd (&sd1); printf ("]\n");
printf ("Output Length  %d \n",s1_len);
printf ("Output String [%s] \n",s1_ptr);

str$free1_dx (&sd1);

return result_code;
}

/**********************************************/


int teststrget1_dx (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[6],pv1[6],pt2[50],pv2[50],presult[50], *s1_ptr;
struct		dsc$descriptor_s sd1;
unsigned short	len, s1_len;
unsigned long	result_code;


*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

len = atol (pv1);
if (strncmp (pt2,"cd",2) == 0)
	str$$malloc_sd (&sd1,pv2);

printf ("Testing str$get1_dx \n");
printf ("Allocate memory to a descriptor \n");
printf ("\n\n");

printf ("Input String %s \n",pv1);
printf ("Input Descriptor "); str$$print_sd (&sd1); printf ("\n");
printf ("\n\n");

result_code = str$get1_dx (&len,&sd1);
str$analyze_sdesc (&sd1,&s1_len, &s1_ptr);

print_result_code (presult,result_code,cont);
if ( (s1_len == 0 ) && (s1_ptr != NULL) )
{	printf ("Memory Address ..FAILED Length is zero have valid address \n");
	strcpy (cont,"P");
}
printf ("\n\n");

printf ("Output String ["); str$$print_sd (&sd1); printf ("]\n");
printf ("Output Length  %d \n",s1_len);
printf ("\n");

str$free1_dx (&sd1);

return result_code;
}


/**********************************************/


int teststrget1_dx_64     (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[6],pv1[6],pt2[50],pv2[50],presult[50], *s1_ptr;
struct		dsc$descriptor_s sd1;
unsigned short	len, s1_len;
unsigned long	result_code;


*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

len = atol (pv1);
if (strncmp (pt2,"cd",2) == 0)
	str$$malloc_sd (&sd1,pv2);

printf ("Testing str$get1_dx_64 \n");
printf ("Allocate memory to a descriptor \n");
printf ("\n\n");

printf ("Input String %s \n",pv1);
printf ("Input Descriptor "); str$$print_sd (&sd1); printf ("\n");
printf ("\n\n");

result_code = str$get1_dx_64 (&len,&sd1);

print_result_code (presult,result_code,cont);
str$analyze_sdesc (&sd1,&s1_len, &s1_ptr);
if ( (s1_len == 0 ) && (s1_ptr != NULL) )
{	printf ("Memory Address ..FAILED Length is zero have valid address \n");
	strcpy (cont,"P");
}
printf ("\n\n");

printf ("Output String ["); str$$print_sd (&sd1); printf ("]\n");
printf ("Output Length  %d \n",s1_len);
printf ("\n");

str$free1_dx (&sd1);

return result_code;
}

/**********************************************/


int teststriszero     (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[6],pv1[6],presult[50];
struct		dsc$descriptor_s sd1;
unsigned long	result_code;


*fstatus = fscanf (fptr,scan3,pt1,pv1,presult);

if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sd1,pv1);

printf ("Testing str$$iszero \n");
printf ("Test if a descriptor is zero \n");
printf ("\n\n");

printf ("Input String %s \n",pv1);

result_code = str$$iszero (&sd1);

print_result_code (presult,result_code,cont);

printf ("\n\n");
printf ("Output String "); str$$print_sd (&sd1); printf ("\n");

str$free1_dx (&sd1);

return result_code;
}

/****************************************************/

int teststriszerotrim     (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[6],pv1[6],presult[50];
char 		pt2[6],pv2[6];
struct		dsc$descriptor_s sd1;
unsigned long	result_code;
long		exponent;


*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sd1,pv1);
exponent = atol (pv2);

printf ("Testing str$$iszerotrim \n");
printf ("Test if a descriptor is zero \n");
printf ("\n\n");

printf ("Input String   %s \n",pv1);
printf ("Input Exponent %ld \n",exponent);

result_code = str$$iszerotrim (&sd1,&exponent);

print_result_code (presult,result_code,cont);

printf ("\n\n");
printf ("Output String   "); str$$print_sd (&sd1); printf ("\n");
printf ("Output Exponent %ld \n",exponent);

str$free1_dx (&sd1);

return result_code;
}

/*********************************************************/

int	teststrleft(FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result_code;
struct dsc$descriptor_s s1,s2,sr;
signed long		endpos;
unsigned short		s1_len;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char 			pt3[10], pv3[10];
char			presult [30], *s1_ptr;

*fstatus = fscanf (fptr,scan7,pt1,pv1,pt2,pv2,pt3,pv3,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr,pv1);
endpos = atol (pv3);

printf ("Testing str$left \n");
printf ("Extract left substring \n\n\n");

printf ("Input A          %s \n",pv1);
printf ("Input B          %s \n",pv2);
printf ("Input End Point  %ld   \n",endpos);
printf ("\n");

result_code = str$left(&s1,&s2,&endpos);
str$analyze_sdesc (&s1,&s1_len,&s1_ptr);

print_result_code (presult,result_code,cont);
//print_desc_compare ("Digit",&sr,&s1,cont);
print_int_compare  ("Length",endpos,s1_len,cont);

printf ("\n\n");
printf ("Output A         ["); str$$print_sd (&s1); printf ("]\n");
printf ("Output B         ["); str$$print_sd (&s2); printf ("]\n");
printf ("Output End Point %ld   \n",endpos);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&sr);

return result_code;
}

/*********************************************************/

int	teststrlen_extr (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result_code;
struct dsc$descriptor_s s1,s2,sr;
signed long		startpos,len;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char 			pt3[10], pv3[10];
char 			pt4[10], pv4[10];
char			presult [30];

*fstatus = fscanf (fptr,scan9,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr,pv1);
startpos = atol (pv3);
len	= atol (pv4);

printf ("Testing str$len_extr \n");
printf ("Extract a substring by length \n\n\n");

printf ("Input Destination    %s \n",pv1);
printf ("Input Source         %s \n",pv2);
printf ("Input Start Point    %ld \n",startpos);
printf ("Input Length         %ld \n",len);
printf ("\n\n");

result_code = str$len_extr(&s1,&s2,&startpos,&len);

print_result_code (presult,result_code,cont);
print_desc_compare ("Digit",&sr,&s1,cont);

printf ("\n\n");
printf ("Output Destination   "); str$$print_sd (&s1); printf ("\n");
printf ("Output Source        "); str$$print_sd (&s2); printf ("\n");
printf ("Output Start Point   %ld \n",startpos);
printf ("Output Length        %ld \n",len);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&sr);

return result_code;
}


/************************************************/


int teststrlzerotrim  (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[6],pv1[6],presult[50];
struct		dsc$descriptor_s sd1;
unsigned long	result_code;


*fstatus = fscanf (fptr,scan3,pt1,pv1,presult);

if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sd1,pv1);

printf ("Testing str$$lzerotrim \n");
printf ("Trim leading zero's from a string \n\n\n");

printf ("Input String    %s \n",pv1);
printf ("\n");

result_code = str$$lzerotrim (&sd1);

print_result_code (presult,result_code,cont);

printf ("\n\n");
printf ("Output String    "); str$$print_sd (&sd1); printf ("\n");

str$free1_dx (&sd1);

return result_code;
}

/********************************************/

int	teststrmatch_wild (FILE *fptr,int *fstatus, char *cont)
{
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char	presult[30];
unsigned long result_code;
struct dsc$descriptor_s s1,s2;

	*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

	if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
	if (strncmp (pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);

	printf ("Testing str$match_wild \n");
	printf ("String matching with wildcards \n\n\n");

	printf ("Input candidate string is %s \n",pv1);
	printf ("Input pattern string is   %s \n",pv2);
	printf ("\n\n");

	result_code = str$match_wild (&s1,&s2);

	print_result_code (presult,result_code,cont);

	printf ("\n\n\n");
	printf ("Output candidate string "); str$$print_sd(&s1);printf ("\n");
	printf ("Output Pattern string   "); str$$print_sd(&s2);printf ("\n\n");
	
	str$free1_dx (&s1);
	str$free1_dx (&s2);

	return result_code;
}


/********************************************/

int	teststrmul(FILE *fptr,int *fstatus, char *cont)
{
unsigned long result_code;
struct dsc$descriptor_s s1,s2,s3,sr;
unsigned long asign,bsign,csign,rsign;
signed long	aexp,bexp,cexp,rexp;
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char 	pt3[10], pv3[10];
char 	pt4[10], pv4[10];
char 	pt5[10], pv5[10];
char 	pt6[10], pv6[10];
char 	pt7[10], pv7[10];
char 	pt8[10], pv8[10];
char 	pt9[10], pv9[10];
char	presult [30];

strcpy (pv3,"\n");
strcpy (pv6,"\n");
strcpy (pv9,"\n");

*fstatus = fscanf (fptr,scan19,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,pt6,pv6,pt7,pv7,pt8,pv8,pt9,pv9,presult);

if (strncmp (pt3,"cd",2) == 0) str$$malloc_sd (&s1,pv3);
if (strncmp (pt6,"cd",2) == 0) str$$malloc_sd (&s2,pv6);
if (strncmp (pt9,"cd",2) == 0) str$$malloc_sd (&s3,pv9);
if (strncmp (pt9,"cd",2) == 0) str$$malloc_sd (&sr,pv9);

asign = atol (pv1);
bsign = atol (pv4);
csign = atol (pv7);
rsign = csign;

aexp = atol(pv2);
bexp = atol(pv5);
cexp = atol(pv8);
rexp = cexp;

printf ("Testing str$mul \n");
printf ("C = A * B \n\n\n");
printf ("Input A sign %lu value ",asign); str$$print_sd(&s1); printf(" exp %ld\n",aexp);
printf ("Input B sign %lu value ",bsign); str$$print_sd(&s2); printf(" exp %ld\n",bexp);
printf ("Input C sign %lu value ",csign); str$$print_sd(&s3); printf(" exp %ld\n",cexp);
printf ("\n");

result_code=str$mul(&asign,&aexp,&s1,&bsign,&bexp,&s2,&csign,&cexp,&s3);

print_result_code  (presult,result_code,cont);
print_ls_compare   ("Exponent",cexp,rexp,cont);
print_lu_compare   ("Sign",csign,rsign,cont);
print_desc_compare ("Digits",&s3,&sr,cont);

printf ("\n\n");
printf ("Output A sign %lu value ",asign);str$$print_sd(&s1); printf(" exp %ld\n",aexp);
printf ("Output B sign %lu value ",bsign);str$$print_sd(&s2); printf(" exp %ld\n",bexp);
printf ("Output C sign %lu value ",csign);str$$print_sd(&s3); printf(" exp %ld\n",cexp);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&s3);
str$free1_dx (&sr);

return result_code;
}


/*************************************************/

int	teststrncompare(FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result_code;
struct dsc$descriptor_s s1,s2;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char			presult [30];

*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s2,pv2);

printf ("Testing str$$ncompare \n");
printf ("String Numeric Compare \n\n\n");
printf ("Input String 1   %s \n",pv1);
printf ("Input String 2   %s \n",pv2);
printf ("\n");

result_code = str$$ncompare(&s1,&s2);

print_result_code (presult,result_code,cont);

printf ("\n\n");
printf ("Output String 1   "); str$$print_sd (&s1); printf ("\n");
printf ("Output String 2   "); str$$print_sd (&s2); printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&s2);

return result_code;
}

/*********************************************************/

int	teststrpos_extr (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result;
struct dsc$descriptor_s s1,s2,sr;
signed long		startpos,endpos;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char 			pt3[10], pv3[10];
char 			pt4[10], pv4[10];
char			presult [30];

*fstatus = fscanf (fptr,scan9,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr,pv1);
startpos = atol (pv3);
endpos   = atol (pv4);

printf ("Testing str$pos_extr \n");
printf ("Extract a substring by position\n\n\n");

printf ("Input Destination      %s \n",pv1);
printf ("Input Source           %s \n",pv2);
printf ("Input Start Position   %ld \n",startpos);
printf ("Input End Position     %ld \n",endpos);
printf ("\n");

result = str$pos_extr(&s1,&s2,&startpos,&endpos);

print_result_code (presult,result,cont);

printf ("\n\n");
printf ("Output Destination     ["); str$$print_sd (&s1); printf ("]\n");
printf ("Output Source           "); str$$print_sd (&s2); printf ("\n");
printf ("Output Start Position   %ld \n",startpos);
printf ("Output End Position     %lu \n",endpos);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&sr);

return result;
}

/*********************************************************/


int	teststrposition(FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[20],pv1[20],presult[MAX_ARRAY];
char 		pt2[20],pv2[20];
char 		pt3[20],pv3[20];
struct		dsc$descriptor_s sd1, sd2;
unsigned long	result, expected_result;
signed long	startpos; 


*fstatus = fscanf (fptr,scan7,pt1,pv1,pt2,pv2,pt3,pv3,presult);

if (strncmp (pt1,"cd",2) == 0 ) str$$malloc_sd (&sd1,pv1);
if (strncmp (pt2,"cd",2) == 0 ) str$$malloc_sd (&sd2,pv2);
startpos = atol (pv3);
expected_result = atol (presult);

printf ("Testing  str$position \n");
printf ("Find a substring within a string option start position \n\n\n");


printf ("Input Source String      %s \n",pv1);
printf ("Input Substring          %s \n",pv2);
printf ("Input Start Position     %ld \n",startpos);
printf ("\n\n");

result = str$position(&sd1,&sd2,&startpos);

printf ("Output Results Position  %ld \n",result);
printf ("\n");

print_int_compare ("Position",(signed) expected_result,(signed) result,cont);
printf ("\n");

printf ("Output Source String     "); str$$print_sd(&sd1); printf ("\n");
printf ("Output Substring         "); str$$print_sd(&sd2); printf ("\n");
printf ("Output Start Position    %ld \n",startpos);
printf ("\n\n");

str$free1_dx (&sd1);
str$free1_dx (&sd2);

return result;
}





/******************************************************/

int	teststrprefix(FILE *fptr,int *fstatus, char *cont)

{
unsigned long result_code;
struct dsc$descriptor_s s1,s2;
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char	presult [30];


	*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

	if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
	if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);

	printf ("Testing str$prefix \n");
	printf ("Prefix a string to another \n\n\n");

	printf("Input Destination %s\n",pv1);
	printf("Input Source      %s\n",pv2);
	printf ("\n");

	result_code = str$prefix(&s1,&s2);

	print_result_code (presult, result_code,cont);

	printf ("\n\n");
	printf ("Output String A "); str$$print_sd(&s1); printf ("\n");
	printf ("Output String B "); str$$print_sd(&s2); printf ("\n");

	str$free1_dx (&s1);
	str$free1_dx (&s2);

	return (int) result_code;
}


/********************************************/


int	teststrrecip (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 	result_code;
struct dsc$descriptor_s s1,s2,s3, sr;
unsigned long 	asign,bsign,csign, rsign;
signed   long	aexp,bexp,cexp, rexp;
char		presult [30];
char 		pt1[10], pv1[10];
char 		pt2[10], pv2[10];
char 		pt3[10], pv3[10];
char 		pt4[10], pv4[10];
char 		pt5[10], pv5[10];
char 		pt6[10], pv6[10];
char 		pt7[10], pv7[10];
char 		pt8[10], pv8[10];
char 		pt9[10], pv9[10];

printf ("Testing str$recip \n");

*fstatus = fscanf (fptr,scan19,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,pt6,pv6,pt7,pv7,pt8,pv8,pt9,pv9,presult);

if (strncmp ( pt3,"cd",2) == 0) str$$malloc_sd (&s1,pv3);
if (strncmp ( pt6,"cd",2) == 0) str$$malloc_sd (&s2,pv6);
if (strncmp ( pt9,"cd",2) == 0) str$$malloc_sd (&s3,pv9);
if (strncmp ( pt9,"cd",2) == 0) str$$malloc_sd (&sr,pv9);

asign = atol (pv1);
bsign = atol (pv4);
csign = atol (pv7);

aexp = atol(pv2);
bexp = atol(pv5);
cexp = atol(pv8);

rexp  = cexp;		// Save expected return exponent
rsign = csign;		// Save expected return sign

printf ("Testing str$recip \n");
printf ("C = 1 / A \n\n\n");
printf ("Input A    sign %lu value ",asign); str$$print_sd(&s1);printf(" exp %ld\n",aexp);
printf ("Precision  sign %lu value ",bsign); str$$print_sd(&s2);printf(" exp %ld\n",bexp);
printf ("Input C    sign %lu value ",csign); str$$print_sd(&s3);printf(" exp %ld\n",cexp);
printf ("\n");

result_code = str$recip(&asign,&aexp,&s1, &bsign,&bexp,&s2, &csign,&cexp,&s3);

print_result_code  (presult,result_code,cont);
print_ls_compare   ("Exponent",cexp,rexp,cont);
print_lu_compare   ("Sign",csign,rsign,cont);
print_desc_compare ("Digits",&s3,&sr,cont);

printf ("\n\n");
printf ("OUTPUT A sign %lu value ",asign);str$$print_sd(&s1);printf(" exp %ld\n",aexp);
printf ("OUTPUT B sign %lu value ",bsign);str$$print_sd(&s2);printf(" exp %ld\n",bexp);
printf ("OUTPUT C sign %lu value ",csign);str$$print_sd(&s3);printf(" exp %ld\n",cexp);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&s3);
str$free1_dx (&sr);

return result_code;
}



/******************************************************/

int	teststrreplace (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 	result_code;
struct dsc$descriptor_s s1,s2,s3, sr;
signed   long	start, end;
char		presult [30];
char 		pt1[50], pv1[50];
char 		pt2[50], pv2[50];
char 		pt3[50], pv3[50];
char 		pt4[50], pv4[50];
char 		pt5[50], pv5[50];


*fstatus = fscanf (fptr,scan11,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
start = atol (pv3);
end   = atol (pv4);
if (strncmp ( pt5,"cd",2) == 0) str$$malloc_sd (&s3,pv5);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr,pv1);


printf ("str$replace \n\n\n");
printf ("Input  Destination "); str$$print_sd(&s1);printf(" \n");
printf ("       Source      "); str$$print_sd(&s2);printf(" \n");
printf ("       Positions   %ld : %ld \n",start,end);
printf ("       Replacement "); str$$print_sd(&s3);printf("\n");
printf ("\n");

result_code = str$replace(&s1,&s2,&start, &end,&s3);

print_result_code  (presult, result_code,cont);
print_desc_compare ("String",&s1,&sr,cont);

printf ("\n\n");
printf ("OUTPUT Destination "); str$$print_sd(&s1); printf("\n");
printf ("OUTPUT Source .... "); str$$print_sd(&s2); printf("\n");
printf ("OUTPUT Replacement "); str$$print_sd(&s3); printf("\n");

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&s3);
str$free1_dx (&sr);

return result_code;
}

/******************************************************/

int	teststrright(FILE *fptr,int *fstatus, char *cont)

{
unsigned long result_code;
long	startpos;
struct dsc$descriptor_s s1,s2, sr;
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char 	pt3[10], pv3[10];
char	presult [30];


	*fstatus = fscanf (fptr,scan7,pt1,pv1,pt2,pv2,pt3,pv3,presult);

	startpos = atol (pv3);

	if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
	if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr,pv1);
	if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);

	printf ("Testing str$right \n");
	printf ("Return right portion of a string \n\n");
	printf ("Input Source      [%.10s]\n",pv2);
	printf ("Input Destination [%.10s]\n",pv1);
	printf ("Input Start pos   [%ld]  \n\n\n",startpos);

	result_code = str$right(&s1,&s2,&startpos);
	
	print_result_code (presult,result_code,cont);
	print_desc_compare("String",&sr,&s1,cont);

	printf ("\n\n");
	printf ("Output Source       "); str$$print_sd (&s2); printf ("\n");
	printf ("Output Destination  "); str$$print_sd (&s1); printf ("\n");
	printf ("Output Start pos   [%ld]  \n\n\n",startpos);

	
	return  result_code;
}

/******************************************************/

int	teststrround (FILE *fptr,int *fstatus, char *cont)
{

char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char 	pt3[10], pv3[10];
char 	pt4[10], pv4[10];
char 	pt5[10], pv5[10];
char 	pt6[10], pv6[10];
char 	pt7[10], pv7[10];
char 	pt8[10], pv8[10];
char	presult [30];
unsigned long asign,csign;
unsigned long result_code, flags;
signed	 long aexp, cexp, places;
struct dsc$descriptor_s adigits,cdigits, sr;


*fstatus = fscanf (fptr,scan17,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,pt6,pv6,pt7,pv7,pt8,pv8,presult);

	places=	atol (pv1);
	flags =	atol (pv2);
	asign = atol (pv3);
	aexp = 	atol (pv4);
	if (strncmp ( pt5,"cd",2) == 0) str$$malloc_sd (&adigits,pv5);
	if (strncmp ( pt5,"cd",2) == 0) str$$malloc_sd (&sr,pv5);
	csign =	atol (pv6);
	cexp =	atol (pv7);
	if (strncmp ( pt8,"cd",2) == 0) str$$malloc_sd (&cdigits,pv8);

	printf ("Testing str$round \n");
	printf ("Round a value\n\n");
	printf ("Input Source       sign %lu value  ",asign);
	str$$print_sd(&adigits);
	printf(" exp %ld\n",aexp);

	printf ("Input Destination  sign %lu value  ",csign);
	str$$print_sd(&cdigits);
	printf(" exp %ld\n",cexp);

	printf ("Input Precision  %lu ",places);

	switch ( flags ) {
	case 0:	printf (" Flags 0  (Round) \n");
		break;
	case 1:	printf (" Flags 1  (Truncate) \n");
		break;
	default:printf ("Unknown Flag type %lu \n",flags);
	}
	printf ("\n");

	result_code = 	str$round (&places,&flags,&asign,&aexp,&adigits,&csign,&cexp,&cdigits);

	print_result_code (presult, result_code,cont);
	print_desc_compare ("Digits",&sr,&adigits,cont);

	printf ("\n\n");
	printf ("Output Source        sign %lu value ",asign);
	str$$print_sd(&adigits);
	printf (" exp %ld\n",aexp);

	printf ("Output Destination   sign %lu value ",csign);
	str$$print_sd(&cdigits);
	printf (" exp %ld\n",cexp);

	printf ("Output Precision  %lu ",places);

	switch ( flags ) {
	case 0:	printf (" Flags 0  (Round) \n");
		break;
	case 1:	printf (" Flags 1  (Truncate) \n");
		break;
	default:printf ("Unknown Flag type %lu \n",flags);
	}

	str$free1_dx (&adigits);
	str$free1_dx (&cdigits);
	str$free1_dx (&sr);

	return (int) result_code;

}





/************************************************/


int teststrrzerotrim (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[6],pv1[6],pt2[255],pv2[255],presult[50];
struct		dsc$descriptor_s sd1;
unsigned long	result;
signed long	exp;


*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

if (strncmp (pt1,"cd",2) == 0) str$$malloc_sd (&sd1,pv1);
exp = atol (pv2);

printf ("Testing str$$xtrailzero \n");
printf ("Remove trailing zero's and increment exponent \n");

printf ("Input String   %s \n",pv1);
printf ("Input Exponent %s \n",pv2);

result	= str$$rzerotrim (&sd1, &exp);

print_result_code (presult,result,cont);

printf ("\n\n");
printf ("Output String "); str$$print_sd (&sd1); printf ("\n");
printf ("Output Exponent %ld \n",exp);

str$free1_dx (&sd1);

return result;
}

/********************************************/

int	teststrsub(FILE *fptr,int *fstatus, char *cont)
{
unsigned long result_code;
struct dsc$descriptor_s s1,s2,s3,sr;
unsigned long asign,bsign,csign,rsign;
signed long	aexp,bexp,cexp,rexp;
char 	pt1[10], pv1[10];
char 	pt2[10], pv2[10];
char 	pt3[10], pv3[10];
char 	pt4[10], pv4[10];
char 	pt5[10], pv5[10];
char 	pt6[10], pv6[10];
char 	pt7[10], pv7[10];
char 	pt8[10], pv8[10];
char 	pt9[10], pv9[10];
char	presult [30];

strcpy (pv3,"\n");
strcpy (pv6,"\n");
strcpy (pv9,"\n");

*fstatus = fscanf (fptr,scan19,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,pt5,pv5,pt6,pv6,pt7,pv7,pt8,pv8,pt9,pv9,presult);

if (strncmp (pt3,"cd",2) == 0) str$$malloc_sd (&s1,pv3);
if (strncmp (pt6,"cd",2) == 0) str$$malloc_sd (&s2,pv6);
if (strncmp (pt9,"cd",2) == 0) str$$malloc_sd (&s3,pv9);
if (strncmp (pt9,"cd",2) == 0) str$$malloc_sd (&sr,pv9);

asign = atol (pv1);
bsign = atol (pv4);
csign = atol (pv7);
rsign = csign;

aexp = atol(pv2);
bexp = atol(pv5);
cexp = atol(pv8);
rexp = cexp;

printf ("Testing str$sub \n");
printf ("C = A - B \n\n\n");
printf ("Input A  sign %lu value ",asign);str$$print_sd(&s1);printf (" exp %ld\n",aexp);
printf ("Input B  sign %lu value ",bsign);str$$print_sd(&s2);printf (" exp %ld\n",bexp);
printf ("Input C  sign %lu value ",csign);str$$print_sd(&s3);printf (" exp %ld\n",cexp);
printf ("\n");

result_code=str$sub(&asign,&aexp,&s1,&bsign,&bexp,&s2,&csign,&cexp,&s3);

print_result_code  (presult, result_code,cont);
print_ls_compare   ("Exponent",cexp,rexp,cont);
print_lu_compare   ("Sign",csign,rsign,cont);
print_desc_compare ("Digits",&s3,&sr,cont);

printf ("\n\n");
printf ("Output A sign %lu value ",asign);str$$print_sd(&s1);printf(" exp %ld\n",aexp);
printf ("Output B sign %lu value ",bsign);str$$print_sd(&s2);printf(" exp %ld\n",bexp);
printf ("Output C sign %lu value ",csign);str$$print_sd(&s3);printf(" exp %ld\n",cexp);

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&s3);
str$free1_dx (&sr);

return result_code;
}


/*********************************************************/

int	teststrtranslate (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result_code;
struct dsc$descriptor_s s1,s2,s3,s4,sr;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char 			pt3[10], pv3[10];
char 			pt4[10], pv4[10];
char			presult [30];

*fstatus = fscanf (fptr,scan9,pt1,pv1,pt2,pv2,pt3,pv3,pt4,pv4,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt2,"cd",2) == 0) str$$malloc_sd (&s2,pv2);
if (strncmp ( pt3,"cd",2) == 0) str$$malloc_sd (&s3,pv3);
if (strncmp ( pt4,"cd",2) == 0) str$$malloc_sd (&s4,pv4);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&sr,pv1);

printf ("Testing str$translate \n");
printf ("Translates a characters string\n\n");

printf ("Input Destination  %s \n",pv1);
printf ("Input Source       %s \n",pv2);
printf ("Input Translation  %s \n",pv3);
printf ("Input Match        %s \n",pv4);
printf ("\n\n");

result_code = str$translate (&s1,&s2,&s3,&s4);

print_result_code (presult,result_code,cont);
print_desc_compare ("Digit",&sr,&s1,cont);

printf ("\n\n");
printf ("Output Destination  ["); str$$print_sd (&s1); printf ("]\n");
printf ("Output Source        "); str$$print_sd (&s2); printf ("\n");
printf ("Output Translate     "); str$$print_sd (&s3); printf ("\n");
printf ("Output Match         "); str$$print_sd (&s4); printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&s2);
str$free1_dx (&s3);
str$free1_dx (&s4);
str$free1_dx (&sr);

return result_code;
}


/*********************************************************/


int	teststrtrim (FILE *fptr,int *fstatus, char *cont)
{
char 		pt1[MAX_ARRAY],pv1[MAX_ARRAY];
char 		pt2[MAX_ARRAY],pv2[MAX_ARRAY];
char 		pt3[MAX_ARRAY],pv3[MAX_ARRAY],presult[MAX_ARRAY];
struct		dsc$descriptor_s sd1, sd2, sd_blank, sd_tab, sr1;
unsigned long	result;
char		*sptr1;		/* Pointer to first string */
unsigned short	slen1, length;


*fstatus = fscanf (fptr,scan7,pt1,pv1,pt2,pv2,pt3,pv3,presult);

str$$malloc_sd (&sd1,pv1);
str$$malloc_sd (&sr1,pv1);
str$$malloc_sd (&sd2,pv2);
str$$malloc_sd (&sd_blank,"BLANK");
str$$malloc_sd (&sd_tab,"TAB");
length = atol(pv3);

if ( pv2[0] == 'B' )
	str$append (&sd2,&sd_blank);
if ( pv2[0] == 'T' )
	str$append (&sd2,&sd_tab);

str$analyze_sdesc(&sd1, &slen1, &sptr1);


printf ("Testing  str$trim \n");
printf ("Trim trailing white space with optional result length \n\n\n");

printf ("Input Destination   ["); str$$print_sd (&sd1); printf ("]\n");
printf ("Input Source        ["); str$$print_sd (&sd2); printf ("]\n");
printf ("Input Length        %d \n",length);
printf ("\n");

result = str$trim (&sd1,&sd2,&length);

print_result_code (presult,result,cont);
printf ("\n\n");
		
printf ("Output Destination  ["); str$$print_sd (&sd1); printf ("]\n");
printf ("Output Source       ["); str$$print_sd (&sd2); printf ("]\n");
printf ("Output Length       %d \n",length);
printf ("\n");

str$free1_dx (&sd1);
str$free1_dx (&sd2);
str$free1_dx (&sr1);

return result;
}



/*********************************************************/

int	teststrupcase (FILE *fptr,int *fstatus, char *cont)
{
unsigned long 		result_code;
struct dsc$descriptor_s s1,s2;
char 			pt1[10], pv1[10];
char 			pt2[10], pv2[10];
char			presult [30];

*fstatus = fscanf (fptr,scan5,pt1,pv1,pt2,pv2,presult);

if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s1,pv1);
if (strncmp ( pt1,"cd",2) == 0) str$$malloc_sd (&s2,pv2);

printf ("Testing str$upcase \n");
printf ("Compare 2 descriptors - strings\n\n\n");

printf ("Input Destination     %s \n",pv1);
printf ("Input Source          %s \n",pv2);
printf ("\n\n");

result_code = str$upcase (&s1,&s2);

print_result_code (presult,result_code,cont);

printf ("\n\n");
printf ("Output Destination   ["); str$$print_sd (&s1); printf ("]\n");
printf ("Output Source         "); str$$print_sd (&s2); printf ("\n");
printf ("\n");

str$free1_dx (&s1);
str$free1_dx (&s2);

return result_code;
}



/*************************************/

#if 0
int testsysasctim(void)
{

	struct dsc$descriptor return_date;
	char date_buffer[64];
	unsigned long status;

	return_date.dsc$w_length = sizeof(date_buffer);
	return_date.dsc$b_dtype = DSC$K_DTYPE_T;
	return_date.dsc$b_class = DSC$K_CLASS_S;
	return_date.dsc$a_pointer = date_buffer;

	status = sys$asctim(&return_date.dsc$w_length, &return_date, NULL, 0);

	printf("Todays date:  %-32.32s\n", date_buffer);

	return status;
}
#endif


