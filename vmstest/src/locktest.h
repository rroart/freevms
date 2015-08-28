#ifndef LOCKTEST_H
#define LOCKTEST_H

int call_sys$deq(unsigned int lock_id, unsigned int access_mode, unsigned int flags);

int call_sys$enqw(unsigned int lock_mode, struct _lksb *lock_status_block, unsigned int flags, char *lock_name,
        unsigned int access_mode);

int call_sys$getlkiw1(unsigned int *lock_id_address, unsigned short code, void *value, unsigned short *length,
        struct _iosb *io_status_block);

void run_lock_test(void);

#endif /* LOCKTEST_H */
