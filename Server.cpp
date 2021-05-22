#include "headers.h"

int PORT;
int FLOORS = 10;
int F_ROOMS = 20;
std::string USAGE = "Usage: <Executable> <Port> [<Floors> <Rooms per Floor>]\n"
                    "Note: arguments in []'s are optional but if included must specify both\n";

// Checks if valid numbers were entered and assigns arguments to global variables
bool isValidArgs(char** args) {

    for (int i = 1; i < 4; i++) {
        std::string str = args[i];
        for (int j = 0; j < str.length(); j++) {
            if (!isdigit(str[j])) {
                std::cout << "\nError: one or more arguments is not a valid number\n" << USAGE << std::endl;
                return false;
            }
        }

        if (std::stoi(str) <= 0) {
            std::cout << "\nError: one or more arguments is not a valid number\n" << USAGE << std::endl;
            return false;
        }
    }

    PORT = std::stoi(args[1]);
    FLOORS = std::stoi(args[2]);
    F_ROOMS = std::stoi(args[3]);
    return true;
}

int main(int argc, char** argv) {

    std::cout << "argc: " << argc << "\n" << std::endl;
    if (argc != 2 && argc != 4) {
        std::cout << USAGE << std::endl;
        return 1;
    }

    // REMAKE A HELPER FUNCTION TO CHECK IF ONE STRING IS A VALID NUMBER
    if (argc == 2) {
        int num;
        std::string str = argv[1];
        for (int i = 0; i < str.length; i++) {

        }
    }

    if (argc == 4) {
        if (!isValidArgs(argv))
            return 1;
    }

    std::cout << "PORT: " << PORT << " FLOORS: " << FLOORS << " F_ROOMS: " << F_ROOMS << "\n" << std::endl;
    return 0;
}