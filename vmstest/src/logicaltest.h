#ifndef LOGICALTEST_H
#define LOGICALTEST_H

int call_sys$crelnm(char *table_name, char *logical_name, char *value,
                    unsigned int *attribute, unsigned char *access_mode);

int call_sys$crelnt(char *table_name, char *parent_table,
                    unsigned int *attribute, unsigned int *quota,
                    unsigned short* protection_mask, unsigned char *access_mode);

int call_sys$dellnm(char *table_name, char *logical_name,
                    unsigned char *access_mode);

int call_sys$trnlnm(char *table_name, char *logical_name, char *value_list,
                    unsigned int *attribute, unsigned char *access_mode);

void run_logical_test(void);

#endif /* LOGICALTEST_H */
