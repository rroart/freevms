/* <descrip.h>
 *
 *	Descriptor definitions, more useful than those in <vms/dscdef.h>
 */
#ifndef _DESCRIP_H
#define _DESCRIP_H

/* Based on the VAX-11 Procedure Calling and Condition Handling Standard, */
/* Revision 9.0 [7-Dec-81] and updated from VMS V5.5's $DSCDEF, Dec'91	  */


struct dsc$descriptor {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  void		*dsc$a_pointer; 
};


struct dsc$descriptor_s {
  unsigned short	dsc$w_length;	
					
					
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  char		*dsc$a_pointer; 
};


struct dsc$descriptor_d {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  char		*dsc$a_pointer; 
};


struct dsc$descriptor_a {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  void		*dsc$a_pointer; 
  char		dsc$b_scale;	
  unsigned char	dsc$b_digits;	
  struct {
    unsigned		 : 4;	
    unsigned dsc$v_fl_redim  : 1;	
    unsigned dsc$v_fl_column : 1;	
    unsigned dsc$v_fl_coeff  : 1;	
    unsigned dsc$v_fl_bounds : 1;	
  }		dsc$b_aflags;
  unsigned char	dsc$b_dimct;	
  unsigned long	dsc$l_arsize;	
					
};


struct dsc$descriptor_p {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  void		*dsc$a_pointer; 
};


struct dsc$descriptor_sd {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  char		*dsc$a_pointer; 
  char		dsc$b_scale;	
  unsigned char	dsc$b_digits;	
  unsigned	: 16;		
};


struct dsc$descriptor_nca {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  void		*dsc$a_pointer; 
  char		dsc$b_scale;	
  unsigned char	dsc$b_digits;	
  struct	{
    unsigned		 : 4;	
    unsigned dsc$v_fl_redim  : 1;	
    unsigned		 : 3;	
  }		dsc$b_aflags;
  unsigned char	dsc$b_dimct;	
  unsigned long	dsc$l_arsize;	
					
};


struct dsc$descriptor_vs {
  unsigned short	dsc$w_maxstrlen;	
  unsigned char	dsc$b_dtype;		
  unsigned char	dsc$b_class;		
  void		*dsc$a_pointer;		
};


struct dsc$descriptor_vsa {
  unsigned short	dsc$w_maxstrlen;	
  unsigned char	dsc$b_dtype;		
  unsigned char	dsc$b_class;		
  void		*dsc$a_pointer;		
  char		dsc$b_scale;		
  unsigned char	dsc$b_digits;		
  struct {
    unsigned		 : 4;	
    unsigned dsc$v_fl_redim  : 1;	
    unsigned		 : 3;	
  }		dsc$b_aflags;
  unsigned char	dsc$b_dimct;		
  unsigned long	dsc$l_arsize;		
};


struct dsc$descriptor_ubs {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  char		*dsc$a_base;	
  long		dsc$l_pos;	
};


struct dsc$descriptor_uba {
  unsigned short	dsc$w_length;	
  unsigned char	dsc$b_dtype;	
  unsigned char	dsc$b_class;	
  char		*dsc$a_base;	
  char		dsc$b_scale;	
  unsigned char	dsc$b_digits;	
  struct {
    unsigned		 : 4;	
    unsigned dsc$v_fl_redim  : 1;	
    unsigned		 : 3;	
  }		dsc$b_aflags;
  unsigned char	dsc$b_dimct;	
  unsigned long	dsc$l_arsize;	
};


struct dsc$descriptor_bfa {	
  int	*dsc$a_handle;	
				
				
  long	dsc$l_byteoff;	
				
  long	dsc$l_logunit;	
				
};


