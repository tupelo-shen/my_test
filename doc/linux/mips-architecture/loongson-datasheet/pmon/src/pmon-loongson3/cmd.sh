#!/bin/sh
cd zloader.ls2k/
#make cfg all tgt=rom CROSS_COMPILE=/opt/gcc-4.9.3-64-gnu/bin/mipsel-linux- DEBUG=-g
make cfg all tgt=rom CROSS_COMPILE=/opt/gcc-4.4-gnu/bin/mipsel-linux- DEBUG=-g
make dtb CROSS_COMPILE=/opt/gcc-4.4-gnu/bin/mipsel-linux-
cp gzrom.bin ../
cp gzrom-dtb.bin ../
cp gzrom.bin /media/fsy/loongson/ejtag-debug/
cp gzrom-dtb.bin /media/fsy/loongson/ejtag-debug/

