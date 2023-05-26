
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

#define MAX_QUEUE_SIZE 100
#define MAX_NUM_QUEUES 10

typedef struct Process{
 int id;
 int priority;
 int time_left;
 int running;
} Process;

typedef struct {
    Process * processes[MAX_QUEUE_SIZE];
    int head;
    int tail;
} Queue;

typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;

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
        if (q->processes[j]->priority < q->processes[remove_idx]->priority){
            remove_idx = j;
        }
        j = (j+1)%MAX_QUEUE_SIZE;
    }
    Process * swap = q->processes[q->head];
    q->processes[q->head] = q->processes[remove_idx];
    q->processes[remove_idx] = swap;
    void *item = q->processes[q->head];
    q->head = (q->head +1) % MAX_QUEUE_SIZE;
    
    return item;
}

void *dequeue_by_time_left(Queue *q){
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    
    int remove_idx = q->head;
    int j = (q->head+1) % MAX_QUEUE_SIZE;
    while(j!=q->tail){
        if (q->processes[j]->time_left < q->processes[remove_idx]->time_left){
            remove_idx = j;
        }
        j = (j+1)%MAX_QUEUE_SIZE;
    }
    Process * swap = q->processes[q->head];
    q->processes[q->head] = q->processes[remove_idx];
    q->processes[remove_idx] = swap;
    void *item = q->processes[q->head];
    q->head = (q->head +1) % MAX_QUEUE_SIZE;
    
    return item;
}

int scheduler_enqueue(Scheduler *s, int queue_idx, Process *item) {
    if (queue_idx < 0 || queue_idx >= s->num_queues) {
        return -1; // invalid queue index
    }
    return enqueue(&(s->queues[queue_idx]), item);
}

void *scheduler_dequeue(Scheduler *s) {
    int i;
    for (i = 0; i < s->num_queues; i++) {
        Queue *q = &(s->queues[i]);
        void *item = dequeue(q);
        if (item != NULL) {
            return item;
        }
    }
    return NULL; // all queues are empty
}
