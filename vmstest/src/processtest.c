#include <string.h>

#include <descrip.h>
#include <iledef.h>
#include <jpidef.h>
#include <starlet.h>
#include <ssdef.h>

#include "expect.h"
#include "processtest.h"

int call_sys$getjpiw1(unsigned short int code, void *value, unsigned short *length)
{
    struct _ile3 item_list[] =
    {
        { *length, code, value, length },
        { 0, 0, NULL, NULL }
    };
    return sys$getjpiw(0, NULL, NULL, item_list, NULL, NULL, 0);
}

int call_sys$getjpiw2(unsigned short int code1, void *value1, unsigned short *length1, unsigned short int code2, void *value2,
                      unsigned short *length2)
{
    struct _ile3 item_list[] =
    {
        { *length1, code1, value1, length1 },
        { *length2, code2, value2, length2 },
        { 0, 0, NULL, NULL }
    };
    return sys$getjpiw(0, NULL, NULL, item_list, NULL, NULL, 0);
}

int call_sys$setpri(unsigned int *process_id, unsigned int priority)
{
    return sys$setpri(process_id, 0, priority, 0, 0, 0);
}

int call_sys$setprn(char *process_name)
{
    struct dsc$descriptor_s process_name_desc =
        { (unsigned short) strlen(process_name), DSC$K_DTYPE_T, DSC$K_CLASS_S, process_name };
    return sys$setprn(&process_name_desc);
}

#define MAX_PROCESS_NAME_LEN 15

void run_process_test(void)
{
    char process_name[MAX_PROCESS_NAME_LEN];
    unsigned short int process_name_length = MAX_PROCESS_NAME_LEN;
    unsigned int extended_process_id = 0;
    short unsigned int extended_process_id_len = sizeof(extended_process_id);

    EXPECT_EQ(call_sys$getjpiw1(JPI$_PRCNAM, NULL, &process_name_length), SS$_ACCVIO);
    EXPECT_EQ(call_sys$getjpiw2(JPI$_PRCNAM, process_name, &process_name_length, JPI$_PID, &extended_process_id, &extended_process_id_len), SS$_NORMAL);
    EXPECT_EQ(strncmp(process_name, "vmstest", process_name_length), 0);
    EXPECT_EQ(extended_process_id_len, sizeof(extended_process_id));
    EXPECT_EQ(call_sys$getjpiw1(9999, process_name, &process_name_length), SS$_BADPARAM);
    //EXPECT_EQ(call_sys$setpri(&extended_process_id, 11), SS$_NORMAL);
    //EXPECT_EQ(call_sys$setpri(NULL, 5), SS$_NORMAL);
    EXPECT_EQ(call_sys$setprn("my_vmstest"), SS$_NORMAL);
}
