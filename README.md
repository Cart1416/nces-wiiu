# Nic Cage Eats Stuff Wii U Port

My first C++ program

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

* Clone this repo
* `cd nces-wiiu`
* `make`
