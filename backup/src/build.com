$ CC VMSBACKUP.C/DEFINE=(HAVE_MT_IOCTLS=0,HAVE_UNIXIO_H=1)
$ CC DCLMAIN.C
$! Probably we don't want match as it probably doesn't implement VMS-style
$! matching, but I haven't looking into the issues yet.
$ CC match
$ LINK/exe=VMSBACKUP.EXE vmsbackup.obj,dclmain.obj,match.obj,sys$input/opt
identification="VMSBACKUP4.1"
