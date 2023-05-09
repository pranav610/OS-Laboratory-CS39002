#include "goodmalloc.hpp"
#include <sys/time.h>
#include <sys/resource.h>

#define MAX_LIST_SIZE 50000

bool use_free = false;
int memory_footprint = 0;
extern Memory MEM;

void MERGESORT(string s, int l, int r)
{
    startScope();

    if (l >= r)
    {
        endScope();
        return;
    }

    int m = (l + r) / 2;
    MERGESORT(s, l, m);
    MERGESORT(s, m + 1, r);

    string temp = "temp";
    createList(temp, r - l + 1);

    int i = l, j = m + 1, k = 0;

    while (i < m + 1 && j < r)
    {
        if (getVal(s, i) < getVal(s, j))
        {
            assignVal(temp, k, getVal(s, i));
            i++;
        }
        else
        {
            assignVal(temp, k, getVal(s, j));
            j++;
        }
        k++;
    }

    while (i < m + 1)
    {
        assignVal(temp, k, getVal(s, i));
        i++, k++;
    }

    while (j < r + 1)
    {
        assignVal(temp, k, getVal(s, j));
        j++, k++;
    }

    j = 0;
    while (j < r - l + 1)
    {
        assignVal(s, l + j, getVal(temp, j));
        j++;
    }
    if (use_free)
        freeElem(temp);
    endScope();
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: %s <--free/--nofree>" << endl;
        return 0;
    }
    if (strcmp(argv[1], "--free") == 0)
    {
        use_free = true;
    }
    else if (strcmp(argv[1], "--nofree") != 0)
    {
        cout << "Usage: %s <--free/--nofree>" << endl;
        return 0;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    // create memory of size 250 MB
    createMem(250e6);
    startScope();

    // create a doubly linked list of size 50000 elements
    string s = "mergesort";
    createList(s, MAX_LIST_SIZE);

    // assign random values to the list
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1000000);

    int i = 0;
    while (i < MAX_LIST_SIZE)
    {
        assignVal(s, i++, dis(gen));
    }

    // print the list
    // printf("The list is\n");
    // assignVal(i, 0, 0);
    // while(getVal(i, 0) < MAX_LIST_SIZE){
    //     cout << getVal(s, getVal(i, 0)) << " ";
    //     assignVal(i, 0, getVal(i, 0) + 1);
    // }
    // cout << endl;

    int l = 0, r = MAX_LIST_SIZE - 1;

    // call mergesort
    MERGESORT(s, l, r);

    // print the sorted list
    // printf("\nThe sorted list is\n");
    // assignVal(i, 0, 0);
    // while(getVal(i, 0) < MAX_LIST_SIZE){
    //     cout << getVal(s, getVal(i, 0)) << " ";
    //     assignVal(i, 0, getVal(i, 0) + 1);
    // }
    // cout << endl;

    if (use_free)
        freeElem(s);
    endScope();

    // Get the total time taken
    gettimeofday(&end, NULL);
    double ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

    // print the memory footprint and time taken stats
    if (use_free)
        cout << "--------------------------------------With FreeElem()--------------------------------------" << endl;
    else
        cout << "------------------------------------Without FreeElem()-------------------------------------" << endl;
    cout << "Total runtime " << ms << " milliseconds" << endl;
    cout << "Total memory footprint " << MEM.mem_footprint / 1e6 << " MB" << endl;
    cout << "-------------------------------------------------------------------------------------------" << endl;

    return 0;
}