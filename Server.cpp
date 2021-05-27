/* 
* Kyle Brown
* 5/27/2021
* CS470 Operating Systems Lab 4
*
* [Compiling]:
*   run the following command 'g++ -o hotel utilities.cpp Server.cpp -pthread'
*   from there the program may be ran such as ./hotel 16000 5 5
*/

#include "headers.h"
#define THREAD_POOL 20 // Change to max number of parallel clients desired

int PORT;
int FLOORS = 10;
int F_ROOMS = 20;
int currRooms;
std::string USAGE = "Usage: <Executable> <Port> [<Floors> <Rooms per Floor>]\n"
                    "Note: arguments in []'s are optional but if included must specify both\n";

int** hotelRooms;

// Thread pool related variables
std::atomic<int> hotelFull;
pthread_mutex_t hotelLock;
pthread_mutex_t queueLock;
pthread_t workerThreads[THREAD_POOL];
pthread_cond_t waitCondition = PTHREAD_COND_INITIALIZER;
std::queue<int> pool;

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

int getRoomsLeft() {
    printHotelContents(hotelRooms, FLOORS, F_ROOMS);
    int roomsLeft = 0;
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < F_ROOMS; j++) {
            if (hotelRooms[i][j] == 0)
                ++roomsLeft;
        }
    }
    return roomsLeft;
}

// Attempts to reserve a room given the floor and room number or returns false
bool reservedRoom(int f, int r) {

    // Double checks that floor and rooms numbers are valid
    if ( (f > 0 && f <= FLOORS) && (r > 0 && r <= F_ROOMS) ) {
        if (hotelRooms[f-1][r-1] == 0) {
            hotelRooms[f-1][r-1] = 1;
            return true;
        }
    }
    return false;
}

// Continuously handles booking requests and alerts clients if available or not
void handleRequests(int connfd) {
    char sendBuff[MAX_BUFF];
    memset(sendBuff, 0, sizeof(sendBuff)); 

    // [ ----- Retrieves client PID ----- ]
    pid_t clientPid = -1;
    int gotPid = read(connfd, &clientPid, sizeof(clientPid));
    if (gotPid == 0) {
        printf("\n[Server] Connection to the client was lost...\n");
        return;
    }
    if (gotPid < 0) {
        fprintf(stderr, "[Server] Error: failed to retrieve pid from client\n%d\n", errno);
        return;
    }

    std::cout << "[Server] A connection has been established with [Client #" << clientPid << "]" << std::endl;
    std::cout << "[Server] Current number of rooms availible is " << currRooms << "\n" << std::endl;

    // [ ----- Sending initial hotel size message ----- ]
    // [ ******************************* ] 
    sprintf(sendBuff,"\n[Server] Our hotel currently has %d rooms available.\n"
    "We have %d floors and %d rooms per floor, for a total of %d suites!\n", currRooms, FLOORS, F_ROOMS, (FLOORS * F_ROOMS));
    write(connfd, sendBuff, strlen(sendBuff));

    // Getting ACK of initial hotel message sending
    int ACK = 0;
    int n = read(connfd, &ACK, sizeof(ACK));
    if (n == 0) {
        printf("\n[Server] Connection to [Client #%d] was lost...\n", clientPid);
        return;
    }
    if (n < 0) {
        fprintf(stderr, "[Server] Error: couldn't recieve the client ACK of welcome message\n%s\n", strerror(errno));
        return;
    }
    if (ACK != 1) {
        printf("[Server] Error: failed to recieve an ACK for the welcome message\n");
        return;
    }
    // [ ******************************* ]

    // Sending dimensions and rooms available [floors, rooms, rooms left]
    int dimensions[] = {FLOORS, F_ROOMS, currRooms};
    write(connfd, dimensions, sizeof(dimensions));

    while (1) {
        int n = 0;
        char notify[MAX_BUFF];
        int roomsRes[] = {0, 0};

        // Wait for client request to come in
        n = read(connfd, roomsRes, sizeof(roomsRes));
        if (n == 0) {
            printf("\n[Server] Connection to [Client #%d] was lost...\n", clientPid);
            return;
        }
        if (n < 0) {
            fprintf(stderr, "[Server] Error: couldn't recieve client room request\n%s\n", strerror(errno));
            return;
        }

        printf("\n[Client #%d] would like to request Floor %d, Room %d\n", clientPid, roomsRes[0], roomsRes[1]);

        // [ ----- Trys to reserve reservation for client room and sends a status message ----- ]
        int success = 0;
        pthread_mutex_lock(&hotelLock);
        if (reservedRoom(roomsRes[0], roomsRes[1])) {
            success = 1;
        }
        pthread_mutex_unlock(&hotelLock);

        if (success == 1) {
            printf("\n[Server] Successfully booked Floor %d, Room %d for [Client #%d]\n", roomsRes[0], roomsRes[1], clientPid);
            sprintf(notify, "\n[Server] We have successfully booked your stay at Floor %d, Room %d! We look forward to your visit\n", roomsRes[0], roomsRes[1]);
        }
        else {
            printf("\n[Server] Could not book Floor %d, Room %d for [Client #%d], it's already taken\n", roomsRes[0], roomsRes[1], clientPid);
            sprintf(notify, "\n[Server] Unfortunately we could not book your request, Floor %d, Room %d is already reserved.\n", roomsRes[0], roomsRes[1]);
        }
        write(connfd, notify, strlen(notify));

        // Getting ACK of the status message sent
        int ACK = 0;
        n = read(connfd, &ACK, sizeof(ACK));
        if (n == 0) {
            printf("\n[Server] Connection to [Client #%d] was lost...\n", clientPid);
            return;
        }
        if (n < 0) {
            fprintf(stderr, "[Server] Error: couldn't recieve the client ACK of the reservation status message\n%s\n", strerror(errno));
            return;
        }
        
        if (ACK != 1) {
            printf("[Server] Error: failed to recieve an ACK for the reservation status message\n");
            return;
        }
        
        // [ ---------------------------------------------------------------------------------- ]

        // Updating the client on the number of rooms available for closing connection purposes
        int rLeft = getRoomsLeft();
        write(connfd, &rLeft, sizeof(rLeft));
        
        if (rLeft == 0) {
            break;
        }
    }

    return;
}

