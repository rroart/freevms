/*
 * include/linux/random.h
 *
 * Include file for the random number generator.
 */

#ifndef _LINUX_RANDOM_H
#define _LINUX_RANDOM_H

#include <linux/ioctl.h>

/* Exported functions */

#ifdef __KERNEL__

extern void rand_initialize(void);
extern void rand_initialize_irq(int irq);
extern void rand_initialize_blkdev(int irq, int mode);

extern void batch_entropy_store(u32 a, u32 b, int num);

extern void add_keyboard_randomness(unsigned char scancode);
extern void add_mouse_randomness(__u32 mouse_data);
extern void add_interrupt_randomness(int irq);
extern void add_blkdev_randomness(int major);

extern void get_random_bytes(void *buf, int nbytes);
void generate_random_uuid(unsigned char uuid_out[16]);

extern __u32 secure_ip_id(__u32 daddr);
extern __u32 secure_tcp_sequence_number(__u32 saddr, __u32 daddr,
                                        __u16 sport, __u16 dport);
extern __u32 secure_tcp_syn_cookie(__u32 saddr, __u32 daddr,
                                   __u16 sport, __u16 dport,
                                   __u32 sseq, __u32 count,
                                   __u32 data);
extern __u32 check_tcp_syn_cookie(__u32 cookie, __u32 saddr,
                                  __u32 daddr, __u16 sport,
                                  __u16 dport, __u32 sseq,
                                  __u32 count, __u32 maxdiff);
extern __u32 secure_tcpv6_sequence_number(__u32 *saddr, __u32 *daddr,
        __u16 sport, __u16 dport);

extern __u32 secure_ipv6_id(__u32 *daddr);

#ifndef MODULE
extern struct file_operations random_fops, urandom_fops;
#endif

#endif /* __KERNEL___ */

#endif /* _LINUX_RANDOM_H */
