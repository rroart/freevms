void lib$insv(int * source ,int * position , char * size , int *base) {
  int src=*source;
  int s=*size;
  int pos=*position;
  int src_mask=(1<<s)-1;
  int dst_mask=0xffffffff-(src_mask<<pos);
  src&=src_mask;
  (*base)&=dst_mask;
  (*base)|=src<<pos;
}