// Worker thread function to pull "work" (client sockets) from a job queue
void * workerThreadFunc(void *arg) {

    while (true) {
        int p_connfd = -1;
        if (hotelFull == 1)
            break;

        // Mutex locking so no two threads dequeue at the same time
        // Conditional wait lets threads sleep until a new job enters the queue
        // to not take up a ton of CPU cycles checking if there is new work
        pthread_mutex_lock(&queueLock);

            // If there is not work in the queue then threads will wait
            if (pool.empty()) {
                pthread_cond_wait(&waitCondition, &queueLock);
                p_connfd = pool.front();
                pool.pop();
            }
            // Otherwise they pull from the queue and start working
            else {
                p_connfd = pool.front();
                pool.pop();
            }
        pthread_mutex_unlock(&queueLock);

        // If a job could be pulled from the queue then the thread will handle it
        if (p_connfd != -1) {
            handleRequests(p_connfd);
            if (close(p_connfd) < 0) {
                fprintf(stderr, "\n[Server] Error: failed to close file descriptor for client socket connection\n%s\n", strerror(errno));
            }
        }
    }

    return NULL;
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
    hotelRooms = (int**) malloc(FLOORS * sizeof(int*));
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
    int sockfd = 0, newfd = 0;
    hotelFull = 0;
    struct sockaddr_in serv_addr;

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

    // Starting the thread pool to await incoming work
    for (int i = 0; i < THREAD_POOL; i++) {
        pthread_create(&workerThreads[i], NULL, workerThreadFunc, NULL);
    }

    while (1) {
        // Terminates server loop and exits if the hotel is completely booked
        currRooms = getRoomsLeft();
        if (currRooms == 0) {
            std::cout << "\n[Server] The hotel is completely booked on reservations\n" << 
            "Please check again with us in a few days\n" << std::endl;
            hotelFull = 1;
            break;
        }

        // Attempt to establish connection from incoming client socket
        newfd = accept(sockfd, (struct sockaddr*)NULL, NULL); 
        if (newfd < 0) {
            fprintf(stderr, "[Server] Error: %s\n", strerror(errno));
            continue;
        }

        // Adds new socket client fd to the work queue for threads to grab
        // Mutex locking so no two threads enqueue at the same time
        // Signal condition notifies threads which are sleeping that there is new work
        pthread_mutex_lock(&queueLock);
        pool.push(newfd);
        pthread_cond_signal(&waitCondition);
        pthread_mutex_unlock(&queueLock);

    }

    // Waits for pthreads to finish
    for (int i = 0; i < THREAD_POOL; i++)
        pthread_join(workerThreads[i], NULL);

    std::cout << "[ ---------- Server Offline ---------- ]" << std::endl;

    // [ ----- Deallocations ----- ]
    for (int i =0; i < FLOORS; i++) {
        free(hotelRooms[i]);
    }

    free(hotelRooms);
    hotelRooms = nullptr;

    return 0;
}