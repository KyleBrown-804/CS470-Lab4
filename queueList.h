#ifndef QUEUE_LIST_H
#define QUEUE_LIST_H
#include <cstdlib>

struct node {
    struct node* next;
    int *p_connfd;
};

typedef struct node node_t;

#endif