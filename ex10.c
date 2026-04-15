[23bcs186@mepcolinux ex10]$cat prog.c 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_QUEUE 100
#define MAX_LINE 256
#define NUM_THREADS 4
#define NUM_PRODUCERS 2

// Shared Queue
char queue[MAX_QUEUE][MAX_LINE];
int front = 0, rear = 0, count = 0;

// File list
const char *files[] = {"file1.txt", "file2.txt", "file3.txt"};
int file_index = 0;
int total_files = 3;

int producers_done = 0;

// Locks
omp_lock_t queue_lock;
omp_lock_t file_lock;

// Enqueue function
void enqueue(char *line) {
    int added = 0;

    while (!added) {
        omp_set_lock(&queue_lock);

        if (count < MAX_QUEUE) {
            strcpy(queue[rear], line);
            rear = (rear + 1) % MAX_QUEUE;
            count++;
            added = 1;
        }

        omp_unset_lock(&queue_lock);

        if (!added) {
            #pragma omp taskyield
        }
    }
}

// Dequeue function
int dequeue(char *line) {
    int success = 0;

    omp_set_lock(&queue_lock);

    if (count > 0) {
        strcpy(line, queue[front]);
        front = (front + 1) % MAX_QUEUE;
        count--;
        success = 1;
    }

    omp_unset_lock(&queue_lock);

    return success;
}

// Producer
void producer() {
    while (1) {
        const char *filename = NULL;

        omp_set_lock(&file_lock);
        if (file_index < total_files) {
            filename = files[file_index++];
        }
        omp_unset_lock(&file_lock);

        if (filename == NULL)
            break;

        FILE *fp = fopen(filename, "r");
        if (!fp) {
            printf("Error opening file %s\n", filename);
            continue;
        }

        char line[MAX_LINE];

        while (fgets(line, MAX_LINE, fp)) {
            enqueue(line);
        }

        fclose(fp);
    }

    #pragma omp atomic
    producers_done++;
}

// Consumer
void consumer() {
    char line[MAX_LINE];
    char *token;
    char *saveptr;

    while (1) {
        if (dequeue(line)) {

            token = strtok_r(line, " \t\n\r", &saveptr);

            while (token != NULL    ) {
                printf("Thread %d: %s\n", omp_get_thread_num(), token);
                token = strtok_r(NULL, " \t\n\r", &saveptr);
            }

        } else {

            int done;
            #pragma omp atomic read
            done = producers_done;

            if (done == NUM_PRODUCERS && count == 0)
                break;

            #pragma omp taskyield
        }
    }
}

// Main function
int main() {

    // Initialize locks
    omp_init_lock(&queue_lock);
    omp_init_lock(&file_lock);

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int id = omp_get_thread_num();

        if (id < NUM_PRODUCERS) {
            producer();
        } else {
            consumer();
        }
    }

    // Destroy locks
    omp_destroy_lock(&queue_lock);
    omp_destroy_lock(&file_lock);

    return 0;
}

[23bcs186@mepcolinux ex10]$cat file1.txt 
apple banana cherry
date elderberry fig

[23bcs186@mepcolinux ex10]$cat file1.txt
grape honeydew kiwi
lemon mango nectarine

[23bcs186@mepcolinux ex10]$cat file2.txt
orange papaya quince
raspberry strawberry tangerine

[23bcs186@mepcolinux ex10]$./prog 
Thread 2: apple
Thread 2: banana
Thread 2: cherry
Thread 2: lemon
Thread 2: mango
Thread 2: nectarine
Thread 2: date
Thread 2: elderberry
Thread 2: fig
Thread 2: orange
Thread 2: papaya
Thread 2: quince
Thread 2: raspberry
Thread 2: strawberry
Thread 2: tangerine
Thread 3: grape
Thread 3: honeydew
Thread 3: kiwi
