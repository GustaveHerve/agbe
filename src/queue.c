#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "ppu.h"

queue *queue_init()
{
    queue *q = malloc(sizeof(queue));
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

int queue_isempty(queue *q)
{
    return q->front == NULL;
}

void queue_push(queue *q, struct pixel data)
{
    if (queue_isempty(q))
    {
        q->rear = malloc(sizeof(queue_node));
        q->front = q->rear;
        q->front->data = data;
    }
    else
    {
        queue_node *new = malloc(sizeof(queue_node));
        new->data = data;
        new->next = NULL;
        q->rear->next = new;
        q->rear = new;
    }
    q->count++;
}

struct pixel queue_pop(queue *q)
{
    //TODO empty handling
    //if (queue_isempty(q))
    struct pixel res = q->front->data;
    free(q->front);
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    q->count--;
    return res;
}

void queue_clear(queue *q)
{
    queue_node *p = q->front;
    while (p != NULL)
    {
        free(p);
        p = p->next;
    }
    q->front = NULL;
    q->rear = NULL;
}

void queue_free(queue *q)
{
    queue_clear(q);
    free(q);
}
