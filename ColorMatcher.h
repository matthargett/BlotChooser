//
// Created by Matt Hargett on 10/18/15.
//

#ifndef WHICH_INKS_COLORMATCHER_H
#define WHICH_INKS_COLORMATCHER_H

#include <string>
#include <unordered_map>

#include "Ink.h"
#include "RequestedColor.h"


class ColorMatcher {
public:
    ColorMatcher(const std::unordered_map<std::string, Ink>& inkDictionary, double totalDeviationAllowed) :
        remainingDeviation_(totalDeviationAllowed), inkDictionary_(inkDictionary) {}

    std::string findClosestCandidate(const RequestedColor& color);
    double remainingDeviationBudget(void);

private:
    double remainingDeviation_;
    std::unordered_map<std::string, Ink> inkDictionary_;
};


#endif //WHICH_INKS_COLORMATCHER_H
