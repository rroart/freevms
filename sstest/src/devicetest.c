#include <string.h>

#include <dvidef.h>
#include <iledef.h>
#include <starlet.h>
#include <ssdef.h>

#include "devicetest.h"
#include "expect.h"

int call_sys$getdviw1(unsigned short int code, void *value, unsigned short *length)
{
    struct _ile3 item_list[] =
    {
        { *length, code, value, length },
        { 0, 0, NULL, NULL }
    };
    return sys$getdviw(0, 0, NULL, item_list, NULL, NULL, 0, NULL);
}

int call_sys$getdviw2(unsigned short int code1, void *value1, unsigned short *length1, unsigned short int code2, void *value2,
        unsigned short *length2)
{
    struct _ile3 item_list[] =
    {
        { *length1, code1, value1, length1 },
        { *length2, code2, value2, length2 },
        { 0, 0, NULL, NULL }
    };
    return sys$getdviw(0, 0, NULL, item_list, NULL, NULL, 0, NULL);
}

#define MAX_DEVICE_NAME_LEN 15

void run_device_test(void)
{
    char device_name[MAX_DEVICE_NAME_LEN];
    unsigned short int device_name_length = MAX_DEVICE_NAME_LEN;

    EXPECT_EQ(call_sys$getdviw1(DVI$_DEVNAM, device_name, &device_name_length), SS$_NORMAL);
    EXPECT_NE(strncmp(device_name, "", MAX_DEVICE_NAME_LEN), 0);
}
