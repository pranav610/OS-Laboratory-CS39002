#ifndef GOODMALLOC_HPP
#define GOODMALLOC_HPP

#include<bits/stdc++.h>

using namespace std;

const long MAXMEM = 100000000;

void createMem(uint32_t);

uint64_t createList(string, uint32_t);
ssize_t assignVal(string, int64_t, int);
int getVal(string, int64_t);
void freeElem(string);

typedef struct _mem_block{
    uint64_t base;
    uint32_t limit;
    string name;
    bool in_use;
    bool operator<(const struct _mem_block &a) const{
        return base < a.base;
    }
}mem_block;

class Memory{
    public:
    uint64_t mem_start;
    long mem_size;
    uint64_t mem_footprint;
    set<mem_block> blocks;
    stack<map<string, set<mem_block>::iterator>> scope_stack;
    map<string, set<mem_block>::iterator> global_scope;

    Memory()
    {
        mem_start = 0;
        mem_size = 0;
        blocks.clear();
    }

    set<mem_block>::iterator findHole(uint32_t, string);
    uint64_t memory_usage();

    void setMemStart(uint64_t mem_start){
        this->mem_start = mem_start;
        blocks.insert({mem_start,1,"_START_", true});
        blocks.insert({MAXMEM+mem_start,1,"_END_", true});
    }

    void setMemSize(long mem_size){
        this->mem_size = mem_size;
    }
};

class ListElement {
    public:
    int val;
    ListElement *next;
    ListElement *prev;
    ListElement(int val=0):val(val),next(NULL),prev(NULL){}
};

void startScope();
void endScope();
int memory_usage();

#endif