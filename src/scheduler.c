
#include <stdio.h>
#include <stdlib.h>
#include "../headers/scheduler.h"

#define MAX_QUEUE_SIZE 10
#define MAX_NUM_QUEUES 10


void scheduler_init(Scheduler *s) {
    s->num_queues = 0;
}

int scheduler_add_queue(Scheduler *s) {
    if (s->num_queues >= MAX_NUM_QUEUES) {
        return -1;
    }
    Queue *q = &(s->queues[s->num_queues]);
    q->head = 0;
    q->tail = 0;
    s->num_queues++;
    return s->num_queues - 1;
}

int enqueue(Queue *q, Process *item) {
    if ((q->tail + 1) % MAX_QUEUE_SIZE == q->head) {
        return -1; // queue is full
    }
    item->enqueued = 1;
    q->processes[q->tail] = item;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    return 0;
}

void *dequeue(Queue *q) {
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    void *item = q->processes[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    return item;
}

void *dequeue_by_priority(Queue *q){
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    
    int remove_idx = q->head;
    int j = (q->head+1) % MAX_QUEUE_SIZE;
    while(j!=q->tail){
        if (q->processes[j]->priority > q->processes[remove_idx]->priority){
            remove_idx = j;
        }
        j = (j+1)%MAX_QUEUE_SIZE;
    }
    Process * swap = q->processes[q->head];
    q->processes[q->head] = q->processes[remove_idx];
    q->processes[remove_idx] = swap;
    Process *item = q->processes[q->head];
    q->head = (q->head +1) % MAX_QUEUE_SIZE;
    
    return item;
}

int scheduler_enqueue(Scheduler *s, int queue_idx, Process *item) {
    if (queue_idx < 0 || queue_idx >= s->num_queues) {
        return -1; // invalid queue index
    }
    return enqueue(&(s->queues[queue_idx]), item);
}

void *scheduler_dequeue(Scheduler *s, int dequeue_type) {
    int i;
    for (i = 0; i < s->num_queues; i++) {
        Queue *q = &(s->queues[i]);
        void *item;
        if(dequeue_type == 0){
            item = dequeue_by_priority(q);
        }else {
            item = dequeue(q);   
        }
        if (item != NULL) {
            return item;
        }
    }
    return NULL; // all queues are empty
}
