struct SCB$ {
unsigned char SCB$W_STRUCLEV[2];
unsigned short int SCB$W_CLUSTER;
unsigned int SCB$L_VOLSIZE;
unsigned int SCB$L_BLKSIZE;
unsigned int SCB$L_SECTORS;
unsigned int SCB$L_TRACKS;
unsigned int SCB$L_CYLINDER;
unsigned int SCB$L_STATUS;
unsigned int SCB$L_STATUS2;
unsigned short int SCB$W_WRITECNT;
unsigned char SCB$T_VOLOCKNAME[12];
unsigned char SCB$Q_MOUNTTIME[4];
unsigned char dummy[456];
unsigned char SCB$W_CHECKSUM[2];
};
