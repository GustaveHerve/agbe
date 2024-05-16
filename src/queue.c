#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "queue.h"

struct queue *queue_init(void)
{
    struct queue *q = malloc(sizeof(struct queue));
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

int queue_isempty(struct queue *q)
{
    return q->front == NULL;
}

void queue_push(struct queue *q, struct pixel data)
{
    if (queue_isempty(q))
    {
        q->rear = malloc(sizeof(struct queue_node));
        q->front = q->rear;
        q->front->data = data;
    }
    else
    {
        struct queue_node *new = malloc(sizeof(struct queue_node));
        new->data = data;
        new->next = NULL;
        q->rear->next = new;
        q->rear = new;
    }
    ++q->count;
}

struct pixel queue_pop(struct queue *q)
{
    if (queue_isempty(q))
        errx(1, "queue_pop: popping empty queue");
    struct pixel res = q->front->data;
    struct queue_node *todelete = q->front;
    q->front = q->front->next;
    free(todelete);
    if (q->front == NULL)
        q->rear = NULL;
    --q->count;
    return res;
}

void queue_clear(struct queue *q)
{
    struct queue_node *p = q->front;
    while (p != NULL)
    {
        struct queue_node *temp = p;
        p = p->next;
        free(temp);
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

void queue_free(struct queue *q)
{
    queue_clear(q);
    free(q);
}
