/*
    ****************************************************************

        Copyright (c) 1992, Carnegie Mellon University

        All Rights Reserved

    Permission  is  hereby  granted   to  use,  copy,  modify,  and
    distribute  this software  provided  that the  above  copyright
    notice appears in  all copies and that  any distribution be for
    noncommercial purposes.

    Carnegie Mellon University disclaims all warranties with regard
    to this software.  In no event shall Carnegie Mellon University
    be liable for  any special, indirect,  or consequential damages
    or any damages whatsoever  resulting from loss of use, data, or
    profits  arising  out of  or in  connection  with  the  use  or
    performance of this software.

    ****************************************************************
*/
/*

Module:

    ROUTE

Facility:

    Routing facility

Abstract:

    Given an IP address, this module determains which interface
    to send it out of.  The Routing Information Block (RIB)
    consists of the list of route knows to the IPACP.  Route
    look-up is augmented by the

Author:

    Bruce R. Miller, CMU Network Development, 1991
    Copyright (c) 1991, Carnegie-Mellon University

Modification History:

*/

#module ROUTE "V1.0"
#pragma builtins
#pragma member_alignment

#include stdio
#include ssdef



/*
    Entry Points:
    -------------

    ROUTE$IP_route ( IPaddr )

        Returns interface number.



    Organization:
    -------------
*/



typedef unsigned char   u_byte;
typedef unsigned short  u_word;
typedef unsigned long   u_long;



/*  Interface definition */

typedef struct
{
    u_long  next;       /* Next route */
    u_long  prev;       /* Previous route */
    u_long  Inum;       /* Interface number */

    u_long  IPaddr;     /* Interface IP address */
    u_long  Network;    /* Network IP address */
    u_long  NetMask;    /* Network Mask */
} Interface;



/*  Route Entry definition */

typedef struct
{
    u_long  next;       /* Next route */
    u_long  prev;       /* Previous route */

    u_long  Create_Time;    /* Creation Time */
    u_long  Delete_Time;    /* Deletion Time (when to timeout) */

    u_long  Next_Hop;   /* Route's router */
    u_long  Network;    /* Network IP address */
    u_long  NetMask;    /* Network Mask */
    u_long  Mask_NOB;   /* Mask number-of-bits */

    u_long  Inum;       /* Interface Number */

    u_long  TOS;        /* Route Type-of-Service */
    u_long  IS_Class;   /* Route IS-IS Class */
    u_long  OSPF_Class; /* Route OSPF Class */
} Route_Entry;



/*  IP Cache Entry definition */
/* NB:  These *must* be quadword alligned!!! */

typedef struct
{
    u_long  next;       /* Next */
    u_long  prev;       /* Previous */
    struct
    {
        u_long  valid :  1; /* cache entry valid? */
        u_long  dummy : 31; /* space filler */
    } flags;

    u_long  Create_Time;    /* Creation time */
    u_long  Delete_Time;    /* Deletion time */

    u_long  IPaddr;     /* IP address */
    u_long  Route;      /* Route Entry ptr */

    u_long  dummy;      /* Insure quadword allignment! */
} IPcache_Entry;

typedef struct
{
    u_long  QHead;
    u_long  QTail;
} IPcache_hashtab_entry;



#define MAX_ARP_ADDR_SIZE 20

/*  ARP Cache Entry definition */

typedef struct
{
    u_long  next;       /* Next */
    u_long  prev;       /* Previous */
    struct
    {
        u_long  valid :  1; /* cache entry valid? */
        u_long  dummy : 31; /* space filler */
    } flags;

    u_long  Create_Time;    /* Creation time */
    u_long  Delete_Time;    /* Deletion time */

    u_long  Inum;       /* Interface Number */
    u_long  AddrSize;   /* address size */
    u_byte  AddrData[MAX_ARP_ADDR_SIZE];    /* address data */
} ARPcache_Entry;



#define Route_LockName      "Route_Lock"
#define IPcache_LockName    "IPcache_Lock"
#define ARPcache_LockName   "ARPcache_Lock"

/* General */
time_now;
zone_id;

/* Linked list of interfaces */
u_long  Interface_List[2];
int     nInterfaces;

/* Linked list of route entries */
last_mod_time;
u_long  Route_List[2];
int     nRoutes;

/* The IP address cache */
u_long  IPcache_FreeList[2];    /*!!!HACK!!! quadword alligned? */
IPcache_hashtab_entry   *IP_Cache;
int             IPcache_ents_tot;

