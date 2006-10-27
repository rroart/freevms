// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ssdef.h>
#include <iodef.h>
#include <sordef.h>
#if 0
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

int sortunix = 1;

struct sor_sort_merge {
  short * key_buffer;
  short lrl;
  int options;
  int file_alloc;
  long user_compare;
  long user_equal;
  char sort_process;
  char work_files;
  int filidx;
  void * fil[16];
  char merge_order;
  long user_input;
  int sortidx;
  int sortidx2;
  void * sort[1024];
};
static struct sor_sort_merge sort_merge;

int sor$begin_sort (short * key_buffer, short * lrl, int * options, int * file_alloc, long * user_compare, long * user_equal, char * sort_process, char * work_files, long * context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    if (*context)
      sort_merge_p = *context;
    else {
      sort_merge_p = malloc (sizeof (struct sor_sort_merge) ); 
      memset (sort_merge_p, 0, sizeof (struct sor_sort_merge));
    }
  }
  if (key_buffer)
    sort_merge_p->key_buffer = key_buffer;
  if (lrl)
    sort_merge_p->lrl = *lrl;
  if (options)
    sort_merge_p->options = *options;
  if (file_alloc)
    sort_merge_p->file_alloc = *file_alloc;
  if (user_compare)
    sort_merge_p->user_compare = *user_compare;
  if (user_equal)
    sort_merge_p->user_equal = *user_equal;
  if (sort_process)
    sort_merge_p->sort_process = *sort_process;
  if (work_files)
    sort_merge_p->work_files = *work_files;
  return SS$_NORMAL;
}

int sor$begin_merge (short * key_buffer, short * lrl, int * options, char * merge_order, long * user_compare, long * user_equal, long * user_input, long * context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    if (*context)
      sort_merge_p = *context;
    else {
      sort_merge_p = malloc (sizeof (struct sor_sort_merge) ); 
      memset (sort_merge_p, 0, sizeof (struct sor_sort_merge));
    }
  }
  if (key_buffer)
    sort_merge_p->key_buffer = key_buffer;
  if (lrl)
    sort_merge_p->lrl = *lrl;
  if (options)
    sort_merge_p->options = *options;
  if (merge_order)
    sort_merge_p->merge_order = *merge_order;
  if (user_compare)
    sort_merge_p->user_compare = *user_compare;
  if (user_equal)
    sort_merge_p->user_equal = *user_equal;
  if (user_input)
    sort_merge_p->user_input = *user_input;
  return SS$_NORMAL;
}

int sor$dtype (long * context, short * dtype_code, int * usage, int * p1) {
}

int sor$end_sort (long * context) {
  // need more cleanup
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    sort_merge_p = *context;
  }
  memset (sort_merge_p, 0, sizeof (struct sor_sort_merge));
  if (context) {
    free(sort_merge_p);
    *context = 0;
  }
  return SS$_NORMAL;
}

int sor$pass_files (void * inp_desc, void * out_desc, char * org, char * rfm, char * bks, short * bls, short * mrs, int * alq, int * fop, char * fsz, long * context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    if (*context)
      sort_merge_p = *context;
    else {
      sort_merge_p = malloc (sizeof (struct sor_sort_merge) ); 
      memset (sort_merge_p, 0, sizeof (struct sor_sort_merge));
    }
  }
  sort_merge_p->fil[sort_merge_p->filidx++] = inp_desc;
  return SS$_NORMAL;
}

int sor$release_rec (void * desc, long * context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    sort_merge_p = *context;
  }
  struct dsc$descriptor * dsc = malloc(sizeof(struct dsc$descriptor));
  struct dsc$descriptor * d = desc;
  dsc->dsc$w_length = d->dsc$w_length;
  dsc->dsc$a_pointer = malloc (d->dsc$w_length);
  memcpy (dsc->dsc$a_pointer, d->dsc$a_pointer, d->dsc$w_length);
  sort_merge_p->sort[sort_merge_p->sortidx++] = dsc;
  return SS$_NORMAL;
}

int sor$return_rec (void * desc, short * length, long * context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    sort_merge_p = *context;
  }
  struct dsc$descriptor * d = sort_merge_p->sort[sort_merge_p->sortidx2++];
  if (d == 0)
    return SS$_ENDOFFILE;
  struct dsc$descriptor * dest = desc;
  memcpy (dest->dsc$a_pointer, d->dsc$a_pointer, d->dsc$w_length);
  if (length)
    *length = d->dsc$w_length;
  return SS$_NORMAL;
}

static int my_compare (struct dsc$descriptor * v1, struct dsc$descriptor * v2) {
  return strcmp(v1->dsc$a_pointer, v2->dsc$a_pointer);
}

static int my_compare_o_n (struct dsc$descriptor * v1, struct dsc$descriptor * v2, int offset, int size) {
  return strncmp(v1->dsc$a_pointer + offset, v2->dsc$a_pointer + offset, size);
}

static int myswap (long * a, long * b) {
  long t = *a;
  *a = *b;
  *b = t;
}

int sor$sort_merge (long * context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    sort_merge_p = *context;
  }
  int i, j;
  for (i = 0; i < sort_merge_p->filidx; i++) {
    struct dsc$descriptor * d = sort_merge_p->fil[i];
    void * file = fopen (d->dsc$a_pointer, "r");
    char buf[sort_merge_p->lrl];
#if 0
    int rd;
    for ( ; ; ) {
      rd = read (fd, buf, 132);
      if (rd == 0) break;
    }
#else
    while (fgets(buf, sort_merge_p->lrl, file)) {
#endif
      struct dsc$descriptor dsc;
      dsc.dsc$w_length = strlen(buf) - 1;
      dsc.dsc$a_pointer = buf;
      sor$release_rec(&dsc, 0);
    }
    fclose(file);
  }
  for (i = sort_merge_p->sortidx - 1; i >= 0; i--) {
    for (j = 1; j <= i; j++) {
      if (sort_merge_p->key_buffer == 0) {
	int (*compare)() = my_compare;
	if (sort_merge_p->user_compare)
	  compare = sort_merge_p->user_compare;
	if (compare (sort_merge_p->sort[j - 1], sort_merge_p->sort[j]) > 0) {
	  myswap(&sort_merge_p->sort[j - 1], &sort_merge_p->sort[j]);
	}
      } else {
	int (*compare)() = my_compare_o_n;
	short * key_buffer = sort_merge_p->key_buffer;
	int num = *key_buffer++;
	int cmp = 0;
	for ( ; num && cmp == 0; num-- ) {
	  int type = *key_buffer++; // unused
	  int order = *key_buffer++; // unused
	  int offset = *key_buffer++;
	  int len = *key_buffer++;
	  cmp = compare (sort_merge_p->sort[j - 1], sort_merge_p->sort[j], offset, len);
	}
	if (cmp > 0)
	  myswap(&sort_merge_p->sort[j - 1], &sort_merge_p->sort[j]);
      }
    }
  }
  return SS$_NORMAL;
}

int sor$spec_file (void * spec_file, void * spec_buffer, long *context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    sort_merge_p = *context;
  }
  return SS$_NORMAL;
}

int sor$stat (int * code, int * result, long * context) {
  struct sor_sort_merge * sort_merge_p = &sort_merge;
  if (context) {
    sort_merge_p = *context;
  }
  return SS$_NORMAL;
}

