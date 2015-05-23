#include <string.h>
#include "system_map.h"

extern struct system_map system_map[];

int sda_find_symbol(long addr, char ** name, long * offset)
{
    int i;
    for ( i = 0; system_map[i+1].name; i++)
    {
        if (addr >= system_map[i].addr && addr < system_map[i+1].addr)
        {
#if 0
            printf("NAME %x %s\n",system_map[i].name,system_map[i].name);
#endif
            if (name)
                *name = system_map[i].name;
            if (offset)
                *offset = addr - system_map[i].addr;
            return 1;
        }
    }
    return 0;
}

int sda_find_addr(char * name, long * addr)
{
    int i;
    for ( i = 0; system_map[i].name; i++)
    {
        if ((strlen(name) == strlen(system_map[i].name)) && (0 == strcmp (name, system_map[i].name)))
        {
            if (addr)
                *addr = system_map[i].addr;
            return 1;
        }
    }
    return 0;
}
