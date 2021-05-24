#include "headers.h"

int PORT;
char mode; // m for manual & a for automatic
char* IP_ADDR;
std::string USAGE = "Usage: \n<Executable> <IP Address> <Port> <Mode>\n"
                    "Note: Do not include a ':' within the address or port\n"
                    "IP Adress must be Ipv4, for localhost use 127.0.0.1\n"
                    "Valid modes: 'manual' or 'automatic'\n";

bool isValidArgs(char** args) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, args[1], &(sa.sin_addr));

    if (result == 0) {
        std::cout << "\nIP Address entered is invalid\n\n" << USAGE << std::endl;
        return false;
    }

    else if (result < 0) {
        std::cout << "\nIP Address entered is not part of a valid address family\n\n" << USAGE << std::endl;
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

    PORT = std::stoi(std::string(args[2]));
    IP_ADDR = args[1];

    return true;
}

int main(int argc, char** argv) {
    
    if (argc != 4) {
        std::cout << USAGE << std::endl;
        return 1;
    }

    if (! isValidArgs(argv))
        return 1;

    // [ ----- CONNECTING TO SERVER ----- ]
    pid_t clientPid = getpid();
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, 0, sizeof(recvBuff));
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);


    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("[Client #%d] Error: Could not create socket\n", clientPid);
        return 1;
    } 

    if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0) {
        printf("[Client #%d] Error: inet_pton could not establish a valid address family\n", clientPid);
        return 1;
    } 

    // Need to try multiple connection attempts 3 attempts/ 10 sec cap
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "[Client #%d] Error: %s\n", clientPid, strerror(errno));
        return 1;
    } else {
        printf("\n[Client #%d] Connection to server established at %s:%d\n", clientPid, IP_ADDR, PORT);
    }

    // Sending the client PID as a unique identifier to the server
    if (write(sockfd, &clientPid, sizeof(clientPid)) < 0) {
        fprintf(stderr, "[Client #%d] Error: failed to send pid to server\n%s\n", clientPid, strerror(errno));
    }

    while ((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    if (n < 0) {
        printf("\n Read error \n");
    } 

    close(sockfd);
    return 0;
}