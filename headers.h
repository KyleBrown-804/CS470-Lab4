#ifndef HEADERS_H
#define HEADERS_H

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <atomic>

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <stdexcept>
#include <errno.h>
#include <ctime> 
#include <cmath>

#define MAX_BUFF 1024

bool isValidNum(std::string str);
void printHotelContents(int** hotel, int f, int r);

#endif