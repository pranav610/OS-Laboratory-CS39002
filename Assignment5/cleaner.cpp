#include "threads.hpp"

extern int X, Y, N;
extern vector<Room> rooms;
extern sem_t cleaning;
extern sem_t clean_start;
extern sem_t clean_end;
extern vector<sem_t> bin_room_sems;
extern vector<sem_t> sig_room_sems;

void *cleaner(void *arg)
{
    while (1)
    {
        sem_wait(&cleaning);
        int room_clean = -1;
        if (sem_trywait(&clean_start) != -1)
            for (int i = 0; i < N; i++)
                sem_post(&sig_room_sems[i]);

        while (1)
        {
            vector<int> dirty_rooms;
            for (int i = 0; i < N; i++)
                if (rooms[i].guest_count == 2)
                    dirty_rooms.push_back(i);

            room_clean = dirty_rooms[rand() % dirty_rooms.size()];

            int x;
            sem_getvalue(&bin_room_sems[room_clean], &x);
            if (x == 1)
                if (sem_trywait(&bin_room_sems[room_clean]) != -1)
                    break;
        }
        
        rooms[room_clean].guest_count = -1;

        int sleep_time = rooms[room_clean].stay_time.first + rooms[room_clean].stay_time.second;

        sleep(sleep_time);
    
        printf("Room %d is cleaned\n", room_clean + 1);
        fflush(stdout);

        rooms[room_clean].stay_time = make_pair(0, 0);

        rooms[room_clean].guest_count = 0;

        bool all_clean = true;
        for (int i = 0; i < N; i++)
            if (rooms[i].guest_count != 0)
                all_clean = false;

        if (all_clean)
            if (sem_trywait(&clean_end) != -1)
                for (int i = 0; i < N; i++)
                    sem_post(&bin_room_sems[i]);
    }
}
