struct FAT$ {
unsigned char FAT$B_RTYPE;
unsigned char FAT$B_RATTRIB;
unsigned short int FAT$W_RSIZE;
unsigned int FAT$L_HIBLK;
unsigned int FAT$L_EFBLK;
unsigned short int FAT$W_FFBYTE;
unsigned char FAT$B_BKTSIZE;
unsigned char FAT$B_VFCSIZE;
unsigned short int FAT$W_MAXREC;
unsigned short int FAT$W_DEFEXT;
unsigned short int FAT$W_GBC;
unsigned char  dummy[8];
unsigned short int FAT$W_VERSIONS;
};

