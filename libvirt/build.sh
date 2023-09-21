DIR=/usr/src
cd ${DIR}
wget archive.debian.org/debian-amd64/pool/main/m/modutils/modutils_2.4.26-1.2_amd64.deb
mkdir ${DIR}/dummy
dpkg-deb -x modutils_2.4.26-1.2_amd64.deb ${DIR}/dummy
cp -p ${DIR}/dummy/sbin/genksyms /sbin
git clone https://github.com/rroart/freevms.git
cd ${DIR}/freevms
./envscript
cd ${DIR}/freevmsx86_64.iomm
#make doxygen
make bzImage
make vmsall
make rootinstall
make image
make image-install
make image-kernel-iomm
