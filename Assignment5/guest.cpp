#include "threads.hpp"

extern int X, Y, N;
extern vector<Room> rooms;
extern vector<int> priorities;
extern vector<sem_t> room_sems;
extern vector<pthread_mutex_t> room_mutexes;
extern vector<pthread_cond_t> room_conds;
extern pthread_mutex_t count_mutex;
extern sem_t cleaning;

int Count = 0;

void print_rooms(){
    for(auto&room:rooms){
        cout<<"Room "<<room.room_id<<": ";
        cout<<"Guest "<<room.guest_id<<" ";
        cout<<"Guest Count "<<room.guest_count<<" ";
        cout<<"Stay Time "<<room.stay_time.first<<" "<<room.stay_time.second<<endl;
    }
}

void *guest(void *arg){
    int guest_id = (long)arg;
    while(sleep(rand() % (SLEEP_MAX - SLEEP_MIN + 1) + SLEEP_MIN)==0){
        int room_id = -1;
        for(auto&room:rooms){
            if(room.guest_id==-1&&room.guest_count<2){
                cout<<guest_id<<"Trying to enter room "<<room.room_id<<endl;
                sem_trywait(&room_sems[room.room_id-1]);
                if(errno==EAGAIN){
                    continue;
                }
                print_rooms();
                room_id = room.room_id;
                break;
            }
        }
        if(room_id==-1){
            for(auto&room:rooms){
                if(room.guest_id!=-1&&priorities[guest_id-1]>room.guest_id&&room.guest_count<2){
                    pthread_cond_signal(&room_conds[room.room_id-1]);
                    sem_trywait(&room_sems[room.room_id-1]);
                    if(errno==EAGAIN){
                        continue;
                    }
                    room_id = room.room_id;
                    break;
                }
            }
        }
        if(room_id==-1){
            continue;
        }

        pthread_mutex_lock(&room_mutexes[room_id-1]);
        cout<<"Guest "<<guest_id<<" enters room "<<room_id<<endl;
        pthread_mutex_lock(&count_mutex);
        Count++;
        if(Count==2*N){
            cout<<"Signal Cleaning"<<endl;
            for(int i=0;i<N;i++){
                sem_post(&cleaning);
            }
        }
        Count=0;
        pthread_mutex_unlock(&count_mutex);
        rooms[room_id-1].guest_id = guest_id;
        rooms[room_id-1].guest_count++;
        int stay_time = rand() % (STAY_MAX - STAY_MIN + 1) + STAY_MIN;
        if(rooms[room_id-1].guest_count==1){
            rooms[room_id-1].stay_time.first = stay_time;
        }
        else if(rooms[room_id-1].guest_count==2){
            rooms[room_id-1].stay_time.second = stay_time;
        }
        print_rooms();
        
        // use cond variable to wait for sleep_time or come out if other thread signals
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += stay_time;
        pthread_cond_timedwait(&room_conds[room_id-1], &room_mutexes[room_id-1], &ts);
        cout<<"Guest "<<guest_id<<" leaves room "<<room_id<<endl;
        rooms[room_id-1].guest_id = -1;
        print_rooms();
        pthread_mutex_unlock(&room_mutexes[room_id-1]);
        sem_post(&room_sems[room_id-1]);
    }

}