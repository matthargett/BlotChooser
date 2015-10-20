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

    auto bestColor = this->inkDictionary_[bestInk].color;
    this->remainingDeviation_ -= requestedColor.color.euclidianDistanceFrom(bestColor);

    return bestInk;
}

string ColorMatcher::findCheaperCandidate(const RequestedColor& requestedColor, string bestInkName) {
    auto bestInk = this->inkDictionary_[bestInkName];
    auto bestDistance = bestInk.color.euclidianDistanceFrom(requestedColor.color);
    // "undo" previous budget deduction that we may be replacing
    this->remainingDeviation_ += bestDistance;
    auto bestCost = bestInk.cost;
    auto cheapestInkName = bestInkName;


    for(auto ink : this->inkDictionary_) {
        auto distance = ink.second.color.euclidianDistanceFrom(requestedColor.color);
        if (distance <= this->remainingDeviation_) {
            if (ink.second.cost < bestCost) {
                cheapestInkName = ink.first;
                bestCost = ink.second.cost;
            }
        }
    }

    auto cheapestColor = this->inkDictionary_[cheapestInkName].color;
    this->remainingDeviation_ -= requestedColor.color.euclidianDistanceFrom(cheapestColor);


    return cheapestInkName;
}
