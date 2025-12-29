# SSE Project - SSE is a Small Engine

## Overview
Currently I really have no idea what this is going to be. I originally meant for it to be a game engine but I feel like it could be something else. It's not really anything right now because if I call it a game there isn't any gameplay, if I call it a simulation its not really simulating anything.<br>
TL;DR It's a program that allows for the rendering of celestial objects at insane distances. So far I have tested 10^1000000 meters away from the center and the frame rate becomes a big issue. Other than that it works perfectly fine with no bugs I could find regarding distance. I do have a couple of ideas in the future to make it run even better but right now its doing just fine.<br>
Currently using OpenGL 4 but I'm planning to add more backends in the future, like vulkan.
I am also currently using glsl 330 core but I definitely plan to update it to a later version if I want to allow for vulkan. I'm probably gonna stick with glsl because from what I've seen it is the most widely used out of all of the shader languages.
My coding schedule is a little bit inconsistent right now because of school so be aware of that when you look at the massive gaps in my push history


## Building
Sadly your kind of on your own for compilation libraries, what I did was just build from the llvm project and just used clang (took about two hours). I might try to automate it and ship with it but for now you will have to deal with it and compile it yourself.

### Linux
Run build.sh to compile<br>
Check the script flags for more options. Please note that by default the script uses all possible cores on your machine, just use --cores to specify the amount of cores you want if you don't want your machine firing on all cylinders.<br>
You need to install the gcc compiler, cmake, python3, ccache, and ninja for this program to work as well as pulling all of the git submodules listed in libs.<br>
I swear I tried my best to make this process as easy as possible. If you think this is hard to compile, imagine how hard it was to make it.

### Windows
For my testing I used MinGW but it has been a while sense I tested it so you might get a ton of bugs. Windows currently isn't fully supported, so far I'm just trying to get it to work which has shown to be enough of a challenge. Don't worry, it will happen some day. I added a flag in the build.sh that allows you to use the windows toolchain if you want to cross compile or compile on windows directly. just do --os windows to cross compile for windows, keep note this doesn't come with MinGW so you will have to install it manually.

### MacOS
Install a VM, thats your best bet right now and for the foreseeable future. Apple is the worst. Opengl 4 isn't allowed on MacOS, they only do metal. There is a plugin for Vulkan so it converts the protocols to metal so I'll look into it when I get to it but I don't have a mac device to test on so I'll have to figure something out. None of this is a promise.

## Loading Custom Programs
### Yaml
A lot of the programming configuration is going to be surrounded by the config.yaml file in the base of your program. A lot of what I have down now for the example might not be even close to what is going to be on the final product.

### Scripting
Right now I am currently using pure c++ for scripting and the program just compiles the scripts runtime and puts it in the cache folder so you don't need to recompile every time. Put your cpp file in the src folder and it will get picked up automatically. To make it so your program actually does something you need to make an extern startup and loop function. The program always runs in the same working directory as the host so I put in a compile variable of the path of your module. Loop runs every frame just before the drawing and updates but right after the inputs until I make the inputs module only.
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