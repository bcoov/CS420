#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "ipcEx.h"


int main(int argc, char** argv)
{

    /* Print out usage statement when no value is specified */
    if (argc != 2) {
        printf("You must specify an integer value as an argument\n");
        return 1;
    }

    // Create shared memory chunk
    int shmem = shmget(IPC_PRIVATE, sizeof(struct ipc_struct), S_IRUSR | S_IWUSR);
    printf("PARENT: Created shared memory with a segment ID of %d\n", shmem);

    // Cast to ipc_struct
    struct ipc_struct * shared_memory = (struct ipc_struct*) shmat(shmem, NULL, 0);
    // Set repeat value (based on CL arg)
    shared_memory->repeat_val = atoi(argv[1]);
    printf("PARENT: The child process should store its string in shared\n");
    printf("        memory a total of %d times.\n\n\n", shared_memory->repeat_val);

    // Fork processes
    pid_t pid = fork();

    // Child process
    if (pid == 0) {
        //printf("Child process\n");

        char buffer[50];

        snprintf(buffer, 50, "%d", shmem);
        execlp("./childProc", buffer, NULL);
    } else {
        //printf("Parent process (waiting)\n");
        wait(NULL);

        printf("PARENT: Child with PID=%d complete\n", pid);
        printf("PARENT: Child left the following in the data buffer:\n");
        printf("============= Buffer start =============\n");
        printf("%s", shared_memory->data);
        printf("============= Buffer end ===============\n\n\n");

        shmdt(shared_memory);
        shmctl(shmem, IPC_RMID, NULL);

        printf("PARENT: Done\n");
    }

    return 0;
}

