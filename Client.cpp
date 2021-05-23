#include "headers.h"

char mode; // m for manual & a for automatic
std::string USAGE = "Usage: \n<Executable> <IP Address> <Port> <Mode>\n"
                    "Note: Do not include a ':' within the address or port\n"
                    "IP Adress must be Ipv4, for localhost use 127.0.0.1\n"
                    "Valid modes: 'manual' or 'automatic'\n";

bool isValidArgs(char** args) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, args[1], &(sa.sin_addr));

    if (result != 1) {
        std::cout << "\nIP Address entered is invalid\n\n" << USAGE << std::endl;
        return false;
    }

    else if (! isValidNum(args[2]) || std::stoi(std::string(args[2])) <= 0) {
        std::cout << "\nPort is not a valid number\n\n" << USAGE << std::endl;
        return false;
    }

    std::string str = args[3];
    if (str == "manual")
        mode = 'm';
    else if (str == "automatic")
        mode = 'a';
    else {
        std::cout << "\nMode is invalid\n\n" << USAGE << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    
    if (argc != 4) {
        std::cout << "Usage: \n<Executable> <IP Address> <Port> <Mode>\n" << 
        "Note: Do not include a ':' within the address or port\n" << 
        "Valid modes: 'manual' or 'automatic'\n" << std::endl;
        return 1;
    }

    if (! isValidArgs(argv))
        return 1;

    std::cout << "IP Address: " << argv[1] << ":" << argv[2] << " " << argv[3] << std::endl;
}