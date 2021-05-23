#include "headers.h"

bool isValidNum(std::string str) {
    for (int i = 0; i < str.length(); i++) {
        if (! isdigit(str[i])) {
            return false;
        }
    }

    if (std::stoi(str) <= 0)
        return false;

    return true;
}