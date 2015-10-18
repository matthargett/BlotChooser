//
// Created by Matt Hargett on 10/18/15.
//

#include <string>
#include <stdexcept>

#include "ColorMatcher.h"

using namespace std;

string ColorMatcher::findClosestCandidate(const RequestedColor& requestedColor) {
    double bestDistance = 1000000;
    float bestCost = 1000000.0f;
    string bestInk = "UNKNOWN";

    // find closest color
    for(auto ink : this->inkDictionary_) {
        double distance = ink.second.color.euclidianDistanceFrom(requestedColor.color);

        if (distance < bestDistance) {
            bestDistance = distance;
            bestInk = ink.first;
        }
    }

    // find next closest color, that is also cheaper, within our budget for color deviation
    for(auto ink : this->inkDictionary_) {
        double distance = ink.second.color.euclidianDistanceFrom(requestedColor.color);
        if (std::abs(distance - bestDistance) <= this->remainingDeviation_) {
            if (ink.second.cost < bestCost) {
                bestInk = ink.first;
                bestCost = ink.second.cost;
            }
        }
    }

    if (bestInk == "UNKNOWN") throw runtime_error("no ink found!");

    return bestInk;
}
