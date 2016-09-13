#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <sys/stat.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define BUFFER_SIZE 256

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

int main(int argc, char** argv)
{
    char *src_file, *dst_file;
    char data_buf[BUFFER_SIZE];

    // Additional vars
    int src_desc, dst_desc;
    int num_bytes;
    struct stat src_stat;

    // verify the user has supplied the correct number of arguments
    // and assign source and destination file names
    if (argc != 3) {
        // write(STDERR, "Error: Incorrect number of arguments\n", 37);
        // exit(-1);
        write_msg(STDERR, "Error: Incorrect number of arguments\n", 1);
    } else {

        // Save file paths to vars (argv[0] == program name)
        src_file = argv[1];
        dst_file = argv[2];
    }

    // Attempt to open the src_file
    src_desc = open(src_file, O_RDONLY);
    if (src_desc < 0) {
        write_msg(STDERR, "Failed to open source file\n", 1);
    }

    // Attempt to get the status of src_file (including file permissions)
    if (fstat(src_desc, &src_stat) < 0) {
        write_msg(STDERR, "Could not check status of source file\n", 1);
    }

    // Attempt to "open" dst_file. In reality, attempt to create the
    // dst_file with the same permissions as the src_file if it does not yet
    // exist. In this case, the file must not exist yet to succeed.
    dst_desc = open(dst_file, O_WRONLY | O_CREAT | O_EXCL, src_stat.st_mode);
    if (dst_desc < 0) {
        write_msg(STDERR, "Failed to create destination file\n", 1);
    }

    // Report what's happening (using system calls!)
    // Note: Couldn't find a way to pass a parameterized string to "write"
    // i.e.: "Copying %s to %s"
    write_msg(STDOUT, "Copying ", 0);
    write_msg(STDOUT, src_file, 0);
    write_msg(STDOUT, " to ", 0);
    write_msg(STDOUT, dst_file, 0);
    write_msg(STDOUT, "\n", 0);

    // Copy src_file to dst_file BUFFER_SIZE bytes at a time
    do {
        num_bytes = read(src_desc, data_buf, BUFFER_SIZE);
        if (num_bytes < 0) {
            write_msg(STDERR, "Error while reading source file\n", 1);
        }
        if (write(dst_desc, data_buf, num_bytes) < 0) {
            write_msg(STDERR, "Error while writing to destination file\n", 1);
        }
    } while (num_bytes == BUFFER_SIZE);
    // Fails when bytes remaing < BUFFER_SIZE (EOF)

    // Attempt to close files
    if (close(src_desc) < 0) {
        write_msg(STDERR, "Error closing source file\n", 1);
    }
    if (close(dst_desc) < 0) {
        write_msg(STDERR, "Error closing destination file\n", 1);
    }

    return 0;
}
