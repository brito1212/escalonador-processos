#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/scheduler.h"

#define MAX_PROCESSES 100

int readFile(struct Process* v, int i) {
    char *filename = "C:\\PROJETOS\\ECOS13-LabSOEmbarcado\\escalonador-processos\\escalonador-processos\\processes_input.txt"; //input txt full path 
    FILE *file = fopen(filename, "r"); 
    if (file == NULL) {
        printf("Não foi possivel encontrar o arquivo.\n");
        printf("Verifique se o caminho do arquivp está correto.\n");

        return 0;
    }

    char line[256]; 

    
    while (fgets(line, sizeof(line), file) != NULL) {
        int a, b, c;
        char i_char[10];

        sprintf(i_char, "%d", i);

        sscanf(line, "%d %d %d", &a, &b, &c); 

        v[i].arrival_time = a;
        v[i].burst_time = b;
        v[i].priority = c;

        char pid[10] = "P";
        strcat(pid, i_char);
        strcpy(v[i].pid, pid);

        // printf("Process %s: Arrival Time %d, Burst Time %d\n", v[i].pid, v[i].arrival_time, v[i].burst_time);

        i++;
    }

    fclose(file); // Close the file

    return i;
}

int compareByArrivalTime(const void* a, const void* b) {
    const struct Process* processA = (const struct Process*)a;
    const struct Process* processB = (const struct Process*)b;

    if (processA->arrival_time < processB->arrival_time)
        return -1;
    else if (processA->arrival_time > processB->arrival_time)
        return 1;
    else 
        return 0;
    
}

void imprime_simples(struct Process* v, int size){
    for (int j = 0; j < size; j++){
        printf("Process %s: Arrival Time %d, Burst Time %d\n", v[j].pid, v[j].arrival_time, v[j].burst_time);
    }
}

void get_process_array(struct Process* temp_v,struct Process* v, int size){    
    for (int i = 0; i < size; i++){
        strcpy(v[i].pid, temp_v[i].pid);
        v[i].arrival_time = temp_v[i].arrival_time;
        v[i].burst_time = temp_v[i].burst_time;
        v[i].priority = temp_v[i].priority;
    }
}

int main(){
    struct Process temp_array[MAX_PROCESSES];
    int n = readFile(temp_array, n);

    struct Process p[n];
    get_process_array(temp_array, p, n);
    qsort(p, (sizeof(p)/sizeof(p[0])), sizeof(p[0]), compareByArrivalTime);

    Scheduler s;
    scheduler_init(&s);

    int q1 = scheduler_add_queue(&s);

    int queue_full = 0;
    int processes_in_queue = 0;

    while(queue_full != -1){
        queue_full = scheduler_enqueue(&s, q1, &p[processes_in_queue]);
        processes_in_queue++;
    }

    int completed = 0;


    return 0;
}