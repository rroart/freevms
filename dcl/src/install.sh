cd diskimage

losetup -o 32901120 /dev/loop0 c.img
../init/src/init /dev/loop0 SYSDISK 64260
mount -t ods2 /dev/loop0 /mnt

losetup -o 512 /dev/loop1 c.img
mount -t ext2 /dev/loop1 /mnt2

mkdir /mnt/vms\$common.dir
mkdir /mnt/vms\$common.dir/sysexe.dir
install ../linux/arch/i386/boot/bzImage /mnt/freevms1.elf
install ../rooti386/vms\$common/sysexe/dcl /mnt/vms\$common.dir/sysexe.dir/dcl.
install /mnt2/vms\$common/sysexe/pagefile.sys /mnt/vms\$common.dir/sysexe.dir/pagefile.sys
install ../rooti386/vms\$common/sysexe/directory.ele /mnt/vms\$common.dir/sysexe.dir/directory.ele
install /mnt2/INET\$CONFIG /mnt/INET\$CONFIG.
#[ -f ../rooti386/vms\$common/sysexe/telnet.ele ] && install ../rooti386/vms\$common/sysexe/telnet.ele /mnt/vms\$common.dir/sysexe.dir/telnet.ele

umount /mnt2
losetup -d /dev/loop1

umount /mnt
losetup -d /dev/loop0
