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

void printHotelContents(int** hotel, int f, int r) {
    
    printf("\n[ --- Hotel Occupancy ---]\n");

    for (int i = 0; i < f; i++) {
        for (int j = 0; j < r; j++) {
            printf("%d ", hotel[i][j]);
        }
        printf("\n");
    }

    printf("[ -----------------------]\n");
}