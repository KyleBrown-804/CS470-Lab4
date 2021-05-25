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

int getRoomsLeft(int** hotel) {
    printHotelContents(hotel, FLOORS, F_ROOMS);
    int roomsLeft = 0;
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < F_ROOMS; j++) {
            if (hotel[i][j] == 0)
                ++roomsLeft;
        }
    }
    return roomsLeft;
}

// Attempts to reserve a room given the floor and room number or returns false
bool reservedRoom(int f, int r, int** hotel) {

    // Double checks that floor and rooms numbers are valid
    if ( (f > 0 && f <= FLOORS) && (r > 0 && r <= F_ROOMS) ) {
        if (hotel[f-1][r-1] == 0) {
            hotel[f-1][r-1] = 1;
            printHotelContents(hotel, FLOORS, F_ROOMS);
            return true;
        }
    }
    return false;
}

// Continuously handles booking requests and alerts clients if available or not
int handleRequests(int connfd, int cPid, int** hotel) {
    
    while (1) {
        int n = 0;
        char notify[MAX_BUFF];
        int roomsRes[] = {0, 0};

        // Wait for client request to come in
        n = read(connfd, roomsRes, sizeof(roomsRes));
        if (n < 0)
            fprintf(stderr, "[Server] Error: couldn't recieve client room request\n%s\n", strerror(errno));

        // might need to add 
        printf("f: %d   r: %d\n", roomsRes[0], roomsRes[1]);

        // Check if room requested is available
        if (reservedRoom(roomsRes[0], roomsRes[1], hotel)) {
            printf("[Server] Successfully booked Floor %d, Room %d for [Client #%d]\n", roomsRes[0], roomsRes[1], cPid);
            sprintf(notify, "[Server] We have successfully booked your stay at Floor %d, Room %d! We look forward to your visit\n", roomsRes[0], roomsRes[1]);
        }
        else {
            printf("[Server] Could not book Floor %d, Room %d for [Client #%d], it's already taken\n", roomsRes[0], roomsRes[1], cPid);
            sprintf(notify, "[Server] Unfortunately we could not book your request, Floor %d, Room %d is already reserved.\n", roomsRes[0], roomsRes[1]);
        }

        // Returning the status of the reservation to the client
        write(connfd, notify, strlen(notify));

        // Updating the client on the number of rooms available for closing connection purposes
        int rLeft = getRoomsLeft(hotel);
        write(connfd, &rLeft, sizeof(rLeft));
        
        if (rLeft == 0) {
            break;
        }
    }

    return 0;
}

int main(int argc, char** argv) {

    if (argc != 2 && argc != 4) {
        std::cout << USAGE << std::endl;
        return 1;
    }

    if (!isValidArgs(argc, argv))
        return 1;

    // [ ----- Creating hotel with (Floors * Rooms) dimensions ----- ]
    // [Note] extra checks are made in the case of malloc failure
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

    // [ ----- Starting server connection ----- ]
    int sockfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[MAX_BUFF]; // [Note] might need to calculate buffer size needed to send data

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        fprintf(stderr, "[Server] Error: %s\n", strerror(errno));

    /*
    *  Allows resuing the same Address/Port in the event that the server previously crashed
    *  or was terminated early with an interrupt. The setsocketopt() calls below are borrowed from:
    *  https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr/25193462
    */
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
        fprintf(stderr, "[Server] Error: %s\n", strerror(errno));

    #ifdef SO_REUSEPORT
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) < 0) 
        fprintf(stderr, "[Server] Error: %s\n", strerror(errno));
    #endif

    // Server socket set up
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(sendBuff, 0, sizeof(sendBuff)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    // Attempting to start server
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        fprintf(stderr, "[Server] Error: %s\n", strerror(errno));

    if (listen(sockfd, 10) < 0)
        fprintf(stderr, "[Server] Error: %s\n", strerror(errno));

    // [ ----- Server successfully started and listening for connections ----- ]
    std::cout << "\nStarting server on port " << PORT << "...\n" << std::endl; 
    std::cout << "[ ---------- Server Live ---------- ]" << std::endl;

    while (1) {
        // Terminates server loop and exits if the hotel is completely booked
        int currRooms = getRoomsLeft(hotelRooms);
        if (currRooms == 0) {
            std::cout << "[Server] The hotel is completely booked on reservations\n" << 
            "Please check again with us in a few days\n" << std::endl;
            break;
        }

        // Attempt to establish connection from incoming client socket
        connfd = accept(sockfd, (struct sockaddr*)NULL, NULL); 
        if (connfd < 0) {
            fprintf(stderr, "[Server] Error: %s\n", strerror(errno));
            continue;
        }

        // [ ----- Retrieves client PID ----- ]
        pid_t clientPid = -1;
        int gotPid = read(connfd, &clientPid, sizeof(clientPid));
        if (gotPid < 0) {
            fprintf(stderr, "[Server] Error: failed to retrieve pid from client\n%s\n", strerror(errno));
            continue;
        }

        std::cout << "[Server] A connection has been established with [Client #" << clientPid << "]" << std::endl;
        std::cout << "[Server] Current number of rooms availible is " << currRooms << "\n" << std::endl;

        // [ ----- Sending initial hotel size message ----- ]
        sprintf(sendBuff,"\n[Server] Our hotel currently has %d rooms available.\n"
        "We have %d floors and %d rooms per floor, for a total of %d suites\n"
        "What room would you like to reserve?\n", currRooms, FLOORS, F_ROOMS, (FLOORS * F_ROOMS));
        write(connfd, sendBuff, strlen(sendBuff));

        // sending dimensions as two ints [floors, rooms]
        int dimensions[] = {FLOORS, F_ROOMS, currRooms};
        write(connfd, dimensions, sizeof(dimensions));

        // Handles incoming requests from multiple clients
        int exitStatus = handleRequests(connfd, clientPid, hotelRooms);

        // Closes file descriptor associated with client socket connection
        close(connfd);
        sleep(1);
    }

    std::cout << "[ ---------- Server Offline ---------- ]" << std::endl;

    // [ ----- Deallocations ----- ]
    for (int i =0; i < FLOORS; i++) {
        free(hotelRooms[i]);
    }

    free(hotelRooms);
    hotelRooms = nullptr;

    return 0;
}