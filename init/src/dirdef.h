struct DIR$ {
unsigned short int DIR$W_SIZE;
unsigned short int DIR$W_VERLIMIT;
unsigned char DIR$B_FLAGS;
unsigned char DIR$B_NAMECOUNT;
unsigned char DIR$T_NAME[10];  //hopefully, all 9 files have same size. Coincidence?
unsigned short int DIR$W_VERSION;
struct FID$ DIR$W_FID;
};
