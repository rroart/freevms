#include <ssdef.h>

#include <descrip.h>

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/mm.h>

#include <stdarg.h>

CH$FILL(ch, size, addr) {
  memset(addr,ch, size);
  return addr+size;
  printk("CH$FILL not implemented\n");
}

CH$RCHAR_A() {
  printk("CH$RCHAR_A not implemented\n");
}

CH$ALLOCATION() {
  printk("CH$ALLOCATION not implemented\n");
}

STR$COPY_DX(x,y)
     struct dsc$descriptor * x, *y;
{
  //  printk("STR$COPY_DX not implemented\n");
  if (0==x->dsc$a_pointer) x->dsc$a_pointer=kmalloc(y->dsc$w_length,GFP_KERNEL);
  memcpy(x->dsc$a_pointer,y->dsc$a_pointer,y->dsc$w_length);
  x->dsc$w_length=y->dsc$w_length;
  return SS$_NORMAL;
}

LIB$CALLG() {
  printk("LIB$CALLG not implemented\n");
}

LIB$FIND_IMAGE_SYMBOL() {
  printk("LIB$FIND_IMAGE_SYMBOL not implemented\n");
}

Signal() {
  printk("Signal not implemented\n");
}

STR$APPEND(x , y)
     struct dsc$descriptor * x, *y;
{
  char * s=kmalloc(x->dsc$w_length+y->dsc$w_length,GFP_KERNEL);
  memcpy(s,x->dsc$a_pointer,x->dsc$w_length);
  memcpy(&s[x->dsc$w_length],y->dsc$a_pointer,y->dsc$w_length);
  kfree( x->dsc$a_pointer);
  x->dsc$a_pointer=s;
  x->dsc$w_length=x->dsc$w_length+y->dsc$w_length;
  return SS$_NORMAL;
  printk("STR$APPEND not implemented\n");
}

STR$CASE_BLIND_COMPARE_not() {
  printk("STR$CASE_BLIND_COMPARE not implemented\n");
}

CH$PTR(int X) {
  return X;
}

CH$PTR_not_again(int X, int par) {
#if 0
  int par;
  va_list args;
  va_start(args,X);
  par=va_arg(args,int);
  va_end(args);
  printk("ch$ptr %x\n",par);
#endif
  return X+par;
}

CH$PTR_not() {
  printk("CH$PTR not implemented\n");
}

int STR$CASE_BLIND_COMPARE(x,y)
     struct dsc$descriptor *x, * y;
{
     return strncmp(x->dsc$a_pointer,y->dsc$a_pointer,y->dsc$w_length);
}

int CH$EQL(n1,ptr1,n2,ptr2) {
  int n = ( n1 < n2 ? n1 : n2);
  return (0==memcmp(ptr1,ptr2,n));
}

int CH$NEQ(n1,ptr1,n2,ptr2) {
  int n = ( n1 < n2 ? n1 : n2);
  return (memcmp(ptr1,ptr2,n));
}

CH$NEQ_not() {
  printk("CH$NEQ not implemented\n");
}

CH$EQL_not() {
  printk("CH$EQL not implemented\n");
}

UPLIT() {
  printk("UPLIT not implemented\n");
}

PLIT() {
  printk("PLIT not implemented\n");
}

CH$MOVE(size, src, addr) {
  memcpy(addr,src, size);
  return addr+size;
  printk("CH$MOVE not implemented\n");
}

INSQUE(x,y) {
  insque(x,y);
  return SS$_NORMAL;
  printk("INSQUE not implemented\n");
}

REMQUE(e, a)
     long *e;
     long *a;
{
  int retval=0;
  if (e==e[1]) retval|=1;
  remque(e,0);
  if (e[0]==e[1]) retval|=2;
  *a=e;
  return retval;
  printk("REMQUE not implemented\n");
}

MIN(x,y) {
  return (x<y ? x :  y);
  printk("MIN not implemented\n");
}

MAX(x,y) {
  return (x>y ? x :  y);
  printk("MAX not implemented\n");
}

MINU(x,y) {
  return MIN(x,y);
  printk("MINU not implemented\n");
}

XLOG$FAO() {
  // not implemented, but no printk
  // printk("XLOG$FAO not implemented\n");
}

MAXU(x,y) {
  return MAX(x,y);
  printk("MAXU not implemented\n");
}

ROT(x,y) {
  return x>>y;
  printk("ROT not implemented\n");
}

SwapBytes_not() {
  printk("SwapBytes not implemented\n");
}

