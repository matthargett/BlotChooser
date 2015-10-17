#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include "InkParser.h"
#include "RequestedColorParser.h"

using namespace std;

#define assert_that(x) if (!(x)) throw std::runtime_error(string(string(#x) + " failed"))

string findClosestCandidate(unordered_map<string, Ink> map, RequestedColor color, double maximumDeviation);
using namespace std;

static void assertThatInkDictionaryIsValid(unordered_map<string, Ink> inkDictionary) {
    assert_that(inkDictionary["HL2810"].color.r == 74);
    assert_that(inkDictionary["HL2810"].cost == 6.93f);
    assert_that(inkDictionary["BH5386"].color.r == 24);
    assert_that(inkDictionary["BH5386"].cost == 10.43f);
}

int main() {
    ifstream inksStream("/Users/matt/Downloads/color_optimization/inks.json");
    InkParser inkParser(inksStream);
    auto inkDictionary = inkParser.namesToRgb();

    assertThatInkDictionaryIsValid(inkDictionary);

    RequestedColorParser requestedColorParser(cin);
    auto requestedColors = requestedColorParser.requestedColors();
    double maximumDeviation = 330.0f / (requestedColors.size() * 2.5f);

    for (auto requestedColor : requestedColors) {
        cout << findClosestCandidate(inkDictionary, requestedColor, maximumDeviation) << endl;
    }

    cout.flush();

    return 0;
}

string findClosestCandidate(unordered_map<string, Ink> inks, RequestedColor requestedColor, double maximumDeviation) {
    double bestDistance = 1000000;
    float bestCost = 1000000.0f;
    string bestInk = "UNKNOWN";

    for(auto ink : inks) {
        double distance = ::sqrt(pow(ink.second.color.r - requestedColor.color.r, 2)  +
                                pow(ink.second.color.g - requestedColor.color.g, 2) +
                                pow(ink.second.color.b - requestedColor.color.b, 2));
        if (distance < bestDistance) {
            bestDistance = distance;
            bestInk = ink.first;
        }
    }

    for(auto ink : inks) {
        double distance = ::sqrt(pow(ink.second.color.r - requestedColor.color.r, 2)  +
                                 pow(ink.second.color.g - requestedColor.color.g, 2) +
                                 pow(ink.second.color.b - requestedColor.color.b, 2));
        if (std::abs(distance - bestDistance) < maximumDeviation) {
            if (ink.second.cost < bestCost) {
                bestInk = ink.first;
                bestCost = ink.second.cost;
            }
        }
    }

    if (bestInk == "UNKNOWN") throw runtime_error("no ink found!");

    return bestInk;
}