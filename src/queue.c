#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "queue.h"
#include "ppu.h"
#include "ppu.h"

queue *queue_init(void)
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
    if (queue_isempty(q))
        errx(1, "queue_pop: popping empty queue");
    struct pixel res = q->front->data;
    queue_node *todelete = q->front;
    q->front = q->front->next;
    free(todelete);
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
        queue_node *temp = p;
        p = p->next;
        free(temp);
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

void queue_free(queue *q)
{
    queue_clear(q);
    free(q);
}
