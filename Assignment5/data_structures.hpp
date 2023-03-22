#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <stdlib.h>
#include <utility>

using namespace std;

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
    pair<int, int> stay_time;
    Room(int);
};

#endif
