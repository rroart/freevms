#ifndef SYSTEMTEST_H
#define SYSTEMTEST_H

int call_sys$getsyiw1(unsigned short int code, void *value,
                      unsigned short *length);

int call_sys$getsyiw2(unsigned short int code1, void *value1,
                      unsigned short *length1, unsigned short int code2, void *value2,
                      unsigned short *length2);

void run_system_test(void);

#endif /* SYSTEMTEST_H */
