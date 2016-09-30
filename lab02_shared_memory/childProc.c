#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ipcEx.h"


int main(int argc, char** argv)
{
    printf("CHILD: Received %d arguments\n", argc);
    char data_string[] = "Hello Shared Memory!\n";

    // Save passed-in argument (shared memory ID)
    int shm_id = atoi(argv[0]);
    printf("CHILD: Attempting to access segment ID %d...\n", shm_id);

    // Access shared memory
    struct ipc_struct * shared_memory = (struct ipc_struct *) shmat(shm_id, NULL, 0);
    printf("CHILD: Parent requested that I store my data %d times\n",
           shared_memory->repeat_val);

    // Save string into shared data buffer, repeating as requested
    for (int i = 0; i < shared_memory->repeat_val; ++i) {
        sprintf(shared_memory->data + strlen(shared_memory->data), "%s", data_string);
    }
    printf("CHILD: Done copying data, exiting\n\n\n\n");

    // Detach from memory
    shmdt((void *) shared_memory);

    return 0;
}



