#include <cstdlib>
#include <ios>
#include <iostream>
#include "InkParser.h"

using namespace std;

const std::unordered_map<string, Ink>& InkParser::namesToRgb() {
    if (this->namesToRgb_.size() == 0) {
        readNamesFromStream();
    }

    return this->namesToRgb_;
}

void InkParser::readNamesFromStream() {
    string inkDataLine;
    getline(inkData_, inkDataLine);  // initial JSON open brace

    while(inkData_.good()) {
        getline(inkData_, inkDataLine);

        if (inkDataLine == "}") break;

        auto firstQuote = inkDataLine.find('"') + 1;
        if (firstQuote == string::npos) {
            throw runtime_error(string("didn't find first quote: ") + inkDataLine );
        }
        string inkName = inkDataLine.substr(firstQuote, inkDataLine.find_last_of('"') - firstQuote);

        getline(inkData_, inkDataLine);
        firstQuote = inkDataLine.find(':') + 3;
        if (firstQuote == string::npos) {
            throw runtime_error("didn't find second quote");
        }
        string inkColor = inkDataLine.substr(firstQuote, inkDataLine.find_last_of('"') - firstQuote);

        getline(inkData_, inkDataLine);
        firstQuote = inkDataLine.find(':') + 2;
        if (firstQuote == string::npos) {
            throw runtime_error("didn't find third quote");
        }
        float inkCost = std::stof(inkDataLine.substr(firstQuote, inkDataLine.find_last_of('"') - firstQuote));
        Ink ink(inkColor, inkCost);

        namesToRgb_[inkName] = ink;
        //cout << inkName << ": " << std::to_string(ink.r) << ", " << std::to_string(ink.cost) << std::endl;

        getline(inkData_, inkDataLine);  // skip close brace for ink entry
    }

}
