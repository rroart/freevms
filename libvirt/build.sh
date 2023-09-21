cd /tmp
wget archive.debian.org/debian-amd64/pool/main/m/modutils/modutils_2.4.26-1.2_amd64.deb
mkdir /tmp/dummy
dpkg-deb -x modutils_2.4.26-1.2_amd64.deb /tmp/dummy
cp -p /tmp/dummy/sbin/genksyms /sbin
git clone https://github.com/rroart/freevms.git
cd /tmp/freevms
./envscript
cd /tmp/freevmsx86_64.iomm
#make doxygen
make bzImage
make vmsall
make rootinstall
make image
make image-install
make image-kernel-iomm
