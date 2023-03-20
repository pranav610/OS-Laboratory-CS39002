#include "main.hpp"

int X, Y, N;
vector<pthread_t> guest_tids;
vector<pthread_t> cleaning_tids;

int main()
{
    cout << "Enter number of rooms: "; cin >> N;
    cout << "Enter number of guests: "; cin >> Y;
    cout << "Enter number of cleaners: "; cin >> X;

    if(Y > N && N > X && X > 1)
    {
        // create threads for guests and cleaners
        guest_tids.resize(Y);
        cleaning_tids.resize(X);             

        for(int i=0; i<Y; i++)
            pthread_create(&guest_tids[i], NULL, guest, (void*)i);
        for(int i=0; i<X; i++)
            pthread_create(&cleaning_tids[i], NULL, cleaner, (void*)i);
        
        // wait for all threads to finish
        for(int i=0; i<Y; i++)
            pthread_join(guest_tids[i], NULL);
        for(int i=0; i<X; i++)
            pthread_join(cleaning_tids[i], NULL);
    }
    else
    {
        cout << "Invalid input" << endl;
        return 0;
    }
    
}