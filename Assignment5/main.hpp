#include <bits/stdc++.h>
#include <semaphore.h>
#include <pthread.h>

#define SLEEP_MIN 10
#define SLEEP_MAX 20
#define STAY_MIN 10
#define STAY_MAX 30

using namespace std;

void *guest(void *arg);
void *cleaner(void *arg);

class Room
{
    public:
        int room_id;
        int guest_id;
        bool is_clean;
        bool is_occupied;
        Room(int room_id)
        {
            this->room_id = room_id;
            this->guest_id = -1;
            this->is_clean = true;
            this->is_occupied = false;
        }
};


