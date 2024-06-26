#ifndef QUEUE_H
#define QUEUE_H

#include "ppu_utils.h"

struct queue_node
{
    struct queue_node *next;
    struct pixel data;
};

struct queue
{
    struct queue_node *front;
    struct queue_node *rear;
    int count;
};

struct queue *queue_init(void);

int queue_isempty(struct queue *q);

void queue_push(struct queue *q, struct pixel data);

struct pixel queue_pop(struct queue *q);

void queue_clear(struct queue *q);

void queue_free(struct queue *q);

#endif
