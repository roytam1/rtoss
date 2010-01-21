#!/bin/bash
if [ "$1" == "-c" ]; then
#./configure --target-list="i386-softmmu x86_64-softmmu mips64el-softmmu" --audio-drv-list=sdl --audio-card-list=ac97,sb16,adlib --disable-linux-aio
./configure --target-list="i386-softmmu" --audio-drv-list=sdl --audio-card-list=ac97,sb16,adlib --disable-linux-aio
#cp -f config-host.mak config-host.mak.backup
#sed 's/\-O2 \-g/\-g \-O3 \-msse2 \-msse \-mmmx \-fomit\-frame\-pointer \-ffast\-math \-pipe \-funroll\-loops \-fforce\-addr \-mfpmath\=sse/'  config-host.mak.backup > config-host.mak
fi
make V=1
#if [ "$1" == "-c" -o "$1" == "-r" ]; then
#make "CFLAGS=-O3" "CXXFLAGS=-O3"
#else
#make "CFLAGS=-O3 -msse" "CXXFLAGS=-O3 -msse"
#fi
