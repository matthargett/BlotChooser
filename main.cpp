#include <cmath>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>

#include "InkParser.h"
#include "RequestedColorParser.h"
#include "ColorMatcher.h"

using namespace std;

#define assert_that(x) if (!(x)) throw std::runtime_error(string(string(#x) + " failed"))

using namespace std;

static void assertThatInkDictionaryIsValid(unordered_map<string, Ink> inkDictionary) {
    assert_that(inkDictionary["HL2810"].color.r == 74);
    assert_that(inkDictionary["HL2810"].cost == 6.93f);
    assert_that(inkDictionary["BH5386"].color.r == 24);
    assert_that(inkDictionary["BH5386"].cost == 10.43f);
}

static void testThatColorMatcherChoosesCheapestWhenTwoColorsMatchPerfectly(void) {
    unordered_map<string, Ink> inkDictionary {
            {"expensive", Ink("000000", 1.0)},
            {"cheapest_", Ink("000000", 0.1)}
    };

    RequestedColor requestedColor(Color("000000"), 1.0);
    float deviationLimit = 0.0f;
    ColorMatcher colorMatcher(inkDictionary, deviationLimit);
    auto matchedColorName = colorMatcher.findClosestCandidate(requestedColor);

    assert_that(matchedColorName == "cheapest_");
}

static void testThatColorMatcherChoosesCheapestWhenTwoColorsMatchWithinDeviationLimit(void) {
    unordered_map<string, Ink> inkDictionary {
            {"expensive", Ink("000000", 1.0)},
            {"cheapest_", Ink("000001", 0.1)}
    };

    RequestedColor requestedColor(Color("000000"), 1.0);
    float deviationLimit = 1.0f;
    ColorMatcher colorMatcher(inkDictionary, deviationLimit);
    auto matchedColorName = colorMatcher.findClosestCandidate(requestedColor);

    assert_that(matchedColorName == "cheapest_");
}

static void testThatColorMatcherChoosesClosestWhenTwoColorsDoNotMatchWithinDeviationLimit(void) {
    unordered_map<string, Ink> inkDictionary {
            {"expensive", Ink("000000", 1.0)},
            {"cheapest_", Ink("000001", 0.1)}
    };

    RequestedColor requestedColor(Color("000000"), 1.0);
    float deviationLimit = 0.1f;
    ColorMatcher colorMatcher(inkDictionary, deviationLimit);
    auto matchedColorName = colorMatcher.findClosestCandidate(requestedColor);

    assert_that(matchedColorName == "expensive");
}

int main() {
    ifstream inksStream("/Users/matt/Downloads/blots_opt/blots.json");
    InkParser inkParser(inksStream);
    auto inkDictionary = inkParser.namesToRgb();

    assertThatInkDictionaryIsValid(inkDictionary);
    testThatColorMatcherChoosesCheapestWhenTwoColorsMatchPerfectly();
    testThatColorMatcherChoosesCheapestWhenTwoColorsMatchWithinDeviationLimit();
    testThatColorMatcherChoosesClosestWhenTwoColorsDoNotMatchWithinDeviationLimit();

    RequestedColorParser requestedColorParser(cin);
    auto requestedColors = requestedColorParser.requestedColors();

    // TODO: track remaining budget of allowed color deviation
    const double MAGIC_NUMBER = 2.0f;    // magic number based on best evaluation results
    double maximumDeviation = 329.99f / (requestedColors.size() * MAGIC_NUMBER);
    ColorMatcher colorMatcher(inkDictionary, maximumDeviation);

    for (auto requestedColor : requestedColors) {
        cout << colorMatcher.findClosestCandidate(requestedColor) << endl;
    }

    // TODO: do last pass to look for further cost optimization with remaining color deviation budget

    cout.flush();

    return 0;
}


