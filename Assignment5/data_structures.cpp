#include "data_structures.hpp"

Room::Room(int room_id) : room_id(room_id), guest_id(-1), guest_count(0), stay_time(make_pair(0,0)) {}
