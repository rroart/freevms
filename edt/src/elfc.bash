#!/bin/bash
#
# elfc.bash <image> <object>
#
option=""
outdir="../binaries"
outext="oz"
shares="oz_kernel_486.oz"
if [ "$1" = "oz_kernel_486" ]
then
  option="-base 180000"
  shares=""
fi
if [ "$1" = "oz_loader_486" ]
then
  option="-raw A000"
  outdir="../objects"
  outext="raw"
  shares=""
fi
echo oz_util_elfconv $option $outdir/$1.$outext ../objects/$1.r $shares
ln -s -f ../binaries/oz_kernel_486.oz oz_kernel_486.oz
../linux/oz_util_elfconv $option $outdir/$1.$outext ../objects/$1.r $shares | sort > ../objects/$1.ms
rm -f oz_kernel_486.oz
