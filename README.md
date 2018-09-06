# Not Cube World

This is my attempt to recreate some of the magic I found in the mysteriously-gone [Cube World](https://picroma.com/cubeworld).
There are [lots](https://www.youtube.com/watch?v=rhk5-cHPXwM) of [examples](https://www.youtube.com/watch?v=sRjav_bkZXY) of
[this](https://www.youtube.com/watch?v=JoRSgFxX0HA&list=PL2XncHqN_7yLUBEILKAinFEODMBtaIfmv), but [Cube World](https://www.youtube.com/watch?v=yM2mHIV3TgI)
was a super [fun](https://www.youtube.com/watch?v=N01LOl-Q8EE), [voxel-based](https://www.youtube.com/watch?v=xdOkTI-P6yU),
[randomly-generated](https://www.youtube.com/watch?v=clXvI57npNg), [adventure](https://www.youtube.com/watch?v=-wnTFW0iH0Q)
[RPG](https://www.youtube.com/watch?v=Qxfvrc2VPik).
Unfortunately, it [disappeared](https://www.youtube.com/watch?v=uxPclAOohDA) off the face of the earth and
[confused everyone](https://www.youtube.com/watch?v=APxEiZtMpkA). There has not been
a single update since 2013 and even the [head dev](https://twitter.com/wol_lay) has appeared to go silent in the last year.

But I miss Cube World. I want to play it again. And ever since I heard `wol_lay` wrote it [entirely in C++](https://www.reddit.com/r/CubeWorld/comments/68yexy/what_engineframework_is_cubeworld_made_in/),
I've kind of wanted to do the same. Considering I know it's fun, it's clearly possible to create in homebrew C++,
and all the data is stored in files called `data1.db` ([sqlite specifically](https://www.sqlite.org/index.html), in case you care),
Cube World seemed like the perfect starting point for doing a game engine from start to finish.

## How to develop

1. Make sure you can see [the Trello board](https://trello.com/b/X0QjsQet/not-cube-world).
2. You need [CMake](https://cmake.org/).
3. Use either Mac or Windows

### Windows-Specific

1. Install [Visual Studio 2017](https://visualstudio.microsoft.com/downloads/).
2. Use cmake to build the solution: `cmake build`.

### Mac-Specific

1. Ensure that the CMake executable exists at `/Applications/CMake.app/Contents/bin/cmake`
2. `mkdir build && cd build`
3. `/Applications/CMake.app/Contents/bin/cmake -G "Unix Makefiles" ..`
4. Install [Visual Studio Code](https://code.visualstudio.com/). The `.vscode` folder defines some useful macros for rebuilding cmake and running the game or editor.

## Code Layout

Coming soon(tm)