#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../headers/scheduler.h"

#define MAX_PROCESSES 100

int quantum=0;
int total_tickets=0; // inicia uma variável de tickets sem nenhum ticket
int contador=0;
int completed = 0;

int read_file(struct Process *v, int i)
{
    char *filename = "D:\\Desenvolvimento\\5oSemestre\\SO_Embarcado\\labFinal\\escalonador-processos\\processes_input.txt"; // input txt full path
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Não foi possivel encontrar o arquivo.\n");
        printf("Verifique se o caminho do arquivp está correto.\n");

        return 0;
    }
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int a, b, c;
        char i_char[10];

        sprintf(i_char, "%d", i);

        sscanf(line, "%d %d %d", &a, &b, &c);


        v[i].arrival_time = a;
        v[i].burst_time = b;
        v[i].priority = c;
        v[i].enqueued = 0;
        v[i].tickets = 0;
        v[i].bursted_time =0;

        char pid[10] = "P";
        strcat(pid, i_char);
        strcpy(v[i].pid, pid);

        printf(" %s  | a = %d, b = %d, c = %d\n", v[i].pid, a, b, c);

        // printf("Process %s: Arrival Time %d, Burst Time %d\n", v[i].pid, v[i].arrival_time, v[i].burst_time);
        i++;
    }
    fclose(file); // Close the file
    printf("%d\n", i);
    return i;
}

