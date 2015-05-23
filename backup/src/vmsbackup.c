/*
 *
 *  Title:
 *  Backup
 *
 *  Decription:
 *  Program to read VMS backup tape
 *
 *  Author:
 *  John Douglas CAREY.  (version 2.x, I think)
 *  Sven-Ove Westberg    (version 3.0)
 *      See ChangeLog file for more recent authors.
 *
 *  Net-addess (as of 1986 or so; it is highly unlikely these still work):
 *  john%monu1.oz@seismo.ARPA
 *  luthcad!sow@enea.UUCP
 *
 *
 *  Installation (again, as of 1986):
 *
 *  Computer Centre
 *  Monash University
 *  Wellington Road
 *  Clayton
 *  Victoria    3168
 *  AUSTRALIA
 *
 */

/*
 *  Buffer overflow fix gets -> fgets:
 *  Pointed out by George John and fixed by Bertrand Jo�l
 */

/* Does this system have the magnetic tape ioctls?  The answer is yes for
 most/all unices, and I think it is yes for VMS 7.x with DECC 5.2 (needs
 verification), but it is no for VMS 6.2.  */
#ifndef HAVE_MT_IOCTLS
#define HAVE_MT_IOCTLS 1
#endif

#ifdef HAVE_UNIXIO_H
/* Declarations for read, write, etc.  */
#include <unixio.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#include    <stdio.h>
#include    <ctype.h>
#include    <errno.h>
#include    <stdlib.h>
#include <string.h>

#include    <sys/types.h>
#if HAVE_MT_IOCTLS
#include    <sys/ioctl.h>
#include    <sys/mtio.h>
#endif
#ifdef REMOTE
#include    <local/rmt.h>
#include    <sys/stat.h>
#endif
#include    <sys/file.h>

#ifndef __vax
/* The help claims that mkdir is declared in stdlib.h but it doesn't
 seem to be true.  AXP/VMS 6.2, DECC ?.?.  On the other hand, VAX/VMS 6.2
 seems to declare it in a way which conflicts with this definition.
 This is starting to sound like a bad dream.  */
int mkdir();
#endif

#include "vmsbackup.h"
#include "sysdep.h"
#include "backdef.h"

int match();
char *strlocase();

/* Byte-swapping routines.  Note that these do not depend on the size
 of datatypes such as short, long, etc., nor do they require us to
 detect the endianness of the machine we are running on.  It is
 possible they should be macros for speed, but I'm not sure it is
 worth bothering.  We don't have signed versions although we could
 add them if needed.  They are, of course little-endian as that is
 the byteorder used by all integers in a BACKUP saveset.  */

unsigned long getu32(addr)
unsigned char *addr;
{
    return (((((unsigned long) addr[3] << 8) | addr[2]) << 8) | addr[1]) << 8
           | addr[0];
}

unsigned int getu16(addr)
unsigned char *addr;
{
    return (addr[1] << 8) | addr[0];
}

struct bbh *block_header;

struct brh *record_header;

struct bsa *data_item;

#ifdef  STREAM
char *def_tapefile = "/dev/rts8";
#else
char *def_tapefile = "/dev/rmt8";
#endif

char filename[128];
int filesize;

char recfmt; /* record format */

char recatt; /* record attributes */

FILE *f = NULL;

/* Number of bytes we have read from the current file so far (or something
 like that; see process_vbn).  */
int file_count;

short reclen;
short fix;
short recsize;
int vfcsize;

/* Number of files we have seen.  */
unsigned int nfiles;
/* Number of blocks in those files.  */
unsigned long nblocks;

#ifdef  NEWD
FILE *lf;
#endif

int fd; /* tape file descriptor */

/* Command line stuff.  */

/* The save set that we are listing or extracting.  */
char *tapefile;

/* We're going to say tflag indicates our best effort at the same
 output format as VMS BACKUP/LIST.  Previous versions of this
 program used tflag for a briefer output format; if we want that
 feature we'll have to figure out what option should control it.  */
int tflag;

int cflag, dflag, eflag, sflag, vflag, wflag, xflag;

/* Extract files in binary mode.  FIXME: Haven't tried to document
 (here or in the manpage) exactly what that means.  I think it probably
 means (or should mean) to give us raw bits, the same as you would get
 if you read the file in VMS directly using SYS$QIO or some such.  But
 I haven't carefully looked at whether that is what it currently does.  */
