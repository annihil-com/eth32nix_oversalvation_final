#!/bin/sh
export ETH32_DIR=`pwd`
export ETH32_ET="/usr/local/games/enemy-territory"
export LD_PRELOAD="$ETH32_DIR/libeth32v6.so:$HOME/et-sdl-sound.so"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

xkbcomp -I$HOME/.xkb ~/.xkb/keymap/mykbd $DISPLAY > /dev/null 2>&1
xrandr --output DFP9 --gamma 0.5:0.5:0.5

cd /usr/local/games/enemy-territory/
vblank_mode=0 ./et.x86 +set com_hunkmegs 256 +set com_zonemegs 256 +set net_port $(( ((RANDOM<<15)|RANDOM) % 63001 + 2000 )) $*

unset LD_PRELOAD

setxkbmap fr
xrandr --output DFP9 --gamma 1:1:1