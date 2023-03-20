#include "main.hpp"

int X, Y, N;
vector<pthread_t> guest_tids;
vector<pthread_t> cleaning_tids;

int main()
{
    cout << "Enter number of rooms: "; cin >> Y;
    cout << "Enter number of guests: "; cin >> N;
    cout << "Enter number of cleaners: "; cin >> X;

    // create threads for guests and cleaners
    guest_tids.resize(N);
    cleaning_tids.resize(X);             

    for(int i=0; i<N; i++)
        pthread_create(&guest_tids[i], NULL, guest, (void*)i);
    for(int i=0; i<X; i++)
        pthread_create(&cleaning_tids[i], NULL, cleaner, (void*)i);
    
    // wait for all threads to finish
    for(int i=0; i<N; i++)
        pthread_join(guest_tids[i], NULL);
    for(int i=0; i<X; i++)
        pthread_join(cleaning_tids[i], NULL);
    
}