void imprime_simples(struct Process *v, int size)
{
    for (int j = 0; j < size; j++)
    {
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
        v[i].time_left = temp_v[i].burst_time;
        v[i].bursted_time = temp_v[i].bursted_time;
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
            queue_full = scheduler_enqueue(s, q, &p[i]);
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
    printf("vai printar");
    FILE *file;
    char *filename = "../processes_output.txt";
    file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Não foi possível abrir o arquivo de saída. \n");
        return 0;
    }

    char table_header[] = "P#   |\tAT\tBT\tST\tP";

    fprintf(file, "%s\n", table_header);

    for (int i = 0; i < n; i++)
    {
        fprintf(file, "%-4s |\t%d\t%d\t%d\t%d\n", p[i].pid, p[i].arrival_time, p[i].burst_time, p[i].start_time, p[i].priority);
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
                    if (p[i].arrival_time >= time || p[i].burst_time == 0)
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
// realizar escalonamento garantido:
void guarantee_scheduling( Queue *q){

    int i = 0, j;
    int num_processes = 0;
    int time_sum = 0;
    int timeDivided = 0;

    // Contar processos
    j = q->head;
    while(j!= q->tail){
        num_processes++;
        j = (j + 1) % MAX_QUEUE_SIZE; // atualiza j para a próxima posição
    }

    //retornar caso a fila esteja vazia
    if(num_processes == 0){
        printf("\n\n * * FIM DO ESCALONAMENTO * *\n\n");
        completed = 1;
        //return;
    }

    // Somar os tempos dos processos e printa-los
    int timeCPU = 20;
    j = q->head;
    printf("\n QUANTUM: %d \n", quantum);
    while(j!= q->tail){
        printf("%s: BT: %d  TL: %d  \n", q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left);

        j = (j + 1) % MAX_QUEUE_SIZE; // atualiza j para a próxima posição
    }
    printf("\n");

    timeDivided = timeCPU / num_processes; // define tempo igual para cada processo

    // Executar processos com o tempo dividido
    Process * aux;
    j = q->head;

    if(contador!=99){
        j = contador;
    }
    while(j!= q->tail){

        printf("\n\n%s: BT: %d  TL: %d   -  Tempo dividido: %d   Quantum: %d \n",  q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left, timeDivided, quantum);

        if(q->processes[j]->time_left < timeDivided){
            quantum = quantum + q->processes[j]->time_left;
            q->processes[j]->time_left = 0;

            printf("%s: BT: %d  TL: %d   -  Tempo dividido: %d   Quantum: %d \n",  q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left, timeDivided, quantum);

        }else{
            quantum = quantum + timeDivided;
            q->processes[j]->time_left = q->processes[j]->time_left - timeDivided;
            printf("%s: BT: %d  TL: %d   -  Tempo dividido: %d   Quantum: %d \n",  q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left, timeDivided, quantum);
        }

        if(q->processes[j]->time_left == 0){
            printf("\n\n processo %d  saiu \n\n",j);
            aux = q->processes[q->head];
            q->processes[q->head] = q->processes[j];
            q->processes[j] = aux;
            dequeue(q);
            contador = 99;
            return 1;
        }else{

        }

        j = (j + 1) % MAX_QUEUE_SIZE; // atualiza j para a próxima posição
        contador=j;
        return;
    }
}

void fairShare_Scheduling( Queue *q){

    int i = 0, j;
    int num_processes = 0;
    int time_sum = 0;
    int timeDivided = 0;

    // Contar processos
    j = q->head;
    while(j!= q->tail){
        num_processes++;
        j = (j + 1) % MAX_QUEUE_SIZE; // atualiza j para a próxima posição
    }

    //retornar caso a fila esteja vazia
    if(num_processes == 0){
        printf("\n\n * * FIM DO ESCALONAMENTO * *\n\n");
        completed = 1;
        //return;
    }

    // Somar os tempos dos processos e printa-los
    j = q->head;
    printf("\n QUANTUM: %d \n", quantum);
    while(j!= q->tail){
        printf("%s: BT: %d  TL: %d  \n", q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left);
        time_sum += q->processes[j]->burst_time;
        j = (j + 1) % MAX_QUEUE_SIZE; // atualiza j para a próxima posição
    }
    printf("\n");

    timeDivided = time_sum / num_processes; // define tempo igual para cada processo


    // Executar processos com o tempo dividido
    Process * aux;
    j = q->head;

    if(contador!=99){
        j = contador;
    }
    while(j!= q->tail){

        printf("\n\n%s: BT: %d  TL: %d   -  Tempo dividido: %d   Quantum: %d \n",  q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left, timeDivided, quantum);

        if(q->processes[j]->time_left < timeDivided){
            quantum = quantum + q->processes[j]->time_left;
            q->processes[j]->time_left = 0;

            printf("%s: BT: %d  TL: %d   -  Tempo dividido: %d   Quantum: %d \n",  q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left, timeDivided, quantum);

        }else{
            quantum = quantum + timeDivided;
            q->processes[j]->time_left = q->processes[j]->time_left - timeDivided;
            printf("%s: BT: %d  TL: %d   -  Tempo dividido: %d   Quantum: %d \n",  q->processes[j]->pid,q->processes[j]->burst_time,q->processes[j]->time_left, timeDivided, quantum);
        }

        if(q->processes[j]->time_left == 0){
            printf("\n\n processo %d  saiu \n\n",j);
            aux = q->processes[q->head];
            q->processes[q->head] = q->processes[j];
            q->processes[j] = aux;
            dequeue(q);
            contador = 99;
            return 1;
        }else{

        }

        j = (j + 1) % MAX_QUEUE_SIZE; // atualiza j para a próxima posição
        contador=j;
        return;
    }
}


int main()
{

    printf("Qual ");

    executaPrioridade();
    //executaGarantido();
    //executaLoteria();
    //executaFairShare();


    return 0;
}

//--------------------------------------LOTERIA
void *lotteryScheduler(Queue *q) {
    printf("entrou na loteria\n");
    printf("%d %d \n", q->head, q->tail);

    // Percorre o buffer
    int j = q->head; // variável j para percorrer a fila
    while (j != q->tail) {
            if(q->processes[j]->tickets == 0) {

                q->processes[j]->tickets = q->processes[j]->time_left; // dá tickets para o processo com base no seu tempo
                total_tickets += q->processes[j]->tickets; // atualiza a quantidade total de tickets
            }
            printf("Tempo do processo %s: %d Tickets: %d\n", q->processes[j]->pid, q->processes[j]->burst_time, q->processes[j]->tickets);

        j = (j + 1) % MAX_QUEUE_SIZE; // atualiza j para a próxima posição
    }

    //int j = (q->head+1) % MAX_QUEUE_SIZE; // j recebe posicao apos a cabeca
    Process *aux;

    j = q->head;
        while(j!= q->tail){ // percorre os processos da fila
            srand(time(NULL));
            int numero_sorteado = (rand() % total_tickets)+1;

            printf("\n---------------------------------------------- numero sorteado: %d   %s\n\n", numero_sorteado,q->processes[j]->pid);

            if(numero_sorteado < q->processes[j]->tickets){
                //subtrair 1 unidade de tempo deste processo e verificar se ele será finalizado ou não
                q->processes[j]->time_left--;
                q->processes[j]->tickets--;
                total_tickets--;
                quantum++;

                printf("Processo %s selecionado!   | Tickets : %d    OK\n",q->processes[j]->pid,q->processes[j]->tickets);

                if(q->processes[j]->tickets== 0){
                    printf("\n\n processo %s  saiu \n\n",q->processes[j]->pid);
                    aux = q->processes[q->head];
                    q->processes[q->head] = q->processes[j];
                    q->processes[j] = aux;
                    dequeue(q);
                    return 1;
                }
                return 0;
            }else{
                printf("nenhum processo selecionado                         X\n");
            }
            j = (j+1)%MAX_QUEUE_SIZE; // j recebe posicao seguinte

        }
}

executaPrioridade(){

       printf("Escalonador de processos\n");
    struct Process temp_array[MAX_PROCESSES];
    printf("Escalonador de processos\n");
    int n = read_file(temp_array, n);

    printf("Escalonador de processos\n");
    struct Process p[n];
    get_process_array(temp_array, p, n);

    Scheduler s1;

    scheduler_init(&s1);
    int q1 = scheduler_add_queue(&s1);

//--------------------------------------------------------------
    int current_time = 0;
    int completed = 0;
    while (completed != n)
    {
        printf("%d",n);

        enqueue_processes_by_time(&s1, q1, p, current_time);
        void *item = scheduler_dequeue(&s1, 0);
        if (item != NULL)
        {

            Process *process = item;
            process->completion_time = process->burst_time + current_time;
            process->start_time = current_time;
            current_time = process->completion_time + 1;
            completed++;

            printf("item nao nulo\n");
        }
        else
        {
            printf("item nulo\n");
            current_time++;
        }
        printf("Current time:  %d\n\n", current_time);
    }

    write_output_file(p, n);

    return 0;
}

executaLoteria(){
    printf("executa loteria\n");


    struct Process temp_array[MAX_PROCESSES];
    int n = read_file(temp_array, n);
    struct Process p[n];
    struct Process *completed_processes[n];
    get_process_array(temp_array, p, n);

    Scheduler s1;
    scheduler_init(&s1);
    int q1 = scheduler_add_queue(&s1);

//-------------------------------------------------------------
    int completed = 0;
    while (completed != n)
    {
        enqueue_processes_by_time(&s1, q1, p, quantum);
        void *item = scheduler_dequeue(&s1, 2);
    }

    write_output_file(p, n);

    return 0;
}

executaGarantido(){
    printf("executa garantido\n");


    struct Process temp_array[MAX_PROCESSES];
    int n = read_file(temp_array, n);
    struct Process p[n];
    struct Process *completed_processes[n];
    get_process_array(temp_array, p, n);

    Scheduler s1;
    scheduler_init(&s1);
    int q1 = scheduler_add_queue(&s1);

//-------------------------------------------------------------

    while (completed != n)
    {
        enqueue_processes_by_time(&s1, q1, p, quantum);
        void *item = scheduler_dequeue(&s1, 1);
        if (item != NULL)
        {
            if(item == 1){
                completed++;
                printf("\n\n...............................................................Completed: %d\n\n", completed);
            }
           // printf("item nao nulo.    Completos: %d\n    NAO NULO", completed);
        }
        else
        {
           // printf("item nulo                 NULO\n");
          //  printf("Quantum: %d \n", quantum);
        }
    }

    write_output_file(p, n);
    return 0;

}

executaFairShare(){
    printf("executa fair share\n");


    struct Process temp_array[MAX_PROCESSES];
    int n = read_file(temp_array, n);
    struct Process p[n];
    struct Process *completed_processes[n];
    get_process_array(temp_array, p, n);

    Scheduler s1;
    scheduler_init(&s1);
    int q1 = scheduler_add_queue(&s1);

//-------------------------------------------------------------
    int completed = 0;
    while (completed != n)
    {

        //verificar o time left para ver se conclui o não
/*
        *lotteryScheduler(q2);
        for(){
            if(q2.processes[i].time_left == 0){
                dequeue(q2);
            }
        }
*/
        enqueue_processes_by_time(&s1, q1, p, quantum);
        void *item = scheduler_dequeue(&s1, 3);
        //quantum++;
        if (item != NULL)
        {
            if(item == 1){
                completed++;
            }
           // printf("item nao nulo.    Completos: %d\n    NAO NULO", completed);
        }
        else
        {
           // printf("item nulo                 NULO\n");
          //  printf("Quantum: %d \n", quantum);
        }
    }

    write_output_file(p, n);

    return 0;

}