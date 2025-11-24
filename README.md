# Nic Cage Eats Stuff Wii U Port

My first C++ program
A port of my Nic Cage Eats Stuff game to the Wii U. The game is simple, collect chicken and dodge the celery, try to get the highest score. Tip: hold a for invincibility

Original games are available on ahf2139.pythonanywhere.com.

Based on a Nintendo WiiU SDL2 starter template.

# Building

Requires devkitPro with the Nintendo WiiU toolchain, SDL2 for WiiU and libromfs-wiiu.

## Building instructions

I built everything using Debian 13 after some trial and error

* Install [devkitpro](https://devkitpro.org/wiki/Getting_Started#Unix-like_platforms)
* On a terminal install needed libraries:
  `dkp-pacman -S wiiu-dev`
  
* Install [libromfs-wiiu](https://github.com/yawut/libromfs-wiiu)

*  and then
  `dkp-pacman -Syu wiiu-sdl2 wiiu-sdl2_image wiiu-sdl2_mixer wiiu-sdl2_ttf`
* extract this to /opt/devkitpro/portlibs/wiiu/
  `https://github.com/yawut/libromfs-wiiu/releases/download/0.7/libromfs-wiiu-0.7.tar.bz2`

* Clone this repo
* `cd nces-wiiu`
* `make`