/* The IP address cache statistics */
int IPcache_ents_used;
int IPcache_overruns;
int IPcache_hits;
int IPcache_misses;

/* The ARP cache */
ARPcache_Entry  (*ARP_Cache)[10];   /*!!!HACK!!! bogus bound */
int         ARPcache_size;



int Calc_NOB ( mask )

u_long mask;
{
    int count=0;

    while (mask)
    {
        if (mask&1) ++count;
        mask = mask >> 1;
    }

    return (count);
}

int MM$GET_MEM ( ptr , size )
char **ptr;
int size;
{
    int Status;

    Status = LIB$GET_VM(&size,ptr,0);
    if (!(Status&1)) signal(Status);

    return(Status);
}

#define Critical_Section_Begin;
#define Critical_Section_End;

int Time_Stamp ()
{
    return(time_now);
}





/****************************************************************

    ARP cache routines

 ****************************************************************/



/****************************************************************

    IP cache routines

 ****************************************************************/

int IPcache_hash(IPaddr)
u_long  IPaddr;
{
    u_byte *IPaddr2 = &IPaddr;
    int hash;

    hash = IPaddr2[0] + IPaddr2[1] + IPaddr2[2] + IPaddr2[3];
    hash = hash % IPcache_ents_tot;
    return (hash);
}




int IPcache_Init(size)

int size;
{
    int i;
    IPcache_Entry *IPent;
    u_long tmp;

    /* itinialize the self-relative IP cache entry free list */
    IPcache_FreeList[1] = IPcache_FreeList[0] = 0;

    /* Allocate and initialize the IP hash table and cache entries */
    /* for now, the hash table is as wide as the number of entries */
    MM$GET_MEM ( &IP_Cache, size*8 );
    MM$GET_MEM ( &tmp, size * sizeof(IPcache_Entry) );
    for (i=0; i<size; i++)
    {
        IP_Cache[i].QHead = IP_Cache[i].QTail = 0;
        IPent = tmp + (i*sizeof(IPcache_Entry));
        printf("IPent = %x(%d)\n",IPent,sizeof(IPcache_Entry));
        IPent->next = IPent->prev = 0;
        _INSQHI(IPent,IPcache_FreeList);
    }
    IPcache_ents_tot    = size;
    IPcache_ents_used   = 0;
    IPcache_overruns    = 0;
    IPcache_hits    = 0;
    IPcache_misses  = 0;
}



int IPcache_Check(IPaddr,Route)

u_long IPaddr,*Route;
{
    int hval,flag=0;
    IPcache_Entry   *IPent;

    hval = IPcache_hash(IPaddr);

    Critical_Section_Begin;

    IPent = (char *)&IP_Cache[hval] + IP_Cache[hval].QHead;
    while (IPent != &IP_Cache[hval].QHead)
    {
        if (IPaddr == IPent->IPaddr)
        {
            /* Maybe we could move this entry to the head of the queue? */
            *Route = IPent->Route;
            flag = 1;
            break;
        }
        (char *)IPent += IPent->next;
    }
    Critical_Section_End;

    if (flag) ++IPcache_hits;
    else ++IPcache_misses;

    return (flag);
}




int IPcache_Add(IPaddr,Route)

u_long IPaddr,*Route;
{
    int hval,RC,flag;
    IPcache_Entry   *IPent;

    hval = IPcache_hash(IPaddr);
    RC = _REMQHI(IPcache_FreeList,&IPent);
    if (RC&5)        /* not very clear, is it? */
    {
        ++IPcache_overruns;
        return 0;
    }

    IPent->flags.valid = 1;
    IPent->IPaddr = IPaddr;
    IPent->Route = Route;

    Critical_Section_Begin;

    _INSQHI(IPent,&IP_Cache[hval]);

    Critical_Section_End;

    ++IPcache_ents_used;
    return (1);
}




/****************************************************************

    Route List routines

 ****************************************************************/

/*  Route module initialization */

int ROUTE$Init ( ARP_Cache_Size , IP_Cache_Size )
{
    int i;



    /* Initialize route list */
    Route_List[1] = Route_List[0] = Route_List;
    nRoutes = 0;

    IPcache_Init(IP_Cache_Size);

    /* Allocate and initialize the ARP cache */
    MM$GET_MEM ( &ARP_Cache, ARP_Cache_Size );
    for (i=0; i<ARP_Cache_Size; i++)
        (*ARP_Cache)[i].flags.valid = 0;
    ARPcache_size = 0;

    return (SS$_NORMAL);
}



