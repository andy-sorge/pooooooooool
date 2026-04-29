// #include <SFML/Window.hpp>
#include <iostream>
#include <string>

#include "include/Display.hpp"

template <class T>
T dot_product(T x1, T y1, T x2, T y2) {
    return x1*x2 + y1*y2;
}

int main(int argc, char** argv) {
    Role role = HOST;
    TableSegment seg = LEFT;
    unsigned int totalDisplays = 1;
    std::string hostAddress = "127.0.0.1";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--host") {
            role = HOST;
        } else if (arg == "--client") {
            role = CLIENT;
            if (i + 1 < argc) {
                hostAddress = argv[++i];
            }
        } else if (arg == "--segment" && i + 1 < argc) {
            std::string value = argv[++i];
            if (value == "left") seg = LEFT;
            else if (value == "center") seg = CENTER;
            else if (value == "right") seg = RIGHT;
        } else if (arg == "--displays" && i + 1 < argc) {
            totalDisplays = static_cast<unsigned int>(std::stoul(argv[++i]));
        }
    }

    Display d(seg, role, totalDisplays, hostAddress);
    d.update();
}
