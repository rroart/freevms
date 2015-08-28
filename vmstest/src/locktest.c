#include <string.h>

#include <descrip.h>
#include <iledef.h>
#include <lckdef.h>
#include <lkidef.h>
#include <lksbdef.h>
#include <ssdef.h>

#include <starlet.h>

#include "expect.h"
#include "locktest.h"

int call_sys$deq(unsigned int lock_id, unsigned int access_mode, unsigned int flags)
{
    return sys$deq(lock_id, NULL, access_mode, flags);
}

int call_sys$enqw(unsigned int lock_mode, struct _lksb *lock_status_block, unsigned int flags, char *lock_name,
        unsigned int access_mode)
{
    struct dsc$descriptor_s lock_name_desc =
        { (unsigned short) strlen(lock_name), DSC$K_DTYPE_T, DSC$K_CLASS_S, lock_name };
    return sys$enqw(0, lock_mode, lock_status_block, flags, &lock_name_desc, 0, NULL, 0, NULL, access_mode, 0);
}

int call_sys$getlkiw1(unsigned int *lock_id_address, unsigned short code, void *value, unsigned short *length,
        struct _iosb *io_status_block)
{
    struct _ile3 item_list[] =
    {
        { *length, code, value, length },
        { 0, 0, NULL, NULL }
    };

    return sys$getlkiw(0, lock_id_address, item_list, io_status_block, NULL, 0, 0);
}

#define MAX_LOCK_NAME_LEN 31

void run_lock_test(void)
{
    struct _iosb io_status_block;
    struct _lksb lock_status_block;
    char lock_name[MAX_LOCK_NAME_LEN + 1];
    unsigned short lock_name_length = MAX_LOCK_NAME_LEN;

    EXPECT_EQ(call_sys$enqw(LCK$K_NLMODE, &lock_status_block, 0, "TEST_LOCK1", 0), SS$_NORMAL);
    EXPECT_EQ(lock_status_block.lksb$w_status, SS$_NORMAL);
    EXPECT_EQ(call_sys$getlkiw1(&lock_status_block.lksb$l_lkid, LKI$_RESNAM, &lock_name, &lock_name_length, &io_status_block),
            SS$_NORMAL);
    /*
    EXPECT_EQ(io_status_block.iosb$l_getxxi_status, SS$_NORMAL);
    EXPECT_EQ(lock_name_length, strlen("TEST_LOCK1"));
    EXPECT_EQ(strncmp(lock_name, "TEST_LOCK1", lock_name_length), 0);
    */
    EXPECT_EQ(call_sys$deq(lock_status_block.lksb$l_lkid, 0, 0), SS$_NORMAL);
}
