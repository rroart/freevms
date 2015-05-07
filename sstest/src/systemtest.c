#include <string.h>

#include <iledef.h>
#include <starlet.h>
#include <ssdef.h>
#include <syidef.h>

#include <linux/version.h>

#include "expect.h"
#include "systemtest.h"

int call_sys$getsyiw1(unsigned short int code, void *value,
                      unsigned short *length)
{
    struct _ile3 item_list[] =
    {
        { *length, code, value, length },
        { 0, 0, NULL, NULL }
    };
    return sys$getsyiw(0, NULL, NULL, item_list, NULL, NULL, 0);
}

int call_sys$getsyiw2(unsigned short int code1, void *value1,
                      unsigned short *length1, unsigned short int code2, void *value2,
                      unsigned short *length2)
{
    struct _ile3 item_list[] =
    {
        { *length1, code1, value1, length1 },
        { *length2, code2, value2, length2 },
        { 0, 0, NULL, NULL }
    };
    return sys$getsyiw(0, NULL, NULL, item_list, NULL, NULL, 0);
}

#define MAX_VERSION_STRING_LENGTH 32
#define MAX_SCSNODE_STRING_LENGTH 16

void run_system_test(void)
{
    char version[MAX_VERSION_STRING_LENGTH];
    unsigned short int version_length = MAX_VERSION_STRING_LENGTH;
    unsigned long long boottime;
    unsigned short int boottime_length = sizeof(unsigned long long);
    char scsnode[MAX_SCSNODE_STRING_LENGTH];
    unsigned short int scsnode_length = MAX_SCSNODE_STRING_LENGTH;

    EXPECT_EQ(call_sys$getsyiw1(SYI$_VERSION, NULL, &version_length), SS$_ACCVIO);
    EXPECT_EQ(call_sys$getsyiw1(11111, version, &version_length), SS$_BADPARAM);
    EXPECT_EQ(call_sys$getsyiw1(SYI$_VERSION, version, &version_length), SS$_NORMAL);
    EXPECT_EQ(strncmp(version, FREEVMS_RELEASE, version_length), 0);
    EXPECT_EQ(call_sys$getsyiw1(SYI$_BOOTTIME, &boottime, &boottime_length), SS$_NORMAL);
    EXPECT_EQ(boottime_length, sizeof(unsigned long long));
    EXPECT_EQ(call_sys$getsyiw1(SYI$_SCSNODE, scsnode, &scsnode_length), SS$_NORMAL);
    EXPECT_EQ(strncmp(scsnode, "MYNODE", scsnode_length), 0);

    memset(version, 0, MAX_VERSION_STRING_LENGTH);
    version_length = MAX_VERSION_STRING_LENGTH;
    memset(scsnode, 0, MAX_SCSNODE_STRING_LENGTH);
    scsnode_length = MAX_SCSNODE_STRING_LENGTH;
    EXPECT_EQ(call_sys$getsyiw2(SYI$_VERSION, version, &version_length, SYI$_SCSNODE, scsnode, &scsnode_length), SS$_NORMAL);
    EXPECT_EQ(strncmp(version, FREEVMS_RELEASE, version_length), 0);
    EXPECT_EQ(strncmp(scsnode, "MYNODE", scsnode_length), 0);
}
