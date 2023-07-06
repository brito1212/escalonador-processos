#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/scheduler.h"

#define MAX_PROCESSES 100

int current_group = 0;

int read_file(struct Process *v, int i) 
{
    printf("ReadFile1\n");
    char *filename = "C:\\PROJETOS\\ECOS13-LabSOEmbarcado\\escalonador-processos\\escalonador-processos\\processes_input.txt"; // input txt full path
    printf("ReadFile2\n");
    FILE *file = fopen(filename, "r");
    printf("ReadFile3\n");
    if (file == NULL)
    {
        printf("Não foi possivel encontrar o arquivo.\n");
        printf("Verifique se o caminho do arquivp está correto.\n");

        return 0;
    }
    printf("ReadFile4\n");
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int a, b, c;
        char i_char[10];

        printf("ReadFile4.1\n");
        sprintf(i_char, "%d", i);

        sscanf(line, "%d %d %d", &a, &b, &c);
        printf("a = %d, b = %d, c = %d\n", a, b, c);

        printf("ReadFile4.2\n");

        printf("Process %s: Arrival Time %d, Burst Time %d\n", i_char, a, b);
        v[i].arrival_time = a;
        printf("ReadFile4.3\n");
        v[i].burst_time = b;
        v[i].priority = c;
        v[i].enqueued = 0;
        v[i].quantum = 0;
        v[i].bursted_time = 0;
        v[i].time_left = b;

        char pid[10] = "P";
        strcat(pid, i_char);
        strcpy(v[i].pid, pid);

        // printf("Process %s: Arrival Time %d, Burst Time %d\n", v[i].pid, v[i].arrival_time, v[i].burst_time);
        printf("ReadFile4.4\n");
        i++;
    }
    printf("ReadFile5\n");
    fclose(file); // Close the file
    printf("ReadFile6\n");
    printf("%d\n", i);
    return i;
}

void imprime_simples(struct Process *v, int size)
{
    for (int j = 0; j < size; j++) {
        printf("Process %s: Arrival Time %d, Burst Time %d\n", v[j].pid, v[j].arrival_time, v[j].burst_time);
    }
}

void get_process_array(struct Process *temp_v, struct Process *v, int size)
{
    for (int i = 0; i < size; i++)
    {
        strcpy(v[i].pid, temp_v[i].pid);
        v[i].arrival_time = temp_v[i].arrival_time;
        v[i].burst_time = temp_v[i].burst_time;
        v[i].priority = temp_v[i].priority;
        v[i].enqueued = temp_v[i].enqueued;
        v[i].quantum = temp_v[i].enqueued;
        v[i].bursted_time = temp_v[i].bursted_time;
        v[i].time_left = temp_v[i].time_left;
    }
}

void enqueue_processes_by_time(Scheduler *s, int q, Process *p, int current_time)
{
    int queue_full = 0;
    int i = 0;
    while (queue_full != -1)
    {
        if (p[i].arrival_time <= current_time && p[i].enqueued == 0)
        {
            p[i].group = current_group;
            queue_full = scheduler_enqueue(s, q, &p[i]);
            if(current_group == 0){
                current_group = 1;
            }else{
                current_group = 0;
            }
        }
        else if (p[i].arrival_time > current_time)
        {
            break;
        }
        i++;
    }
}

void *write_output_file(Process *p, int n)
{
    FILE *file;
    char *filename = "C:\\PROJETOS\\ECOS13-LabSOEmbarcado\\escalonador-processos\\escalonador-processos\\processes_output.txt";
    file = fopen(filename, "a");

    if (file == NULL)
    {
        printf("Não foi possível abrir o arquivo de saída. \n");
        return 0;
    }

    

    int conclusion = 0;
    for (int i = 0; i < n; i++)
    {
        if (p[i].completion_time > conclusion && p[i].enqueued == 1)
        {
            conclusion = p[i].completion_time;
        }
    }

    fprintf(file, "\n");

    int time = 0;

    while (time != conclusion)
    {
        char formatted_time[20];
        sprintf(formatted_time, "%d-%d", time, time + 1);
        fprintf(file, "%-7s |\t", formatted_time);

        for (int i = 0; i < n; i++)
        {

            if (p[i].start_time == time && p[i].burst_time != 0)
            {

                fprintf(file, "##\t");
                p[i].start_time++;
                p[i].burst_time--;
            }
            else
            {
                if (p[i].arrival_time <= time && p[i].burst_time != 0)
                {
                    fprintf(file, "--\t");
                }
                else
                {
                    if (p[i].arrival_time >= time || p[i].burst_time == 0 || p[i].enqueued == 0)
                    {
                        fprintf(file, "  \t");
                    }
                }
            }
        }
        fprintf(file, "\n");
        time++;
    }
}