/*

    ROUTE$Add

    Add a route to the known routes list.  We insert it into the
    list based on the number of bits in the NetMask.

    NB: Should we check for duplicates?

*/

int ROUTE$Add ( Next_Hop, Network, NetMask, Inum, TOS, IS_Class, OSPF_Class )

u_long Next_Hop, Network, NetMask, Inum, TOS, IS_Class, OSPF_Class;
{

    Route_Entry *RE,*pred;

    /* Allocate and initialize the route entry */
    MM$GET_MEM( &RE , sizeof(Route_Entry));
    RE->next = RE->prev = RE;
    RE->Next_Hop    = Next_Hop;
    RE->Network     = Network;
    RE->NetMask     = NetMask;
    RE->Mask_NOB    = Calc_NOB(NetMask);
    RE->Inum        = Inum;
    RE->TOS     = TOS;
    RE->IS_Class    = IS_Class;
    RE->OSPF_Class  = OSPF_Class;

    Critical_Section_Begin;
    pred = Route_List[1];
    while (pred != &Route_List[0])
    {
        if (pred->Mask_NOB > RE->Mask_NOB)
        {
            _INSQUE ( RE , pred );
            break;
        }
        pred = pred->prev;
    }
    if (pred == &Route_List[0])
        _INSQUE ( RE , pred );
    Critical_Section_End;

    printf("ROUTE$Add(Rtr=%8x  Net=%8x  Msk=%8x  Int=%2d  NOB=%2d)\n",
           RE->Next_Hop,
           RE->Network,
           RE->NetMask,
           RE->Inum,
           RE->Mask_NOB);
    ++nRoutes;
    return (SS$_NORMAL);
}



/*
    ROUTE$Find

    Search the route list from front to back.  Since the list
    is sorted in descending order by the number of bits in the
    NetMask, the first match will be the best (longest) match.
*/

int ROUTE$Find ( IPaddr , Next_Hop, Network, NetMask, Inum )

u_long IPaddr;
u_long *Next_Hop, *Network, *NetMask, *Inum;
{
    int Status;
    Route_Entry *RE,*result=NULL;

    printf("ROUTE$Find(%8x) ",IPaddr);

    Critical_Section_Begin;

    /* First check the cache */
    Status = IPcache_Check(IPaddr,&result);
    if (!Status)
    {
        /* search through list and break-out on first match */
        RE = Route_List[0];
        while (RE != Route_List)
        {
            if ((IPaddr & RE->NetMask) == (RE->Network & RE->NetMask))
            {
                result = RE;
                break;
            }
            RE = RE->next;
        }
    }

    /* Did we find a match? */
    if (result != NULL)
    {
        if (!Status)
            IPcache_Add(IPaddr,result);
        else printf(" !cache hit!");

        *Next_Hop   = result->Next_Hop;
        *Network    = result->Network;
        *NetMask    = result->NetMask;
        *Inum       = result->Inum;
    }

    Critical_Section_End;

    if (result != NULL)
        printf(" => Int=%2d  Rtr=%8x  Net=%8x  Msk=%8x\n",
               result->Inum,
               result->Next_Hop,
               result->Network,
               result->NetMask);
    else printf(" no route\n",IPaddr);

    return (result == NULL);
}



/****************************************************************

    Router game routines

 ****************************************************************/





/*
    GAME$Print

    Print out all of the routes in the list to stdout.
*/

int GAME$Print_Stats ()
{
    int total;
    float ratio;

    printf("Route stats :\n");

    total = IPcache_hits + IPcache_misses;
    printf("\tIPcache stats: %d accesses (%d hits, %d misses)",
           total, IPcache_hits, IPcache_misses);
    ratio = (total ? ((float)IPcache_hits/total) : 0);
    printf("   succ ratio = %f%%\n",ratio);
    printf("\tIPcache stats:  %d entries (%d in-use w/ %d overruns\n",
           IPcache_ents_tot,IPcache_ents_used,IPcache_overruns);

}



/*
    GAME$Print

    Print out all of the routes in the list to stdout.
*/

int GAME$Print ()
{
    int count=0;
    Route_Entry *RE;

    RE = Route_List[0];

    printf("Route List (%d):\n",nRoutes);
    Critical_Section_Begin;
    /* loop through list and print entries */
    while (RE != Route_List)
    {
        printf("   Rtr=%8x  Net=%8x  Msk=%8x  Int=%2d  NOB=%2d\n",
               RE->Next_Hop,
               RE->Network,
               RE->NetMask,
               RE->Inum,
               RE->Mask_NOB);
        ++count;
        RE = RE->next;
    }
    Critical_Section_End;

    return (count);
}



