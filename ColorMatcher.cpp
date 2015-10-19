//
// Created by Matt Hargett on 10/18/15.
//

#include <string>
#include <stdexcept>

#include "ColorMatcher.h"

using namespace std;

double ColorMatcher::remainingDeviationBudget() {
    return this->remainingDeviation_;
}

string ColorMatcher::findClosestCandidate(const RequestedColor& requestedColor) {
    double bestDistance = this->remainingDeviation_;
    float bestCost = 1000000.0f;
    string bestInk = "UNKNOWN";

    // find closest color
    for(auto ink : this->inkDictionary_) {
        double distance = ink.second.color.euclidianDistanceFrom(requestedColor.color);

        if (distance <= bestDistance) {
            bestDistance = distance;
            bestInk = ink.first;
        }
    }

    if (bestInk == "UNKNOWN") throw runtime_error("no ink found!");

    // TODO: extract into its own method and do in separate pass in main() after all colors are matched within deviation budget
    // find next closest color, that is also cheaper, within our budget for color deviation
//    for(auto ink : this->inkDictionary_) {
//        double distance = ink.second.color.euclidianDistanceFrom(requestedColor.color);
//        if (std::abs(distance - bestDistance) <= this->remainingDeviation_) {
//            if (ink.second.cost < bestCost) {
//                bestInk = ink.first;
//                bestCost = ink.second.cost;
//            }
//        }
//    }

    auto bestColor = this->inkDictionary_[bestInk].color;
    this->remainingDeviation_ -= requestedColor.color.euclidianDistanceFrom(bestColor);

    return bestInk;
}
