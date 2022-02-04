#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

struct node {
    void* data;
    struct node* nextnode;
};

typedef struct node* node_t;

struct queue {
    /* TODO */
    node_t head;
    node_t tail;
    int count;
};

queue_t queue_create(void)
{
    /* TODO */
    queue_t queue = (queue_t)malloc(sizeof(struct queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    return queue;
}

int queue_destroy(queue_t queue)
{
    /* TODO */
    if (queue == NULL) {   // check whether queue is null
        return -1;
    } else if (queue->head != NULL) {  // check queue is empty
        return -1;
    }
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    /* TODO */
    node_t newnode = (node_t)malloc(sizeof(struct node));
    newnode->data = data;

    if (queue->head == NULL) {
        // queue is empty
        queue->head = newnode;
    }else if (queue->tail == NULL) {
        // tail is empty
        queue->head->nextnode = newnode;
        queue->tail = newnode;
    }else {
        // both head and tail are not empty
        queue->tail->nextnode = newnode;
        queue->tail = newnode;
    }
    queue->count ++;
    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    /* TODO */
    if (queue == NULL) {
        return -1;
    }else if ((queue->head) == NULL) {
        return -1;
    }else if (data == NULL) {
        return -1;
    }
    // normal
    *data = queue->head->data;
    // free the space
    node_t temp;
    temp = queue->head;
    //check whether there is other node inside the queue
    if (queue->head->nextnode != NULL) {
        queue->head = queue->head->nextnode;
    }else {
        queue->head = NULL;
    }
    queue->count --;
    free(temp);
    return 0;
}



int queue_delete(queue_t queue, void *data)
{
    /* TODO */
    if ((queue == NULL) || (data == NULL)){
        return -1;
    } else if (queue->head == NULL) {
        return -1;
    }

    if (queue->head->data == data) {
        // check the first element
        node_t temp;
        temp = queue->head;
        //check whether there is other node inside the queue
        if (queue->head->nextnode != NULL) {
            queue->head = queue->head->nextnode;
        }else {
            queue->head = NULL;
        }
        queue->count --;
        free(temp);
        return 0;
    }


    int counter = 0;
    // in case the function modify the length of the queue
    int original_len = queue->count;

    node_t iterator, temp;
    iterator = queue->head;

    while (counter < original_len - 1) {
        if (iterator->nextnode == NULL) {
            return -1;
        }

        if (iterator->nextnode->data == data) {
            temp = iterator->nextnode;
            //find the data, link the node after the node
            iterator->nextnode = iterator->nextnode->nextnode;
            // use the temp variable to store the next node and free it
            queue->count --;
            free(temp);
            return 0;
        }
        iterator = iterator->nextnode;
        counter++;
    }
    return -1;

}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    /* TODO */
    if ((queue == NULL) || (func == NULL)){
        return -1;
    }

    int counter = 0;
    // in case the function modify the length of the queue
    int original_len = queue->count;

    node_t temp;
    temp = queue->head;
    while (counter < original_len) {
        if (func(queue, temp->data, arg) == 1) {
            *data = temp->data;
            break;
        }
        temp = temp->nextnode;
        counter++;
    }

    return 0;
}

int queue_length(queue_t queue)
{
    /* TODO */
    if (queue == NULL) {
        return -1;
    }
    return queue->count;
}


static int inc_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (int)(long)arg;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += inc;

    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int match = (int)(long)arg;
    (void)q; //unused

    if (*a == match)
        return 1;

    return 0;
}

static int printint(queue_t q, void *data, void *arg)
{
    (void)q;
    (void)arg;

    int *a = (int*) data;
    printf("%d\n", *a);
    return 0;
}