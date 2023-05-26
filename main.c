#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct process {
    char pid[10];
    int arrival_time;
    int burst_time;
    int start_time;
    int completion_time;
    int priority;
};

int readFile(struct process* v, int i) {
    char *filename = "C:\\PROJETOS\\ECOS13-LabSOEmbarcado\\PROJETO-SO-EMBARCADO\\processes_input.txt";
    FILE *file = fopen(filename, "r"); 
    if (file == NULL) {
        printf("Unable to open the file.\n");
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
    const struct process* processA = (const struct process*)a;
    const struct process* processB = (const struct process*)b;

    if (processA->arrival_time < processB->arrival_time)
        return -1;
    else if (processA->arrival_time > processB->arrival_time)
        return 1;
    else 
        return 0;
    
}

void imprime_simples(struct process* v, int size){
    for (int j = 0; j < size; j++){
        printf("Process %s: Arrival Time %d, Burst Time %d\n", v[j].pid, v[j].arrival_time, v[j].burst_time);
    }
}

void get_process_array(struct process* temp_v,struct process* v, int size){    
    for (int i = 0; i < size; i++){
        strcpy(v[i].pid, temp_v[i].pid);
        v[i].arrival_time = temp_v[i].arrival_time;
        v[i].burst_time = temp_v[i].burst_time;
        v[i].priority = temp_v[i].priority;
    }
}

int main(){
    struct process temp_array[100];
    int n = readFile(temp_array, n);
    struct process p[n];
    get_process_array(temp_array, p, n);

    qsort(p, (sizeof(p)/sizeof(p[0])), sizeof(p[0]), compareByArrivalTime);

    return 0;
}