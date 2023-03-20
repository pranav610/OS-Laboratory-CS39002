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
vector<sem_t> room_sems;
vector<pthread_mutex_t> room_mutexes;
vector<pthread_cond_t> room_conds;
pthread_mutex_t count_mutex;
sem_t cleaning;

int main()
{   
    cout << "Enter number of rooms: "; cin >> N;
    cout << "Enter number of guests: "; cin >> Y;
    cout << "Enter number of cleaners: "; cin >> X;

    if(Y > N && N > X && X > 1)
    {
        // initialize semaphore

        // create rooms
        for(int i=0; i<N; i++)
            rooms.push_back(Room(i+1));

        // randomize guest priorities woth distinct numbers from 1 to Y
        for(int i=0; i<Y; i++)
            priorities.push_back(i+1);

        // random shuffle
        auto rd = random_device {}; 
        auto rng = default_random_engine { rd() };
        shuffle(begin(priorities), end(priorities), rng);

        for(auto X: priorities)
            cout << X << " ";
        cout << endl;

        // create semaphores for rooms
        room_sems.resize(N);
        for(int i=0; i<N; i++)
            sem_init(&room_sems[i], 0, 1);

        // create mutexes for rooms
        room_mutexes.resize(N);
        for(int i=0; i<N; i++)
            pthread_mutex_init(&room_mutexes[i], NULL);

        // create conditions for rooms
        room_conds.resize(N);
        for(int i=0; i<N; i++)
            pthread_cond_init(&room_conds[i], NULL);

        // create mutex for counting
        pthread_mutex_init(&count_mutex, NULL);

        // create semaphore for cleaners
        sem_init(&cleaning, 0, 0);


        // create threads for guests and cleaners
        guest_tids.resize(Y);
        cleaning_tids.resize(X);       

        for(int i=0; i<Y; i++)
            pthread_create(&guest_tids[i], NULL, guest, (void*)(i+1));
        for(int i=0; i<X; i++)
            pthread_create(&cleaning_tids[i], NULL, cleaner, (void*)(i+1));
        
        // wait for all threads to finish
        for(int i=0; i<Y; i++)
            pthread_join(guest_tids[i], NULL);
        for(int i=0; i<X; i++)
            pthread_join(cleaning_tids[i], NULL);

        // destroy semaphores
        for(int i=0; i<N; i++)
            sem_destroy(&room_sems[i]);
        sem_destroy(&cleaning);

        // destroy mutexes
        for(int i=0; i<N; i++)
            pthread_mutex_destroy(&room_mutexes[i]);
        pthread_mutex_destroy(&count_mutex);

        // destroy conditions
        for(int i=0; i<N; i++)
            pthread_cond_destroy(&room_conds[i]);
    }
    else
    {
        cout << "Invalid input" << endl;
    }

    return 0;
    
}