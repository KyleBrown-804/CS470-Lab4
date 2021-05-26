#include "headers.h"

int PORT;
char MODE; // m for manual & a for automatic
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
        MODE = 'm';
    else if (str == "automatic")
        MODE = 'a';
    else {
        std::cout << "\nMode is invalid\n\n" << USAGE << std::endl;
        return false;
    }

    PORT = std::stoi(std::string(args[2]));
    IP_ADDR = args[1];

    return true;
}

bool isValidSpace(int x, int max) {
    if (x > 0 && x <= max)
        return true;
    return false;
}

void runAutomaticMode(int socketfd, int floors, int rooms, int rLeft, int clientPid) {

    // Keeps running until all rooms are reserved
    while (1) {
        

    }
}

void runManualMode(int socketfd, int floors, int rooms, int rLeft, int clientPid) {
    printf("floors: %d  rooms: %d\n", floors, rooms);

    while (rLeft > 0) {
        int f_choice = 0, r_choice = 0, n = 0;
        char respBuffer[MAX_BUFF];
        memset(respBuffer, 0, sizeof(respBuffer));
        std::string f;
        std::string r;

        // Checks for valid floor from user
        printf("\nWhat floor would you like? (Choose between 1-%d)\n", floors);
        std::getline(std::cin, f);
        while (! isValidNum(f)) {
            printf("Invalid entry, try again\n");
            printf("\nWhat floor would you like? (Choose between 1-%d)\n", floors);
            std::getline(std::cin, f);
        }

        f_choice = std::stoi(std::string(f));
        if (! isValidSpace(f_choice, floors)) {
            printf("Invalid floor entered, try again\n");
            continue;
        }

        // Checks for valid room number from user
        printf("\nWhat room number on that floor would you like? (Choose between 1-%d)\n", rooms);
        std::getline(std::cin, r);
        while (! isValidNum(r)) {
            printf("Invalid entry, try again\n");
            printf("\nWhat room number on that floor would you like? (Choose between 1-%d)\n", rooms);
            std::getline(std::cin, r);
        }

        r_choice = std::stoi(std::string(r));
        if (! isValidSpace(r_choice, rooms)) {
            printf("Invalid room number entered, try again\n");
            continue;
        }

        // Sends the desired room to the server to check if it is available for booking
        else {
            printf("f_choice: %d  r_choice: %d\n", f_choice, r_choice);

            int post[] = {f_choice, r_choice};

            // Sending desired room reservation to the server
            if (write(socketfd, post, sizeof(post)) < 0) {
                fprintf(stderr, "[Client #%d] Error: failed to send reservation request to server\n%s\n", clientPid, strerror(errno));
            }

            // Recieve response to see if the room was booked or not
            // while ( (n = read(socketfd, &respBuffer, sizeof(respBuffer)-1)) > 0) {
            //     printf("%s", respBuffer);
            //     if (respBuffer[n-1] == '@') // this almost always works but not always
            //         break;

            //     // change to respBuffer
            //     memset(respBuffer, 0, sizeof(respBuffer));
            // }

            // if (n < 0)
            //     fprintf(stderr, "[Client #%d] Error: %s\n", clientPid, strerror(errno));

            // Awaits server's notification of the number of rooms left to close connections
            int bookingResp[] = {-1, -1};
            n = read(socketfd, bookingResp, sizeof(bookingResp));
            if (n < 0)
                fprintf(stderr, "[Client #%d] Error: %s\n", clientPid, strerror(errno));


            // Checks to makes sure that spaces left was sent successfully
            if (bookingResp[1] > -1) {
                rLeft = bookingResp[1];
                printf("[Client #%d] There are currently %d rooms left available\n", clientPid, rLeft);

                // Successful booking
                if (bookingResp[0] == 1) {
                    printf("Room was booked!\n");
                }
                else if (bookingResp[0] == 0) {
                    printf("Room was not booked...\n");
                }
                else {
                    printf("This shouldn't print\n");
                }
            }
            else {
                printf("[Client #%d] There was an error trying to recieve the spaces left\n", clientPid);
            }

        }
    }

    printf("[Client] There are no more rooms available the connection will now close\n");
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

    // Attempts to connect to the server over a 12 second interval and exits
    // if a connection could not be made after 4 failed attempts
    for (int i = 0; i < 4; i++) {
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            fprintf(stderr, "[Client #%d] Error: %s\n", clientPid, strerror(errno));
            if (i == 3) {
                printf("\n4 Attempts were made to connect and all failed, please check that the server is online\n\n");
                return 1;
            }
            printf("[Client #%d] Retrying connection in 3 seconds...\n", clientPid);
            sleep(3);
        } 
        else {
            printf("\n[Client #%d] Connection to server established at %s:%d\n", clientPid, IP_ADDR, PORT);
            break;
        }
    }

    // Sending the client PID as a unique identifier to the server
    if (write(sockfd, &clientPid, sizeof(clientPid)) < 0) {
        fprintf(stderr, "[Client #%d] Error: failed to send pid to server\n%s\n", clientPid, strerror(errno));
    }

    // Reads in intial hotel message stating dimensions and rooms availible
    // while ((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
    //     printf("%s", recvBuff);
    //     if (recvBuff[n-1] == '@') // this almost always works but not always
    //         break;
        
    //     memset(recvBuff, 0, sizeof(recvBuff));
    // }

    // n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
    // printf("%s\n", recvBuff);

    // if (n < 0) {
    //     printf("\n[Client #%d] Error reading in initial hotel message\n", clientPid);
    //     return 1;
    // }

    printf("Here\n");

    // Reads in dimensions and rooms available for client side error checking
    int dimens[] = {0, 0, 0};
    n = 0;
    n = read(sockfd, dimens, sizeof(dimens));
    if (n < 0) {
        printf("\n[Client #%d] Error reading in dimensions from hotel\n", clientPid);
        return 1;
    }

    // Additional case check to assure that proper dimensions were sent
    if (dimens[0] == 0 || dimens[1] == 0 || dimens[2] == 0) {
        printf("\n[Client #%d] Error reading in dimensions from hotel\n", clientPid);
        return 1;
    }

    printf("floors: %d   rooms: %d   allRooms: %d\n", dimens[0], dimens[1], dimens[2]);

    // [ ----- Manual / Automatic running modes ----- ]
    if (MODE == 'm')
        runManualMode(sockfd, dimens[0], dimens[1], dimens[2], clientPid);
    else
        runAutomaticMode(sockfd, dimens[0], dimens[1], dimens[2], clientPid);

    close(sockfd);
    return 0;
}