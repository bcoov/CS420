#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "utils.h"

#define BUF_SIZE 1024
#define INVALID -1

// Data to pass to threads
typedef struct {
    int thread_num;
    int proc_num;
    char * t_filename;
    sem_t * sema4;
} ThreadData;

// Quick and dirty method of seeing if value increased a tens place
int timeToIncrement(int value) {
    if (value == 10 ||
        value == 100 ||
        value == 1000 ||
        value == 10000 ||
        value == 100000 ||
        value == 1000000) {
        return 1;
    } else {
        return -1;
    }
}

void * thread_work(void * t_arg) {
    ThreadData * t_data = t_arg;
    
    // Lock semaphore
    sem_wait(t_data->sema4);
    
/*    printf("\tHello from thread #%d in process %d!\n", t_data->thread_num,*/
/*                                                       t_data->proc_num);*/

    // File operations (critical section)
    FILE * data_file = open_file(t_data->t_filename, "r+");
    
    int df_num = fileno(data_file);
    int num_read = 0;
    int c = INVALID;
    char buffer[BUF_SIZE];
    
    // Move to end of file, before the last non-newline character
    fseek(data_file, -2, SEEK_END);
    
    // Find beginning of 2nd-to-last line and consume first char
    c = fgetc(data_file);
    while (c != '\n') {
        if (c == '0' && ftell(data_file) <= 2) {
            // First line (the "0" line); break, c already equals 0
            break;
        }
        // Backtrack 2 bytes/chars and consume next char
        fseek(data_file, -2, SEEK_CUR);
        c = fgetc(data_file);
    }
    
    // Move up one char if still on newline for some reason
    if (c == '\n') {
        c = fgetc(data_file);
    }
    // Read in last line to buffer
    while (c != '\n' && c != EOF && c != NULL) {
        buffer[num_read] = (char) c;
        num_read++;
        c = fgetc(data_file);
    }
    // Null-terminate char sequence
    buffer[num_read] = '\0';

    // Write new value
    int read_val = atoi(buffer);
    char num_str[10];
    sprintf(num_str, "%d", read_val + 1);
    
    // Add one if tens place increased (i.e., 9 -> 10)
    if (timeToIncrement(read_val + 1) > 0) {
        num_read++;
    }
    // Write to file
    fseek(data_file, 0, SEEK_END);
    write(df_num, num_str, num_read);
    write(df_num, "\n", 1);
    close_file(data_file);
    
    // Unlock semaphore; Critical section complete
    sem_post(t_data->sema4);
    
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    int i = atoi(argv[0]);
    int num_threads = atoi(argv[1]);
    char * filename = argv[2];
    
    sem_t * thread_sem = sem_open("bcoover", 1);
    pthread_t threads[num_threads];
    ThreadData data[num_threads];
    
    printf("Process %d says \"Hello, World!\"\n", i);
    
    // Spawn child threads
    for (int t = 0; t < num_threads; t++) {
        data[t].thread_num = t;
        data[t].proc_num = i;
        data[t].t_filename = filename;
        data[t].sema4 = thread_sem;
        pthread_create(&threads[t], NULL, thread_work, &data[t]);
    }
    // Wait for threads to complete
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    return 0;
}
