//
// Created by Matt Hargett on 10/11/15.
//

#ifndef WHICH_INKS_COLOR_H
#define WHICH_INKS_COLOR_H

#include <string>
#include <cstdlib>


class Color {
public:
    Color(const std::string& color) {
        this->r = ::strtoul(color.substr(0,2).c_str(), NULL, 16);
        this->g = ::strtoul(color.substr(2,2).c_str(), NULL, 16);
        this->b = ::strtoul(color.substr(4,2).c_str(), NULL, 16);
    }
    Color() {}
    Color(const Color& other): r(other.r), g(other.g), b(other.b) {}

    double euclidianDistanceFrom(const Color &other);

    int r;
    int g;
    int b;
};


#endif //WHICH_INKS_COLOR_H
