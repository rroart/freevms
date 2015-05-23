// $Id$
// $Locker$

// Author. Roar Thron�s.

#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include <stdio.h>
#include <string.h>

extern char * module_name;

extern char *strndup (__const char *__string, size_t __n);

int out;

void genwrite()
{
    if (module_name)
    {
        int len = strlen(module_name);
        char * dup = strndup(module_name,len+4);
        dup[len]='.';
        dup[len+1]='c';
        dup[len+2]=0;
        char * dupagain = strndup(dup,len+2);
        out = fopen(dupagain, "w");
    }
    else
        out = fopen("msgtables.c", "w");
#if 0
    fprintf(out, "#include \"cli.h\"\n\n");
    if (module_name)
        fprintf(out, "struct _cdu %s[] = {\n",module_name);
    else
        fprintf(out, "struct _cdu cdu_root[] = {\n");
    int i = 0;

    fprintf(out,"};\n");

    fclose(out);
#endif
}
