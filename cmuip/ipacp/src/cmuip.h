#ifndef CMUIP_H
#define CMUIP_H

struct tcb_structure;
BLISSIF(int i);
BLISSIFNOT(int i);
ACT_OPEN (void);
LOG_OPEN (void);
long long tcp$service_connections (void);
void user$process_user_requests (void);
long long Time_Stamp();
CNF$Define_IPACP_Interface (void);
RPC$INIT();
void mm$init (void);
void ip$init  (void);
void user$init_routines (void);
MINU(int, int);
MIN(int, int);
MAXU(int, int);
void NML$INIT (void);
void CNF$Configure_ACP (void);
void CNF$Net_Device_Init (void);
void tcp$init (void);
void    NML$GETNAME();
CNF$Get_Local_IP_addr (void);
void SEG$Process_Received_Segments(void);
void CHECK_ERRMSG_Q (void);
void user$purge_all_io ();
void NML$PURGE(int);
REMQUE(long * e, long * a);
//int ip_send(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);
void user$post_io_status (UARG,STATUS,NBYTES,FLAGS,ICMCODE);
swapbytesseghdr(long x, long y);
void ASCII_DEC_BYTES(DESC,COUNT,SOURCE,LEN);
int ip$send(IP$Src,IP$Dest,Service,Life,Seg,SegSize,ID,Frag,Delete_Seg,Protocol,Buf,Bufsize);
INSQUE(long x, long y);
void Conect_Insert(struct tcb_structure * TCB,signed long CN_Index);
long CH$MOVE(long size, long src, long addr);
void mm$qblk_free(Ptr);
Signal();
void ASCII_HEX_BYTES(DESC,COUNT,SOURCE,LEN);
MAX(x,y);
TCB_Find(lclport,frnaddr,frnport);
VTCB_Scan ( ASTRTN , ASTP1 , ASTP2 );
CH$FILL(long ch, long size, long addr);
void user$net_connection_info(struct user_info_args * uargs,
                              long Lcl_Host,long Frn_Host,long Lcl_Port,long Frn_Port,
                              long Frn_Name,long Frn_Nlen);
swapbytesiphdr(long x, long y);
Fatal_Error() ;
int CH$NEQ(long n1, long ptr1, long n2, long ptr2);
int CH$EQL(long n1, long ptr1, long n2, long ptr2);
long CH$PTR(long X);
#ifndef CH$PLUS
CH$PLUS(long x, long y);
#endif
UPLIT();
ch$move(long a, long b, long c);
exe$finish_rdb();
mm$uarg_get (void);
tcp$send_ctl(struct tcb_structure * tcb,long type);
int tcp$send_data(struct tcb_structure * tcb);
void icmp$close(struct user_close_args * uargs);
void icmp$open(struct user_open_args * uargs);
void icmp$receive(struct user_recv_args * uargs);
void icmp$send(struct user_send_args * uargs);
void ipu$close(struct user_close_args * Uargs);
void ipu$open(struct user_open_args * Uargs);
void ipu$receive(struct user_recv_args * Uargs);
void ipu$send(struct user_send_args * Uargs);
void ipu$status(struct user_status_args * Uargs);
void tcp$abort(struct user_abort_args * Uargs);
void tcp$close(struct user_close_args * Uargs);
void tcp$info(struct user_info_args * Uargs);
void tcp$open(struct user_open_args * Uargs);
void tcp$receive(struct user_recv_args * Uargs);
void tcp$send(struct user_send_args * Uargs);
void tcp$send_ack(struct tcb_structure * tcb);
void tcp$send_enqueue();
void tcp$send_enqueue(tcb,bufcount,buf,pushf);
void tcp$status(struct user_status_args * Uargs);
void udp$close(struct user_close_args * Uargs);
void udp$open(struct user_open_args * Uargs);
void udp$receive(struct user_recv_args * Uargs);
void udp$send(struct user_send_args * Uargs);
swapbytesicmphdr(long x,long y);
snmp_input(in_buff,in_len,out_buff,out_len);
CH$DIFF();
int swapbytes(long WrdCnt, long Start);
send_2_operator(TEXT);
void insque(void * entry, void * pred);
Subm();
Addm();
Warn_Error();
snmp_agent_parse(data, length, out_data, out_length, sourceip);
get_community(sessionid);
shift_array(begin, length, shift_amount);
setVariable(var_val, var_val_type, var_val_len, statP, statLen);
goodValue(inType, inLen, actualType, actualLen);
SCH$IOLOCKW();
SCH$IOUNLOCK();
FORKLOCK();
FORKUNLOCK();
int find_ucb(R1, U);
int Dismount();
unlock_iodb(long * R4);
lock_iodb(long * R4);
build_vcb(R5,R8);
Build_ACP_QB(R5);
int Calc_Check0(R0,R2,Byte_Count,Start,Srca,Dsta,ptclt);
unsigned long remque(void * entry, void * addr);
int VMS_IO$POST(long IOSB$ADRS, long IRP$ADRS, long UCB$ADRS);

// from string.h, which inclusion make type overlap. needed due to tcpmacros.h
#ifdef __x86_64__
unsigned long strlen(const char *s);
#ifndef memcpy
void *memcpy(void *dest, const void *src, unsigned long n);
#endif
#ifndef memset
void *memset(void *s, int c, unsigned long n);
#endif
#else
unsigned int strlen(const char *s);
#ifndef memcpy
void *memcpy(void *dest, const void *src, unsigned int n);
#endif
#ifndef memset
void *memset(void *s, int c, unsigned int n);
#endif
#endif
// from stdlib.h, which inclusion make type overlap
void free(void *ptr);
#ifdef __x86_64__
void *malloc(unsigned long size);
#else
void *malloc(unsigned int size);
#endif

#ifdef __i386__
//externals of cmuip
int aqempty(void * q);
int exe_std$alononpaged (int reqsize, int *alosize_p, void **pool_p);
void qhead_init(void * l);
int ioc$searchdev(struct return_values *r, void * devnam);
int exe$ipid_to_epid(unsigned long pid);
#endif

#endif
