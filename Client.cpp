#include "headers.h"

int main(int argc, char** argv) {
    
    if (argc != 3) {
        std::cout << "Usage: \n<Executable> <IP Address> <Port>\n" << 
        "Note: Do not include a ':' within the address or port\n" << std::endl;
    }

    else {
        std::cout << "IP Address: " << argv[1] << ":" << argv[2] << std::endl;
    }
}