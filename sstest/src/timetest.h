#ifndef TIMETEST_H
#define TIMETEST_H

int call_sys$asctim(unsigned short *time_name_len, char *time_name, unsigned long long *date_time);

int call_sys$bintim(char *time_name, unsigned long long *date_time);

int call_sys$gettim(unsigned long long *date_time);

int call_sys$numtim(unsigned short int *time_vector, unsigned long long *date_time);

void run_time_test(void);

#endif /* TIMETEST_H */
