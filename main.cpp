// #include <SFML/Window.hpp>
#include <iostream>

#include "include/Display.hpp"


template <class T>
T dot_product(T x1, T y1, T x2, T y2) {
    return x1*x2 + y1*y2;
}


int main() {
    Display d(LEFT, HOST, 1);
    d.update();
}