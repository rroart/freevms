/*
	Home block definitions for DFU
*/

#define	HM2$C_LEVEL1		257	/* Structure level 1		*/
#define	HM2$C_LEVEL2		512	/* Structure level 2		*/

/*
**	Volume characteristics bit positions and mask values
*/

#define	HM2$V_READCHECK		  0	/* Verify all reads from volume	*/
#define	HM2$V_WRITCHECK		  1	/* Verify all writes to volume	*/
#define	HM2$V_ERASE		  2	/* Erase/overwrite on delete	*/
#define	HM2$V_NOHIGHWATER	  3	/* Disable highwater marking	*/
#define	HM2$V_CLASS_PROT	  4	/* Enable classification checks	*/

#define	HM2$M_READCHECK		  1
#define	HM2$M_WRITCHECK		  2
#define	HM2$M_ERASE		  4
#define	HM2$M_NOHIGHWATER	  8
#define	HM2$M_CLASS_PROT	 16

/*
**	Define the sizes of various fields
*/

#define	HM2$S_HM2DEF		512	/* Size of home block in bytes	*/

  /* The following date fields are all quadwords	*/
#define	HM2$S_CREDATE		  8	/* Creation date		*/
#define	HM2$S_RETAINMIN		  8	/* Min file retention period	*/
#define	HM2$S_RETAINMAX		  8	/* Max file retention period	*/
#define	HM2$S_REVDATE		  8	/* Revision date		*/

  /* The following fields are ASCII strings		*/
#define	HM2$S_MIN_CLASS		 20	/* Min security classification	*/
#define	HM2$S_MAX_CLASS		 20	/* Max security classification	*/

  /* The following field is not defined in the reference*/
#define	HM2$S_FILETAB_FID	  3	/* Converted 6 bytes -> 3 words	*/

  /* The following fields are ASCII strings		*/
#define	HM2$S_STRUCNAME		 12	/* Volume set name		*/
#define	HM2$S_VOLNAME		 12	/* Volume label			*/
#define	HM2$S_OWNERNAME		 12	/* Volume owner			*/
#define	HM2$S_FORMAT		 12	/* "DECFILE11B  "		*/


/*
**	For the sake of convenience, a structure has been defined which
**	includes all currently known data fields.
*/

struct  HM2_Struct {
  unsigned int	 homelbn;	/* LBN for this copy of the home block	*/
  unsigned int	 alhomelbn;	/* LBN of secondary home block		*/
  unsigned int   altidxlbn;	/* Backup LBN for index file header	*/
  union {
    unsigned short  struclev;	/* ODS version and level		*/
    struct {
      unsigned char  strucver;	/*   ODS version field			*/
      unsigned char  struclev;	/*   ODS level field			*/
      } strucver_fields;
    } struclev_overlay;
  unsigned short cluster;	/* Volume cluster factor		*/
  unsigned short homevbn;	/* VBN of home block in INDEXF.SYS	*/
  unsigned short alhomevbn;	/* VBN of secondary home block in INDEXF*/
  unsigned short altidxvbn;	/* VBN of backup index file headerin INDEXF */
  unsigned short ibmapvbn;	/* Starting VBN of index file bitmap	*/
  unsigned int	 ibmaplbn;	/* Starting LBN of index file bitmap	*/
  unsigned int	 maxfiles;	/* Max files allowed on volume		*/
  unsigned short ibmapsize;	/* Size of index file bitmap (blocks)	*/
  unsigned short resfiles;	/* Number of reserved files on volume	*/
  unsigned short devtype;	/* Not currently used = Always = 0	*/
  unsigned short rvn;		/* Relative vol. number in a volume set	*/
  unsigned short setcount;	/* No. volumes in vol. set (if RVN = 1)	*/
  union {
    unsigned short volchar;	/* Volume characteristics		*/
    struct {
      unsigned short readcheck  : 1;	/* 0 - Verify all reads from volume	*/
      unsigned short writcheck  : 1;	/* 1 - Verify all writes to volume	*/
      unsigned short erase      : 1;	/* 2 - Erase/overwrite on delete	*/
      unsigned short nohighwater: 1;	/* 3 - Disable highwater marking	*/
      unsigned short class_prot : 1;	/* 4 - Enable classification checks	*/
      } volchar_bits;
    } volchar_overlay;

  union {
    unsigned int  volowner;	/* UIC of volume owner			*/
    struct {
      unsigned short uicmember;/*   UIC member number			*/
      unsigned short uicgroup;  /*   UIC group number			*/
      } volowner_fields;
    }volowner_overlay;

  unsigned int	 sec_mask;	/* --Not defined.  Latent ?, SEVMS ?	*/
  union {
    unsigned short protect;	/* Volume protection			*/
    struct {
      unsigned short system : 4;	/*    System access			*/
      unsigned short owner  : 4;	/*    Owner access			*/
      unsigned short group  : 4;	/*    Group access			*/
      unsigned short world  : 4;	/*    World access			*/
				/*    Bit	Meaning if set
				       0	No read access
				       1	No write access
				       2	No create access
				       3	No delete access	*/
      } protect_bits;
    } protect_overlay;

  unsigned short fileprot;	/* Def file protection - not supported	*/
  unsigned short recprot;	/* Def record protection - supported ?	*/
  unsigned short checksum1;	/* Checksum for all preceding entries	*/
  unsigned char	 credate[8];	/* Volume creation date			*/
  unsigned char	 window;	/* Default mapping window size		*/
  unsigned char	 lru_lim;	/* #Dirs in directory access cache	*/
  unsigned short extend;	/* Default file extend size		*/
  unsigned char	 retainmin[8];	/* Min file retention period		*/
  unsigned char	 retainmax[8];	/* Max file retention period		*/
  unsigned char	 revdate[8];	/* Last time volume data was revised	*/
  unsigned char	 min_class[20];	/* Min security classification		*/
  unsigned char	 max_class[20];	/* Max security classification		*/
  unsigned short filetab_fid[3];/* File lookup table FID ??		*/

  union {				/* Lowest structure level on vol*/
    unsigned short lowstruclev;
    struct {
      unsigned char  lowstrucver;	/*   Structure version field	*/
      unsigned char  lowstruclev;	/*   Structure level field	*/
      } lowstruclev_fields;
    } lowstructlev_overlay;

  union {
    unsigned short highstruclev;	/* Highest struct level on vol	*/
    struct {
      unsigned char  highstrucver;	/*   Structure version field	*/
      unsigned char  highstruclev;	/*   Structure level field	*/
      } highstruclev_fields;
    } highstruclev_overlay;

  unsigned char	 reserved1[310];/* Unused bytes				*/
  unsigned int	 serialnum;	/* Serial no. of physical media 	*/
  unsigned char	 strucname[12];	/* Volume set name			*/
  unsigned char	 volname[12];	/* Volume label				*/
  unsigned char	 ownername[12];	/* Volume owner				*/
  unsigned char	 format[12];	/* "DECFILE11B  "			*/
  unsigned short reserved2;	/* Unused word				*/
  unsigned short checksum2;	/* Checksum for preceeding 510 bytes	*/
  };
