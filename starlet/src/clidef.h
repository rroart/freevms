#ifndef clidef_h
#define clidef_h

#define CLI$M_DEBUG 0x1
#define CLI$M_DBGTRU 0x2
#define CLI$M_VERIFY 0x4
#define CLI$M_BATCH 0x8
#define CLI$M_INDIRECT 0x10
#define CLI$M_VFYINP 0x20
#define CLI$M_TRMVRBLV 0x300
#define CLI$M_DBGEXCP 0x10000
	
#define CLI$K_WORKAREA 128              
#define CLI$C_WORKAREA 128              

#define CLI$K_UTILOPR 0                 
#define CLI$K_INPSPEC 1                 
#define CLI$K_OUTSPEC 2                 
#define CLI$K_PARDONE 3                 
#define CLI$K_VALCONV 4                 
#define CLI$K_CLINT 5                   

#define CLI$K_INITPRS 0                 
#define CLI$K_GETCMD 1                  
#define CLI$K_GETQUAL 2                 
#define CLI$K_GETOPT 3                  
#define CLI$K_GETLINE 4                 

#define CLI$K_CLISERV 5                 

#define CLI$K_INPUT1 16                 
#define CLI$K_INPUT2 17                 
#define CLI$K_INPUT3 18                 
#define CLI$K_INPUT4 19                 

#define CLI$M_PARMREQ 0x1
#define CLI$M_ABSADR 0x2
#define CLI$M_EXPNAM 0x4
#define CLI$M_LASTVAL 0x1
#define CLI$M_DUMMY 0x2
#define CLI$M_PARMPRS 0x1
#define CLI$M_CONCATINP 0x2
#define CLI$M_MOREINP 0x4
#define CLI$M_PARMDEF 0x8
#define CLI$M_MOREVALS 0x1
#define CLI$M_KEYVALU 0x2
#define CLI$K_REQDESC 28                
#define CLI$C_REQDESC 28                

#define CLI$K_OUTPUT1 32                
#define CLI$K_OUTPUT2 33                
#define CLI$K_OUTPUT3 34                
#define CLI$K_OUTPUT4 35                

#define CLI$K_ENDPRM1 48                
#define CLI$K_ENDPRM2 49                
#define CLI$K_ENDPRM3 50                
#define CLI$K_ENDPRM4 51                

#define CLI$K_NUMERVAL 64               
#define CLI$K_ASCIIVAL 65               
#define CLI$K_KEYWORD 66                
#define CLI$K_KEYVAL 67                 
#define CLI$K_FILSPEC 68                

#define CLI$K_PRESENT 80                
#define CLI$K_GETVALUE 81               
#define CLI$K_ENDPARSE 82               
#define CLI$K_DCLPARSE 83               
#define CLI$K_DISPATCH 84               
#define CLI$K_NEXTQUAL 85               

#define CLI$M_NOWAIT 0x1
#define CLI$M_NOCLISYM 0x2
#define CLI$M_NOLOGNAM 0x4
#define CLI$M_NOKEYPAD 0x8
#define CLI$M_NOTIFY 0x10
#define CLI$M_NOCONTROL 0x20
#define CLI$M_TRUSTED 0x40
#define CLI$M_AUTHPRIV 0x80
#define CLI$M_SUBSYSTEM 0x100
#define CLI$K_SPAWN_VERSION 1           
#define CLI$C_SPAWN_VERSION 1           
#define CLI$K_SRVDESC 84                
#define CLI$C_SRVDESC 84                

#define CLI$M_ALLOCCUR 0x1
#define CLI$M_QDUSRV 0x2
#define CLI$M_QDEXPA 0x4
#define CLI$M_QUALTRU 0x1
#define CLI$M_QUALEXP 0x2
#define CLI$K_QUALDEF 20                
#define CLI$C_QUALDEF 20                
#define CLI$K_QDBITS 20                 
#define CLI$C_QDBITS 20                 
	
struct _clidef {
  unsigned int cli$l_iniargcnt;       
  void *cli$a_progxfer;               
  void *cli$a_utilserv;               
  void *cli$a_imghdadr;               
  void *cli$a_imgfiled;               
  unsigned int cli$l_linkflag;        
  union  {
    unsigned int cli$l_cliflag;     
    struct  {
      unsigned cli$v_debug : 1;   
      unsigned cli$v_dbgtru : 1;  
      unsigned cli$v_verify : 1;  
      unsigned cli$v_batch : 1;   
      unsigned cli$v_indirect : 1; 
      unsigned cli$v_vfyinp : 1;  
      unsigned clidef$$_fill_1 : 2; 
      unsigned cli$v_trmvrblv : 2; 
      unsigned clidef$$_fill_2 : 6; 
      unsigned cli$v_dbgexcp : 1; 
      unsigned cli$v_fill_93_ : 7;
    };

  };
  int cli$l_arglist;                  

};
 
