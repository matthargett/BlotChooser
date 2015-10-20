//
// Created by Matt Hargett on 10/11/15.
//

#ifndef WHICH_INKS_INK_H
#define WHICH_INKS_INK_H


#include <string>
#include <cstdlib>
#include "Color.h"

class Ink {
public:
    Ink(const std::string& color_, double cost_): color(color_), cost(cost_) {}
    Ink(const Ink& other) : color(other.color), cost(other.cost) {}
    Ink(): color("FFFFFF") {}
    Color color;
    double cost;
};


#endif //WHICH_INKS_INK_H
