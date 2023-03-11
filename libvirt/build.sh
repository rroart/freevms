cd /tmp
wget archive.debian.org/debian-amd64/pool/main/m/modutils/modutils_2.4.26-1.2_amd64.deb
mkdir /tmp/dummy
dpkg-deb -x modutils_2.4.26-1.2_amd64.deb /tmp/dummy
cp -p /tmp/dummy/sbin/genksyms /sbin
git clone https://github.com/rroart/freevms.git
cd /tmp/freevms
./envscript
cd /tmp/freevmsx86_64.iomm
sed -e "s/extern/static/" -i freevmsx86_64.iomm/debian/libc6-dev_2.3.6.ds1-13etch10_amd64/usr/include/sys/sysmacros.h
#make doxygen
make bzImage
make vmsall
make rootinstall
make image
make image-install
make image-kernel-iomm
