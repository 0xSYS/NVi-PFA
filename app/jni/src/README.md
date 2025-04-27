# The core source code
This is where most the interesting stuff happens.

#### The code might be very messy at the moment

### Building the core as executable is now possible on linux

# Requirements
To get linux executable you need to make sure you have make and CMake installed.
Required dependencies:
- SDL3
- bass
- bassmidi

if you're on arch you can install both bass and bassmidi from the AUR (bass libbassmidi)
SDL 3 is available to install from pacman (sdl3)

# How to build
```
cd app/jni/src
mkdir build && cd build
cmake -DEXE_TYPE=ON ..
make
```