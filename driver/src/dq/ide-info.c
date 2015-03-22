#pragma module IDE_INFO "X-1"

/************************************************************************/
/*                                  */
/* Copyright © Digital Equipment Corporation, 1994 All Rights Reserved. */
/* Unpublished rights reserved under the copyright laws of the United   */
/* States.                              */
/*                                  */
/* The software contained on this media is proprietary to and embodies  */
/* the confidential technology of Digital Equipment Corporation.    */
/* Possession, use, duplication or dissemination of the software and    */
/* media is authorized only pursuant to a valid written license from    */
/* Digital Equipment Corporation.                   */
/*                                  */
/* RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the    */
/* U.S. Government is subject to restrictions as set forth in       */
/* Subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19,  */
/* as applicable.                           */
/*                                  */
/************************************************************************/

/************************************************************************/
/*                                  */
/* Abstract:                                */
/*  This program dumps the identify page information from an IDE    */
/*  drive.  This information is obtained through the READRCT driver */
/*  function code.                          */
/*                                  */
/* Author:                              */
/*  Benjamin J. Thomas III / November 1994              */
/*                                  */
/* Revision History:                            */
/*                                  */
/*  X-1     Benjamin J. Thomas III      November, 1994  */
/*      Initial version.                    */
/*                                  */
/************************************************************************/

/************************************************************************/
/*                                  */
/*  $ CC IDE-INFO                           */
/*  $ LINK IDE-INFO                         */
/*                                  */
/************************************************************************/

#include    "../../../freevms/librtl/src/descrip.h"         /* Define string descriptors */
#include    "../../../freevms/starlet/src/iodef.h"          /* I/O function code */
#include    "../../../freevms/starlet/src/lib$routines.h"       /* Define LIB$ routines */
#include    "../../../freevms/starlet/src/ssdef.h"          /* Define SS$ status codes */
#include    "../../../freevms/starlet/src/starlet.h"            /* Get system service definitions */
#include    <stdio.h>           /* Define standard I/O values */
#include    <stdlib.h>          /* Get standard definitions */
#include    <string.h>          /* String definitions */
#include    "../../../freevms/starlet/src/stsdef.h"         /* Status values */

/* Define the Identify Drive information buffer */

typedef unsigned short int WORD;        /* Define a WORD (16 bits) */
typedef unsigned char BYTE;             /* Define a BYTE (8 bits) */

#define MODEL_LENGTH 40

#pragma member_alignment save
#pragma nomember_alignment
typedef struct
{
    WORD    config;         /* 0 - Configuration information */
    WORD    cyls;           /* 1 - Number of cylinders */
    WORD    rsvd2;          /* 2 - Reserved word */
    WORD    heads;          /* 3 - Number of heads */
    WORD    ubytes_track;       /* 4 - Unformatted bytes/track */
    WORD    ubytes_sector;      /* 5 - Unformatted bytes/sector */
    WORD    sectors;        /* 6 - Number of sectors */
    WORD    unique7[3];     /* 7-9 - Vendor unique */
    char    serial_number[20];  /* 10-19 - ASCII serial number */
    WORD    buffer_type;        /* 20 - Buffer type */
    WORD    buffer_size_blocks; /* 21 - Buffer size (in blocks) */
    WORD    ecc_bytes;      /* 22 - Number of ECC bytes/sector */
    char    firmware_revision[8];   /* 23-26- ASCII firmware revision */
    char    model_number[MODEL_LENGTH];/* 27-46 - ASCII drive model */
    BYTE    rw_multiple;        /* 47 - Number of sectors/interrupt */
    BYTE    unique47;           /* 47 - Vendor unique */
    WORD    dblword_io;     /* 48- Doubleword I/O flag */
    WORD    capabilities;       /* 49 - Capabilities */
    WORD    rsvd50;         /* 50 - Reserved */
    WORD    pio_cycle;      /* 51 - Programmed I/O cycle times */
    WORD    dma_cycle;      /* 52 - DMA I/O cycle times */
    WORD    valid54_58;     /* 53 - Valid bit for next 4 fields */
    WORD    curr_cyls;      /* 54 -     1) Current cylinder count */
    WORD    curr_heads;     /* 55 -     2) Current head count */
    WORD    curr_sectors;       /* 56 -     3) Current sector count */
    unsigned int max_sectors;       /* 57-58 -  4) Maximum sector number */
    WORD    multiple_sectors;   /* 59 - Current sectors/interrupt setting */
    unsigned int lba_maxblock;      /* 60-61 - LBA mode maximum block number */
    WORD    single_word_dma;    /* 62 - Single word DMA info */
    WORD    multi_word_dma;     /* 63 - Multi word DMA info */
    WORD    rsvd64[64];     /* 64-127 - Reserved */
    WORD    unique128[32];      /* 128-159 - Vendor unique */
    WORD    rsvd160[96];        /* 160-255 - Reserved */
} ID_PAGE;

