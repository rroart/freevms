#ifndef pagedef_h
#define pagedef_h

#define MMG$C_VAX_PAGE_SIZE 512         
#define MMG$C_ALPHA_MAX_PAGE_SIZE 65536 
#define IOC$C_DISK_BLKSIZ 512           
#define IOC$M_BLOCK_BYTEMASK 0x1FF
#define IOC$M_BLOCK_BLKNUM 0xFFFFFE00
	
union _blockdef {
  struct  {
      unsigned ioc$v_block_bytemask : 9; 
      unsigned ioc$v_block_blknum : 23; 
    };
};
 
#endif 
 