int flag_binary;

/* More full listing (/FULL).  */
int flag_full;

/* Which save set are we reading?  */
int selset;

/* These variables describe the files we will be operating on.  GARGV is
 a vector of GARGC elements, and the elements from GOPTIND to the end
 are the names.  */
char **gargv;
int goptind, gargc;

int setnr;

char label[LABEL_SIZE];

char *block;
/* Default blocksize, as specified in -b option.  */
int blocksize = 32256;

#if HAVE_MT_IOCTLS
struct mtop op;
#endif

int typecmp();

FILE * openfile(fn)
char *fn;
{
    char ufn[256];
    char ans[80];
    char *p, *q, s, *ext;
    int procf;

    procf = 1;
    /* copy fn to ufn and convert to lower case */
    p = fn;
    q = ufn;
    while (*p)
    {
        if (isupper(*p))
            *q = *p - 'A' + 'a';
        else
            *q = *p;
        p++;
        q++;
    }
    *q = '\0';

    /* convert the VMS to UNIX and make the directory path */
    p = ufn;
    q = ++p;
    while (*q)
    {
        if (*q == '.' || *q == ']')
        {
            s = *q;
            *q = '\0';
            if (procf && dflag)
                mkdir(p, 0777);
            *q = '/';
            if (s == ']')
                break;
        }
        *q++;
    }
    *q++;
    if (!dflag)
        p = q;
    /* strip off the version number */
    while (*q && *q != ';')
    {
        if (*q == '.')
            ext = q;
        q++;
    }
    if (cflag)
    {
        *q = ':';
    }
    else
    {
        *q = '\0';
    }
    if (!eflag && procf)
        procf = typecmp(++ext);
    if (procf && wflag)
    {
        printf("extract %s [ny]", filename);
        fflush(stdout);
        fgets(ans, 1, stdin);
        if (*ans != 'y')
            procf = 0;
    }
    if (procf)
        /* open the file for writing */
        return (fopen(p, "w"));
    else
        return (NULL );
}

int typecmp(str)
/* Compare the filename type in str with our list
 of file type to be ignored.  Return 0 if the
 file is to be ignored, return 1 if the
 file is not in our list and should not be ignored. */
register char *str;
{
    static char *type[] =
    {
        "exe", /* vms executable image */
        "lib", /* vms object library */
        "obj", /* rsx object file */
        "odl", /* rsx overlay description file */
        "olb", /* rsx object library */
        "pmd", /* rsx post mortem dump */
        "stb", /* rsx symbol table */
        "sys", /* rsx bootable system image */
        "tsk", /* rsx executable image */
        "dir", /* directory */
        "upd", "tlo", "tlb", /* text library */
        "hlb", /* help library */
        "" /* null string terminates list */
    };
    register int i;

    i = -1;
    while (*type[++i])
        if (strncmp(str, type[i], 3) == 0)
            return (0); /* found a match, file to be ignored */
    return (1); /* no match found */
}

