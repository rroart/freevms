// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ssdef.h>

#include <descrip.h>

#ifndef NOKERNEL
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "cmuip.h"

#ifdef NOKERNEL
#define printk myprintk
#define kfree free
#define kmalloc(x,y) malloc(x)

unsigned long remque(void * entry, void * addr);

void myprintk(char *s)
{
    struct dsc$descriptor dsc;
    dsc.dsc$a_pointer=s;
    dsc.dsc$w_length=strlen(s);
    send_2_operator(&dsc);
}

#endif

CH$FILL(long ch, long size, long addr)
{
    memset(addr,ch, size);
    return addr+size;
    printk("CH$FILL not implemented\n");
}

CH$RCHAR_A()
{
    printk("CH$RCHAR_A not implemented\n");
}

CH$ALLOCATION()
{
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

LIB$CALLG()
{
    printk("LIB$CALLG not implemented\n");
}

LIB$FIND_IMAGE_SYMBOL()
{
    printk("LIB$FIND_IMAGE_SYMBOL not implemented\n");
}

Signal()
{
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

long CH$PTR(long X)
{
    return X;
}

int STR$CASE_BLIND_COMPARE(x,y)
struct dsc$descriptor *x, * y;
{
    return strncmp(x->dsc$a_pointer,y->dsc$a_pointer,y->dsc$w_length);
}

int CH$EQL(long n1, long ptr1, long n2, long ptr2)
{
    int n = ( n1 < n2 ? n1 : n2);
    return (0==memcmp(ptr1,ptr2,n));
}

int CH$NEQ(long n1, long ptr1, long n2, long ptr2)
{
    int n = ( n1 < n2 ? n1 : n2);
    return (memcmp(ptr1,ptr2,n));
}

UPLIT()
{
    printk("UPLIT not implemented\n");
}

PLIT()
{
    printk("PLIT not implemented\n");
}

long CH$MOVE(long size, long src, long addr)
{
    memcpy(addr,src, size);
    return addr+size;
    printk("CH$MOVE not implemented\n");
}

INSQUE(long x, long y)
{
    insque(x,y);
    return SS$_NORMAL;
    printk("INSQUE not implemented\n");
}

REMQUE(long * e, long * a)
{
    int retval=0;
    if (e==e[1]) retval|=1;
    remque(e,0);
    if (e[0]==e[1]) retval|=2;
    *a=e;
    return retval;
    printk("REMQUE not implemented\n");
}

MIN(x,y)
{
    return (x<y ? x :  y);
    printk("MIN not implemented\n");
}

MAX(x,y)
{
    return (x>y ? x :  y);
    printk("MAX not implemented\n");
}

MINU(x,y)
{
    return MIN(x,y);
    printk("MINU not implemented\n");
}

XLOG$FAO()
{
    // not implemented, but no printk
    // printk("XLOG$FAO not implemented\n");
}

MAXU(x,y)
{
    return MAX(x,y);
    printk("MAXU not implemented\n");
}

ROT(x,y)
{
    return x>>y;
    printk("ROT not implemented\n");
}

QL$FAO()
{
    printk("QL$FAO not implemented\n");
}

XQL$FAO()
{
    printk("XQL$FAO not implemented\n");
}

#define IPMEMDEB
#undef IPMEMDEB

#ifdef IPMEMDEB
long mymi=0;
long myms[1024];
#endif

LIB$GET_VM_PAGE(size, addr)
long * addr;
{
#if 0
    *addr=kmalloc(4096*(size/8+1),GFP_KERNEL); // check
#else
    //  *addr=kmalloc(4096*((size>>9)+1),GFP_KERNEL); // check
    // *addr=kmalloc(4096*(size/8+1),GFP_KERNEL); // check
#ifdef IPMEMDEB
    myms[mymi++]=size*512;
#endif
    *addr=kmalloc(512*size,GFP_KERNEL); // check
#ifdef IPMEMDEB
    myms[mymi++]=*addr;
    if (mymi>1000) mymi=0;
#endif
#endif
    return SS$_NORMAL;
    printk("LIB$GET_VM_PAGE not implemented\n");
}

LIB$STAT_VM()
{
    printk("LIB$STAT_VM not implemented\n");
}

LIB$RESET_VM_ZONE()
{
    printk("LIB$RESET_VM_ZONE not implemented\n");
}

LIB$DELETE_VM_ZONE()
{
    printk("LIB$DELETE_VM_ZONE not implemented\n");
}

LIB$CREATE_USER_VM_ZONE()
{
    printk("LIB$CREATE_USER_VM_ZONE not implemented\n");
}

WARN$FAO()
{
    printk("WARN$FAO not implemented\n");
}

ERROR$FAO()
{
    printk("ERROR$FAO not implemented\n");
}

LIB$GET_VM(size, addr)
long * addr;
{
#ifdef IPMEMDEB
    myms[mymi++]=size;
#endif
    *addr=kmalloc(size,GFP_KERNEL);
#ifdef IPMEMDEB
    myms[mymi++]=*addr;
    if (mymi>1000) mymi=0;
#endif
    return SS$_NORMAL;
    printk("LIB$GET_VM not implemented\n");
}

CH$DIFF()
{
    printk("CH$DIFF not implemented\n");
}

LIB$FREE_VM_PAGE(long size, long addr)
{
#ifdef IPMEMDEB
    myms[mymi++]=size*512+1;
    myms[mymi++]=addr;
    if (mymi>1000) mymi=0;
#endif
    kfree(addr);
    return SS$_NORMAL;
    printk("LIB$FREE_VM_PAGE not implemented\n");
}

FORKLOCK()
{
    return;
    printk("FORKLOCK not implemented\n");
}

FORKUNLOCK ()
{
    return;
    printk("FORKUNLOCK not implemented\n");
}

ch$move(long a, long b, long c)
{
    return CH$MOVE(a,b,c);
    printk("ch$move not implemented\n");
}

LIB$CREATE_VM_ZONE()
{
    printk("LIB$CREATE_VM_ZONE not implemented\n");
}

STR$UPCASE()
{
    printk("STR$UPCASE not implemented\n");
}

STR$MATCH_WILD()
{
    printk("STR$MATCH_WILD not implemented\n");
}

STR$TRIM()
{
    printk("STR$TRIM not implemented\n");
}

ACT$FAO()
{
    printk("ACT$FAO not implemented\n");
}

Begin_Lock()
{
    printk("Begin_Lock not implemented\n");
}

CH$PLUS(long x, long y)
{
    return x+y;
    printk("CH$PLUS not implemented\n");
}

LIB$ASN_WTH_MBX()
{
    printk("LIB$ASN_WTH_MBX not implemented\n");
}

SCH$IOLOCKW()
{
    printk("SCH$IOLOCKW not implemented\n");
    return SS$_NORMAL;
}

SCH$IOUNLOCK()
{
    printk("SCH$IOUNLOCK not implemented\n");
    return SS$_NORMAL;
}

STR$FREE1_DX(struct dsc$descriptor * d)
{
    kfree(d->dsc$a_pointer);
    return;
    printk("STR$FREE1_DX not implemented\n");
}

UNlock_IODB()
{
    printk("UNlock_IODB not implemented\n");
}

Unlock_IODB()
{
    printk("Unlock_IODB not implemented\n");
}

Subm()
{
    printk("Subm not implemented\n");
}

Addm()
{
    printk("Addm not implemented\n");
}

End_Lock()
{
    printk("End_Loc not implemented\n");
}

rpc_service()
{
    printk("rpc_service not implemented\n");
}

Warn_Error()
{
    printk("Warn_Error not implemented\n");
}

RPC_SERVICE()
{
    printk("RPC_SERVICE not implemented\n");
}

RPC$INIT()
{
    printk("RPC$INIT not implemented\n");
}

RPC$CONFIG_AUTH()
{
    printk("RPC$CONFIG_AUTH not implemented\n");
}

RPC$CONFIG()
{
    printk("RPC$CONFIG not implemented\n");
}

PokeAddr()
{
    printk("PokeAddr not implemented\n");
}

LOG$OUT()
{
    printk("LOG$OUT not implemented\n");
}

Fatal_Error()
{
    printk("Fatal_Error not implemented\n");
}

exe$finish_rdb()
{
    printk("exe$finish_rdb not implemented\n");
}

RPC$CHECK_PORT()
{
    // don't be loud;  printk("RPC$CHECK_PORT not implemented\n");
    return -1;
}

CH$RCHAR()
{
    printk("CH$RCHAR not implemented\n");
}

RPC$INPUT()
{
    printk("RPC$INPUT not implemented\n");
    return 0;
}

swapbytesiphdr(long x, long y)
{
    swapbytes(3,y+2);
    swapbytes(1,y+10);
}

swapbytesicmphdr(long x,long y)
{
    swapbytes(1,y+2);
}

swapbytesseghdr(long x, long y)
{
    swapbytes(2,y);
    swapbytes(3,y+14);
}

inline BLISSIF(int i)
{
    return i&1;
}

inline BLISSIFNOT(int i)
{
    return BLISSIF(i)==0;
}

inline DEVICELOCK()
{
    return;
    printk("DEVICELOCK not impl\n");
}

inline DEVICEUNLOCK()
{
    return;
    printk("DEVICEUNLOCK not impl\n");
}

#include "libasnmbx.c"

#ifdef NOKERNEL
#define panic printf
static int mycli() {}
static int mysti() {}

#ifdef __i386__
#define OFFSET 4
#endif

#ifdef __x86_64__
#define OFFSET 8
#endif

void insque(void * entry, void * pred)
{
    if (entry==pred) panic("same\n");
    if (entry==*(long *)pred) panic("same\n");
    if (entry==*(long *)(((long)pred)+OFFSET)) panic("same\n");
    int flag=mycli();
    //mycheckaddr();
    *(void **)entry=*(void **)pred;
    *(void **)(entry+OFFSET)=pred;
    *(void **)((*(void **)pred)+OFFSET)=entry;
    *(void **)pred=entry;
    //mycheckaddr();
    mysti(flag);
}

unsigned long remque(void * entry, void * addr)
{
    int flag=mycli();
    //mycheckaddr();
    *(void **)(*(void **)(entry+OFFSET))=*(void **)entry;
    *(void **)((*(void **)entry)+OFFSET)=*(void **)(entry+OFFSET);
    addr=entry;
    //mycheckaddr();
    mysti(flag);
    return (unsigned long) addr;
}
#endif
