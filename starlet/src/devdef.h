#ifndef devdef_h
#define devdef_h

#define		DEV$M_REC		0x1
#define		DEV$M_CCL		0x2
#define		DEV$M_TRM		0x4
#define		DEV$M_DIR		0x8
#define		DEV$M_SDI		0x10
#define		DEV$M_SQD		0x20
#define		DEV$M_SPL		0x40
#define		DEV$M_OPR		0x80
#define		DEV$M_RCT		0x100
#define		DEV$M_QSVD		0x200
#define		DEV$M_QSVBL		0x400
#define		DEV$M_MPDEV_SECONDARY	0x800
#define		DEV$M_MPDEV_MEMBER	0x1000
#define		DEV$M_NET		0x2000
#define		DEV$M_FOD		0x4000
#define		DEV$M_DUA		0x8000
#define		DEV$M_SHR		0x10000
#define		DEV$M_GEN		0x20000
#define		DEV$M_AVL		0x40000
#define		DEV$M_MNT		0x80000
#define		DEV$M_MBX		0x100000
#define		DEV$M_DMT		0x200000
#define		DEV$M_ELG		0x400000
#define		DEV$M_ALL		0x800000
#define		DEV$M_FOR		0x1000000
#define		DEV$M_SWL		0x2000000
#define		DEV$M_IDV		0x4000000
#define		DEV$M_ODV		0x8000000
#define		DEV$M_RND		0x10000000
#define		DEV$M_RTM		0x20000000
#define		DEV$M_RCK		0x40000000
#define		DEV$M_WCK		0x80000000
#define		DEV$M_CLU		0x1
#define		DEV$M_DET		0x2
#define		DEV$M_RTT		0x4
#define		DEV$M_CDP		0x8
#define		DEV$M_2P		0x10
#define		DEV$M_MSCP		0x20
#define		DEV$M_SSM		0x40
#define		DEV$M_SRV		0x80
#define		DEV$M_RED		0x100
#define		DEV$M_NNM		0x200
#define		DEV$M_WBC		0x400
#define		DEV$M_WTC		0x800
#define		DEV$M_HOC		0x1000
#define		DEV$M_LOC		0x2000
#define		DEV$M_DFS		0x4000
#define		DEV$M_DAP		0x8000
#define		DEV$M_NLT		0x10000
#define		DEV$M_SEX		0x20000
#define		DEV$M_SHD		0x40000
#define		DEV$M_VRT		0x80000
#define		DEV$M_LDR		0x100000
#define		DEV$M_NOLB		0x200000
#define		DEV$M_NOCLU		0x400000
#define		DEV$M_VMEM		0x800000
#define		DEV$M_SCSI		0x1000000
#define		DEV$M_WLG		0x2000000
#define		DEV$M_NOFE		0x4000000
#define		DEV$M_FILL_2		0x8000000
#define		DEV$M_CRAMIO		0x10000000
#define		DEV$M_DTN		0x20000000
#define		DEV$M_FILLER		0x40000000
#define		DEV$M_POOL_MBR		0x80000000
	
union _devdef {
  struct  {
    unsigned dev$v_rec		: 1;         
    unsigned dev$v_ccl		: 1;         
    unsigned dev$v_trm		: 1;         
    unsigned dev$v_dir		: 1;         
    unsigned dev$v_sdi		: 1;         
    unsigned dev$v_sqd		: 1;         
    unsigned dev$v_spl		: 1;         
    unsigned dev$v_opr		: 1;         
    unsigned dev$v_rct		: 1;         
    unsigned dev$v_qsvd		: 1;        
    unsigned dev$v_qsvbl	: 1;       
    unsigned dev$v_mpdev_secondary	: 1; 
    unsigned dev$v_mpdev_member	: 1; 
    unsigned dev$v_net		: 1;         
    unsigned dev$v_fod		: 1;         
    unsigned dev$v_dua		: 1;         
    unsigned dev$v_shr		: 1;         
    unsigned dev$v_gen		: 1;         
    unsigned dev$v_avl		: 1;         
    unsigned dev$v_mnt		: 1;         
    unsigned dev$v_mbx		: 1;         
    unsigned dev$v_dmt		: 1;         
    unsigned dev$v_elg		: 1;         
    unsigned dev$v_all		: 1;         
    unsigned dev$v_for		: 1;         
    unsigned dev$v_swl		: 1;         
    unsigned dev$v_idv		: 1;         
    unsigned dev$v_odv		: 1;         
    unsigned dev$v_rnd		: 1;         
    unsigned dev$v_rtm		: 1;         
    unsigned dev$v_rck		: 1;         
    unsigned dev$v_wck		: 1;         
  };
  struct  {
    unsigned dev$v_clu		: 1;         
    unsigned dev$v_det		: 1;         
    unsigned dev$v_rtt		: 1;         
    unsigned dev$v_cdp		: 1;         
    unsigned dev$v_2p		: 1;          
    unsigned dev$v_mscp		: 1;        
    unsigned dev$v_ssm		: 1;         
    unsigned dev$v_srv		: 1;         
    unsigned dev$v_red		: 1;         
    unsigned dev$v_nnm		: 1;         
    unsigned dev$v_wbc		: 1;         
    unsigned dev$v_wtc		: 1;         
    unsigned dev$v_hoc		: 1;         
    unsigned dev$v_loc		: 1;         
    unsigned dev$v_dfs		: 1;         
    unsigned dev$v_dap		: 1;         
    unsigned dev$v_nlt		: 1;         
    unsigned dev$v_sex		: 1;         
    unsigned dev$v_shd		: 1;         
    unsigned dev$v_vrt		: 1;         
    unsigned dev$v_ldr		: 1;         
    unsigned dev$v_nolb		: 1;        
    unsigned dev$v_noclu	: 1;       
    unsigned dev$v_vmem		: 1;        
    unsigned dev$v_scsi		: 1;        
    unsigned dev$v_wlg		: 1;         
    unsigned dev$v_nofe		: 1;        
    unsigned dev$v_fill_2	: 1;      
    unsigned dev$v_cramio	: 1;      
    unsigned dev$v_dtn		: 1;         
    unsigned dev$v_filler	: 1;      
    unsigned dev$v_pool_mbr	: 1;    
  };
};
 
#endif 
 
