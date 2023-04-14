#include "goodmalloc.hpp"


#define MAX_ARRAY_SIZE 50000

void MERGESORT(string s,string l,string r)
{
    if(getVal(l,0) == getVal(r,0))return;
    startScope();
    string m="m";
    struct ListElement *list = (struct ListElement *)createList(m, 1);
    assignVal(m, 0, (getVal(l,0)+getVal(r,0))/2);

    MERGESORT(s,l,m);
    MERGESORT(s,m,r);

    string temp="temp";
    struct ListElement *list = (struct ListElement *)createList(temp, getVal(r,0)-getVal(l,0)+1);

    string i="i";
    struct ListElement *list = (struct ListElement *)createList(i, 1);
    assignVal(i, 0, getVal(l,0));

    string j="j";
    struct ListElement *list = (struct ListElement *)createList(j, 1);
    assignVal(j, 0, getVal(m,0)+1);

    string k="k";
    struct ListElement *list = (struct ListElement *)createList(k, 1);
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

    // create memory of size 250 MB
    createMem(250e6);
    startScope();

    // create a doubly linked list of size 50000 elements

    string s = "mergesort";

    struct ListElement *list = (struct ListElement *)createList(s, MAX_ARRAY_SIZE);

    // assign random values to the list

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1000000);

    string i = "i";
    struct ListElement *list = (struct ListElement *)createList(i, 1);
    assignVal(i, 0, 0);

    while(getVal(i, 0) < MAX_ARRAY_SIZE){
        assignVal(s, getVal(i, 0), dis(gen));
        assignVal(i, 0, getVal(i, 0) + 1);
    }

    // print the list
    printf("The list is\n");
    assignVal(i, 0, 0);
    while(getVal(i, 0) < MAX_ARRAY_SIZE){
        printf("%d ", getVal(s, getVal(i, 0)));
        assignVal(i, 0, getVal(i, 0) + 1);
    }


    // call mergesort

    string l="l";
    string r="r";
    struct ListElement *list = (struct ListElement *)createList(l, 1);
    struct ListElement *list = (struct ListElement *)createList(r, 1);
    assignVal(l, 0, 0);
    assignVal(r, 0, MAX_ARRAY_SIZE - 1);

    MERGESORT(s, l , r);

    // print the sorted list
    printf("\nThe sorted list is\n");
    assignVal(i, 0, 0);
    while(getVal(i, 0) < MAX_ARRAY_SIZE){
        printf("%d ", getVal(s, getVal(i, 0)));
        assignVal(i, 0, getVal(i, 0) + 1);
    }

    endScope();
    return 0;
}