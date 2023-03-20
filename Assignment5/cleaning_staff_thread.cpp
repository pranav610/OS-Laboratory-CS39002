#include "threads.hpp"

extern int X, Y, N;
extern vector<Room> rooms;
extern vector<Guest> guests;

void *cleaner(void *arg)
{
    while (1)
    {
        sem_wait(&cleaning);

        while (1)
        {
            vector<int> dirty_rooms;
            for (int i = 0; i < N; i++)
                if (rooms[i].guest_count == 2)
                    dirty_rooms.push_back(i);

            int room_clean = dirty_rooms[rand() % dirty_rooms.size()];

            if (sem_trywait(&sem_room[room_clean]) != -1)
                break;
        }
        
        pthread_cond_signal(&cond_room[room_clean]);
        sem_wait(&sem_rooms);
        pthread_mutex_lock(&room_lock[room_clean]);

        rooms[room_clean].guest_count = 0;

        int sleep_time = 0;
        for (auto &t : room[room_clean].times)
            sleep_time += t;

        sleep(sleep_time);

        rooms[room_clean].times.clear();

        pthread_mutex_unlock(&room_lock[room_clean]);
        sem_post(&sem_rooms);
        sem_post(&sem_room[room_clean]);
    }
}
