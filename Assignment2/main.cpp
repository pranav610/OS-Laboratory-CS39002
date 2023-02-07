#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

pid_t background_pid = 0;
bool is_background_job = false;

void sigtstp_handler(int signal) {
    if (background_pid == 0) {
        std::cout << "No background jobs running" << std::endl;
        return;
    }
    std::cout << "Moving process " << background_pid << " to background" << std::endl;
    kill(background_pid, SIGSTOP);
    is_background_job = true;
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &sigtstp_handler;
    sigaction(SIGTSTP, &sa, nullptr);

    while (true) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) {
            // The user pressed Ctrl-Z at the shell prompt
            sigtstp_handler(SIGTSTP);
            continue;
        }

        // Split the line into arguments
        char *argv[64];
        int argc = 0;
        char *token = strtok(const_cast<char *>(line.c_str()), " ");
        while (token != nullptr) {
            argv[argc++] = token;
            token = strtok(nullptr, " ");
        }
        argv[argc] = nullptr;

        // Execute the command
        background_pid = fork();
        if (background_pid == 0) {
            execvp(argv[0], argv);
            std::cerr << "Error: failed to execute command\n";
            return 1;
        } else {
            int status;
            if (!is_background_job) {
                waitpid(background_pid, &status, 0);
            }
            is_background_job = false;
        }
    }
    return 0;
}
