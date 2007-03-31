#include <System_map.h>

extern struct System_map System_map[];

int sda_find_symbol(long addr, char ** name, long * offset) {
  int i;
  for ( i = 0; System_map[i+1].name; i++) {
    if (addr >= System_map[i].addr && addr < System_map[i+1].addr) {
#if 0
      printf("NAME %x %s\n",System_map[i].name,System_map[i].name);
#endif
      if (name)
	*name = System_map[i].name;
      if (offset)
	*offset = addr - System_map[i].addr;
      return 1;
    }
  }
  return 0;
}

int sda_find_addr(char * name, long * addr) {
  int i;
  for ( i = 0; System_map[i].name; i++) {
    if (strlen(name) == strlen(System_map[i].name) && 0 == strcmp (name, System_map[i].name)) {
      if (addr)
	*addr = System_map[i].addr;
      return 1;
    }
  }
  return 0;
}
