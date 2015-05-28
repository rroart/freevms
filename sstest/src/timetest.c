#include <string.h>

#include <descrip.h>
#include <ssdef.h>

#include <starlet.h>

#include "expect.h"
#include "timetest.h"

int call_sys$asctim(unsigned short *time_name_len, char *time_name, unsigned long long *date_time)
{
    struct dsc$descriptor_s time_name_desc =
        { *time_name_len, DSC$K_DTYPE_T, DSC$K_CLASS_S, time_name };
    return sys$asctim(time_name_len, &time_name_desc, date_time, 0);
}

int call_sys$bintim(char *time_name, unsigned long long *date_time)
{
    struct dsc$descriptor_s time_name_desc =
        { (unsigned short) strlen(time_name), DSC$K_DTYPE_T, DSC$K_CLASS_S, time_name };
    return sys$bintim(&time_name_desc, date_time);
}

int call_sys$gettim(unsigned long long *date_time)
{
    return sys$gettim(date_time);
}

int call_sys$numtim(unsigned short int *time_vector, unsigned long long *date_time)
{
    return sys$numtim(time_vector, date_time);
}

#define MAX_TIME_NAME_LEN 23

void run_time_test(void)
{
    unsigned long long date_time = 0;
    unsigned long long date_time_copy = 0;
    unsigned short time_name_length = MAX_TIME_NAME_LEN;
    char time_name[MAX_TIME_NAME_LEN];
    unsigned short int time_vector[7];

    EXPECT_EQ(call_sys$gettim(&date_time), SS$_NORMAL);
    EXPECT_NE(memcmp(&date_time, &date_time_copy, sizeof(date_time)), 0);
    EXPECT_EQ(call_sys$asctim(&time_name_length, time_name, &date_time), SS$_NORMAL);
    EXPECT_EQ(time_name_length, MAX_TIME_NAME_LEN);
    EXPECT_NE(strncmp(time_name, "", time_name_length), 0);
    EXPECT_EQ(call_sys$bintim(time_name, &date_time_copy), SS$_NORMAL);
    EXPECT_EQ(memcmp(&date_time, &date_time_copy, sizeof(date_time)), 0);
    EXPECT_EQ(call_sys$numtim(time_vector, &date_time_copy), SS$_NORMAL);
}
