#INSTALL = install this uses setxattr on debian stable and it segfaults
INSTALL=cp

cd diskimage

losetup -o 32901120 /dev/loop0 c.img
../init/src/init /dev/loop0 SYSDISK 64260
mount -t ods2 /dev/loop0 /mnt

losetup -o 512 /dev/loop1 c.img
mount -t ext2 /dev/loop1 /mnt2

mkdir /mnt/lib.dir
mkdir /mnt/vms\$common.dir
mkdir /mnt/vms\$common.dir/sysexe.dir
mkdir /mnt/vms\$common.dir/syslib.dir
$INSTALL ../linux/arch/i386/boot/bzImage /mnt/freevms1.elf
$INSTALL ../rooti386/vms\$common/sysexe/copy.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/authorize /mnt/vms\$common.dir/sysexe.dir/authorize.
$INSTALL ../rooti386/vms\$common/sysexe/create.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/dcl /mnt/vms\$common.dir/sysexe.dir/dcl.
$INSTALL ../rooti386/vms\$common/sysexe/delete.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/diff.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/directory.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/install /mnt/vms\$common.dir/sysexe.dir/install.
$INSTALL ../rooti386/vms\$common/sysexe/loginout /mnt/vms\$common.dir/sysexe.dir/loginout.
$INSTALL /mnt2/vms\$common/sysexe/pagefile.sys /mnt/vms\$common.dir/sysexe.dir/pagefile.sys
$INSTALL ../rooti386/vms\$common/sysexe/search.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/sda /mnt/vms\$common.dir/sysexe.dir/sda.
$INSTALL ../rooti386/vms\$common/sysexe/startup.com /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/sysuaf.dat /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/type.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/sysexe/show /mnt/vms\$common.dir/sysexe.dir/show.
[ -f ../rooti386/vms\$common/sysexe/telnet ] && $INSTALL ../rooti386/vms\$common/sysexe/telnet /mnt/vms\$common.dir/sysexe.dir/telnet.
$INSTALL ../rooti386/vms\$common/sysexe/vmount.exe /mnt/vms\$common.dir/sysexe.dir/
$INSTALL ../rooti386/vms\$common/syslib/starlet.ele /mnt/vms\$common.dir/syslib.dir/
$INSTALL ../rooti386/vms\$common/syslib/librtl.ele /mnt/vms\$common.dir/syslib.dir/
[ -f /mnt2/INET\$CONFIG ] && $INSTALL /mnt2/INET\$CONFIG /mnt/INET\$CONFIG.
$INSTALL ../rooti386/lib/ld-linux.ele /mnt/lib.dir/
$INSTALL ../rooti386/lib/libc.ele /mnt/lib.dir/
$INSTALL ../rooti386/lib/libdl.ele /mnt/lib.dir/

umount /mnt2
losetup -d /dev/loop1

umount /mnt
losetup -d /dev/loop0

