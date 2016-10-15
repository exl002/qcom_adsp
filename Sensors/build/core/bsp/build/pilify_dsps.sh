#!/bin/bash

./pil-splitter.py "$1" dsps

addr=`readelf -h "$1" | grep "Start of program headers" | awk '{print $5}'`

prog_size=`readelf -h "$1" | grep "Size of program headers" | awk '{print $5}'`

prog_headers=`readelf -h "$1" | grep "Number of program headers" | awk '{print $5}'`

#Set the size to 52 bytes (52 = ascii '4')
echo -n 4 | dd conv=notrunc,sync of=dsps.mdt seek=7 bs=4 obs=4 ibs=4 count=1

# copy program headers to the end of the mdt file:
dd conv=notrunc oflag=append if="$1" of=dsps.mdt bs=1 count=$(($prog_headers*$prog_size)) skip=$addr
