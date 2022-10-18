#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <string.h>
int ip; // Input Pipe
int op; // Output Pipe

/**
 * Closes all opened files
 */
void closeAll();

int getBufferSize(const char *buff);

int main() {
    const char *inputPipe = "input";
    const char *outputPipe = "output";

    ip = open(inputPipe, O_RDONLY|O_NONBLOCK);
    if (ip < 0) {
        perror("open input");
        return EXIT_FAILURE;
    }

    op = open(outputPipe, O_WRONLY|O_NONBLOCK);
    if (op < 0) {
        perror("open output");
        return EXIT_FAILURE;
    }

    int fds[3] = {ip, STDIN_FILENO, op};

    struct pollfd pollSet[3];

    pollSet[0].fd = fds[0];
    pollSet[0].events = POLLIN;

    pollSet[1].fd = fds[1];
    pollSet[1].events = POLLIN;

    pollSet[2].fd = fds[2];

    char buff[2][255];
    size_t len;

    while (1) {
        if (poll(pollSet, 3, -1) < 0) {
            perror("poll");
            return  EXIT_FAILURE;
        }

        for (int i = 0; i < 3; ++i) {
            const short *revents = &pollSet[i].revents;

            if (*revents & POLLHUP) {
                closeAll();
                printf("Pipe %d has been closed!\n", i + 1);
                return EXIT_SUCCESS;
            } else if (*revents & (POLLERR | POLLNVAL)) {
                printf("Error\n");
                return EXIT_FAILURE;
            }
        }

        for (int i = 0; i < 2; i++) {
            const short *revents = &pollSet[i].revents;

            if (*revents & POLLIN) {
                len = read(fds[i], &(buff[i]), 255);
                if (len > 0) {
                    const int pid = fork();
                    if (pid < 0) {
                        perror("fork");
                        return EXIT_FAILURE;
                    }
                    else if (pid == 0) {
                        write(i == 0 ? STDOUT_FILENO : op, buff[i], getBufferSize(buff[i]));
                        return EXIT_SUCCESS;
                    }
                }
            }
        }
    }

    closeAll();
    return 0;
}

int getBufferSize(const char *buff) {
    const char *nPos = index(buff, '\n');
    const int buffLen = nPos == NULL ? 255 : ((int)(nPos - &buff[0])) + 1;
    return buffLen;
}

void closeAll() {
    close(ip);
    close(op);
}
