#ifndef PROCESSTEST_H
#define PROCESSTEST_H

int call_sys$getjpiw1(unsigned short int code, void *value, unsigned short *length);

int call_sys$getjpiw2(unsigned short int code1, void *value1, unsigned short *length1, unsigned short int code2, void *value2,
                      unsigned short *length2);

int call_sys$setpri(unsigned int *process_id, unsigned int priority);

int call_sys$setprn(char *process_name);

void run_process_test(void);

#endif /* PROCESSTEST_H */
