virt-install \
  --name debian12 \
  --memory 1024 \
  --vcpus 1 \
  --disk size=4 \
  --os-variant debianbullseye \
  --location http://ftp.debian.org/debian/dists/bookworm/main/installer-amd64 \
  --initrd-inject=build.sh \
  --initrd-inject=debian12.preseed.cfg \
  --extra-args="preseed/file=/debian12.preseed.cfg" \
  --unattended \
  profile=jeos,admin-password-file=pwd.txt
