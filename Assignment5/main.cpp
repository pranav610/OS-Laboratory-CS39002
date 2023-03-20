#include <vector>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <random>

#include "threads.hpp"
#include "data_structures.hpp"

using namespace std;

int X, Y, N;
vector<pthread_t> guest_tids;
vector<pthread_t> cleaning_tids;
vector<Room> rooms;
vector<Guest> guests;

int main()
{   
    cout << "Enter number of rooms: "; cin >> N;
    cout << "Enter number of guests: "; cin >> Y;
    cout << "Enter number of cleaners: "; cin >> X;

    if(Y > N && N > X && X > 1)
    {   
        // create rooms
        for(int i=0; i<N; i++)
            rooms.push_back(Room(i+1));

        // create guests
        for(int i=0; i<Y; i++)
            guests.push_back(Guest(i+1));

        // randomize guest priorities woth distinct numbers from 1 to Y
        vector<int> priorities(Y);
        for(int i=0; i<Y; i++)
            priorities[i] = i+1;

        // random shuffle
        auto rd = random_device {}; 
        auto rng = default_random_engine { rd() };
        shuffle(begin(priorities), end(priorities), rng);

        // assign priorities to guests
        for(int i=0; i<Y; i++)
            guests[i].priority = priorities[i];

        // clear priorities vector
        priorities.clear();

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
    }
    else
    {
        cout << "Invalid input" << endl;
    }

    return 0;
    
}