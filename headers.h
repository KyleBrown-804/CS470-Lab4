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

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <errno.h>
#include <ctime> 
#include <cmath>

bool isValidNum(std::string str);

#endif