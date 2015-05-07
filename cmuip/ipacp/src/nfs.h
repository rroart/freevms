/*
    ****************************************************************

        Copyright (c) 1992, Carnegie Mellon University

        All Rights Reserved

    Permission  is  hereby  granted   to  use,  copy,  modify,  and
    distribute  this software  provided  that the  above  copyright
    notice appears in  all copies and that  any distribution be for
    noncommercial purposes.

    Carnegie Mellon University disclaims all warranties with regard
    to this software.  In no event shall Carnegie Mellon University
    be liable for  any special, indirect,  or consequential damages
    or any damages whatsoever  resulting from loss of use, data, or
    profits  arising  out of  or in  connection  with  the  use  or
    performance of this software.

    ****************************************************************
*/

#define FIDSIZE     (16+6)

#define MAXDATA     8192
#define MAXPATHLEN  1024
#define MAXNAMLEN    255
#define COOKIESIZE     4
#define FHSIZE        32



/*
2.3.1.  stat

   The "stat" type is returned with every procedure's results.  A value
   of NFS_OK indicates that the call completed successfully and the
   results are valid.  The other values indicate some kind of error
   occurred on the server side during the servicing of the procedure.
   The error values are derived from UNIX error numbers.
*/

typedef enum
{
    NFS_OK          =  0,
    NFSERR_PERM     =  1,
    NFSERR_NOENT        =  2,
    NFSERR_IO       =  5,
    NFSERR_NXIO     =  6,
    NFSERR_ACCES        = 13,
    NFSERR_EXIST        = 17,
    NFSERR_NODEV        = 19,
    NFSERR_NOTDIR       = 20,
    NFSERR_ISDIR        = 21,
    NFSERR_FBIG     = 27,
    NFSERR_NOSPC        = 28,
    NFSERR_ROFS     = 30,
    NFSERR_NAMETOOLONG  = 63,
    NFSERR_NOTEMPTY     = 66,
    NFSERR_DQUOT        = 69,
    NFSERR_STALE        = 70,
    NFSERR_WFLUSH       = 99
} nfs_stat;



/*
    2.3.2.  ftype

      The enumeration "ftype" gives the type of a file.  The type NFNON
      indicates a non-file, NFREG is a regular file, NFDIR is a
      directory, NFBLK is a block-special device, NFCHR is a character-
      special device, and NFLNK is a symbolic link.
*/

typedef enum
{
    NFNON = 0,
    NFREG = 1,
    NFDIR = 2,
    NFBLK = 3,
    NFCHR = 4,
    NFLNK = 5
} ftype;




/*
    2.3.3.  fhandle

      The "fhandle" is the file handle passed between the server and the
      client.  All file operations are done using file handles to refer
      to a file or directory.  The file handle can contain whatever
      information the server needs to distinguish an individual file.
*/

typedef unsigned char fhandle[FHSIZE];



/*
2.3.4.  timeval

      The "timeval" structure is the number of seconds and microseconds
      since midnight January 1, 1970, Greenwich Mean Time.  It is used
      to pass time and date information.

*/

typedef struct
{
    unsigned int seconds;
    unsigned int useconds;
} timeval;




/*
    2.3.5.  fattr

      The "fattr" structure contains the attributes of a file; "type" is
      the type of the file; "nlink" is the number of hard links to the
      file (the number of different names for the same file); "uid" is
      the user identification number of the owner of the file; "gid" is
      the group identification number of the group of the file; "size"
      is the size in bytes of the file; "blocksize" is the size in bytes
      of a block of the file; "rdev" is the device number of the file if
      it is type NFCHR or NFBLK; "blocks" is the number of blocks the
      file takes up on disk; "fsid" is the file system identifier for
      the filesystem containing the file; "fileid" is a number that
      uniquely identifies the file within its filesystem; "atime" is the
      time when the file was last accessed for either read or write;
      "mtime" is the time when the file data was last modified
      (written); and "ctime" is the time when the status of the file was
      last changed.  Writing to the file also changes "ctime" if the
      size of the file changes.
*/

typedef struct
{
    enum ftype   type;
    unsigned int mode;
    unsigned int nlink;
    unsigned int uid;
    unsigned int gid;
    unsigned int size;
    unsigned int blocksize;
    unsigned int rdev;
    unsigned int blocks;
    unsigned int fsid;
    unsigned int fileid;
    timeval      atime;
    timeval      mtime;
    timeval      ctime;
} fattr;



/*
    2.3.6.  sattr

      The "sattr" structure contains the file attributes which can be
      set from the client.  The fields are the same as for "fattr"
      above.  A "size" of zero means the file should be truncated.  A
      value of -1 indicates a field that should be ignored.
*/

typedef struct
{
    unsigned int mode;
    unsigned int uid;
    unsigned int gid;
    unsigned int size;
    timeval      atime;
    timeval      mtime;
} sattr;



/*
    2.3.7.  filename

      The type "filename" is used for passing file names or pathname
      components.
*/

typedef struct
{
    unsigned long length;
    unsigned char data[MAXNAMLEN];
} filename;



/*
    2.3.8.  path

    The type "path" is a pathname.  The server considers it as a
    string with no internal structure, but to the client it is the
    name of a node in a filesystem tree.
*/

typedef struct
{
    unsigned long length;
    unsigned char data[MAXPATHLEN];
} path;



/*
    2.3.9.  attrstat

      The "attrstat" structure is a common procedure result.  It
      contains a "status" and, if the call succeeded, it also contains
      the attributes of the file on which the operation was done.
*/

typedef struct
{
    nfs_stat status;
    fattr attributes;
} attrstat;



/*
    2.3.10.  diropargs

      The "diropargs" structure is used in directory operations.  The
      "fhandle" "dir" is the directory in which to find the file "name".
      A directory operation is one in which the directory is affected.
*/

typedef struct
{
    fhandle  dir;
    filename name;
} diropargs;



/*
    2.3.11.  diropres

      The results of a directory operation are returned in a "diropres"
      structure.  If the call succeeded, a new file handle "file" and
      the "attributes" associated with that file are returned along with
      the "status".
*/

typedef struct
{
    nfs_stat status;
    fhandle file;
    fattr   attributes;
} diropres ;



/* This structure is passed to NFSPROC_READDIR (#16) */

typedef struct
{
    fhandle dir;
    unsigned cookie;
    unsigned count;
} readdirargs;



typedef struct
{
    unsigned length;
    unsigned data[];
} nfsdata;
