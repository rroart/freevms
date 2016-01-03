/*
 * include/linux/random.h
 *
 * Include file for the random number generator.
 */

#ifndef _LINUX_RANDOM_H
#define _LINUX_RANDOM_H

/* Exported functions */

#ifdef __KERNEL__

extern void rand_initialize(void);
extern void rand_initialize_irq(int irq);

extern void add_keyboard_randomness(unsigned char scancode);
extern void add_interrupt_randomness(int irq);
extern void add_blkdev_randomness(int major);

void generate_random_uuid(unsigned char uuid_out[16]);

#endif /* __KERNEL___ */

#endif /* _LINUX_RANDOM_H */
