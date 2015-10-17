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
double getEuclidianDistanceBetween(Color first, Color second);

using namespace std;

static void assertThatInkDictionaryIsValid(unordered_map<string, Ink> inkDictionary) {
    assert_that(inkDictionary["HL2810"].color.r == 74);
    assert_that(inkDictionary["HL2810"].cost == 6.93f);
    assert_that(inkDictionary["BH5386"].color.r == 24);
    assert_that(inkDictionary["BH5386"].cost == 10.43f);
}

int main() {
    ifstream inksStream("/Users/matt/Downloads/blots_opt/blots.json");
    InkParser inkParser(inksStream);
    auto inkDictionary = inkParser.namesToRgb();

    assertThatInkDictionaryIsValid(inkDictionary);

    RequestedColorParser requestedColorParser(cin);
    auto requestedColors = requestedColorParser.requestedColors();

    // TODO: track remaining budget of allowed color deviation
    const double MAGIC_NUMBER = 2.5f;    // magic number based on best evaluation results
    double maximumDeviation = 330.0f / (requestedColors.size() * MAGIC_NUMBER);

    for (auto requestedColor : requestedColors) {
        cout << findClosestCandidate(inkDictionary, requestedColor, maximumDeviation) << endl;
    }

    // TODO: do last pass to look for further cost optimization with remaining color deviation budget

    cout.flush();

    return 0;
}

string findClosestCandidate(unordered_map<string, Ink> inks, RequestedColor requestedColor, double maximumDeviation) {
    double bestDistance = 1000000;
    float bestCost = 1000000.0f;
    string bestInk = "UNKNOWN";

    // find closest color
    for(auto ink : inks) {
        double distance = getEuclidianDistanceBetween(ink.second.color, requestedColor.color);

        if (distance < bestDistance) {
            bestDistance = distance;
            bestInk = ink.first;
        }
    }

    // find next closest color, that is also cheaper, within our budget for color deviation
    for(auto ink : inks) {
        double distance = getEuclidianDistanceBetween(ink.second.color, requestedColor.color);
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

double getEuclidianDistanceBetween(Color first, Color second) {
    return ::sqrt(pow(second.r - first.r, 2)  +
                  pow(second.g - first.g, 2) +
                  pow(second.b - first.b, 2));
}