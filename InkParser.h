//
// Created by Matt Hargett on 10/11/15.
//

#ifndef WHICH_INKS_INKPARSER_H
#define WHICH_INKS_INKPARSER_H


#include <istream>
#include <unordered_map>
#include <string>
#include "Ink.h"

class InkParser {
private:
    std::istream& inkData_;
    std::unordered_map<std::string, Ink> namesToRgb_;

public:
    InkParser(std::istream& inkData) : inkData_(inkData) {}
    const std::unordered_map<std::string, Ink>& namesToRgb();

    void readNamesFromStream();
};


#endif //WHICH_INKS_INKPARSER_H
