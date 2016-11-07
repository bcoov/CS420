#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "utils.h"



int main(int argc, char** argv)
{
    char* filename = NULL;  // DO NOT MODIFY THIS VARIABLE NAME
    int num_procs = 0;      // DO NOT MODIFY THIS VARIABLE NAME
    int num_threads = 0;    // DO NOT MODIFY THIS VARIABLE NAME
    
    // TODO: parse arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:f:t:")) != -1) {
        switch (opt) {
        case 'p':
            num_procs = atoi(optarg);
        case 't':
            num_threads = atoi(optarg);
        case 'f':
            filename = optarg;
        }
    }
    
    // TODO: write initial '0' to file
    FILE * output = open_file(filename, "w");
    int fd = fileno(output);
    
    write(fd, "0\n", 2);
    close_file(output);

    // TODO: create a named semaphore
    sem_t * sema4 = sem_open("bcoover", O_CREAT | O_EXCL, S_IRWXU, 1);    
    
    // TODO: fork off child processes and wait for them to finish
    printf("Preparing to fork %d child processes...\n", num_procs);
    int pids[num_procs];
    int proc = 0;
    char thread[10];
    sprintf(thread, "%d", num_threads);
    
    for (int i = 0; i < num_procs; ++i) {
        char num[10];
        sprintf(num, "%d", i);
        pids[i] = fork();
        if (pids[i] == 0) {
            execlp("./fileWriter", num, thread, filename, NULL);
        }
    }
    // Wait for each process to finish
    while (proc < num_procs) {
        wait(NULL);
        proc++;
    }
    
    // TODO: clean up and close named semaphore
    printf("Cleaning up semaphore...\n");
    sem_close(sema4);
    sem_unlink("bcoover");
    
    
    /////////////////////////////////////////////////////////////////////////////////////
    //
    // ** IMPORTANT: DO NOT REMOVE/CHANGE/MODIFY/EDIT OR DO ANYTHING TO THE CODE BETWEEN
    //               HERE AND THE END OF THIS FILE.  YOU CANNOT CHANGE THIS CODE TO MAKE
    //               IT WORK WITH YOUR PROGRAM.  IF YOUR PROGRAM DOESN'T WORK WITH THIS
    //               CODE, THEN YOUR PROGRAM -DOES NOT WORK-.  CHANGE YOUR PROGRAM AS
    //               NECESSARY TO WORK WITH THIS CODE.  ANY CHANGES TO THE CODE BELOW WILL
    //               RESULT IN 0 CREDIT FOR THIS LAB.  BE SURE ALL OF YOU CHILD PROCESSES
    //               HAVE TERMINATED BEFORE THIS POINT.
    //
    /////////////////////////////////////////////////////////////////////////////////////
    // This section checks the output file to see if everthing went as planned.  A message
    // is printed to the terminal indicated if everything works correctly.
    /////////////////////////////////////////////////////////////////////////////////////
    FILE* fpt = open_file(filename, "r");   // open the file for reading
    int max_value = num_procs * num_threads;
    int value;
    int success = 1;
    
    // The following loop will read the file after it has been written by all of the
    // child processes and threads.  It will read each line and compare it to the
    // expected value.  If an error is encoutered, the 'success' flag will be set
    // to false.
    for (int expected_value=0; expected_value <= max_value; expected_value++) {
        fscanf(fpt, "%i", &value);
        if (expected_value != value) {
            success = 0;
            break;
        }
    }
    close_file(fpt);                        // close the file
    
    
    if (success) {
        printf("\n\n-SUCCESS-\n\n");
    } else {
        printf("\n\n-EPIC FAIL-\n\n");
    }
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    
    return 0;
}

