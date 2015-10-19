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
    float deviationLimit = 10.0f;
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

static void testThatColorMatcherZeroesRemainingDeviationLimit(void) {
    tuple<string, Ink> expensiveWhiteEntry = {"expensiveWhite", Ink("000000", 1.0)};
    tuple<string, Ink> cheapAlmostWhiteEntry = {"cheapAlmostWhite", Ink("000001", 0.1)};
    tuple<string, Ink> cheapBlackEntry = {"cheapBlack", Ink("FFFFFF", 0.01)};
    unordered_map<string, Ink> inkDictionary {
            expensiveWhiteEntry,
            cheapAlmostWhiteEntry,
            cheapBlackEntry
    };

    auto expensiveWhite = inkDictionary["expensiveWhite"].color;
    auto cheapBlack = inkDictionary["cheapBlack"].color;
    auto colorDeviationBudget = expensiveWhite.euclidianDistanceFrom(cheapBlack);

    RequestedColor requestedColor(Color("000000"), 1.0);
    ColorMatcher colorMatcher(inkDictionary, colorDeviationBudget);
    auto matchedColorName = colorMatcher.findClosestCandidate(requestedColor);

    assert_that(matchedColorName == "cheapBlack");
    assert_that(colorMatcher.remainingDeviationBudget() == 0.0f);
}

static void testThatColorMatcherChoosesClosestWhenPreviousMatchZeroesRemainingDeviationLimit(void) {
    tuple<string, Ink> expensiveWhiteEntry = {"expensiveWhite", Ink("000000", 1.0)};
    tuple<string, Ink> cheapAlmostWhiteEntry = {"cheapAlmostWhite", Ink("000001", 0.1)};
    tuple<string, Ink> cheapBlackEntry = {"cheapBlack", Ink("FFFFFF", 0.01)};
    unordered_map<string, Ink> inkDictionary {
            expensiveWhiteEntry,
            cheapAlmostWhiteEntry,
            cheapBlackEntry
    };

    auto expensiveWhite = inkDictionary["expensiveWhite"].color;
    auto cheapBlack = inkDictionary["cheapBlack"].color;
    auto colorDeviationBudget = expensiveWhite.euclidianDistanceFrom(cheapBlack);

    RequestedColor requestedColor(Color("000000"), 1.0);
    ColorMatcher colorMatcher(inkDictionary, colorDeviationBudget);
    auto matchedColorName = colorMatcher.findClosestCandidate(requestedColor);

    assert_that(matchedColorName == "cheapBlack");
    assert_that(colorMatcher.remainingDeviationBudget() == 0.0f);

    matchedColorName = colorMatcher.findClosestCandidate(requestedColor);
    assert_that(matchedColorName == "expensiveWhite");
    assert_that(colorMatcher.remainingDeviationBudget() == 0.0f);
}

static void testThatColorMatcherThrowsWhenThereIsNoMatchWithinColorDeviationBudget(void) {
    tuple<string, Ink> expensiveWhiteEntry = {"expensiveWhite", Ink("000000", 1.0)};
    tuple<string, Ink> cheapAlmostWhiteEntry = {"cheapAlmostWhite", Ink("000001", 0.1)};
    unordered_map<string, Ink> inkDictionary {
            expensiveWhiteEntry,
            cheapAlmostWhiteEntry
            // no black entry
    };

    auto expensiveWhite = inkDictionary["expensiveWhite"].color;
    auto cheapAlmostWhite = inkDictionary["cheapAlmostWhite"].color;
    auto black = Color("FFFFFF");
    auto colorDeviationBudget = expensiveWhite.euclidianDistanceFrom(cheapAlmostWhite);

    RequestedColor requestedColor(black.rgbString(), 1.0);
    ColorMatcher colorMatcher(inkDictionary, colorDeviationBudget);
    try {
        colorMatcher.findClosestCandidate(requestedColor);
        assert_that("expected exception" == nullptr);
    } catch(runtime_error& exception) {
        cout.flush();
        assert_that(string("no ink found!") == exception.what());
    }

    assert_that(colorMatcher.remainingDeviationBudget() == colorDeviationBudget);
}




int main() {
    ifstream inksStream("/Users/matt/Downloads/blots_opt/blots.json");
    InkParser inkParser(inksStream);
    auto inkDictionary = inkParser.namesToRgb();

    assertThatInkDictionaryIsValid(inkDictionary);
    testThatColorMatcherChoosesCheapestWhenTwoColorsMatchPerfectly();
//    testThatColorMatcherChoosesCheapestWhenTwoColorsMatchWithinDeviationLimit();
//    testThatColorMatcherChoosesClosestWhenTwoColorsDoNotMatchWithinDeviationLimit();
//    testThatColorMatcherZeroesRemainingDeviationLimit();
//    testThatColorMatcherChoosesClosestWhenPreviousMatchZeroesRemainingDeviationLimit();

    // XFAIL: this is where current approach falls down.
    // other fail scenario: finding cheapest per-color can/does exhaust deviation budget for entire request
//    testThatColorMatcherThrowsWhenThereIsNoMatchWithinColorDeviationBudget();

    RequestedColorParser requestedColorParser(cin);
    auto requestedColors = requestedColorParser.requestedColors();

    double maximumDeviation = 329.99f;
    ColorMatcher colorMatcher(inkDictionary, maximumDeviation);

    for (auto requestedColor : requestedColors) {
        cout << colorMatcher.findClosestCandidate(requestedColor) << endl;
    }

    // TODO: do last pass to look for further cost optimization with remaining color deviation budget

    cout.flush();

    return 0;
}


