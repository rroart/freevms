virt-install \
  --name debian13 \
  --memory 1024 \
  --vcpus 1 \
  --disk size=4 \
  --os-variant debiantrixie \
  --location http://ftp.debian.org/debian/dists/trixie/main/installer-amd64 \
  --initrd-inject=build.sh \
  --initrd-inject=debian13.preseed.cfg \
  --extra-args="preseed/file=/debian13.preseed.cfg" \
  --unattended \
  profile=jeos,admin-password-file=pwd.txt
