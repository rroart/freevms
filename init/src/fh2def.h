struct FH2$ { 
unsigned char FH2$B_IDOFFSET;
unsigned char FH2$B_MPOFFSET;
unsigned char FH2$B_ACOFFSET;
unsigned char FH2$B_RSOFFSET;
unsigned short int FH2$W_SEG_NUM;
unsigned char FH2$W_STRUCLEV[2];
struct FID$ FH2$W_FID;  
struct FID$ FH2$W_EXT_FID;
unsigned char FH2$W_RECATTR[32];
unsigned char FH2$L_FILECHAR[4];
/*char  FCH$V_NOBACKUP   Set if incremental backup is to be disabled
                           for this file.
char  FCH$V_WRITEBACK  Set if the file may be  write-back  cached;
                           i.e., if a cache is used for the file data,
                           data written by a user is only written back
                           to  the  disk  when  is it removed from the
                           cache.   Clear  for   write-through   cache
                           operation.
          FCH$V_READCHECK  Set if the file is to be read-checked.  All
                           read  operations  on  the  file,  including
                           reads  of  the  file  header(s),  will   be
                           performed  with  a  read,  read-compare  to
                           assure data integrity.
          FCH$V_WRITCHECK  Set if the file  is  to  be  write-checked.
                           All write operations on the file, including
                           modifications of the file  header(s),  will
                           be  performed with a write, read-compare to
                           assure data integrity.
          FCH$V_CONTIGB    Set if the  file  is  allocated  contiguous
                           best   effort;   i.e.,   as  contiguous  as
                           possible.
          FCH$V_LOCKED     Set if the file is  deaccess-locked.   This
                           bit is used as a flag warning that the file
                           was not properly  closed  and  may  contain
                           inconsistent  data.   Access to the file is
                           denied if this bit is set.
          FCH$V_CONTIG     Set if the file  is  logically  contiguous;
                           i.e.,  if  for  all  virtual  blocks in the
                           file, virtual block i maps to logical block
                           k+i  on  one  volume  for  some constant k.
                           This bit may be implicitly set  or  cleared
                           by  file  system  operations  that allocate
                           space to the file; the user may only  clear
                           it explicitly.
          FCH$V_BADACL     Set if the access control list of this file
                           is not valid (e.g., a system crash occurred
                           while it was being updated).
          FCH$V_SPOOL      Set if the  file  is  a  spool  file.   (An
                           intermediate file for later printing, etc.)
                           File operations not related to  spool  file
                           handling are prohibited.
          FCH$V_DIRECTORY  Set if the file is a directory.
          FCH$V_BADBLOCK   Set if there is a bad  data  block  in  the
                           file.  It indicates that deferred bad block
                           processing is to be done  on  the  file  at
                           some  suitable  later time (such as when it
                           is deleted.)
          FCH$V_MARKDEL    Set if the file is marked for  delete.   If
                           this  bit  is  set, further accesses to the
                           file are  denied,  and  the  file  will  be
                           physically   deleted   when  no  users  are
                           accessing it.
          FCH$V_NOCHARGE   Set if the space used by this file  is  not
                           to be charged to its owner.
          FCH$V_ERASE      Set  if  the   file   is   to   be   erased
                           (overwritten) when it is deleted.
*/
unsigned char FH2$W_RECPROT[2];
unsigned char FH2$B_MAP_INUSE;
unsigned char FH2$B_ACC_MODE;
unsigned char FH2$L_FILEOWNER[4];
unsigned char FH2$W_FILEPROT[2];
struct FID$ FH2$W_BACKLINK;
unsigned char FH2$W_JOURNAL[2]; 
/*          FJN$V_RUACCESS   Set if the file is permitted to be accessed
                           only  if  the  accessor  is running under a
                           recovery unit.
          FJN$V_RUJNL      Set if a recovery unit  journal  is  to  be
                           enabled
          FJN$V_BIJNL      Set if a before  image  journal  is  to  be
                           enabled
          FJN$V_AIJNL      Set if an after  image  journal  is  to  be
                           enabled
          FJN$V_ATJNL      Set if an audit  trail  journal  is  to  be
                           enabled
          FJN$V_NORU       Set if the file is not to be accessed  from
                           within a recovery unit
*/
unsigned char dummy1[2];
unsigned int  FH2$L_HIGHWATER; 
unsigned char FH2$C_LENGTH[80];
unsigned char dummy2[8];
unsigned char FH2$R_CLASS_PROT[20];
};  // end struct FH2$


