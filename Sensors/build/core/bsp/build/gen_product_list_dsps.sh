#!/bin/bash

echo -n "" > ../../../../BuildProducts.txt

if [ -f ../../../build/ms/dsps.elf ]; then
  echo "./dsps_proc/build/ms/dsps.elf" >> ../../../../BuildProducts.txt
fi

if [ -f ../../../build/ms/bin/DSPSBLD/dsps.mbn ]; then
  echo "./dsps_proc/build/ms/bin/DSPSBLD/dsps.mbn" >> ../../../../BuildProducts.txt
fi

if [ -f ../../../build/ms/bin/DSPSBLDZ/dsps.mbn ]; then
  echo "./dsps_proc/build/ms/bin/DSPSBLDZ/dsps.mbn" >> ../../../../BuildProducts.txt
fi