void process_summary(buffer, rsize)
unsigned char *buffer;
size_t rsize;
{
    size_t c;
    size_t dsize;
    char *text;
    int type;

    /* These are the various fields from the summary.  */
    unsigned long id = 0;
    char *saveset = "";
    size_t saveset_size = 0;
    char *command = "";
    size_t command_size = 0;
    char *written_by = "";
    size_t written_by_size = 0;
    unsigned short grp = 0377;
    unsigned short usr = 0377;
    char *os = "<unknown OS>"; /* '\0' terminated.  */
    char *osversion = "unknown";
    size_t osversion_size = 7;
    char *nodename = "";
    size_t nodename_size = 0;
    char *written_on = "";
    size_t written_on_size = 0;
    char *backup_version = "unknown";
    size_t backup_version_size = 7;
    char date[23] = "<unknown date>";
    short date_length = 0;
    unsigned long blksz = 0;
    unsigned int grpsz = 0;
    unsigned int bufcnt = 0;

    if (!tflag)
        return;

    /* check the header word */
    if (buffer[0] != 1 || buffer[1] != 1)
    {
        printf("Cannot print summary; invalid data header\n");
        return;
    }
    c = 2;
    while (c < rsize)
    {
        dsize = getu16(((struct bsa *) &buffer[c])->bsa$w_size);
        type = getu16(((struct bsa *) &buffer[c])->bsa$w_type);
        text = ((struct bsa *) &buffer[c])->bsa$t_text;

        /* Probably should define constants for the cases in this
         switch, but I don't know anything about whether they
         have "official" names we should be using or anything
         like that.  */
        switch (type)
        {
        case 0:
            /* This seems to be used for padding at the end
             of the summary.  */
            goto end_of_summary;
        case 1:
            saveset = text;
            saveset_size = dsize;
            break;
        case 2:
            command = text;
            command_size = dsize;
            break;
        case 4:
            written_by = text;
            written_by_size = dsize;
            break;
        case 5:
            if (dsize == 4)
            {
                usr = getu16(text);
                grp = getu16(text + 2);
            }
            break;
        case 6:
            if (dsize != 8 || !(time_vms_to_asc(&date_length, date, text) & 1))
            {
                strcpy(date, "error converting date");
                date_length = strlen(date);
            }
            break;
        case 7:
            if (dsize == 2)
            {
                unsigned short oscode;

                oscode = getu16(text);
                if (oscode == 0x800)
                {
                    os = "OpenVMS AXP";
                }
                else if (oscode == 0x400)
                {
                    os = "OpenVMS VAX";
                }
            }
            break;
        case 8:
            osversion = text;
            osversion_size = dsize;
            break;
        case 9:
            nodename = text;
            nodename_size = dsize;
            break;
        case 10:
            if (dsize >= 4)
            {
                id = getu32(text);
            }
            break;
        case 11:
            written_on = text;
            written_on_size = dsize;
            break;
        case 12:
            backup_version = text;
            backup_version_size = dsize;
            break;
        case 13:
            if (dsize >= 4)
            {
                blksz = getu32(text);
            }
            break;
        case 14:
            if (dsize >= 2)
            {
                grpsz = getu16(text);
            }
            break;
        case 15:
            if (dsize >= 2)
            {
                bufcnt = getu16(text);
            }
            break;
        default:
            /* I guess we'll silently ignore these, for future
             expansion.  */
            break;
        }
        c += dsize + 4;
    }
end_of_summary:
    printf("Save set:          %.*s\n", saveset_size, saveset);
    printf("Written by:        %.*s\n", written_by_size, written_by);
    printf("UIC:               [%06o,%06o]\n", grp, usr);
    printf("Date:              %.*s\n", date_length, date);
    printf("Command:           %.*s\n", command_size, command);
    printf("Operating system:  %s version %.*s\n", os, osversion_size,
           osversion);
    printf("BACKUP version:    %.*s\n", backup_version_size, backup_version);
    printf("CPU ID register:   %08x\n", id);
    printf("Node name:         %.*s\n", nodename_size, nodename);
    printf("Written on:        %.*s\n", written_on_size, written_on);
    printf("Block size:        %lu\n", blksz);
    if (grpsz != 0)
        printf("Group size:        %u\n", grpsz);
    printf("Buffer count:      %u\n\n", bufcnt);
    /* The extra \n is to provide the blank line between the summary
     and the list of files that follows.  */
}

