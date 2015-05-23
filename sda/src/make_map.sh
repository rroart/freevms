#!/bin/sh

echo \#include \"system_map.h\" > system_map.c
echo "struct system_map system_map[] = {" >> system_map.c
for X in `cat ../../linux/system.map|sed -e "s/ . /,\"/g"`; do
echo "{" >> system_map.c
#Y=`echo $X|sed -e "s/:.:/, \"/"`
echo 0x$X\" >> system_map.c
echo "}," >> system_map.c
done
echo "{ 0,0 }" >> system_map.c
echo "};" >> system_map.c
