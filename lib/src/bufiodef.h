#ifndef bufiodef_h
#define bufiodef_h
	
#define BUFIO$K_64 -1                   
#define BUFIO$K_HDRLEN32 12             
#define BUFIO$K_HDRLEN64 24             
	
struct _bufio {
  void *bufio$ps_pktdata;             
  void *bufio$ps_uva32;               
  unsigned short int bufio$w_size;    
  unsigned char bufio$b_type;         
  char bufio$b_fill_1;
  int bufio$l_fill_2;
  void *bufio$pq_uva64;               
};
 
#endif 
 
