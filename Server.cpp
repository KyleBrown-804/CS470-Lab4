#include "headers.h"

int PORT;
int FLOORS = 10;
int F_ROOMS = 20;
std::string USAGE = "Usage: <Executable> <Port> [<Floors> <Rooms per Floor>]\n"
                    "Note: arguments in []'s are optional but if included must specify both\n";

// Checks if valid numbers were entered and assigns arguments to global variables
bool isValidArgs(int argc, char** args) {

    for (int i = 1; i < argc; i++) {
        if (! isValidNum(std::string(args[i]))) {
            std::cout << "\nError: one or more arguments is not a valid number\n" << USAGE << std::endl;
            return false;
        }
    }

    PORT = std::stoi(std::string(args[1]));
    if (argc == 4) {
        FLOORS = std::stoi(args[2]);
        F_ROOMS = std::stoi(args[3]);
    }
    return true;
}

int main(int argc, char** argv) {

    std::cout << "argc: " << argc << "\n" << std::endl;
    if (argc != 2 && argc != 4) {
        std::cout << USAGE << std::endl;
        return 1;
    }

    // REMAKE A HELPER FUNCTION TO CHECK IF ONE STRING IS A VALID NUMBER
    if (!isValidArgs(argc, argv))
        return 1;

    std::cout << "PORT: " << PORT << " FLOORS: " << FLOORS << " F_ROOMS: " << F_ROOMS << "\n" << std::endl;
    
    // Declaring a 2D array of size M x N (Floors x Rooms)
    // extra checks are made in the case of malloc failure
    int** hotelRooms = (int**) malloc(FLOORS * sizeof(int*));
    if (! hotelRooms) {
        std::cout << "An error occured while trying to malloc\n" << "\n" << std::endl;
        return 1;
    }

    for (int i = 0; i < FLOORS; i++) {
        hotelRooms[i] = (int*) malloc(F_ROOMS * sizeof(int));
        if (! hotelRooms[i]) {
            std::cout << "An error occured while trying to malloc\n" << "\n" << std::endl;
            return 1;
        }
    }




    // [ ----- DEALLOCATIONS ----- ]
    for (int i =0; i < FLOORS; i++) {
        free(hotelRooms[i]);
    }

    free(hotelRooms);
    hotelRooms = nullptr;

    return 0;
}