void process_file(buffer, rsize)
char *buffer;
size_t rsize;
{
    int i, n;
    char *p, *q;
    long nblk;
    short dsize, lnch;
    short dtype;
    char *data;
    char *cfname;
    char *sfilename;

    /* Various other stuff extracted from the saveset.  */
    unsigned short grp = 0377;
    unsigned short usr = 0377;

    /* Number of blocks which should appear in output.  This doesn't
     seem to always be the same as nblk.  */
    size_t blocks;

    int c;

    int procf;

    /* check the header word */
    if (buffer[0] != 1 || buffer[1] != 1)
    {
        printf("Snark: invalid data header\n");
        exit(1);
    }

    c = 2;
    while (c < rsize)
    {
        dsize = getu16(((struct bsa *) &buffer[c])->bsa$w_size);
        dtype = getu16(((struct bsa *) &buffer[c])->bsa$w_type);
        data = ((struct bsa *) &buffer[c])->bsa$t_text;

        /* Probably should define constants for the cases in this
         switch, but I don't know anything about whether they
         have "official" names we should be using or anything
         like that.  */
        switch (dtype)
        {
        case 0x2a:
            /* Copy the text into filename, and '\0'-terminate
             it.  */
            p = data;
            q = filename;
            for (i = 0; i < dsize && q < filename + sizeof(filename) - 1; i++)
                *q++ = *p++;
            *q = '\0';
            break;
        case 0x2b:
            /* In my example, two bytes, 0x1 0x2.  */
            break;
        case 0x2c:
            /* In my example, 6 bytes,
             0x7a 0x2 0x57 0x0 0x1 0x1.  */
            break;
        case 0x2e:
            /* In my example, 4 bytes, 0x00000004.  Maybe
             the allocation.  */
            break;
        case 0x2f:
            if (dsize == 4)
            {
                usr = getu16(data);
                grp = getu16(data + 2);
            }
            break;
        case 0x34:
            recfmt = data[0];
            recatt = data[1];
            recsize = getu16(&data[2]);
            /* bytes 4-7 unaccounted for.  */
            nblk = getu16(&data[10])
                   /* Adding in the following amount is a
                    change that I brought over from
                    vmsbackup 3.1.  The comment there
                    said "subject to confirmation from
                    backup expert here" but I'll put it
                    in until someone complains.  */
                   + (64 * 1024) * getu16(&data[8]);
            lnch = getu16(&data[12]);
            /* byte 14 unaccounted for */
            vfcsize = data[15];
            if (vfcsize == 0)
                vfcsize = 2;
            /* bytes 16-31 unaccounted for */
            break;
        case 0x2d:
            /* In my example, 6 bytes.  hex 2b3c 2000 0000.  */
            break;
        case 0x30:
            /* In my example, 2 bytes.  0x44 0xee.  */
            break;
        case 0x31:
            /* In my example, 2 bytes.  hex 0000.  */
            break;
        case 0x32:
            /* In my example, 1 byte.  hex 00.  */
            break;
        case 0x33:
            /* In my example, 4 bytes.  hex 00 0000 00.  */
            break;
        case 0x4b:
            /* In my example, 2 bytes.  Hex 01 00.  */
            break;
        case 0x50:
            /* In my example, 1 byte.  hex 00.  */
            break;
        case 0x57:
            /* In my example, 1 byte.  hex 00.  */
            break;
        case 0x4f:
            /* In my example, 4 bytes.  05 0000 00.  */
            break;
        case 0x35:
            /* In my example, 2 bytes.  04 00.  */
            break;
        case 0x36:
        case 0x37:
            /* In my example, 8 bytes.  Presumably a date.  */
            break;
        case 0x38:
            /* In my example, 8 bytes.  Presumably expires
             date, since my examples has all zeroes here
             and BACKUP prints "<None specified>" for
             expires.  */
            break;
        case 0x39:
            /* In my example, 8 bytes.  Presumably a date.  */
            break;
        case 0x47:
            /* In my example, 4 bytes.  01 00c6 00.  */
            break;
        case 0x48:
            /* In my example, 2 bytes.  88 aa.  */
            break;
        case 0x4a:
            /* In my example, 2 bytes.  01 00.  */
            break;
            /* then comes 0x0, offset 0x2b7.  */
        }
        c += dsize + 4;
    }

#ifdef  DEBUG
    printf("recfmt = %d\n", recfmt);
    printf("recatt = %d\n", recatt);
    printf("reclen = %d\n", recsize);
    printf("vfcsize = %d\n", vfcsize);
#endif
    /* I believe that "512" here is a fixed constant which should not
     depend on the device, the saveset, or anything like that.  */
    filesize = (nblk - 1) * 512 + lnch;
    blocks = (filesize + 511) / 512;
#ifdef DEBUG
    printf("nbk = %d, lnch = %d\n", nblk, lnch);
    printf("filesize = 0x%x\n", filesize);
#endif

    /* open the file */
    if (f != NULL )
    {
        fclose(f);
        file_count = 0;
        reclen = 0;
    }
    procf = 0;
    if (goptind < gargc)
    {
        if (dflag)
        {
            cfname = filename;
        }
        else
        {
            cfname = strrchr(filename, ']') + 1;
        }
        sfilename = malloc(strlen(cfname) + 5);
        if (sfilename == NULL )
        {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }

        if (cflag)
        {
            for (i = 0; i < strlen(cfname); i++)
            {
                sfilename[i] = cfname[i];
                sfilename[i + 1] = '\0';
            }
        }
        else
        {
            for (i = 0; i < strlen(cfname) && cfname[i] != ';'; i++)
            {
                sfilename[i] = cfname[i];
                sfilename[i + 1] = '\0';
            }
        }
        for (i = goptind; i < gargc; i++)
        {
            procf |= match(strlocase(sfilename), strlocase(gargv[i]));
        }
        free(sfilename);
    }
    else
        procf = 1;
    if (tflag && procf && !flag_full)
        printf("%-52s %8d \n", filename, blocks);
    if (tflag && procf && flag_full)
    {
        printf("%s\n", filename);
        printf("                      Size:       %d\n", blocks);
        printf("                      Owner: [%06o,%06o]\n", grp, usr);

        printf("  Record format:      ");
        switch (recfmt)
        {
        case FAB$C_UDF:
            printf("(undefined)");
            break;
        case FAB$C_FIX:
            printf("(fixed)");
            break;
        case FAB$C_VAR:
            printf("Variable length");
#if 0
            /* This might be bytes 16 and 17 of the 0x34.
             But I'm not completely sure where it is stored.  */
            if (?)
                printf (", maximum %u bytes", ?);
#endif
            break;
        case FAB$C_VFC:
            printf("(VFC)");
            break;
        case FAB$C_STM:
            printf("Stream");
            break;
        case FAB$C_STMLF:
            printf("Stream_LF");
            break;
        case FAB$C_STMCR:
            printf("Stream_CR");
            break;
        default:
            printf("<unknown>");
            break;
        }
        printf("\n");

        printf("  Record attributes:  ");
        if (recatt & (1 << FAB$V_FTN))
            printf("Fortran ");
        if (recatt & (1 << FAB$V_CR))
            printf("Carriage return ");
        if (recatt & (1 << FAB$V_PRN))
            printf("Print file ");
        if (recatt & (1 << FAB$V_BLK))
            printf("Non-spanned");
        printf("\n");
    }

    if (xflag && procf)
    {
        /* open file */
        f = openfile(filename);
        if (f != NULL && vflag)
            printf("extracting %s\n", filename);
    }
    ++nfiles;
    nblocks += blocks;
}

