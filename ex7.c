[23bcs186@mepcolinux ex7]$cat calculatornew.c 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5
#define TASKS 20

typedef struct
{
    int num1;
    int num2;
    char op;
} task;

task work_buffer[BUFFER_SIZE];

int in = 0;
int out = 0;

sem_t empty;
sem_t full;
sem_t mutex;

FILE *result_file;

/* Producer thread (Server) */
void *server(void *arg)
{
    char ops[] = {'+', '-', '*', '/'};

    for(int i=0;i<TASKS;i++)
    {
        task t;

        t.num1 = rand()%50 + 1;
        t.num2 = rand()%50 + 1;
        t.op = ops[rand()%4];

        sem_wait(&empty);
        sem_wait(&mutex);

        work_buffer[in] = t;
        in = (in + 1) % BUFFER_SIZE;

        printf("Server produced: %d %c %d\n",t.num1,t.op,t.num2);

        sem_post(&mutex);
        sem_post(&full);

        sleep(1);
    }

    pthread_exit(NULL);
}

/* Worker thread */
void *worker(void *arg)
{
    char operation = *(char*)arg;

    while(1)
    {
        sem_wait(&full);
        sem_wait(&mutex);

        task t = work_buffer[out];

        if(t.op == operation)
        {
            out = (out + 1) % BUFFER_SIZE;

            sem_post(&mutex);
            sem_post(&empty);

            double result;

            switch(operation)
            {
                case '+':
                    result = t.num1 + t.num2;
                    break;

                case '-':
                    result = t.num1 - t.num2;
                    break;

                case '*':
                    result = t.num1 * t.num2;
                    break;

                case '/':
                    result = (double)t.num1 / t.num2;
                    break;
            }

            fprintf(result_file,
                    "Thread %lu computed: %d %c %d = %.2f\n",
                    pthread_self(),
                    t.num1,
                    operation,
                    t.num2,
                    result);

            fflush(result_file);
        }
        else
        {
            sem_post(&mutex);
            sem_post(&full);
            usleep(1);
        }
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t server_thread;
    pthread_t add_thread, sub_thread, mul_thread, div_thread;

    char add='+';
    char sub='-';
    char mul='*';
    char div='/';

    srand(time(NULL));

    result_file = fopen("result_file.txt","w");

    sem_init(&empty,0,BUFFER_SIZE);
    sem_init(&full,0,0);
    sem_init(&mutex,0,1);

    pthread_create(&server_thread,NULL,server,NULL);

    pthread_create(&add_thread,NULL,worker,&add);
    pthread_create(&sub_thread,NULL,worker,&sub);
    pthread_create(&mul_thread,NULL,worker,&mul);
    pthread_create(&div_thread,NULL,worker,&div);

    pthread_join(server_thread,NULL);

    sleep(5);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    fclose(result_file);

    return 0;
}

[23bcs186@mepcolinux ex7]$./cal 
Server produced: 18 - 17
Server produced: 13 + 46
Server produced: 29 * 24
Server produced: 5 + 36
Server produced: 11 + 38
Server produced: 34 - 3
Server produced: 7 * 47
Server produced: 4 - 16
Server produced: 15 - 32
Server produced: 9 - 42
Server produced: 42 - 9
Server produced: 39 + 23
Server produced: 26 * 1
Server produced: 18 / 7
Server produced: 38 * 17
Server produced: 29 * 2
Server produced: 28 / 10
Server produced: 33 * 13
Server produced: 42 / 27
Server produced: 21 + 38
[23bcs186@mepcolinux ex7]$cat result_file.txt 
Thread 139681512593152 computed: 18 - 17 = 1.00
Thread 139681520985856 computed: 13 + 46 = 59.00
Thread 139681504200448 computed: 29 * 24 = 696.00
Thread 139681520985856 computed: 5 + 36 = 41.00
Thread 139681520985856 computed: 11 + 38 = 49.00
Thread 139681512593152 computed: 34 - 3 = 31.00
Thread 139681504200448 computed: 7 * 47 = 329.00
Thread 139681512593152 computed: 4 - 16 = -12.00
Thread 139681512593152 computed: 15 - 32 = -17.00
Thread 139681512593152 computed: 9 - 42 = -33.00
Thread 139681512593152 computed: 42 - 9 = 33.00
Thread 139681520985856 computed: 39 + 23 = 62.00
Thread 139681504200448 computed: 26 * 1 = 26.00
Thread 139681495807744 computed: 18 / 7 = 2.57
Thread 139681504200448 computed: 38 * 17 = 646.00
Thread 139681504200448 computed: 29 * 2 = 58.00
Thread 139681495807744 computed: 28 / 10 = 2.80
Thread 139681504200448 computed: 33 * 13 = 429.00
Thread 139681495807744 computed: 42 / 27 = 1.56
Thread 139681520985856 computed: 21 + 38 = 59.00