int GAME$Add_Interface(IPaddr,NetMask)
{
    Interface   *IF;

    printf("Adding interface #%d (IPaddr=%8x,Mask=%8x)\n",
           nInterfaces,IPaddr,NetMask);
    /* Allocate and initialize the route entry */
    MM$GET_MEM( &IF , sizeof(Interface));
    IF->next = IF->prev = IF;
    IF->IPaddr      = IPaddr;
    IF->NetMask     = NetMask;
    IF->Inum        = nInterfaces++;

    _INSQUE ( IF , Interface_List );

    /* our own addresses */
    ROUTE$Add ( 0x00000000, IPaddr, 0xFFFFFFFF, 0, 0, 0, 0);

}



int GAME$Add_Router(GwyIP,Network,NetMask)
{
    Interface *IF;
    int Inum=0;

    printf("Adding gateway, addr=%8x  Net=%8x, Msk=%8x\n",
           GwyIP,Network,NetMask);

    /* search through list and break-out on first match */
    IF = Interface_List[0];
    while (IF != Interface_List)
    {
        if ((GwyIP & IF->NetMask) == (IF->IPaddr & IF->NetMask))
        {
            Inum = IF->Inum;
            break;
        }
        IF = IF->next;
    }
    /* Did we find a match? */
    if (Inum == 0)
    {
        printf("    Gateway inaccessable\n");
    }

    ROUTE$Add ( GwyIP, Network, NetMask, Inum);

    return (Inum != 0);
}



int GAME$Init( IPcache_size, ARPcache_size )
{

    printf("Initializing IPACP\n");
    time_now = 0;
    ROUTE$Init(IPcache_size,ARPcache_size);

    /* Initialize interface list */
    Interface_List[1] = Interface_List[0] = Interface_List;

    nInterfaces = 0;    /* loopback */

    /* The loopback interface */
    GAME$Add_Interface(0x7F000000,0xFF000000);

    /* static rules */
    /* The loopback addresses */
    ROUTE$Add ( 0x00000000, 0xFF000000, 0xFF000000, 0, 0, 0, 0);

}



int GAME$Time_Passes(ms)
{
}



int GAME$ICMP_DUnreach(Code,SrcIP,DstIP)
{
}



int GAME$ICMP_TExceed(SrcIP,DstIP,TTL)
{
}



int GAME$ICMP_Redirect(Code,GwyIP,SrcIP,DstIP)
{
}



int GAME$ICMP_Gwy_Advert()
{
}



int GAME$IP_Receive(SrcIP,DstIP,Int)
{
}



int GAME$IP_Send(DstIP)
{
    u_long Status;
    u_long router, net, mask, int_num;

    printf("Sending IP datagram to %8x\n",DstIP);
    Status = ROUTE$Find ( DstIP, &router, &net, &mask, &int_num );
    printf("   IP datagram routed : *** %8x => %d ***\n",DstIP,int_num);
}



main ()
{
    u_long Status;
    u_long Ether1int,Ether2int,SLIPint,X25int;

    GAME$Init(2,2);
    printf("\n");

    GAME$Print();
    printf("\n");

    /* Add some interfaces */
    /* Two EtherNets */
    Ether1int   = GAME$Add_Interface ( 0x8002e845, 0xFFFF0000);
    Ether2int   = GAME$Add_Interface ( 0x80020745, 0xFFFFFF00);
    /* A SLIP connection */
    SLIPint = GAME$Add_Interface ( 0x80030102, 0xFFFF0000);
    /* and a X.25 link to the outside world */
    X25int  = GAME$Add_Interface ( 0x80025502, 0x00000000);
    printf("\n");

    /* Add some routers */
    GAME$Add_Router ( 0x80025502, 0x00000000, 0x00000000 );
    GAME$Add_Router ( 0x80020101, 0x80020000, 0xFFFF0000 );
    printf("\n");

    Status = GAME$IP_Send ( 0x8002e845 );
    Status = GAME$IP_Send ( 0xb0050505 );
    Status = GAME$IP_Send ( 0x00000000 );
    Status = GAME$IP_Send ( 0xFF000001 );
    Status = GAME$IP_Send ( 0x8003e845 );
    Status = GAME$IP_Send ( 0x8002e845 );
    printf("\n");

    GAME$Time_Passes(

        GAME$Print();
        printf("\n");

        GAME$Print_Stats();
        printf("\n");
}
