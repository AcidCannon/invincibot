# Invincibot
StarCraft2 Bot for CMPUT 350 Couse Project at University of Alberta.

## Build requirements
1. Download (the password is **iagreetotheeula**) the following map packs:
    * [Ladder 2017 Season 1](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2017Season1.zip)
    * If it says `zip can't be downloaded securely`, right click the link and choose open link in a new tab.

2. Put the downloaded maps into the Maps folder **(create it if the folder doesn't exist):**
    * Windows: C:\Program Files\StarCraft II\Maps
    * OS X: /Applications/StarCraft II/Maps.
    * Linux: anywhere.

3. Download and install [CMake](https://cmake.org/download/).

4. ~~A compiler with C++14 support.~~

5. ~~Windows: Download and install Visual Studio ([2019](https://www.visualstudio.com/downloads/) or older).~~

6. ~~Linux: Install 'gcc-c++'.~~

7. ~~Linux: Install the 'make' utility.~~

8. ~~OS X: Install XCode.~~

9. ~~OS X: Install XCode command-line tools.~~


## Build instructions
OS X(cmdline)

```
# Get the project.
$ git clone --recursive git@github.com:qianyyu/invincibot.git && cd invincibot

# Create build directory.
$ mkdir build && cd build

# Generate a Makefile.
# Use 'cmake -DCMAKE_BUILD_TYPE=Debug ../' if debuginfo is needed
# Debug build also contains additional debug features and chat commands support.
$ cmake ../

# Build the project.
$ VERBOSE=1 cmake --build . --parallel

# Launch the bot with the specified path to a SC2 map, e.g.
$ ./bin/invincibot -c -a zerg -d Medium -m 0
```

## Playing against the built-in AI

For example,

```
./bin/invincibot -c -a zerg -d Medium -m 0
```

Result
```
ComputerDifficulty  : Medium
ComputerOpponent    : True
ComputerRace        : zerg
GamePort            : 0
LadderServer        :
Map                 : BelShirVestigeLE.SC2Map
OpponentId          :
StartPort           : 0
```

Map Index:

0 - BelShirVestigeLE.SC2Map

1 - CactusValleyLE.SC2Map
 
2 - ProximaStationLE.SC2Map
## Playing against another agent
For example,
```
TODO
```

## Coding Standard
We follow slightly modified [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

~~See [.travis/lint.sh](.travis/lint.sh) for details.~~


## Known Issues
1. system_error: random_device failed to open /dev/urandom: Too many open files
    -  Solution: run `ulimit -SHn 10000` in the terminal
    - Note: This setting could restore once restarting the machine


## License
MIT License

Copyright (c) 2020 Qian Yu, Zijian Xi, Zihao Huang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
