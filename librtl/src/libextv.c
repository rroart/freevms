int lib$extv(int * position , char * size , int *base) {
  int retval;
  int s=*size;
  int pos=*position;
  int src_mask=(1<<s)-1;
  retval=((*base)>>pos)&src_mask;
  if (retval&(1<<(s-1))) {
    int signs=0xffffffff;
    signs&=~src_mask;
    retval|=signs;
  }
  return retval;
}