/*
 *
 *  process a virtual block record (file record)
 *
 */
void process_vbn(buffer, rsize)
char *buffer;
unsigned short rsize;
{
    int c, i;
    int j;

    if (f == NULL )
    {
        return;
    }
    i = 0;
    while (file_count + i < filesize && i < rsize)
    {
        switch (recfmt)
        {
        case FAB$C_FIX:
            if (reclen == 0)
            {
                reclen = recsize;
            }
            fputc(buffer[i], f);
            i++;
            reclen--;
            break;

        case FAB$C_VAR:
        case FAB$C_VFC:
            if (reclen == 0)
            {
                reclen = getu16(&buffer[i]);
#ifdef  NEWD
                fprintf(lf, "---\n");
                fprintf(lf, "reclen = %d\n", reclen);
                fprintf(lf, "i = %d\n", i);
                fprintf(lf, "rsize = %d\n", rsize);
#endif
                fix = reclen;
                if (flag_binary)
                    for (j = 0; j < 2; j++)
                    {
                        fputc(buffer[i + j], f);
                    }
                i += 2;
                if (recfmt == FAB$C_VFC)
                {
                    if (flag_binary)
                        for (j = 0; j < vfcsize; j++)
                        {
                            fputc(buffer[i + j], f);
                        }
                    i += vfcsize;
                    reclen -= vfcsize;
                }
            }
            else if (reclen == fix && recatt == (1 << FAB$V_FTN))
            {
                /****
                 if (buffer[i] == '0')
                 fputc('\n', f);
                 else if (buffer[i] == '1')
                 fputc('\f', f);
                 *** sow ***/
                fputc(buffer[i], f); /** sow **/
                i++;
                reclen--;
            }
            else
            {
                fputc(buffer[i], f);
                i++;
                reclen--;
            }
            if (reclen == 0)
            {
                if (!flag_binary)
                    fputc('\n', f);
                if (i & 1)
                {
                    if (flag_binary)
                        fputc(buffer[i], f);
                    i++;
                }
            }
            break;

        case FAB$C_STM:
        case FAB$C_STMLF:
            if (reclen < 0)
            {
                printf("SCREAM\n");
            }
            if (reclen == 0)
            {
                reclen = 512;
            }
            c = buffer[i++];
            reclen--;
            if (c == '\n')
            {
                reclen = 0;
            }
            fputc(c, f);
            break;

        case FAB$C_STMCR:
            c = buffer[i++];
            if (c == '\r' && !flag_binary)
                fputc('\n', f);
            else
                fputc(c, f);
            break;

        default:
            fclose(f);
            remove(filename);
            fprintf(stderr, "Invalid record format = %d\n", recfmt);
            return;
        }
    }
    file_count += i;
}

