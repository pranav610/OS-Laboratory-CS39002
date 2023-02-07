#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>

int main() {
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    setsid();
    execl("./main", "main", nullptr);
  } else {
    // Parent process
    int status;
    waitpid(pid, &status, WNOHANG);
    tcsetpgrp(0, getpid());
    kill(pid, SIGTSTP);
    tcsetpgrp(0, pid);
  }
  return 0;
}
