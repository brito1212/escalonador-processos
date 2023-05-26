#ifndef SCHEDULER_H
#define SCHEDULER_H

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

void scheduler_init(Scheduler *s);

int scheduler_add_queue(Scheduler *s);

int enqueue(Queue *q, Process *item);

void *dequeue(Queue *q);

void *dequeue_by_priority(Queue *q);

void *dequeue_by_time_left(Queue *q);

int scheduler_enqueue(Scheduler *s, int queue_idx, Process *item);

void *scheduler_dequeue(Scheduler *s);


#endif
