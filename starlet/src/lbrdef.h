#ifndef lbrdef_h
#define lbrdef_h

#define LBR$C_TYP_UNK 0                 
#define LBR$C_TYP_OBJ 1                 
#define LBR$C_TYP_MLB 2                 
#define LBR$C_TYP_HLP 3                 
#define LBR$C_TYP_TXT 4                 
#define LBR$C_TYP_SHSTB 5               
#define LBR$C_TYP_NCS 6                 
#define LBR$C_TYP_DECMX 6               
#define LBR$C_TYP_EOBJ 7                
#define LBR$C_TYP_ESHSTB 8              
#define ELBR$C_TYP_DECMX 8              
#define LBR$C_TYP_RDEC 127              

#define LBR$C_TYP_USRLW 128             
#define LBR$C_TYP_USRHI 255             

#define LBR$C_FLUSHDATA 1               
#define LBR$C_FLUSHALL 0                

#define LBR$C_MAXRECSIZ 2048            
#define LBR$C_PAGESIZE 512              
#define LBR$C_HASHSIZE 512              
#define LBR$C_TEXTPAGE 508              
#define LBR$C_DEXTQ 50                  
#define LBR$C_MAXCTL 64                 
#define LBR$C_MAXHDRSIZ 128             

#define LBR$C_DEFENTALL 300             
#define LBR$C_RETRYOPEN 120             
#define LBR$C_RETRYWAIT 1               
#define LBR$C_MINREAD 2                 
#define LBR$C_MAXREAD 100               
#define LBR$C_MEMXTRA 50                
#define LBR$C_PUTBUFSIZ 30              
#define LBR$C_FLSHBFSIZ 1               
#define LBR$C_MAXIDXRD 20               
#define LBR$C_MAXKEYLEN 128             

#define LBR$C_MAXLUHREC 32768           

#define ELBR$C_MAXRECSIZ 8192           

#define LBR$C_CREATE 0                  
#define LBR$C_READ 1                    
#define LBR$C_UPDATE 2                  
#define LBR$C_MAXFUNC 2                 
	
#define LBR$C_ADDMOD 1                  
#define LBR$C_DELMOD 2                  
#define LBR$C_REPMOD 3                  

struct _lbrdef {
  int lbrdef$$_fill_1;                
  unsigned int lbr$l_ic_ctltbl;       
  unsigned int lbr$l_ic_func;         
  unsigned int lbr$l_ic_type;         
};

struct _lbrdef1 {
  int lbrdef$$_fill_2;                
  unsigned int lbr$l_op_ctltbl;       
  unsigned int lbr$l_op_fns;          
  unsigned int lbr$l_op_creopt;       
  unsigned int lbr$l_op_dns;          
  unsigned int lbr$l_op_rlfna;        
  unsigned int lbr$l_op_rns;          
  unsigned int lbr$l_op_rnslen;       
};
struct _lbrdef2 {
  int lbrdef$$_fill_3;                
  unsigned int lbr$l_cl_ctltbl;       
};
struct _lbrdef3 {
  int lbrdef$$_fill_4;                
  unsigned int lbr$l_gh_ctltbl;       
  unsigned int lbr$l_gh_retary;       

};
struct _lbrdef4 {
  int lbrdef$$_fill_5;                
  unsigned int lbr$l_si_ctltbl;       
  unsigned int lbr$l_si_idxnum;       

};
struct _lbrdef5 {
  int lbrdef$$_fill_6;                
  unsigned int lbr$l_lk_ctltbl;       
  unsigned int lbr$l_lk_keynam;       
  unsigned int lbr$l_lk_txtrfa;       

};
struct _lbrdef6 {
  int lbrdef$$_fill_7;                
  unsigned int lbr$l_ik_ctltbl;       
  unsigned int lbr$l_ik_keynam;       
  unsigned int lbr$l_ik_txtrfa;       

};
struct _lbrdef7 {
  int lbrdef$$_fill_8;                
  unsigned int lbr$l_rk_ctltbl;       
  unsigned int lbr$l_rk_keynam;       
  unsigned int lbr$l_rk_oldrfa;       
  unsigned int lbr$l_rk_newrfa;       

};
struct _lbrdef8 {
  int lbrdef$$_fill_9;                
  unsigned int lbr$l_dk_ctltbl;       
  unsigned int lbr$l_dk_keynam;       

};
struct _lbrdef9 {
  int lbrdef$$_fill_10;               
  unsigned int lbr$l_dd_ctltbl;       
  unsigned int lbr$l_dd_txtrfa;       

};
struct _lbrdef10 {
  int lbrdef$$_fill_11;               
  unsigned int lbr$l_gr_ctltbl;       
  unsigned int lbr$l_gr_bufdes;       
  unsigned int lbr$l_gr_buflen;       

};
struct _lbrdef11 {
  int lbrdef$$_fill_12;               
  unsigned int lbr$l_pr_ctltbl;       
  unsigned int lbr$l_pr_bufdes;       
  unsigned int lbr$l_pr_txtrfa;       

};
struct _lbrdef12 {
  int lbrdef$$_fill_13;               
  unsigned int lbr$l_pe_ctltbl;       

};
struct _lbrdef13 {
  int lbrdef$$_fill_14;               
  unsigned int lbr$l_sr_ctltbl;       
  unsigned int lbr$l_sr_idxnum;       
  unsigned int lbr$l_sr_rfa;          
  unsigned int lbr$l_sr_usrtn;        

};
struct _lbrdef14 {
  int lbrdef$$_fill_15;               
  unsigned int lbr$l_su_keydes;       
  unsigned int lbr$l_su_txtrfa;       

};
struct _lbrdef15 {
  int lbrdef$$_fill_16;               
  unsigned int lbr$l_gi_ctltbl;       
  unsigned int lbr$l_gi_idxnum;       
  unsigned int lbr$l_gi_usrtn;        

};
struct _lbrdef16 {
  int lbrdef$$_fill_17;               
  unsigned int lbr$l_gu_keyadr;       
  unsigned int lbr$l_gu_txtrfa;       

};

	
struct _lbrdef17 {
  int lbrdef$$_fill_18;               
  unsigned int lbr$l_au_ctltbl;       
  unsigned int lbr$l_au_flags;        
  unsigned int lbr$l_au_keynam;       

};
struct _lbrdef18 {
  int lbrdef$$_fill_19;               
  unsigned int lbr$l_gu_ctltbl;       
  unsigned int lbr$l_gu_usrtn;        

};
struct _lbrdef19 {
  int lbrdef$$_fill_20;               
  unsigned int lbr$l_uu_updesc;       
};
 
#endif 
 
