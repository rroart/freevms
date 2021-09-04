virt-install \
  --name fv7 \
  --memory 1024 \
  --vcpus 1 \
  --disk size=4 \
  --os-variant debianwheezy \
  --location http://archive.debian.org/debian/dists/wheezy/main/installer-amd64 \
  --initrd-inject=fv.preseed.cfg \
  --extra-args="preseed/file=/fv.preseed.cfg" \
  --unattended \
  profile=jeos,admin-password-file=pwd.txt