CH$WCHAR_A_not() {
  printk("CH$WCHAR_A not implemented\n");
}

QL$FAO() {
  printk("QL$FAO not implemented\n");
}

XQL$FAO() {
  printk("XQL$FAO not implemented\n");
}

LIB$GET_VM_PAGE(size, addr) 
     long * addr;
{
  *addr=kmalloc(4096*(size/8+1),GFP_KERNEL);
  return SS$_NORMAL;
  printk("LIB$GET_VM_PAGE not implemented\n");
}

LIB$STAT_VM() {
  printk("LIB$STAT_VM not implemented\n");
}

LIB$RESET_VM_ZONE() {
  printk("LIB$RESET_VM_ZONE not implemented\n");
}

LIB$DELETE_VM_ZONE() {
  printk("LIB$DELETE_VM_ZONE not implemented\n");
}

LIB$CREATE_USER_VM_ZONE() {
  printk("LIB$CREATE_USER_VM_ZONE not implemented\n");
}

WARN$FAO() {
  printk("WARN$FAO not implemented\n");
}  
 
LIB$SYS_FAOL_not() {
  printk("LIB$SYS_FAOL not implemented\n");
}

ERROR$FAO() {
  printk("ERROR$FAO not implemented\n");
}

LIB$GET_VM(size, addr) 
     long * addr;
{
  *addr=kmalloc(size,GFP_KERNEL);
  return SS$_NORMAL;
  printk("LIB$GET_VM not implemented\n");
}

CH$DIFF() {
  printk("CH$DIFF not implemented\n");
}

LIB$FREE_VM_PAGE(size, addr) {
  kfree(addr);
  return SS$_NORMAL;
  printk("LIB$FREE_VM_PAGE not implemented\n");
}

FORKLOCK() {
  printk("FORKLOCK not implemented\n");
}

FORKUNLOCK () {
  printk("FORKUNLOCK not implemented\n");
}

Lock_IODB_not() {
  printk("Lock_IODB_not not implemented\n");
}

UnLock_IODB_not() {
  printk("UnLock_IODB_not not implemented\n");
}

ch$move(a,b,c) {
  return CH$MOVE(a,b,c);
  printk("ch$move not implemented\n");
}

DRV$Fatal_FAO_not() {
  printk("DRV$Fatal_FAO not implemented\n");
}

QUEUE_EMPTY_not() {
  printk("QUEUE_EMPTY not implemented\n");
}

LIB$CREATE_VM_ZONE() {
  printk("LIB$CREATE_VM_ZONE not implemented\n");
}

STR$UPCASE() {
  printk("STR$UPCASE not implemented\n");
}

STR$MATCH_WILD() {
  printk("STR$MATCH_WILD not implemented\n");
}

STR$TRIM() {
  printk("STR$TRIM not implemented\n");
}

ACT$FAO() {
  printk("ACT$FAO not implemented\n");
}

ASCII_Dec_Bytes_not(DESC,COUNT,SOURCE,LEN) {
#if 0
  struct dsc$descriptor * d = DESC;
  char * s=&SOURCE;
  snprintf(d->dsc$a_pointer,len,"%d.%d.%d.%d",s[0],s[1],s[2],s[3]);
  return SS$_NORMAL;
#endif
  printk("ASCII_Dec_Bytes not implemented\n");
}

ASCII_Hex_Bytes_not(DESC,COUNT,SOURCE,LEN) {
#if 0
  struct dsc$descriptor * d = DESC;
  char * s=&SOURCE;
  snprintf(d->dsc$a_pointer,len,"%d.%d.%d.%x",s[0],s[1],s[2],s[3]);
  return SS$_NORMAL;
#endif
  printk("ASCII_Hex_Bytes not implemented\n");
}

Begin_Lock() {
  printk("Begin_Lock not implemented\n");
}

CH$PLUS(x,y) {
  return x+y;
  printk("CH$PLUS not implemented\n");
}

INSQue_not() {
  printk("INSQue not implemented\n");
}

LIB$ASN_WTH_MBX() {
  printk("LIB$ASN_WTH_MBX not implemented\n");
}

SCH$IOLOCKW() {
  printk("SCH$IOLOCKW not implemented\n");
  return SS$_NORMAL;
}

SCH$IOUNLOCK() {
  printk("SCH$IOUNLOCK not implemented\n");
  return SS$_NORMAL;
}

STR$FREE1_DX(struct dsc$descriptor * d) {
  kfree(d->dsc$a_pointer);
  return;
  printk("STR$FREE1_DX not implemented\n");
}

