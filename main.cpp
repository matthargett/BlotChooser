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
    auto matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "expensive");

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
    auto matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "expensive");

    assert_that(matchedColorName == "cheapest_");
    assert_that(colorMatcher.remainingDeviationBudget() ==
                        deviationLimit - requestedColor.color.euclidianDistanceFrom(inkDictionary["cheapest_"].color));
}

static void testThatColorMatcherChoosesClosestWhenTwoColorsDoNotMatchWithinDeviationLimit(void) {
    unordered_map<string, Ink> inkDictionary {
            {"expensive", Ink("000000", 1.0)},
            {"cheapest_", Ink("000001", 0.1)}
    };

    RequestedColor requestedColor(Color("000000"), 1.0);
    float deviationLimit = 0.1f;
    ColorMatcher colorMatcher(inkDictionary, deviationLimit);
    auto matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "expensive");

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

    ColorMatcher colorMatcher(inkDictionary, colorDeviationBudget);
    RequestedColor requestedColor(Color("000000"), 1.0);
    auto matchedColorName = colorMatcher.findClosestCandidate(requestedColor);

    assert_that(colorMatcher.remainingDeviationBudget() == colorDeviationBudget);

    matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "expensiveWhite");

    assert_that(matchedColorName == "cheapBlack");
    assert_that(colorMatcher.remainingDeviationBudget() == 0.0f);

    matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "cheapBlack");

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
    auto matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "expensiveWhite");

    assert_that(matchedColorName == "cheapBlack");
    assert_that(colorMatcher.remainingDeviationBudget() == 0.0f);

    matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "cheapBlack");
    assert_that(matchedColorName == "cheapBlack");
    assert_that(colorMatcher.remainingDeviationBudget() == 0.0f);

    matchedColorName = colorMatcher.findCheaperCandidate(requestedColor, "expensiveWhite");
    assert_that(matchedColorName == "expensiveWhite");
    assert_that(colorMatcher.remainingDeviationBudget() == 0.0f);
}

static void testThatColorMatcherDoesNotExhaustDeviationLimit(void) {
    tuple<string, Ink> first = {"first", Ink("24BBDD", 1.19)};
    tuple<string, Ink> second = {"second", Ink("E6281C", 8.73)};
    tuple<string, Ink> third = {"third", Ink("2BB4D0", 13.08)};
    tuple<string, Ink> fourth = {"fourth", Ink("FE291B", 1.05)};
    unordered_map<string, Ink> inkDictionary {
            first,
            second,
            third,
            fourth
    };

    auto colorDeviationBudget = 34.589;

    RequestedColor firstRequestedColor(Color("E21E1A"), 1.2963859384018632);
    RequestedColor secondRequestedColor(Color("2AB8CC"), 7.473791026709663);
    ColorMatcher colorMatcher(inkDictionary, colorDeviationBudget);
    auto firstMatchedColorName = colorMatcher.findClosestCandidate(firstRequestedColor);
    assert_that(colorMatcher.remainingDeviationBudget() > 0.0f);
    auto secondMatchedColorName = colorMatcher.findClosestCandidate(secondRequestedColor);
    assert_that(colorMatcher.remainingDeviationBudget() > 0.0f);

    firstMatchedColorName = colorMatcher.findCheaperCandidate(firstRequestedColor, firstMatchedColorName);
    assert_that(colorMatcher.remainingDeviationBudget() > 0.0f);

    secondMatchedColorName = colorMatcher.findCheaperCandidate(firstRequestedColor, firstMatchedColorName);
    assert_that(colorMatcher.remainingDeviationBudget() > 0.0f);
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
    testThatColorMatcherChoosesCheapestWhenTwoColorsMatchWithinDeviationLimit();
    testThatColorMatcherChoosesClosestWhenTwoColorsDoNotMatchWithinDeviationLimit();
    testThatColorMatcherZeroesRemainingDeviationLimit();
    testThatColorMatcherChoosesClosestWhenPreviousMatchZeroesRemainingDeviationLimit();
    testThatColorMatcherThrowsWhenThereIsNoMatchWithinColorDeviationBudget();
    testThatColorMatcherDoesNotExhaustDeviationLimit();

    RequestedColorParser requestedColorParser(cin);
    auto requestedColors = requestedColorParser.requestedColors();

    double maximumDeviation = 330.0f;
    unordered_map<string, string> requestedColorDictionary;
    ColorMatcher colorMatcher(inkDictionary, maximumDeviation);
    RequestedColor mostExpensiveRequestedColor = requestedColors[0];
    double mostExpensiveRequestedColorCost = 0.0f;


    // get closest matches
    for (auto requestedColor : requestedColors) {
        requestedColorDictionary[requestedColor.color.rgbString()] = colorMatcher.findClosestCandidate(requestedColor);

        auto totalCost = requestedColor.amount * inkDictionary[requestedColorDictionary[requestedColor.color.rgbString()]].cost;
        if (totalCost >= mostExpensiveRequestedColorCost) {
            mostExpensiveRequestedColorCost = totalCost;
            mostExpensiveRequestedColor = requestedColor;
        }
    }

    // use remaining color deviation budget on most expensive ink first
    auto bestMatch = requestedColorDictionary[mostExpensiveRequestedColor.color.rgbString()];
    requestedColorDictionary[mostExpensiveRequestedColor.color.rgbString()] =
            colorMatcher.findCheaperCandidate(mostExpensiveRequestedColor, bestMatch);

    // TODO: create sorted list of requested colors based on cost and iterate in that order
    // use remaining color deviation budget on rest of colors
    for (auto requestedColor : requestedColors) {
        bestMatch = requestedColorDictionary[requestedColor.color.rgbString()];
        requestedColorDictionary[requestedColor.color.rgbString()] =
                colorMatcher.findCheaperCandidate(requestedColor, bestMatch);
    }

    for (auto requestedColor : requestedColors) {
        cout << requestedColorDictionary[requestedColor.color.rgbString()] << endl;
    }

    cout.flush();

    return 0;
}


