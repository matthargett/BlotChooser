#include <cmath>

#include "Color.h"

double Color::euclidianDistanceFrom(const Color &other) {
    return sqrt(std::pow(other.r - this->r, 2) +
                std::pow(other.g - this->g, 2) +
                std::pow(other.b - this->b, 2));
}