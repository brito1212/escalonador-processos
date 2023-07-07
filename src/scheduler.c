
#include <stdio.h>
#include <stdlib.h>
#include "../headers/scheduler.h"

#define MAX_QUEUE_SIZE 10
#define MAX_NUM_QUEUES 10
#define RESOURCES_TIME 30

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


void *dequeue_by_garantee(Queue *q){
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }
    
    // int remove_idx = q->head;
    int j = (q->head+1) % MAX_QUEUE_SIZE;

    Process * swap = q->processes[q->head];
    q->processes[q->head] = q->processes[j];
    q->processes[j] = swap;
    Process *item = q->processes[q->head];
    q->head = (q->head +1) % MAX_QUEUE_SIZE;


    if (item->quantum >= item->burst_time){
        item->bursted_time = item->burst_time;
    }
    else {
        item->bursted_time = item->quantum;
    }
    int aux = item->time_left - item->bursted_time;
    if (aux < 0) {
        item->bursted_time = item->time_left;
        item->time_left = 0;
    }else if (aux >= 0) {
        item->time_left = aux;
    }
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

void *set_garantee_quantum(Queue *q){
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }

    int total_processes = 0;
    int j = (q->head+1) % MAX_QUEUE_SIZE;
    while(j!=q->tail){
        total_processes++;
        j = (j+1)%MAX_QUEUE_SIZE;
    }


    int quantum = 0;
    if (total_processes > 0){
        quantum = RESOURCES_TIME/total_processes;
    }
    
    j = (q->head+1) % MAX_QUEUE_SIZE;
    while(j!=q->tail){
        q->processes[j]->quantum = quantum;
        j = (j+1)%MAX_QUEUE_SIZE;
    }

    return NULL;
}

void *set_fair_share_quantum(Queue *q, int group){
    if (q->head == q->tail) {
        return NULL; // queue is empty
    }

    int total_time = 0;
    int total_processes = 0;
    int j = (q->head+1) % MAX_QUEUE_SIZE;
    while(j!=q->tail){
        if (q->processes[j]->group == group){
            total_time = total_time + q->processes[j]->burst_time;
            total_processes++;
        }
        j = (j+1)%MAX_QUEUE_SIZE;
    }

    int quantum = total_time/total_processes;
    j = (q->head+1) % MAX_QUEUE_SIZE;
    while(j!=q->tail){
        if (q->processes[j]->group == group){
            q->processes[j]->quantum = quantum;
        }
        j = (j+1)%MAX_QUEUE_SIZE;
    }

    return NULL;
}

void *scheduler_dequeue(Scheduler *s, int dequeue_type) {
    int i;
    for (i = 0; i < s->num_queues; i++) {
        Queue *q = &(s->queues[i]);
        void *item;
        if(dequeue_type == 0){
            item = dequeue_by_priority(q);
        }else if (dequeue_type == 1) {
            set_garantee_quantum(q);
            item = dequeue_by_garantee(q);   
        } else if (dequeue_type == 2){
            set_fair_share_quantum(q, 0);
            set_fair_share_quantum(q, 1);
            item = dequeue(q);
        }
        if (item != NULL) {
            return item;
        }
    }
    return NULL; // all queues are empty
}


