#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <stdlib.h>

#define SLEEP_MIN 10
#define SLEEP_MAX 20
#define STAY_MIN 10
#define STAY_MAX 30

class Room
{   
    public:
        int room_id;
        int guest_id;
        int guest_count;
        int last_stay;
        Room(int);
};

class Guest
{
    public:
        int guest_id;
        int room_id;
        int stay_time;
        int priority;
        Guest(int);
};

#endif

