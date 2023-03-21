#include "threads.hpp"

extern int X, Y, N;
extern vector<Room> rooms;
extern vector<int> priorities;
extern vector<sem_t> room_sems;
extern vector<pthread_mutex_t> room_mutexes;
extern vector<pthread_cond_t> room_conds;
extern sem_t stay_count_sem;
extern sem_t cleaning;
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
                    pthread_cond_signal(&room_conds[room.room_id - 1]);
                    if (sem_trywait(&room_sems[room.room_id - 1]) < 0)
                        continue;
                    room_id = room.room_id;
                    break;
                }
            }
        }
        if (room_id == -1)
            continue;
        
        pthread_mutex_lock(&room_mutexes[room_id - 1]);

        printf("Guest %d enters room %d\n", guest_id, room_id);
        fflush(stdout);

        sem_wait(&stay_count_sem);
        stay_count++;
        if (stay_count == 2 * N)
        {
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
        pthread_cond_timedwait(&room_conds[room_id - 1], &room_mutexes[room_id - 1], &ts);

        rooms[room_id - 1].guest_id = -1;

        printf("Guest %d leaves room %d\n", guest_id, room_id);
        fflush(stdout);

        pthread_mutex_unlock(&room_mutexes[room_id - 1]);
        sem_post(&room_sems[room_id - 1]);
    }
    return NULL;
}