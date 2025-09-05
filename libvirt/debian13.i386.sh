virt-install \
  --name debian386 \
  --memory 1024 \
  --vcpus 1 \
  --disk size=4 \
  --os-variant debiantrixie \
  --location http://ftp.debian.org/debian/dists/trixie/main/installer-i386 \
  --initrd-inject=debian386.preseed.cfg \
  --extra-args="preseed/file=/debian386.preseed.cfg" \
  --unattended \
  profile=jeos,admin-password-file=pwd.txt
