#include "headers.h"

bool isValidNum(std::string str) {
    for (int i = 0; i < str.length(); i++) {
        if (! isdigit(str[i])) {
            return false;
        }
    }

    try {
        if (std::stoi(str) <= 0)
            return false;
    }
    catch (std::invalid_argument& e) {
        return false;
    }

    return true;
}