/*
 *
 *  process a backup block
 *
 */
void process_block(block, blocksize)
char *block;
int blocksize;
{

    unsigned short bhsize, rsize, rtype;
    unsigned long bsize, i;

    i = 0;

    /* read the backup block header */
    block_header = (struct bbh *) &block[i];
    i += sizeof(struct bbh);

    bhsize = getu16(block_header->bbh$w_size);
    bsize = getu32(block_header->bbh$l_blocksize);

    /* check the validity of the header block */
    if (bhsize != sizeof(struct bbh))
    {
        fprintf(stderr, "Invalid header block size: expected %d got %d\n",
                sizeof(struct bbh), bhsize);
        exit(1);
    }
    if (bsize != 0 && bsize != blocksize)
    {
        fprintf(stderr, "Snark: Invalid block size\n");
        exit(1);
    }
#ifdef  DEBUG
    printf("new block: i = %d, bsize = %d\n", i, bsize);
#endif

    /* read the records */
    while (i < bsize)
    {
        /* read the backup record header */
        record_header = (struct brh *) &block[i];
        i += sizeof(struct brh);

        rtype = getu16(record_header->brh$w_rtype);
        rsize = getu16(record_header->brh$w_rsize);
#ifdef  DEBUG
        printf("rtype = %d\n", rtype);
        printf("rsize = %d\n", rsize);
        printf("flags = 0x%x\n",
               getu32 (record_header->brh$l_flags));
        printf("addr = 0x%x\n",
               getu32 (record_header->brh$l_address));
        printf("i = %d\n", i);
#endif

        switch (rtype)
        {

        case brh$k_null:
#ifdef  DEBUG
            printf("rtype = null\n");
#endif
            /* This is the type used to pad to the end of
             a block.  */
            break;

        case brh$k_summary:
#ifdef  DEBUG
            printf("rtype = summary\n");
#endif
            process_summary(&block[i], rsize);
            break;

        case brh$k_file:
#ifdef  DEBUG
            printf("rtype = file\n");
#endif
            process_file(&block[i], rsize);
            break;

        case brh$k_vbn:
#ifdef  DEBUG
            printf("rtype = vbn\n");
#endif
            process_vbn(&block[i], rsize);
            break;

        case brh$k_physvol:
#ifdef  DEBUG
            printf("rtype = physvol\n");
#endif
            break;

        case brh$k_lbn:
#ifdef  DEBUG
            printf("rtype = lbn\n");
#endif
            break;

        case brh$k_fid:
#ifdef  DEBUG
            printf("rtype = fid\n");
#endif
            break;

        default:
            /* It is quite possible that we should just skip
             this without even printing a warning.  */
            fprintf(stderr, " Warning: unrecognized record type\n");
            fprintf(stderr, " record type = %d, size = %d\n", rtype, rsize);
        }
#ifdef pyr
        i = i + rsize;
#else
        i += rsize;
#endif
    }
}

int rdhead()
{
    int i, nfound;
    char name[80];
    nfound = 1;
    /* read the tape label - 4 records of 80 bytes */
    while ((i = read(fd, label, LABEL_SIZE)) != 0)
    {
        if (i != LABEL_SIZE)
        {
            fprintf(stderr, "Snark: bad label record\n");
            exit(1);
        }
        if (strncmp(label, "VOL1", 4) == 0)
        {
            sscanf(label + 4, "%14s", name);
            if (vflag || tflag)
                printf("Volume: %s\n", name);
        }
        if (strncmp(label, "HDR1", 4) == 0)
        {
            sscanf(label + 4, "%14s", name);
            sscanf(label + 31, "%4d", &setnr);
        }
        /* get the block size */
        if (strncmp(label, "HDR2", 4) == 0)
        {
            nfound = 0;
            sscanf(label + 5, "%5d", &blocksize);
#ifdef  DEBUG
            printf("blocksize = %d\n", blocksize);
#endif
        }
    }
    if ((vflag || tflag) && !nfound)
        printf("Saveset name: %s   number: %d\n", name, setnr);
    /* get the block buffer */
    block = (char *) malloc(blocksize);
    if (block == (char *) 0)
    {
        fprintf(stderr, "memory allocation for block failed\n");
        exit(1);
    }
    return (nfound);
}