struct _clidef1 {
  union  {
    int clidef$$_fill_3;            
    struct  {
      union  {
	unsigned char cli$b_rqtype; 
	struct  {
	  unsigned cli$v_subtyp : 4; 
	  unsigned cli$v_prityp : 4; 
	};
      };
      union  {
	unsigned short int cli$w_servcod; 
	struct  {
	  union  {
	    unsigned char cli$b_rqindx; 
	    unsigned char cli$b_bitnum; 
	  };
	  union  {
	    unsigned char cli$b_rqflgs; 
	    struct  {
	      unsigned cli$v_parmreq : 1; 
	      unsigned cli$v_absadr : 1; 
	      unsigned cli$v_expnam : 1; 
	      unsigned cli$v_fill_94_ : 5;
	    };
	    struct  {
	      unsigned cli$v_lastval : 1; 
	      unsigned cli$v_dummy : 1; 
	      unsigned cli$v_fill_95_ : 6;
	    };
	  };
	};
      };
      union  {
	unsigned char cli$b_rqstat; 
	struct  {
	  unsigned cli$v_parmprs : 1; 
	  unsigned cli$v_concatinp : 1; 
	  unsigned cli$v_moreinp : 1; 
	  unsigned cli$v_parmdef : 1; 
	  unsigned cli$v_fill_96_ : 4;
	};
	struct  {
	  unsigned cli$v_morevals : 1; 
	  unsigned cli$v_keyvalu : 1; 
	  unsigned cli$v_fill_97_ : 6;
	};
      };
    };
  };
  void *cli$a_erract;                 
  union  {
    unsigned long long cli$q_rqdesc;  
    struct  {
      unsigned short int cli$w_rqsize; 
      short int clidef$$_fill_4;  
      union  {
	unsigned int cli$l_rqvalu; 
	void *cli$a_rqaddr;     
      };
    };
  };
  void *cli$a_prsact;                 
  void *cli$a_absact;                 
  void *cli$a_qualst;                 

};
 
 
	
struct _clidef2 {
  char clidef$$_fill_8 [8]; // check. was 4. or fix "align" in def1
  unsigned long long cli$q_namdesc;     
  unsigned long long cli$q_valdesc;     
  unsigned long long cli$q_tabdesc;     
  int cli$l_itmlst;                   
  int cli$l_attr;                     

};
struct _clidef3 {
  char clidef$$_fill_9 [4];
  unsigned int cli$l_new_mask;        
  unsigned int cli$l_old_mask;        

};
struct _clidef4 {
  char clidef$$_fill_10 [4];
  unsigned int cli$l_pid;             

};

struct _clidef5 {
  char clidef$$_fill_11 [4];
  union  {
    unsigned char cli$b_flags;      
    unsigned short int cli$w_flags; 
    struct  {
      unsigned cli$v_nowait : 1;  
      unsigned cli$v_noclisym : 1; 
      unsigned cli$v_nolognam : 1; 
      unsigned cli$v_nokeypad : 1; 
      unsigned cli$v_notify : 1;  
      unsigned cli$v_nocontrol : 1; 
      unsigned cli$v_trusted : 1; 
      unsigned cli$v_authpriv : 1; 
      unsigned cli$v_subsystem : 1; 
      unsigned cli$v_fill_98_ : 7;
    };
  };
  char clidef$$_fill_5 [2];           
  unsigned int cli$l_outpid;          
  int cli$l_lststatus;                
  unsigned long long cli$q_cmdstr;      
  unsigned long long cli$q_input;       
  unsigned long long cli$q_output;      
  unsigned long long cli$q_prcnam;      
  int cli$l_astadr;                   
  int cli$l_astprm;                   
  unsigned char cli$b_efn;            
  unsigned char cli$b_version;        
  char clidef$$_fill_6 [2];           
  unsigned long long cli$q_prompt;      
  unsigned long long cli$q_cli;         
  unsigned long long cli$q_table;       

};
struct _clidef6 {
  unsigned char cli$b_qdblksiz;       
  unsigned char cli$b_qdcode;         
  union  {
    unsigned char cli$b_qdflgs;     
    struct  {
      unsigned cli$v_alloccur : 1; 
      unsigned cli$v_qdusrv : 1;  
      unsigned cli$v_qdexpa : 1;  
      unsigned cli$v_fill_99_ : 5;
    };
  };
  union  {
    unsigned char cli$b_qdstat;     
    struct  {
      unsigned cli$v_qualtru : 1; 
      unsigned cli$v_qualexp : 1; 
      unsigned cli$v_fill_100_ : 6;
    };
  };
  union  {
    unsigned long long cli$q_qdvaldesc; 
    struct  {
      unsigned short int cli$w_qdvalsiz; 
      short int clidef$$_fill_7;  
      void *cli$a_qdvaladr;       
    };
  };
  void *cli$a_truact;                 
  void *cli$a_flsact;                 
  unsigned int cli$l_usrval;          

};
 
	
union _clidef7 {
  unsigned int cli$l_workarea [32];   

};
struct _clidef8 {
  char clidef$$_fill_12 [4];
  unsigned int cli$l_codeset;         
};
 
#endif 
 
