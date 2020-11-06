# Invincibot

## Build requirements
1. Download (the password is **iagreetotheeula**) the following map packs:
    * [Ladder 2017 Season 1](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2017Season1.zip)

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
$ ./bin/invincibot
```

## Coding Standard
We follow slightly modified [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

~~See [.travis/lint.sh](.travis/lint.sh) for details.~~

## Knwon Issues
1. system_error: random_device failed to open /dev/urandom: Too many open files
    -  Solution: run `ulimit -SHn 10000` in the terminal
    - Note: This setting could restore once restarting the machine

## License
TODO