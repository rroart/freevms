int lib$extzv(int * position , char * size , int *base) {
  int retval;
  int s=*size;
  int pos=*position;
  int src_mask=(1<<s)-1;
  retval=((*base)>>pos)&src_mask;
  return retval;
}
