#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<idbdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ucbnidef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<nmadef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>

#include<linux/mm.h>
#include<linux/if_ether.h>

int lan$setmode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c){ 
  if ((i->irp$l_func&(IO$M_CTRL|IO$M_STARTUP))==(IO$M_CTRL|IO$M_STARTUP)) {
    struct dsc$descriptor * d=i->irp$l_qio_p2;
    long * l=d;
    long len=l[0]/(2*sizeof(long));
    long *addr = d->dsc$a_pointer;

    unsigned short int chan;
    struct _ucbnidef * newucb;
    ioc_std$clone_ucb(u /*ddb->ddb$ps_ucb*/ /*&er$ucb*/,&newucb);
    //    exe$assign(dsc,&chan,0,0,0);

    struct _ucbnidef * ni=newucb;

    //struct _lsbdef * lsb=kmalloc(sizeof(struct _lsbdef),GFP_KERNEL);
    //bzero(lsb,sizeof(struct _lsbdef));
    for(;len;len--) {
      switch (*addr++) {
      case NMA$C_PCLI_PTY:
	//lsb->lsb$l_valid_pty=*addr++;
	ni->ucb$l_ni_pty=*addr++;
	break;
      default:
      }
    }
    //lsb->lsb$l_next_lsb=ni->ucb$l_ni_lsb;
    //ni->ucb$l_ni_lsb=lsb;
  }
}

int lan$sensemode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { }

int lan$setchar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { }

int lan$sensechar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { }

int lan$eth_type_trans(struct _ucb * u, void * data ) {
	struct _ucbnidef * ni = u;
 	struct net_device * dev = ni -> ucb$l_extra_l_1;
	
	struct ethhdr *eth;
	unsigned char *rawp;
	
	eth= data;
	
	if(*eth->h_dest&1)
	{
#if 0
		if(memcmp(eth->h_dest,dev->broadcast, ETH_ALEN)==0)
			skb->pkt_type=PACKET_BROADCAST;
		else
			skb->pkt_type=PACKET_MULTICAST;
#endif
	}
	
	/*
	 *	This ALLMULTI check should be redundant by 1.4
	 *	so don't forget to remove it.
	 *
	 *	Seems, you forgot to remove it. All silly devices
	 *	seems to set IFF_PROMISC.
	 */
	 
	else if(1 /*dev->flags&IFF_PROMISC*/)
	{
#if 0
		if(memcmp(eth->h_dest,dev->dev_addr, ETH_ALEN))
			skb->pkt_type=PACKET_OTHERHOST;
#endif
	}
	
	if (ntohs(eth->h_proto) >= 1536)
		return eth->h_proto;
		
#if 0
	rawp = skb->data;
#else
	rawp = eth; // wrong
#endif

	/*
	 *	This is a magic hack to spot IPX packets. Older Novell breaks
	 *	the protocol design and runs IPX over 802.3 without an 802.2 LLC
	 *	layer. We look for FFFF which isn't a used 802.2 SSAP/DSAP. This
	 *	won't work for fault tolerant netware but does for the rest.
	 */
	if (*(unsigned short *)rawp == 0xFFFF)
		return htons(ETH_P_802_3);
		
	/*
	 *	Real 802.2 LLC
	 */
	return htons(ETH_P_802_2);
}

int lan$netif_rx(struct _ucb * u, char * buf ) {
  int proto=lan$eth_type_trans(u, buf);
  struct _ucbnidef * ni;
  struct _ucb * head=u;
  struct _ucb * tmp=u->ucb$l_fqfl;
  while (head!=tmp) {
    ni = tmp;
    if (proto == ni->ucb$l_ni_pty) break;
    tmp=tmp->ucb$l_fqfl;
  }
}

init_etherdev() {
  printk("init etherdev not imopl\n");
}

register_netdev() {
  printk("reg net not impl\n");
}

unregister_netdev() {
  printk ("unreg netdev not impl\n");
} 

register_inetaddr_notifier() {
  printk(" register_inetaddr_notifie not impl\n");
}

dev_close() {
  printk("dev close not impol\n");
}

__u32 in_aton(const char *str)
{
	unsigned long l;
	unsigned int val;
	int i;

	l = 0;
	for (i = 0; i < 4; i++) 
	{
		l <<= 8;
		if (*str != '\0') 
		{
			val = 0;
			while (*str != '\0' && *str != '.') 
			{
				val *= 10;
				val += *str - '0';
				str++;
			}
			l |= val;
			if (*str != '\0') 
				str++;
		}
	}
	return(htonl(l));
}


