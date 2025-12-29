
#include <iostream>

extern "C" {
    void setup() {
        std::cout << "Hello from example!\n";
        std::cout << "Path: " << MODULE_PATH << "\n";
    }
    // void loop() {
    //     std::cout << "loop\n";
    // }
}