void rdtail()
{
    int i;
    char name[80];
    /* read the tape label - 4 records of 80 bytes */
    while ((i = read(fd, label, LABEL_SIZE)) != 0)
    {
        if (i != LABEL_SIZE)
        {
            fprintf(stderr, "Snark: bad label record\n");
            exit(1);
        }
        if (strncmp(label, "EOF1", 4) == 0)
        {
            sscanf(label + 4, "%14s", name);
            if (vflag || tflag)
                printf("End of saveset: %s\n\n\n", name);
        }
    }
}

/* Perform the actual operation.  The way this works is that main () parses
 the arguments, sets up the global variables like cflags, and calls us.
 Does not return--it always calls exit ().  */
void vmsbackup()
{
    int c, i, eoffl;

    /* Nonzero if we are reading from a saveset on disk (as
     created by the /SAVE_SET qualifier to BACKUP) rather than from
     a tape.  */
    int ondisk;

    if (tapefile == NULL )
        tapefile = def_tapefile;

#ifdef  NEWD
    /* open debug file */
    lf = fopen("log", "w");
    if (lf == NULL)
    {
        perror("log");
        exit(1);
    }
#endif

    /* open the tape file */
    fd = open(tapefile, O_RDONLY);
    if (fd < 0)
    {
        perror(tapefile);
        exit(1);
    }

#if HAVE_MT_IOCTLS
    /* rewind the tape */
    op.mt_op = MTREW;
    op.mt_count = 1;
    i = ioctl(fd, MTIOCTOP, &op);
    if (i < 0)
    {
        if (errno == EINVAL || errno == ENOTTY)
        {
            ondisk = 1;
        }
        else
        {
            perror(tapefile);
            exit(1);
        }
    }
#else
    ondisk = 1;
#endif

    if (ondisk)
    {
        /* process_block wants this to match the size which
         backup writes into the header.  Should it care in
         the ondisk case?  */
        blocksize = 32256;
        block = malloc(blocksize);
        if (block == (char *) 0)
        {
            fprintf(stderr, "memory allocation for block failed\n");
            exit(1);
        }
        eoffl = 0;
    }
    else
    {
        eoffl = rdhead();
    }

    nfiles = 0;
    nblocks = 0;

    /* read the backup tape blocks until end of tape */
    while (!eoffl)
    {
        if (sflag && setnr != selset)
        {
            if (ondisk)
            {
                fprintf(stderr, "-s not supported for disk savesets\n");
                exit(1);
            }
#if HAVE_MT_IOCTLS
            op.mt_op = MTFSF;
            op.mt_count = 1;
            i = ioctl(fd, MTIOCTOP, &op);
            if (i < 0)
            {
                perror(tapefile);
                exit(1);
            }
#else
            abort ();
#endif
            i = 0;
        }
        else
            i = read(fd, block, blocksize);
        if (i == 0)
        {
            if (ondisk)
            {
                /* No need to support multiple save sets.  */
                eoffl = 1;
            }
            else
            {
                if (vflag || tflag)
                    printf("\n\
Total of %u files, %lu blocks\n", nfiles,
                           nblocks);
                rdtail();
                eoffl = rdhead();
            }
        }
        else if (i == -1)
        {
            perror("error reading saveset");
            exit(1);
        }
        else if (i != blocksize)
        {
            fprintf(stderr, "bad block read i = %d\n", i);
            exit(1);
        }
        else
        {
            eoffl = 0;
            process_block(block, blocksize);
        }
    }
    if (vflag || tflag)
    {
        if (ondisk)
        {
            printf("\nTotal of %u files, %lu blocks\n", nfiles, nblocks);
            printf("End of save set\n");
        }
        else
        {
            printf("End of tape\n");
        }
    }

    /* close the tape */
    close(fd);

#ifdef  NEWD
    /* close debug file */
    fclose(lf);
#endif

    /* exit cleanly */
    exit(0);
}
