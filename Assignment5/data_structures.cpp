#include "data_structures.hpp"

Room::Room(int room_id) : room_id(room_id), guest_id(-1), guest_count(0), last_stay(0) {}
Guest::Guest(int guest_id) : guest_id(guest_id), room_id(-1), stay_time(0), priority(guest_id) {}
