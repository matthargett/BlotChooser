//
// Created by Matt Hargett on 10/11/15.
//

#ifndef WHICH_INKS_REQUESTEDCOLORPARSER_H
#define WHICH_INKS_REQUESTEDCOLORPARSER_H

#include <istream>
#include <unordered_map>
#include <string>
#include <vector>
#include "RequestedColorParser.h"
#include "Color.h"
#include "RequestedColor.h"

class RequestedColorParser {
private:
    std::istream&requestedColorData_;
    std::vector<RequestedColor> requestedColors_;

public:
    RequestedColorParser(std::istream& requestedColorData) : requestedColorData_(requestedColorData) {}
    const std::vector<RequestedColor>& requestedColors();

    void readNamesFromStream();
};


#endif //WHICH_INKS_REQUESTEDCOLORPARSER_H
