#ifndef _IPCEX_H_
#define _IPCEX_H_

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 128
#endif

#include <string.h>

// Streamline message/error reporting
void write_msg(int loc, char * msg, int error) {
    char msg_str[50];
    int msg_len;

    strcpy(msg_str, msg);
    msg_len = strlen(msg_str);
    write(loc, msg_str, msg_len);

    // Indicate that this message is an error and exit program
    if (error != 0) {
        exit(-1);
    }
}

struct ipc_struct {
    int  repeat_val;
    char data[BUFFER_SIZE];
};


#endif
