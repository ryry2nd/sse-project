# SSE Project - SSE is a Small Engine

## Overview
This project is a rendering engine with module support to allow for object scenes, physics engines or whatever your heart desires. Currently the engine is using OpenGL 4.6 but I'm planning to add more backends in the future, like vulkan. My coding schedule is a little bit inconsistent right now because of school and work so be aware of that when you look at the massive gaps in my push history.


## Disclaimer
The engine architecture is in its early stages so the README could be completely wrong in some parts or not yet be implemented.

## Build
I tried my best to make building as easy as possible.

### Linux
Run setup.sh to install docker and all of the required packages inside docker and run build.sh to compile<br>
Check the script flags for more options. Please note that by default the script uses all possible cores on your machine, just use --cores to specify the amount of cores you want if you don't want your machine firing on all cylinders.<br>
TL;DR if you just want it to work just run setup.sh and then build.sh with no flags <br>

### Windows
To compile on Windows you just have to install WSL and use the -os windows flag to run it on windows. Sadly I have yet to implement cross compiling. Don't worry this project will not be feature complete until I get it fully working on windows

## Module Development
### Configuration
All build time configuration will be in cmake format.

### Scripting
To do scripting I suggest you just copy the example module and rewrite it. To compile you have to put in a CMakeLists.txt file in the base of the module. Then all you have to do is put your cpp file in src.<br>
This is an example of how my module system works and how to interact with the engine core.
```
#include <iostream>

extern "C" {
    void setup() {
        std::cout << "Hello from example!\n";
        std::cout << "Path: " << MODULE_PATH << "\n";
    }
    void loop() {
        std::cout << "loop\n";
    }
}
```
