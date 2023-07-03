#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/scheduler.h"

#define MAX_PROCESSES 100

int read_file(struct Process* v, int i) {
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
        v[i].enqueued = 0;

        char pid[10] = "P";
        strcat(pid, i_char);
        strcpy(v[i].pid, pid);

        // printf("Process %s: Arrival Time %d, Burst Time %d\n", v[i].pid, v[i].arrival_time, v[i].burst_time);

        i++;
    }

    fclose(file); // Close the file

    return i;
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
        v[i].enqueued = temp_v[i].enqueued;
    }
}


void enqueue_processes_by_time(Scheduler * s, int q, Process * p, int current_time) {
    int queue_full = 0;
    int i = 0;
    while(queue_full != -1){
        if (p[i].arrival_time <= current_time && p[i].enqueued == 0){
            queue_full = scheduler_enqueue(s, q, &p[i]);
        }
        else if (p[i].arrival_time > current_time) {
            break;
        }
        i++;
    }
}


void * write_output_file(Process * p, int n){
    FILE * file;
    char * filename = "C:\\PROJETOS\\ECOS13-LabSOEmbarcado\\escalonador-processos\\escalonador-processos\\processes_output.txt";
    file = fopen(filename, "w");

    if(file == NULL){
        printf("Não foi possível abrir o arquivo de saída. \n");
        return 0;
    }

    char table_header[] = "P#   |\tAT\tBT\tST\tP";

    fprintf(file, "%s\n", table_header);

    for (int i = 0; i < n; i++) {
        fprintf(file, "%-4s |\t%d\t%d\t%d\t%d\n", p[i].pid, p[i].arrival_time, p[i].burst_time, p[i].start_time, p[i].priority);
    }


    fprintf(file, "\n");
    fprintf(file, "\n");
    fprintf(file, "\n");
    fprintf(file, "tempo    \t");

    int conclusion = 0;
    for (int i = 0; i < n; i++) {
        fprintf(file, "%-3s\t", p[i].pid);
        if (p[i].completion_time > conclusion) {
            conclusion = p[i].completion_time;
        }
    }

    fprintf(file, "\n");

    int time = 0;

    while (time != conclusion) {
        char formatted_time[20];
        sprintf(formatted_time, "%d-%d", time, time+1);
        fprintf(file, "%-7s |\t", formatted_time);

        for (int i = 0; i < n; i++) {

            if (p[i].start_time == time && p[i].burst_time != 0) {

                fprintf(file, "##\t");
                p[i].start_time++;
                p[i].burst_time--;

            }
            else {
                if (p[i].arrival_time <= time && p[i].burst_time != 0) {
                    fprintf(file, "--\t");
                }
                else {
                    if (p[i].arrival_time >= time || p[i].burst_time == 0) {
                        fprintf(file, "  \t");
                    }
                }
            }





        }
        fprintf(file, "\n");
        time++;
    }

}

int main(){
    struct Process temp_array[MAX_PROCESSES];
    int n = read_file(temp_array, n);

    struct Process p[n];
    struct Process * completed_processes[n];
    get_process_array(temp_array, p, n);

    Scheduler s;
    scheduler_init(&s);

    int q1 = scheduler_add_queue(&s);


    int current_time = 0;
    int completed = 0;
    while (completed != n) {
        enqueue_processes_by_time(&s, q1, p, current_time);
        void * item = scheduler_dequeue(&s, 0);
        if(item != NULL){
            Process * process = item;
            process->completion_time = process->burst_time + current_time;
            process->start_time = current_time;
            current_time = process->completion_time + 1;
            completed_processes[completed] = process;
            completed++;
        }else{
            current_time++;
        }
    }
    
    write_output_file(p, n);
    return 0;
}