# SSE Project - SSE is a Small Engine

## Overview
It's a rendering engine with module support to allow for object spaces or physics engines or whatever your heart desires. Currently using OpenGL 4 but I'm planning to add more backends in the future, like vulkan. My coding schedule is a little bit inconsistent right now because of school so be aware of that when you look at the massive gaps in my push history.


## Disclaimer
The engine architecture is in its early stages so the README could be completely wrong in some parts

## Building
I tried my best to make building as easy as possible. All of the libraries get pulled during the cmake configure step.

### Linux
Run setup.sh to install all of the required packages and run build.sh to compile<br>
Check the script flags for more options. Please note that by default the script uses all possible cores on your machine, just use --cores to specify the amount of cores you want if you don't want your machine firing on all cylinders.<br>

### Windows
Windows is a little rough, I suggest you install git bash so you can run the build.sh command but technically you don't have to. You need to have cmake, ccache, clang-20, llvm-20, ninja, python3, python3-venv, nasm, MinGW headers. Keep in mind to use the MinGW version of clang, not the MSVC one.

### MacOS
Install a VM, thats your best bet right now and for a while. Apple is the worst. Opengl 4 isn't allowed on MacOS, they only do metal. There is a plugin for Vulkan so it converts the protocols to metal so I'll look into it when I get to it but I don't have a mac device to test on so I'll have to figure something out. None of this is a promise.

## Loading Custom Programs
### Yaml
A lot of the programming configuration is going to be surrounded by the config.yaml file in the base of your program. A lot of what I have down now for the example might not be even close to what is going to be on the final product.

### Scripting
To do scripting I suggest you just copy the example module and rewrite it. To compile you have to put in a CMakeLists.txt file in the base of the module. Then all you have to do is put your cpp file in src.
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
