#include "threads.hpp"

extern int X, Y, N;
extern vector<Room> rooms;
extern vector<int> priorities;
extern vector<sem_t> room_sems;
extern vector<sem_t> bin_room_sems;
extern sem_t stay_count_sem;
extern sem_t cleaning;
extern sem_t clean_start;
extern sem_t clean_end;

extern int stay_count;

void *guest(void *arg)
{
    int guest_id = (long)arg;
    while (sleep(rand() % (SLEEP_MAX - SLEEP_MIN + 1) + SLEEP_MIN) == 0)
    {
        int room_id = -1;
        for (auto &room : rooms)
        {
            if (room.guest_id == -1 && room.guest_count < 2)
            {
                if (sem_trywait(&room_sems[room.room_id - 1]) < 0)
                    continue;

                room_id = room.room_id;
                break;
            }
        }
        if (room_id == -1)
        {
            for (auto &room : rooms)
            {
                if (room.guest_id != -1 && priorities[guest_id - 1] > priorities[room.guest_id - 1] && room.guest_count < 2)
                {
                    sem_post(&bin_room_sems[room.room_id - 1]);
                    int x;
                    
                    while (1)
                    {
                        sem_getvalue(&room_sems[room.room_id - 1], &x);
                        if (x == 1)
                            break;
                    }

                    if (sem_trywait(&room_sems[room.room_id - 1]) < 0)
                        continue;

                    room_id = room.room_id;
                    break;
                }
            }
        }

        if (room_id == -1)
            continue;

        printf("Guest %d enters room %d\n", guest_id, room_id);
        fflush(stdout);

        sem_wait(&stay_count_sem);
        stay_count++;
        if (stay_count == 2 * N)
        {
            printf("Cleaning started\n");
            sem_post(&clean_start);
            sem_post(&clean_end);
            for (int i = 0; i < N; i++)
                sem_post(&cleaning);
            stay_count = 0;
        }
        sem_post(&stay_count_sem);

        rooms[room_id - 1].guest_id = guest_id;
        rooms[room_id - 1].guest_count++;
        int stay_time = rand() % (STAY_MAX - STAY_MIN + 1) + STAY_MIN;

        if (rooms[room_id - 1].guest_count == 1)
            rooms[room_id - 1].stay_time.first = stay_time;
        else if (rooms[room_id - 1].guest_count == 2)
            rooms[room_id - 1].stay_time.second = stay_time;

        // use cond variable to wait for sleep_time or come out if other thread signals
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += stay_time;
        sem_timedwait(&bin_room_sems[room_id - 1], &ts);

        rooms[room_id - 1].guest_id = -1;

        printf("Guest %d leaves room %d\n", guest_id, room_id);
        fflush(stdout);

        sem_post(&room_sems[room_id - 1]);
    }
    return NULL;
}