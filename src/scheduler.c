#include <stdio.h>
#include <stdlib.h>
#include "../headers/scheduler.h"

#define MAX_QUEUE_SIZE 10
#define MAX_NUM_QUEUES 10


void scheduler_init(Scheduler *s) {
    s->num_queues = 0; //Inializa o escalonador sem nenhuma fila
}

int scheduler_add_queue(Scheduler *s) {
    if (s->num_queues >= MAX_NUM_QUEUES) {
        return -1; // Nao eh possivel adicionar fila, o escalonador esta cheio.
    }
    Queue *q = &(s->queues[s->num_queues]); // Adiciona fila
    q->head = 0;
    q->tail = 0;
    s->num_queues++; // Aponta para o proximo indice do buffer
    return s->num_queues - 1; //Retorna fila criada
}

int enqueue(Queue *q, Process *item) {
    if ((q->tail + 1) % MAX_QUEUE_SIZE == q->head) {
        return -1; // fila esta cheia
    }
    item->enqueued = 1;
    q->processes[q->tail] = item; //processo eh adicionado na cauda da fila
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE; // cauda muda para prox posicao
    return 0;
}

void *dequeue(Queue *q) {
    if (q->head == q->tail) {
        return NULL; // fila esta vazia
    }
    void *item = q->processes[q->head]; // item sera o processo apontado pela cabeca
    q->head = (q->head + 1) % MAX_QUEUE_SIZE; // cabeca aponta para prox posicao
    return item;
}

void *dequeue_by_priority(Queue *q){
    if (q->head == q->tail) {
        return NULL; // fila esta vazia
    }

    int remove_idx = q->head; // remove index recebe o processo da cabeca
    int j = (q->head+1) % MAX_QUEUE_SIZE; // j recebe posicao apos a cabeca
    while(j!=q->tail){ // percorre os processos da fila
        if (q->processes[j]->priority > q->processes[remove_idx]->priority){ //se processo do meio for maior que processo da cabeca
            remove_idx = j; // remove index  recebe processo j (que tem maior prioridade)
        }
        j = (j+1)%MAX_QUEUE_SIZE; // j recebe posicao seguinte
    }

    //apos percorrer toda a fila
    Process * swap = q->processes[q->head]; // swap recebe processo da cabeca
    q->processes[q->head] = q->processes[remove_idx]; // cabeca recebe processo de maior prioridade
    q->processes[remove_idx] = swap; // posicao do meio recebe processo da cabeca
    Process *item = q->processes[q->head]; // item recebe processo de maior prioridade
    q->head = (q->head +1) % MAX_QUEUE_SIZE; // cabeca aponta para prox processo

    return item; // retorna processo de maior prioridade
}

int scheduler_enqueue(Scheduler *s, int queue_idx, Process *item) {
    if (queue_idx < 0 || queue_idx >= s->num_queues) {
        return -1; // indice invalido da fila
    }
    return enqueue(&(s->queues[queue_idx]), item); //adiciona processo na fila
}

void *scheduler_dequeue(Scheduler *s, int dequeue_type) {
    int i;
    for (i = 0; i < s->num_queues; i++) {
        Queue *q = &(s->queues[i]);
        void *item;

        //escolhe qual metodo vai utilizar
        switch(dequeue_type){
            case 0:
                item = dequeue_by_priority(q);
                break;
            case 1:
                item = guarantee_scheduling(q);
                break;
            case 2:
                item = lotteryScheduler(q);
                break;
            case 3:
                item =  fairShare_Scheduling(q);
                break;
            default:
                item = dequeue(q);
                break;
        }

        if (item != NULL) {
            return item;
        }
    }
    return NULL; // todas as filas estao vazias
}