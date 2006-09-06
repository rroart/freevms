// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <libdef.h>
#include <ssdef.h>
#include <starlet.h>

// long has been chosen over int because of different behaviour on amd64 than axp

#define MAX_ZONE 16

static long vm_zone_table[MAX_ZONE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct zone_data {
  int algorithm;
  long algorithm_argument;
  int flags;
  int extend_size;
  int initial_size;
  int block_size;
  int alignment;
  int page_limit;
  int smallest_block_size;
  int (*get_page)();
  int (*free_page)();
};

int lib$free_vm_page (int * number_of_pages , long * base_address );
int lib$get_vm_page (int * number_of_pages , long * base_address );

static struct zone_data default_data = {
  algorithm : 1,
  algorithm_argument : 0,
  flags : 0,
  extend_size : 4096,
  initial_size : 0,
  block_size : 8,
  alignment : 8,
  page_limit : 0, // set max later
  smallest_block_size : 8,
  get_page : lib$get_vm_page,
  free_page : lib$free_vm_page
};

struct vm_zone {
  long * user_argument;
  int (*user_allocation_procedure)();
  int (*user_deallocation_procedure)();
  int (*user_reset_procedure)();
  int (*user_delete_procedure)();
  void *zone_name;
  struct zone_data * zone_data;
  void * pool;
};

static default_user_argument = 0;
$DESCRIPTOR (default_zone_name, "default_zone");

// rip from memoryalc start

struct _gen {
  struct _gen * gen$l_flink;
#if 0
  struct _gen * gen$l_blink;
  unsigned short int gen$w_size;
  unsigned char gen$b_type;
  unsigned char gen$b_subtype;
#else
  int gen$w_size; //change to l;
#endif
  int gen$l_poison;
};

// drop alignment later
static int lib$allocate(int requestsize, void ** poolhead, int alignment, unsigned int * allocatedsize, void ** returnblock) {
#if 0
  if (requestsize&15)
    requestsize=((requestsize>>4)+1)<<4; // mm book said something about align
#endif
  struct _gen * nextnext, * next, * cur = poolhead;
  while (cur->gen$l_flink) {
    next=cur->gen$l_flink;

    if (requestsize<=next->gen$w_size) {
#if 0
      check_packet(next,requestsize,0);
      poison_packet(next,requestsize,0);
#endif
      *allocatedsize=requestsize;
      *returnblock=next;
      nextnext=next->gen$l_flink;
      if (requestsize<next->gen$w_size) {
	int newsize=next->gen$w_size-requestsize;
	next=(long)next+requestsize;
	next->gen$l_flink=nextnext;
	next->gen$w_size=newsize;
	next->gen$l_poison=0x87654321;
	nextnext=next;
      }
      cur->gen$l_flink=nextnext;
      return SS$_NORMAL;
    }
    cur=next;
  }
  *allocatedsize=0;
  *returnblock=0;
  return SS$_INSFMEM;
}

static int lib$deallocate(void * returnblock, void ** poolhead, int size) {
#if 0
  if (requestsize&15)
    requestsize=((requestsize>>4)+1)<<4; // mm book said something about align
#endif
  struct _gen * middle = returnblock;
  struct _gen * nextnext, * next, * cur = poolhead;
  while (cur->gen$l_flink && ((unsigned long)cur->gen$l_flink<(unsigned long)returnblock)) {
    cur=cur->gen$l_flink;
  }
	
  next=cur->gen$l_flink;
  nextnext=next->gen$l_flink;
 
  middle->gen$w_size=size;
  middle->gen$l_flink=nextnext;
  middle->gen$l_poison=0x87654321;

  if (next && nextnext && ((unsigned long)next+next->gen$w_size)==(unsigned long)middle && ((unsigned long)middle+middle->gen$w_size)==nextnext) {
    next->gen$w_size+=middle->gen$w_size+nextnext->gen$w_size;
    next->gen$l_flink=nextnext->gen$l_flink;
#if 0
    poison_packet(next,next->gen$w_size,1);
#endif
    return SS$_NORMAL;
  }

  if (next && ((unsigned long)next+next->gen$w_size)==(unsigned long)middle) {
    next->gen$w_size+=middle->gen$w_size;
    next->gen$l_flink=nextnext;
#if 0
    poison_packet(next,next->gen$w_size,1);
#endif
    return SS$_NORMAL;
  }

  if (next && nextnext && ((unsigned long)middle+middle->gen$w_size)==nextnext) {
    middle->gen$w_size+=nextnext->gen$w_size;
    next->gen$l_flink=middle;
#if 0
    poison_packet(middle,middle->gen$w_size,1);
#endif
    return SS$_NORMAL;
  }

  next->gen$l_flink=middle;
  middle->gen$l_flink=nextnext;
#if 0
  poison_packet(middle,size,1);
#endif

  return SS$_NORMAL;
}

// rip from memoryalc end

int default_user_allocation_procedure(long * number_of_bytes , long * base_address, long * user_argument) {
#if 0
  *base_address = malloc (*number_of_bytes);
#endif
  return SS$_NORMAL;
}

int default_user_deallocation_procedure(long * number_of_bytes , long * base_address, long * user_argument) {
#if 0
  free (*base_address);
#endif
  return SS$_NORMAL;
}

int default_user_reset_procedure(long * user_argument) {
}

int default_user_delete_procedure(long * user_argument) {
}

static struct vm_zone default_zone = {
  &default_user_argument, 0 /*default_user_allocation_procedure*/, 0 /*default_user_deallocation_procedure*/, 0 /*default_user_reset_procedure*/, 0 /*default_user_delete_procedure*/, &default_zone_name, &default_data, 0
};

int lib$create_user_vm_zone (unsigned long * zone_id , long * user_argument , int (*user_allocation_procedure)() , int (*user_deallocation_procedure)() , int (*user_reset_procedure)() , int (*user_delete_procedure)() , void *zone_name) {
  struct vm_zone * vz = malloc (sizeof (struct vm_zone));
  vz->user_argument = user_argument;
  vz->user_allocation_procedure = user_allocation_procedure;
  vz->user_deallocation_procedure = user_deallocation_procedure;
  vz->user_reset_procedure = user_reset_procedure;
  vz->user_delete_procedure = user_delete_procedure;
  vz->zone_name = zone_name;
  *zone_id = vz;
  return SS$_NORMAL;
}

int lib$create_vm_zone (unsigned long * zone_id , int * algorithm, long *algorithm_argument , int * flags , int * extend_size , int * initial_size , int * block_size , int * alignment , int * page_limit , int * smallest_block_size ,void *zone_name , int (*get_page)() , int (*free_page)() ) {
  struct vm_zone * vz = malloc (sizeof (struct vm_zone));
  struct zone_data * zone_data = malloc (sizeof (struct zone_data));
  *vz = default_zone;
  vz->zone_name = zone_name;
  vz->zone_data = zone_data;
  *zone_data = default_data;
  if (algorithm)
    zone_data->algorithm = *algorithm;
  if (algorithm_argument)
    zone_data->algorithm_argument = *algorithm_argument;
  if (flags)
    zone_data->flags = *flags;
  if (extend_size)
    zone_data->extend_size = *extend_size;
  if (initial_size)
    zone_data->initial_size = *initial_size;
  if (block_size)
    zone_data->block_size = *block_size;
  if (alignment)
    zone_data->alignment = *alignment;
  if (page_limit)
    zone_data->page_limit = *page_limit;
  if (get_page)
    zone_data->get_page = get_page;
  if (free_page)
    zone_data->free_page = free_page;
  *zone_id = vz;
}

int lib$delete_vm_zone (unsigned long * zone_id) {
  struct vm_zone * vz;
  if (zone_id == 0 || *zone_id == 0)
    vz = &default_zone;
  else
    vz = *zone_id;
  if (vz->user_delete_procedure)
    return vz->user_delete_procedure(vz->user_argument);
  free (vz);
  return SS$_NORMAL;
}

int lib$reset_vm_zone (unsigned long * zone_id) {
  struct vm_zone * vz;
  if (zone_id == 0 || *zone_id == 0)
    vz = &default_zone;
  else
    vz = *zone_id;
  if (vz->user_reset_procedure)
    return vz->user_reset_procedure(vz->user_argument);
  free (vz->pool);
  vz->pool = 0;
  return SS$_NORMAL;
}

int lib$free_vm (long * number_of_bytes , long * base_address ,unsigned long * zone_id) {
  struct vm_zone * vz;
  if (zone_id == 0 || *zone_id == 0)
    vz = &default_zone;
  else
    vz = *zone_id;
  if (vz->user_deallocation_procedure)
    return vz->user_deallocation_procedure(number_of_bytes, base_address, vz->user_argument);
  lib$deallocate (*base_address, &vz->pool, *number_of_bytes);
  return SS$_NORMAL;
}

int lib$free_vm_page (int * number_of_pages , long * base_address ) {
  free (*base_address);
  return SS$_NORMAL;
}

int lib$get_vm (long * number_of_bytes, long * base_address ,unsigned long * zone_id) {
  struct vm_zone * vz;
  if (zone_id == 0 || *zone_id == 0)
    vz = &default_zone;
  else
    vz = *zone_id;
  if (vz->user_allocation_procedure)
    return vz->user_allocation_procedure(number_of_bytes, base_address, vz->user_argument);
  if (vz->pool == 0) { // temp fix
    long size = 20480;
    lib$get_vm_page (&size, &vz->pool);
    long *l = vz->pool;
    l[0] = 0;
    l[1] = size * 4096;
  }
  int ret_size;
  lib$allocate (*number_of_bytes, &vz->pool, 0 /* not yet */, &ret_size, base_address);
  return SS$_NORMAL;
}

int lib$get_vm_page (int * number_of_pages , long *base_address) {
  *base_address = malloc (*number_of_pages * 512); // check
  return SS$_NORMAL;
}

int lib$show_vm (int * code , int (*user_action_procedure)() , long * user_specified_argument) {
  // not yet
  return SS$_NORMAL;
}

int lib$show_vm_zone (unsigned long * zone_id , int * detail_level , int (*user_action_procedure)() , long *user_arg)
{
  struct vm_zone * vz;
  if (zone_id == 0 || *zone_id == 0)
    vz = &default_zone;
  else
    vz = *zone_id;
  if (*detail_level < 1)
    printf("Zone Id = %x\n\n", vz);
  if (*detail_level >= 1) {
    printf("Zone Id = %x,  Zone name = %s\n\n", vz, ((struct dsc$descriptor *)vz->zone_name)->dsc$a_pointer);
    struct zone_data * zone_data = vz->zone_data;
    printf("\tAlgorithm = %x\n\n", zone_data->algorithm);
    printf("\tFlags = %x\n\n", zone_data->flags);
    printf("\tInitial size = %x\n", zone_data->initial_size);
    printf("\tExtend size  = %x\t\tPage limit = %x\n", zone_data->extend_size, zone_data->page_limit);
  }
  return SS$_NORMAL;
}

int lib$verify_vm_zone (unsigned long * zone_id) {
  struct vm_zone * vz;
  if (zone_id == 0 || *zone_id == 0)
    vz = &default_zone;
  else
    vz = *zone_id;
  return SS$_NORMAL;
}

