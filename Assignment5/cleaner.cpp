#include "threads.hpp"

extern int X, Y, N;
extern vector<Room> rooms;
extern sem_t cleaning;
extern vector<sem_t> room_sems;
extern vector<pthread_cond_t> room_conds;
extern vector<pthread_mutex_t> room_mutexes;

void *cleaner(void *arg)
{
    while (1)
    {
        sem_wait(&cleaning);
        int room_clean = -1;
        while (1)
        {
            vector<int> dirty_rooms;
            for (int i = 0; i < N; i++)
                if (rooms[i].guest_count == 2)
                    dirty_rooms.push_back(i);

            room_clean = dirty_rooms[rand() % dirty_rooms.size()];

            if (sem_trywait(&room_sems[room_clean]) != -1)
                break;
        }

        pthread_cond_signal(&room_conds[room_clean]);
        pthread_mutex_lock(&room_mutexes[room_clean]);

        rooms[room_clean].guest_count = 0;

        int sleep_time = rooms[room_clean].stay_time.first + rooms[room_clean].stay_time.second;

        sleep(sleep_time);

        printf("Room %d is cleaned\n", room_clean + 1);
        fflush(stdout);

        rooms[room_clean].stay_time = make_pair(0, 0);

        pthread_mutex_unlock(&room_mutexes[room_clean]);
        sem_post(&room_sems[room_clean]);
    }
}
