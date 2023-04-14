#include "goodmalloc.hpp"

#define MAX_ARRAY_SIZE 5000

int calls = 0;

void MERGESORT(string s,string l,string r)
{
    // cout << "Args: " << s << " " << l << " " << r << endl;
    startScope();
    // printf("Call %d: l=%d r=%d\n",calls++,getVal(l,0),getVal(r,0));
    if(getVal(l,0) >= getVal(r,0)){endScope();return;}

    string m="m";
    createList(m, 1);
    assignVal(m, 0, (getVal(l,0)+getVal(r,0))/2);
    // string r = "r";
    createList(r, 1);
    assignVal(r, 0, getVal(m,0));
    MERGESORT(s,l,r);
    freeList(r);

    string m_="m_";
    createList(m_, 1);
    assignVal(m_, 0, getVal(m,0)+1);
    // string l = "l";
    createList(l, 1);
    assignVal(l, 0, getVal(m,0)+1);
    MERGESORT(s,l,r);
    freeList(l);

    string temp="temp";
    createList(temp, getVal(r,0)-getVal(l,0)+1);

    string i="i";
    createList(i, 1);
    assignVal(i, 0, getVal(l,0));

    string j="j";
    createList(j, 1);
    assignVal(j, 0, getVal(m,0)+1);

    string k="k";
    createList(k, 1);
    assignVal(k, 0, 0);

    while(getVal(i,0) < getVal(m,0)+1 && getVal(j,0) < getVal(r,0)+1)
    {
        if(getVal(s,getVal(i,0)) < getVal(s,getVal(j,0)))
        {
            assignVal(temp, getVal(k,0), getVal(s,getVal(i,0)));
            assignVal(i, 0, getVal(i,0)+1);
        }
        else
        {
            assignVal(temp, getVal(k,0), getVal(s,getVal(j,0)));
            assignVal(j, 0, getVal(j,0)+1);
        }
        assignVal(k, 0, getVal(k,0)+1);
    }

    while(getVal(i,0) < getVal(m,0)+1)
    {
        assignVal(temp, getVal(k,0), getVal(s,getVal(i,0)));
        assignVal(i, 0, getVal(i,0)+1);
        assignVal(k, 0, getVal(k,0)+1);
    }

    while(getVal(j,0) < getVal(r,0)+1)
    {
        assignVal(temp, getVal(k,0), getVal(s,getVal(j,0)));
        assignVal(j, 0, getVal(j,0)+1);
        assignVal(k, 0, getVal(k,0)+1);
    }

    assignVal(j, 0, 0);
    while(getVal(j,0) < getVal(r,0)-getVal(l,0)+1)
    {
        assignVal(s, getVal(l,0)+getVal(j,0), getVal(temp,getVal(j,0)));
        assignVal(j, 0, getVal(j,0)+1);
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

    string i = "i";
    createList(i, 1);
    assignVal(i, 0, 0);

    while(getVal(i, 0) < MAX_ARRAY_SIZE){
        assignVal(s, getVal(i, 0), dis(gen));
        assignVal(i, 0, getVal(i, 0) + 1);
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

    string l="l";
    string r="r";
    createList(l, 1);
    createList(r, 1);
    assignVal(l, 0, 0);
    assignVal(r, 0, MAX_ARRAY_SIZE - 1);


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