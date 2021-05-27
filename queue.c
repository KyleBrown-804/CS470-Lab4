#include "queueList.h" 

// This small queue/linkedlist data structure is borrowed from
// Jacob Sorber's video on thread pools (link below)
// https://www.youtube.com/watch?v=FMNnusHqjpw&t=200s

node_t* head = NULL;
node_t* tail = NULL;

void enqueue(int *client_socket) {
    node_t *newnode = malloc(sizeof(node_t));
    newnode->p_connfd = client_socket;
    newnode->next = NULL;
    if (tail == NULL) {
        head = newnode;
    } else {
        tail->next = newnode;
    }
    tail = newnode;
}

int* dequeue() {
    if (head == NULL) {
        return NULL;
    } else {
        int *result = head->p_connfd;
        node_t *temp = head;
        head = head->next;
        if (head == NULL) {
            tail = NULL;
        }
        free(temp);
        return result;
    }
}