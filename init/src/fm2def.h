struct FM2$C_FORMAT2 {
unsigned short int FM2$V_COUNT2;
unsigned short int FM2$L_LBN2;
};

/*
3.5.4  Map Area Description

3.5.4.1  FM2$C_PLACEMENT = Format 0 - two bytes

                |----|----------------------------------|
FM2$V_FORMAT    | 00 |     Placement Control Flags      |  FM2$W_WORD0
                |----|----------------------------------|


          Retrieval pointer format 0 is used to store  placement  data
          in  the  file  header.   It  describes the placement control
          supplied with the  allocation  that  created  the  following
          retrieval  pointer,  allowing  the placement of a file to be
          replicated  when  the  file  is  copied  or  backed  up  and
          restored.   The  following  flag  bits  exist in a placement
          pointer:

          FM2$V_EXACT      Set if exact placement was requested (i.e.,
                           the allocation must be done as specified)

          FM2$V_ONCYL      Set if the allocation was to be located  on
                           one cylinder of the volume

          FM2$V_LBN        Set if the allocation was to be located  at
                           the starting LBN contained in the following
                           retrieval pointer

          FM2$V_RVN        Set if the allocation was to be located  on
                           the  specified  volume (i.e., the volume on
                           which this extent is located)

3.5.4.2  FM2$C_FORMAT1 = Format 1 - four bytes.


                |----|--------------|-------------------|
FM2$V_FORMAT    | 01 |   High LBN   |    Block Count    |  FM2$B_COUNT1
FM2$V_HIGHLBN   |----|--------------|-------------------|
                |             Low Order LBN             |  FM2$W_LOWLBN
                |---------------------------------------|


          Retrieval pointer format 1 provides an 8 bit count field and
          a 22 bit LBN field.  It is therefore capable of representing
          a group of up to 256 blocks on a volume up to  2**22  blocks
          in size.



3.5.4.3  FM2$C_FORMAT2 = Format 2 - six bytes.


                |----|----------------------------------|
FM2$V_FORMAT    | 10 |           Block Count            |  FM2$V_COUNT2
                |----|----------------------------------|
                |                                       |  FM2$L_LBN2
                |--                LBN                --|
                |                                       |
                |---------------------------------------|


          Retrieval pointer format 2 provides a 14 bit count field and
          a  32  bit LBN field.  It is capable of representing a group
          of up to 16384 blocks on a volume  up  to  2**32  blocks  in
          size.



3.5.4.4  FM2$C_FORMAT3 = Format 3 - eight bytes.


                |----|----------------------------------|
FM2$V_FORMAT    | 11 |      High Order Block Count      |  FM2$V_COUNT2
                |----|----------------------------------|
                |         Low Order Block Count         |  FM2$W_LOWCOUNT
                |---------------------------------------|
                |                                       |  FM2$L_LBN3
                |--                LBN                --|
                |                                       |
                |---------------------------------------|
Files-11 On-Disk Structure                                     Page 22


          Retrieval pointer format 3 provides a 30 bit count field and
          a  32 bit LBN field.  It is capable of describing a group of
          up to 2**30 blocks on a volume up to 2**32 blocks in size.





3.5.5  Access Control List (Reserved for future use by DEC).
3.5.6  Reserved Area
3.5.7  End Checksum Description
3.5.7.1  FH2$W_CHECKSUM - 2 Bytes       Block Checksum
  */

