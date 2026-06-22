# SSE Project - SSE is a Small Engine

## Overview
This project is a rendering engine with module support to allow for object scenes, physics engines or whatever your heart desires. Currently the engine is using OpenGL 4.6 but I'm planning to add more backends in the future, like vulkan. My coding schedule is a little bit inconsistent right now because of school so be aware of that when you look at the massive gaps in my push history.


## Disclaimer
The engine architecture is in its early stages so the README could be completely wrong in some parts

## Build
I tried my best to make building as easy as possible. All of the libraries get pulled during the cmake configure step.

### Linux
Run setup.sh to install docker and all of the required packages inside docker and run build.sh to compile<br>
Check the script flags for more options. Please note that by default the script uses all possible cores on your machine, just use --cores to specify the amount of cores you want if you don't want your machine firing on all cylinders.<br>
TL;DR if you just want it to work just run setup.sh and then build.sh with no flags <br>

### Windows
Windows is a little rough, I suggest you just install wsl and use the --os windows flag to cross compile.

### MacOS
MacOS isn't going to be supported in a while. Don't even bother using a VM. VMs don't work with this project unless you are using gpu passthrough because I am using OpenGL 4.6

## Module Development
### Configuration
A lot of the runtime side module configuration is going to be surrounded by the config.yaml file in the base of your program.
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
