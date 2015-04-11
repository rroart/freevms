#ifndef LOGICALTEST_H
#define LOGICALTEST_H

int call_sys$crelnm(char *table_name, char *logical_name, char *value);
int call_sys$crelnt(char *table_name, char *parent_table);
int call_sys$dellnm(char *table_name, char *logical_name);
int call_sys$trnlnm(char *table_name, char *logical_name, char *value_list);

void run_logical_test(void);

#endif /* LOGICALTEST_H */
