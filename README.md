# RAD! 2D

A very simple 2d drawing engine. Focus is on simplicity and portability.

Currently only Windows is supported (until I have time to test on Linux, macOS and emscripten).

# Building: Windows

Building requires CMake v3.15 or greater and at least Microsoft's Build Tools installed.

```
git clone https://github.com/zerotri/rad2d
cd rad2d/
cmake -Bbuild
cmake --build build
```