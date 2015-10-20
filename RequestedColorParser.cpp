//
// Created by Matt Hargett on 10/11/15.
//

#include <string>
#include <iostream>
#include <fstream>
#include "RequestedColorParser.h"

using namespace std;

const std::vector<RequestedColor>& RequestedColorParser::requestedColors() {
    if (requestedColors_.empty()) {
        readNamesFromStream();
    }

    return requestedColors_;
}

void RequestedColorParser::readNamesFromStream() {
    string requestedColorDataLine;

    ofstream requestedColorLog;
    requestedColorLog.open("requestedColor.log", ios::app);


    while (!requestedColorData_.eof()) {
        getline(requestedColorData_, requestedColorDataLine);
        requestedColorLog << requestedColorDataLine << endl;

        if (requestedColorDataLine == "" || requestedColorDataLine == "\n") break;

        auto firstSpace = requestedColorDataLine.find(' ');
        auto lastSpace = requestedColorDataLine.find_last_of(' ');
        if (firstSpace == string::npos) {
            firstSpace = requestedColorDataLine.find('\t');
            lastSpace = requestedColorDataLine.find_last_of('\t');
            if (firstSpace == string::npos) {
                throw runtime_error(string("didn't find first space/tab: ") + requestedColorDataLine);
            }
        }
        string colorString = requestedColorDataLine.substr(0, firstSpace);
        Color color(colorString);

        float requestedAmount = std::stof(requestedColorDataLine.substr(lastSpace, requestedColorDataLine.length() - lastSpace));

        requestedColors_.push_back(RequestedColor(color, requestedAmount));
    }

    requestedColorLog << endl;
    requestedColorLog.close();

}