#define DSC$K_DTYPE_Z	0		
#define DSC$K_DTYPE_BU	2		
#define DSC$K_DTYPE_WU	3		
#define DSC$K_DTYPE_LU	4		
#define DSC$K_DTYPE_QU	5		
#define DSC$K_DTYPE_OU	25		
#define DSC$K_DTYPE_B	6		
#define DSC$K_DTYPE_W	7		
#define DSC$K_DTYPE_L	8		
#define DSC$K_DTYPE_Q	9		
#define DSC$K_DTYPE_O	26		
#define DSC$K_DTYPE_F	10		
#define DSC$K_DTYPE_D	11		
#define DSC$K_DTYPE_G	27		
#define DSC$K_DTYPE_H	28		
#define DSC$K_DTYPE_FC	12		
#define DSC$K_DTYPE_DC	13		
#define DSC$K_DTYPE_GC	29		
#define DSC$K_DTYPE_HC	30		
#define DSC$K_DTYPE_CIT 31		
	
#define DSC$K_DTYPE_T	14		
#define DSC$K_DTYPE_T2	38		
#define DSC$K_DTYPE_VT	37		
#define DSC$K_DTYPE_NU	15		
#define DSC$K_DTYPE_NL	16		
#define DSC$K_DTYPE_NLO 17		
#define DSC$K_DTYPE_NR	18		
#define DSC$K_DTYPE_NRO 19		
#define DSC$K_DTYPE_NZ	20		
#define DSC$K_DTYPE_P	21		
#define DSC$K_DTYPE_V	1		
#define DSC$K_DTYPE_VU	34		
	
#define DSC$K_DTYPE_ZI	22		
#define DSC$K_DTYPE_ZEM 23		
#define DSC$K_DTYPE_DSC 24		
#define DSC$K_DTYPE_BPV 32		
#define DSC$K_DTYPE_BLV 33		
#define DSC$K_DTYPE_ADT 35		
 
#ifndef NO_VMS_V6
	
#define DSC$K_DTYPE_FS	52		
#define DSC$K_DTYPE_FT	53		
#define DSC$K_DTYPE_FSC 54		
#define DSC$K_DTYPE_FTC 55		
#define DSC$K_DTYPE_FX	57		
#define DSC$K_DTYPE_FXC 58		
#endif
	
#define DSC$K_DTYPE_CAD 178		
#define DSC$K_DTYPE_ENT 179		
#define DSC$K_DTYPE_GBL 180		
#define DSC$K_DTYPE_EPT 181		
#define DSC$K_DTYPE_R11 182		
#define DSC$K_DTYPE_FLD 183		
#define DSC$K_DTYPE_PCT 184		
#define DSC$K_DTYPE_DPC 185		
#define DSC$K_DTYPE_LBL 186		
#define DSC$K_DTYPE_SLB 187		
#define DSC$K_DTYPE_MOD 188		
#define DSC$K_DTYPE_EOM 189		
#define DSC$K_DTYPE_RTN 190		
#define DSC$K_DTYPE_EOR 191		
	
	
	


#define DSC$K_CLASS_Z	0
#define DSC$K_CLASS_S	1		
#define DSC$K_CLASS_D	2		
#define DSC$K_CLASS_V	3		
#define DSC$K_CLASS_A	4		
#define DSC$K_CLASS_P	5		
#define DSC$K_CLASS_PI	6		
#define DSC$K_CLASS_J	7		
#define DSC$K_CLASS_JI	8		
#define DSC$K_CLASS_SD	9		
#define DSC$K_CLASS_NCA 10		
#define DSC$K_CLASS_VS	11		
#define DSC$K_CLASS_VSA 12		
#define DSC$K_CLASS_UBS 13		
#define DSC$K_CLASS_UBA 14		
#define DSC$K_CLASS_SB	15		
#define DSC$K_CLASS_UBSB 16		
	
#define DSC$K_CLASS_BFA 191		
	
#define DSC$K_Z_BLN 8	
	


#define $DESCRIPTOR(name,string) struct dsc$descriptor_s name =\
 {sizeof(string)-1,DSC$K_DTYPE_T,DSC$K_CLASS_S,string}

#endif	
