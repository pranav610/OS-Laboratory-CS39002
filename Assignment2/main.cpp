#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length of a command */

void sigint_handler(int signo);

int main(void) {
    char *args[MAX_LINE/2 + 1]; /* Command line arguments */
    int should_run = 1; /* Flag to determine when to exit program */
    pid_t pid;
    int background;

    signal(SIGINT, SIG_IGN);

    while (should_run) {
        background = 0;
        printf("osh>");
        fflush(stdout);

        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) if command included &, parent will not wait
         */

        char input[MAX_LINE];
        if (fgets(input, MAX_LINE, stdin) == NULL) {
            break;
        }
        int length = strlen(input);
        if (input[length - 1] == '\n') {
            input[length - 1] = '\0';
        }

        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (strcmp(args[i-1], "&") == 0) {
            background = 1;
            args[i-1] = NULL;
        }

        pid = fork();
        if (pid == 0) {
            /* Child process */
            if(!background)
            signal(SIGINT, SIG_DFL);
            if (execvp(args[0], args) == -1) {
                printf("Error executing command\n");
            }
            exit(0);
        } else if (pid > 0) {
            /* Parent process */
            if (!background) {
                wait(NULL);
            }
        } else {
            /* Error */
            printf("Error in fork\n");
        }
    }

    return 0;
}

void sigint_handler(int signo) {
    
}
