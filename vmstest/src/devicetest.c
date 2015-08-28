#include <string.h>

#include <descrip.h>
#include <dvidef.h>
#include <iledef.h>
#include <starlet.h>
#include <ssdef.h>

#include "devicetest.h"
#include "expect.h"

int call_sys$assign(char *device_name, unsigned short *channel, unsigned int access_mode, char *mailbox_name, unsigned int flags)
{
    struct dsc$descriptor_s device_name_desc =
        { (unsigned short) strlen(device_name), DSC$K_DTYPE_T, DSC$K_CLASS_S, device_name };
    if (mailbox_name != NULL)
    {
        struct dsc$descriptor_s mailbox_name_desc =
            { (unsigned short) strlen(mailbox_name), DSC$K_DTYPE_T, DSC$K_CLASS_S, mailbox_name };
        return sys$assign(&device_name_desc, channel, access_mode, &mailbox_name_desc, flags);
    }
    return sys$assign(&device_name_desc, channel, access_mode, 0, flags);
}

int call_sys$dassgn(unsigned short channel)
{
    return sys$dassgn(channel);
}

int call_sys$getdviw1(unsigned short code, void *value, unsigned short *length)
{
    struct _ile3 item_list[] =
        {
            { *length, code, value, length },
            { 0, 0, NULL, NULL } };
    return sys$getdviw(0, 0, NULL, item_list, NULL, NULL, 0, NULL);
}

int call_sys$getdviw2(unsigned short code1, void *value1, unsigned short *length1, unsigned short code2, void *value2,
        unsigned short *length2)
{
    struct _ile3 item_list[] =
        {
            { *length1, code1, value1, length1 },
            { *length2, code2, value2, length2 },
            { 0, 0, NULL, NULL } };
    return sys$getdviw(0, 0, NULL, item_list, NULL, NULL, 0, NULL);
}

#define MAX_DEVICE_NAME_LEN 15

void run_device_test(void)
{
    char device_name[MAX_DEVICE_NAME_LEN + 1];
    unsigned short device_name_length = MAX_DEVICE_NAME_LEN;
    unsigned short channel = 0;

    EXPECT_EQ(call_sys$getdviw1(DVI$_DEVNAM, device_name, &device_name_length), SS$_NORMAL);
    EXPECT_NE(device_name_length, 0);
    EXPECT_NE(strncmp(device_name, "", device_name_length), 0);
    device_name[device_name_length] = '\0';

    EXPECT_EQ(call_sys$assign("", &channel, 0, NULL, 0), SS$_ACCVIO);
    EXPECT_EQ(call_sys$assign("invalidDevice", &channel, 0, NULL, 0), SS$_NOSUCHDEV);
    EXPECT_EQ(call_sys$assign("DQA001", NULL, 0, NULL, 0), SS$_ACCVIO);
    EXPECT_EQ(call_sys$assign("dqa001", &channel, 0, NULL, 0), SS$_NORMAL);
    EXPECT_NE(channel, 0);

    EXPECT_EQ(call_sys$dassgn(0), SS$_IVCHAN);
    EXPECT_EQ(call_sys$dassgn(channel), SS$_NORMAL);
}
