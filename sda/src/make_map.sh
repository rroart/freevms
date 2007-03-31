#!/bin/sh

echo \#include \"System_map.h\" > System_map.c
echo "struct System_map System_map[] = {" >> System_map.c
for X in `cat ../../linux/System.map|sed -e "s/ . /,\"/g"`; do
echo "{" >> System_map.c
#Y=`echo $X|sed -e "s/:.:/, \"/"`
echo 0x$X\" >> System_map.c
echo "}," >> System_map.c
done
echo "{ 0,0 }" >> System_map.c
echo "};" >> System_map.c
