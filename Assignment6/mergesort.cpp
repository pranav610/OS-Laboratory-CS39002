#include "goodmalloc.hpp"

#define MAX_ARRAY_SIZE 5000

int calls = 0;

void MERGESORT(string s, int l, int r)
{
    // cout << "Args: " << s << " " << l << " " << r << endl;
    startScope();
    // printf("Call %d: l=%d r=%d\n",calls++,getVal(l,0),getVal(r,0));
    if(l>r){endScope();return;}

    int m = (l+r)/2;
    MERGESORT(s,l,m);
    MERGESORT(s,m+1,r);

    string temp="temp";
    createList(temp, r-l+1);

    int i=l, j=m+1,k=0;

    while(i < m+1 && j < r)
    {
        if(getVal(s,i) < getVal(s,j))
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

    while(i < m+1)
    {
        assignVal(temp, k, getVal(s, i));
        i++, k++;
    }

    while(j < r+1)
    {
        assignVal(temp, k, getVal(s, j));
        j++, k++;
    }

    j=0;
    while(j < r-l+1)
    {
        assignVal(s, l+j, getVal(temp, j));
        j++;
    }
    endScope();
}

int main()
{
    // Get the total memory footprint for the code

    struct rusage usage;
    struct timeval start, end;

    getrusage(RUSAGE_SELF, &usage);
    start = usage.ru_utime;

    // create memory of size 250 MB
    createMem(250e6);
    startScope();

    // create a doubly linked list of size 50000 elements

    string s = "mergesort";
    createList(s, MAX_ARRAY_SIZE);

    // assign random values to the list

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1000000);

    int i=0;
    while(i < MAX_ARRAY_SIZE){
        assignVal(s, i++, dis(gen));
    }

    // print the list
    // printf("The list is\n");
    // assignVal(i, 0, 0);
    // while(getVal(i, 0) < MAX_ARRAY_SIZE){
    //     printf("%d ", getVal(s, getVal(i, 0)));
    //     assignVal(i, 0, getVal(i, 0) + 1);
    // }
    // printf("\n");
    // call mergesort

    int l = 0, r = MAX_ARRAY_SIZE - 1;

    MERGESORT(s, l , r);

    // print the sorted list
    // printf("\nThe sorted list is\n");
    // assignVal(i, 0, 0);
    // while(getVal(i, 0) < MAX_ARRAY_SIZE){
    //     printf("%d ", getVal(s, getVal(i, 0)));
    //     assignVal(i, 0, getVal(i, 0) + 1);
    // }

    endScope();

    // end the timer
    getrusage(RUSAGE_SELF, &usage);
    end = usage.ru_utime;

    // Get the total time taken
    long seconds = (end.tv_sec - start.tv_sec);

    printf("\nWith FreeElem()\n");
    printf("Total memory footprint %ld \n", usage.ru_maxrss);
    printf("Total runtime %ld \n", seconds);

    return 0;
}