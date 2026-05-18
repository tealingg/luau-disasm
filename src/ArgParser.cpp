#include "ArgParser.hpp"

#include <algorithm>

ArgParser::ArgParser(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        this->arguments.push_back(std::string(argv[i]));
    }
}

bool ArgParser::doesArgExist(const std::string& argument) {
    return std::find(this->arguments.begin(), this->arguments.end(),
                     argument) != this->arguments.end();
}

const std::string& ArgParser::getArgValue(const std::string& argument) {
    std::vector<std::string>::iterator iterator;
    iterator =
        std::find(this->arguments.begin(), this->arguments.end(), argument);
    if (iterator != this->arguments.end() &&
        iterator++ != this->arguments.end()) {
        return *iterator;
    }

    return std::string();
}
