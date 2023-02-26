#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <iostream>
#include <string.h>

using namespace std;

#define CONSUMER_COUNT 10

int main(int argc, char const *argv[])
{
    key_t unique_key = ftok("./shared_memory_file", 15);

    if (unique_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    string graph = "";
    FILE *fp = fopen("my_graph.txt", "r");

    while (1)
    {
        char c = fgetc(fp);
        if (c == EOF)
            break;

        graph += c;
    }

    int shmid = shmget(unique_key, graph.length() + 5, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    char *str = (char *)shmat(shmid, (void *)0, 0);

    memcpy(str, graph.c_str(), graph.length());

    if (fork() == 0)
    {
        execl("./producer", "./producer", NULL);
        exit(EXIT_SUCCESS);
    }
    else
    {
        sleep(2);
        for (int i = 0; i < CONSUMER_COUNT; ++i)
        {
            if (fork() == 0)
            {
                if(argc > 1)
                    execl("./consumer", "./consumer", to_string(i).c_str(), argv[1], NULL);
                else
                    execl("./consumer", "./consumer", to_string(i).c_str(), NULL);
                exit(EXIT_SUCCESS);
            }
        }
    }

    for(int i = 0; i < CONSUMER_COUNT + 1; ++i)
        wait(NULL);
    wait(NULL);

    shmid = shmget(unique_key, 0, 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    str = (char *)shmat(shmid, (void *)0, 0);
    printf("%s\n", str);

    shmdt(str);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