ID_PAGE buffer;             /* Data buffer */

#pragma member_alignment restore

/* Define what an IOSB looks like */

typedef struct                  /* Standard I/O status block */
{
    short int   status;             /* Status word */
    short int   byte_cnt;           /* Transferred byte count */
    int     unused;             /* Unused */
} IOSB_T;

void copy(char *out, char *in,int nchar)
{

    /* This routine is used to copy a string with byte swapping     */
    /*                                  */
    /* Input:                               */
    /*  out pointer to destination                  */
    /*  in  pointer to source                   */
    /*  nchar   number of characters to copy                */
    /*                                  */
    /* Output:                              */
    /*  none                                */

    int i;              /* Loop index */

    /* Move all the characters two at a time and swap them */

    for (i=0; i<nchar; i+=2)
    {
        out[i] = in[i+1];
        out[i+1] = in[i];
    }

    out[nchar] = 0;         /* Terminate the string */
}

int main (int argc, char *argv[])
{

    int status;             /* Routine status */
    int chan;               /* Device channel */
    int efn;                /* Event flag */
    char    dev_name[31];           /* Device name string */
    char    string[128];            /* Buffer for ID strings */
    IOSB_T  iosb;               /* I/O status block for QIO call */
    int debug_info[128];        /* Define debug information buffer */
    int i;              /* Loop counter */
    int tmo_time;           /* Timeout time */

    struct dsc$descriptor_d         /* Define a string descriptor */
            dev_dsc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    /* Get device name */

    if (argc == 2)              /* Is there one argument ? */
    {
        ++argv;             /* Move to second argument */
        strcpy(dev_name,*argv);     /* Get the device name */
    }
    else
    {
        printf("\nUsage: ide-info <device-name>\n");
        exit(SS$_INSFARG);      /* Exit */
    }
    /* Assign a channel to the device */

    dev_dsc.dsc$w_length = strlen(dev_name);    /* Set string length */
    dev_dsc.dsc$a_pointer= dev_name;    /* Point to the string */
    status = sys$assign(&dev_dsc,&chan,0,0,0);  /* Assign the channel */
    if (!$VMS_STATUS_SUCCESS(status))   /* Check for success */
    {
        printf("? Failed to assign channel, status is %X\n",status);
        exit(status);           /* Exit with status */
    }

    /* Get an EFN to use */

    status = lib$get_ef(&efn);      /* Acquire an EFN */
    if (!$VMS_STATUS_SUCCESS(status))   /* Check for success */
    {
        printf("? Failed to acquire EFN, status = %X\n",status);
        exit(status);           /* Exit with status */
    }

    /* Ask for the ID page from the drive */

    status = sys$qiow(efn,chan,IO$_READRCT,&iosb,0,0,&buffer,512,0,0,0,0);
    if (!$VMS_STATUS_SUCCESS(status))
    {
        printf("? QIO service call failed, status is %X\n",status);
        exit(status);           /* Exit with failure status */
    }
    if (!$VMS_STATUS_SUCCESS(iosb.status))
    {
        printf("? QIO operation failed, IOSB status is %X\n",iosb.status);
        exit(iosb.status);      /* Exit with status */
    }

    /* Print the information obtained from the page */

    printf("ID Page information for %s:\n\n",dev_name);
    copy(string,buffer.model_number,MODEL_LENGTH);
    printf("Drive Model: \"%s\"\n",string);
    copy(string,buffer.serial_number,20);
    printf("\tS/N: \"%s\"  ",string);
    copy(string,buffer.firmware_revision,8);
    printf("F/W rev: \"%s\"\n",string);
    printf("Config: %x\n",buffer.config);
    printf("Geometry: Cylinders: %d  ",buffer.cyls);
    printf("Heads: %d  ",buffer.heads);
    printf("Sectors: %d\n",buffer.sectors);
    printf("Unformatted: bytes/track: %d  ",buffer.ubytes_track);
    printf("bytes/sector: %d\n",buffer.ubytes_sector);
    printf("Buffer type: %d  ",buffer.buffer_type);
    printf("Buffer size (in blocks): %d\n",buffer.buffer_size_blocks);
    printf("Number of ECC bytes/sector: %d\n",buffer.ecc_bytes);
    printf("Number of sectors/interrupt: %d\n",buffer.rw_multiple);
    printf("Vendor unique: %d\n",buffer.unique47);
    printf("Doubleword I/O flag: %d\n",buffer.dblword_io);
    printf("Capabilities: %x  (LBA - %d, DMA - %d)\n",buffer.capabilities,
           ((buffer.capabilities&0x100)>>8),((buffer.capabilities&0x200)>>9));
    printf("Reserved: %d\n",buffer.rsvd50);
    printf("Cycle times: PIO %d  ",buffer.pio_cycle);
    printf("DMA %d\n",buffer.dma_cycle);
    printf("Valid bit for next 4 fields: %d\n",buffer.valid54_58);
    printf("Current: Cylinders %d  ",buffer.curr_cyls);
    printf("Heads %d  ",buffer.curr_heads);
    printf("Sectors %d  ",buffer.curr_sectors);
    printf("Maximum sector number %d\n",buffer.max_sectors);
    printf("Current sectors/interrupt setting: %d valid: %d\n",buffer.multiple_sectors&0xFF,
           ((buffer.multiple_sectors&0x100)>>8));
    printf("LBA mode maximum block number: %d\n",buffer.lba_maxblock);
    printf("Single word DMA info: %d\n",buffer.single_word_dma);
    printf("Multi word DMA info: %d\n",buffer.multi_word_dma);

    /* Ask for the debug information from the driver */

    printf("\n\nDebug Information:\n\n");
    status = sys$qiow(efn,chan,IO$_RDSTATS,&iosb,0,0,&debug_info,
                      sizeof(debug_info),0,0,0,0);
    if (!$VMS_STATUS_SUCCESS(status))
    {
        printf("? QIO service call failed, status is %X\n",status);
        exit(status);       /* Exit with failure status */
    }
    if (!$VMS_STATUS_SUCCESS(iosb.status))
    {
        if (iosb.status == SS$_NODATA)
        {
            printf("\t%% DEBUG driver is not loaded\n");
            exit(SS$_NORMAL);       /* Just exit at this point */
        }
        else
        {
            printf("? QIO operation failed, IOSB status is %X\n",iosb.status);
            exit(iosb.status);      /* Exit with status */
        }
    }

    /* Print out the debug information */

    printf("Total interrupts: %d",debug_info[0]);
    printf("\tTotal unexpected interrupts: %d\n",debug_info[1]);
    printf("Number of CRAMs - %d\n",debug_info[2]);
    printf("Transfer buffer address - %x\n",debug_info[3]);
    printf("Base SPTE address - %x",debug_info[4]);
    printf("\tBase S0 address - %x\n",debug_info[5]);
    tmo_time = debug_info[6]-2;
    printf("Timeout time: %d seconds\n",tmo_time);
    printf("\n\tSeconds\tCount\n");

    for (i=0; i<=tmo_time; i++)
    {
        printf("\t%d\t%d\n",i,debug_info[7+i]);
    }
    printf("\t>%d\t%d\n",tmo_time,debug_info[7+tmo_time+1]);
}
