#ifndef QUEUE_H
#define QUEUE_H

#include "ppu_utils.h"

typedef struct queue_node
{
    struct queue_node *next;
    struct pixel data;
} queue_node;

typedef struct queue
{
    struct queue_node *front;
    struct queue_node *rear;
    int count;
} queue;

queue *queue_init(void);
int queue_isempty(queue *q);
void queue_push(queue *q, struct pixel data);
struct pixel queue_pop(queue *q);
void queue_clear(queue *q);
void queue_free(queue *q);

#endif
