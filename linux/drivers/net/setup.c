
/*
 *  New style setup code for the network devices
 */

#include <linux/config.h>
#include <linux/netdevice.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/netlink.h>

/* Pad device name to IFNAMSIZ=16. F.e. __PAD6 is string of 9 zeros. */
#define __PAD6 "\0\0\0\0\0\0\0\0\0"
#define __PAD5 __PAD6 "\0"
#define __PAD4 __PAD5 "\0"
#define __PAD3 __PAD4 "\0"
#define __PAD2 __PAD3 "\0"


/*
 *  Devices in this list must do new style probing. That is they must
 *  allocate their own device objects and do their own bus scans.
 */

struct net_probe
{
    int (*probe)(void);
    int status; /* non-zero if autoprobe has failed */
};

static struct net_probe pci_probes[] __initdata =
{
    /*
     *  Early setup devices
     */
    {NULL, 0}
};


/*
 *  Run the updated device probes. These do not need a device passed
 *  into them.
 */

static void __init network_probe(void)
{
    struct net_probe *p = pci_probes;

    while (p->probe != NULL)
    {
        p->status = p->probe();
        p++;
    }
}


/*
 *  Initialise the line discipline drivers
 */

static void __init network_ldisc_init(void)
{
}


static void __init special_device_init(void)
{
}

/*
 *  Initialise network devices
 */

void __init net_device_init(void)
{
    /* Devices supporting the new probing API */
    network_probe();
    /* Line disciplines */
    network_ldisc_init();
    /* Special devices */
    special_device_init();
    /* That kicks off the legacy init functions */
}