UNlock_IODB() {
  printk("UNlock_IODB not implemented\n");
}

Unlock_IODB() {
  printk("Unlock_IODB not implemented\n");
}

Swapbytes_not() {
  printk("Swapbytes not implemented\n");
}

Subm() {
  printk("Subm not implemented\n");
}

Addm() {
  printk("Addm not implemented\n");
}

Calc_Checksum_not(Byte_Count,Start,Srca,Dsta,PtclT) {
  return Calc_Checksum(Byte_Count,Start,Srca,Dsta,PtclT);
  printk("Calc_Checksum not implemented\n");
}

Calc_checksum_not(Byte_Count,Start,Srca,Dsta,PtclT) {
  return Calc_Checksum(Byte_Count,Start,Srca,Dsta,PtclT);
  printk("Calc_checksum not implemented\n");
}

End_Lock() {
  printk("End_Loc not implemented\n");
}

Gen_Checksum_not() {
  printk("Gen_Checksum not implemented\n");
}

find_cpu_data() {
  printk("find_cpu_data not implemented\n");
}

Queue_Not_Empty_not() {
  printk("Queue_Not_Empty not implemented\n");
}

rpc_service() {
  printk("rpc_service not implemented\n");
}

drv$qblk_free_not() {
  printk("drv$qblk_free not implemented\n");
}

drv$seg_free_not() {
  printk("drv$seg_free not implemented\n");
}

Warn_Error() {
  printk("Warn_Error not implemented\n");
}

RPC_SERVICE() {
  printk("RPC_SERVICE not implemented\n");
}

RPC$INIT() {
  printk("RPC$INIT not implemented\n");
}

RPC$CONFIG_AUTH() {
  printk("RPC$CONFIG_AUTH not implemented\n");
}

RPC$CONFIG() {
  printk("RPC$CONFIG not implemented\n");
}

PokeAddr() {
  printk("PokeAddr not implemented\n");
}

LOG$OUT() {
  printk("LOG$OUT not implemented\n");
}

Fatal_Error() {
  printk("Fatal_Error not implemented\n");
}

DRV$Warn_FAO_not() {
  printk("DRV$Warn_FAO not implemented\n");
}

DRV$WARN_FAO_not() {
  printk("DRV$WARN_FAO not implemented\n");
}

DRV$QL_FAO_not() {
  printk("DRV$QL_FAO not implemented\n");
}

DRV$OPR_FAO_not() {
  printk("DRV$OPR_FAO not implemented\n");
}

DRV$Error_FAO_not() {
  printk("DRV$Error_FAO not implemented\n");
}

exe$finish_rdb() {
  printk("exe$finish_rdb not implemented\n");
}

drv$seg_get_not() {
  printk("drv$seg_ge not implemented\n");
}

drv$device_error_not() {
  printk("drv$device_error not implemented\n");
}

RPC$CHECK_PORT() {
  // don't be loud;  printk("RPC$CHECK_PORT not implemented\n");
  return -1;
}

CH$RCHAR() {
  printk("CH$RCHAR not implemented\n");
}

DRV$FATAL_FAO_not() {
  printk("DRV$FATAL_FAO not implemented\n");
}

DRV$QBLK_Free_not() {
  printk("DRV$QBLK_Free not implemented\n");
}

DRV$XLOG_FAO_not() {
  printk("DRV$XLOG_FAO not implemented\n");
}

RPC$INPUT() {
  printk("RPC$INPUT not implemented\n");
  return 0;
}

drv$ip_receive_not() {
  printk("drv$ip_receive not implemented\n");
}

SEG$LOG_Segment_not() {
  printk("SEG$LOG_Segment not implemented\n");
}

DRV$XQL_FAO_not() {
  printk("DRV$XQL_FAO not implemented\n");
}

swapbytesiphdr(x,y) {
  swapbytes(3,y+2);
  swapbytes(1,y+10);
}

swapbytesicmphdr(x,y) {
  swapbytes(1,y+2);
}

swapbytesseghdr(x,y) {
  swapbytes(2,y);
  swapbytes(3,y+14);
}

inline BLISSIF(int i) {
  return i&1;
}

inline BLISSIFNOT(int i) {
  return BLISSIF(i)==0;
}

malloc_not(size) 
{
  return kmalloc(size,GFP_KERNEL);
}

inline DEVICELOCK(){
  return;
  printk("DEVICELOCK not impl\n");
}

inline DEVICEUNLOCK(){
  return;
  printk("DEVICEUNLOCK not impl\n");
}

#include "libasnmbx.c"
