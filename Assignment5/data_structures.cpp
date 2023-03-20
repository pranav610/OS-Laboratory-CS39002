#include "data_structures.hpp"

Room::Room(int room_id) : room_id(room_id), guest_id(-1), guest_count(0), last_stay(0) {}
