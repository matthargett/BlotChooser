//
// Created by Matt Hargett on 10/11/15.
//

#ifndef WHICH_INKS_REQEUSTEDCOLOR_H
#define WHICH_INKS_REQEUSTEDCOLOR_H


#include "Color.h"

class RequestedColor {
public:
    RequestedColor(Color color_, float amount_) : color(color_), amount(amount_) {}
    Color color;
    float amount;
};


#endif //WHICH_INKS_REQEUSTEDCOLOR_H
