#include <vector>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <random>
#include <semaphore.h>

#include "threads.hpp"
#include "data_structures.hpp"

using namespace std;

int X, Y, N;
vector<pthread_t> guest_tids;
vector<pthread_t> cleaning_tids;
vector<Room> rooms;
vector<int> priorities;
vector<sem_t> bin_room_sems;
vector<sem_t> sig_room_sems;
sem_t stay_count_sem;
sem_t cleaning;
sem_t clean_start, clean_end;
int stay_count = 0;

int main()
{
    cout << "Enter number of rooms: ";
    cin >> N;
    cout << "Enter number of guests: ";
    cin >> Y;
    cout << "Enter number of cleaners: ";
    cin >> X;

    if (Y > N && N > X && X > 1)
    {
        FILE* fp = freopen("log.txt", "w", stdout);

        // create rooms
        for (int i = 0; i < N; i++)
            rooms.push_back(Room(i + 1));

        // randomize guest priorities woth distinct numbers from 1 to Y
        for (int i = 0; i < Y; i++)
            priorities.push_back(i + 1);

        // random shuffle
        auto rd = random_device{};
        auto rng = default_random_engine{rd()};
        shuffle(begin(priorities), end(priorities), rng);

        // print guest priorities
        cout << "Guest priorities..." << endl;
        for (int i = 0; i < Y; i++)
            cout << "Guest " << i + 1 << ": " << priorities[i] << endl;
        cout << endl;

        // create semaphores for rooms
        bin_room_sems.resize(N);
        for (int i = 0; i < N; i++)
            sem_init(&bin_room_sems[i], 0, 1);

        // create binary semaphores for rooms
        sig_room_sems.resize(N);
        for (int i = 0; i < N; i++)
            sem_init(&sig_room_sems[i], 0, 0);

        // create semaphore for stay count
        sem_init(&stay_count_sem, 0, 1);

        // create semaphore for cleaners
        sem_init(&cleaning, 0, 0);
        sem_init(&clean_start, 0, 0);
        sem_init(&clean_end, 0, 0);

        // create threads for guests and cleaners
        guest_tids.resize(Y);
        cleaning_tids.resize(X);

        for (int i = 0; i < Y; i++)
            pthread_create(&guest_tids[i], NULL, guest, (void *)((long)i + 1));
        for (int i = 0; i < X; i++)
            pthread_create(&cleaning_tids[i], NULL, cleaner, (void *)((long)i + 1));

        // wait for all threads to finish
        for (int i = 0; i < Y; i++)
            pthread_join(guest_tids[i], NULL);
        for (int i = 0; i < X; i++)
            pthread_join(cleaning_tids[i], NULL);

        // destroy semaphores
        for (int i = 0; i < N; i++)
            sem_destroy(&bin_room_sems[i]);
        sem_destroy(&cleaning);
        sem_destroy(&stay_count_sem);
        sem_destroy(&clean_start);
        sem_destroy(&clean_end);
        for(int i = 0; i < N; i++)
            sem_destroy(&sig_room_sems[i]);

        fclose(fp);
    }
    else
    {
        cout << "Invalid input" << endl;
    }

    return 0;
}