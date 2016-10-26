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



int main(int argc, char** argv)
{
    int i = atoi(argv[0]);
    printf("Process %d says \"Hello, World!\"\n", i);
    
    return 0;
}