void *write_output_file_header(Process *p, int n){

    FILE *file;
    char *filename = "C:\\PROJETOS\\ECOS13-LabSOEmbarcado\\escalonador-processos\\escalonador-processos\\processes_output.txt";
    file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Não foi possível abrir o arquivo de saída. \n");
        return 0;
    }

    char table_header[] = "P#   |\tAT\tBT\tP";

    fprintf(file, "%s\n", table_header);

    for (int i = 0; i < n; i++)
    {
        fprintf(file, "%-4s |\t%d\t%d\t%d\n", p[i].pid, p[i].arrival_time, p[i].burst_time, p[i].priority);
    }

    fprintf(file, "\n");
    fprintf(file, "\n");
    fprintf(file, "\n");
    fprintf(file, "tempo    \t");

    int conclusion = 0;
    for (int i = 0; i < n; i++)
    {
        fprintf(file, "%-3s\t", p[i].pid);
        if (p[i].completion_time > conclusion)
        {
            conclusion = p[i].completion_time;
        }
    }

    fprintf(file, "\n");

}

// realizar escalonamento garantido:
void guarantee_scheduling(Scheduler *s, int q, Process *p, int current_time)
{
    int i = 0;
    int num_processes = 0;
    int time_sum = 0;
    int timeDivided = 0;

    // Contar processos
    while (p[num_processes].arrival_time != -1 && num_processes < MAX_PROCESSES)
    {
        num_processes++;
    }

    // Somar os tempos dos processos
    for (i = 0; i < num_processes; i++)
    {
        time_sum += p[i].burst_time;
    }
    timeDivided = time_sum / num_processes; // define tempo igual para cada processo

    // Executar processos com o tempo dividido
    for (i = 0; i < num_processes; i++)
    {
        if (p[i].arrival_time <= current_time && p[i].enqueued == 0)
        {
            scheduler_enqueue(s, q, &p[i]);
            if (p[i].burst_time > timeDivided)
            {
                p[i].burst_time -= timeDivided;
                p[i].enqueued = 1;
            }
            else
            {
                p[i].enqueued = 0;
                p[i].burst_time = 0;
            }
        }
    }
}


int main()
{

    printf("Escalonador de processos\n");
    struct Process temp_array[MAX_PROCESSES];
    printf("Escalonador de processos\n");
    int n = read_file(temp_array, n);

    printf("Escalonador de processos\n");
    struct Process p[n];
    get_process_array(temp_array, p, n);

    Scheduler s;
    scheduler_init(&s);

    int q1 = scheduler_add_queue(&s);

    int current_time = 0;
    int completed = 0;
    write_output_file_header(p, n);
    while (completed != n)
    {
        enqueue_processes_by_time(&s, q1, p, current_time);

        void *item = scheduler_dequeue(&s, 1);
        if (item != NULL)
        {
            
            Process *process = item;
            if(process->time_left == 0) {
                // completed_processes[completed] = process;
                completed++;
                print_process(process);
            }
            else {
                scheduler_enqueue(&s, q1, item);
                // completed_processes[completed] = process;
                // completed++;
            }

            process->completion_time = process->bursted_time + current_time;
            process->start_time = current_time;
            current_time = process->completion_time + 1;

            // write_output_file(p, n);
        }
        else
        {
            current_time++;
        }
    }

    
    return 0;
}


