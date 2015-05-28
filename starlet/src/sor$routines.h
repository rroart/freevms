#ifndef SOR_ROUTINES_H
#define SOR_ROUTINES_H

#include <zarg.h>

#define SOR$BEGIN_MERGE sor$begin_merge

unsigned int sor$begin_merge(short * key_buffer, short * lrl, int * options,
                             char * merge_order, long * user_compare, long * user_equal,
                             long * user_input, long * context);
#define sor$begin_merge(...) sor$begin_merge(_buildargz9(__VA_ARGS__))

#define SOR$BEGIN_SORT sor$begin_sort

unsigned int sor$begin_sort(short * key_buffer, short * lrl, int * options,
                            int * file_alloc, long * user_compare, long * user_equal,
                            char * sort_process, char * work_files, long * context);
#define sor$begin_sort(...) sor$begin_sort(_buildargz9(__VA_ARGS__))

#define SOR$DO_MERGE sor$do_merge

int sor$do_merge(void);

#define SOR$END_SORT sor$end_sort

unsigned int sor$end_sort(long * context);
#define sor$end_sort(...) sor$end_sort(_buildargz(__VA_ARGS__))

#define SOR$INIT_MERGE sor$init_merge

int sor$init_merge(void);

#define SOR$INIT_SORT sor$init_sort

int sor$init_sort(void);

#define SOR$PASS_FILES sor$pass_files

unsigned int sor$pass_files(void * inp_desc, void * out_desc, char * org,
                            char * rfm, char * bks, short * bls, short * mrs, int * alq, int * fop,
                            char * fsz, long * context);
#define sor$pass_files(...) sor$pass_files(_buildargz11(__VA_ARGS__))

#define SOR$RELEASE_REC sor$release_rec

unsigned int sor$release_rec(void * desc, long * context);
#define sor$release_rec(...) sor$release_rec(_buildargz2(__VA_ARGS__))

#define SOR$RETURN_REC sor$return_rec

unsigned int sor$return_rec(void * desc, short * length, long * context);
#define sor$return_rec(...) sor$return_rec(_buildargz3(__VA_ARGS__))

#define SOR$SORT_MERGE sor$sort_merge

unsigned int sor$sort_merge(long * context);
#define sor$sort_merge(...) sor$sort_merge(_buildargz(__VA_ARGS__))

#define SOR$SPEC_FILE sor$spec_file

unsigned int sor$spec_file(void * spec_file, void * spec_buffer, long *context);
#define sor$spec_file(...) sor$spec_file(_buildargz3(__VA_ARGS__))

#define SOR$STAT sor$stat

unsigned int sor$stat(int * code, int * result, long * context);
#define sor$stat(...) sor$stat(_buildargz3(__VA_ARGS__))

#endif /* SOR_ROUTINES_H */
