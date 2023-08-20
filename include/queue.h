#ifndef QUEUE_H
#define QUEUE_H

#include "ppu_utils.h"

struct queue_node
{
    struct queue_node *next;
    struct pixel data;
} typedef queue_node;

struct queue
{
    struct queue_node *front;
    struct queue_node *rear;
    int count;
} typedef queue;

queue *queue_init(void);
int queue_isempty(queue *q);
void queue_push(queue *q, struct pixel data);
struct pixel queue_pop(queue *q);
void queue_clear(queue *q);
void queue_free(queue *q);

#endif
