#ifndef GOODMALLOC_HPP
#define GOODMALLOC_HPP

#include<bits/stdc++.h>
using namespace std;

const long MAXMEM = 100000000;

void createMem(uint32_t);

uint64_t createList(string, uint32_t);
ssize_t assignVal(string, int64_t, int);
ssize_t freeList(string);

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
    set<mem_block> blocks;
    stack<map<string, set<mem_block>::iterator>> scope_stack;
    
    Memory()
    {
        mem_start = NULL;
        mem_size = 0;
        blocks.clear();
    }

    set<mem_block>::iterator findHole(uint32_t, string);

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
/*

class Node{
    int val;
    Node* next;
    Node* prev;
    public:
    Node(int val){
        this->val = val;
        next = NULL;
        prev = NULL;
    }
    int getVal() const {
        return val;
    }
    Node* getNext() const {
        return next;
    }
    Node* getPrev() const {
        return prev;
    }
    void setNext(Node* next){
        this->next = next;
    }
    void setPrev(Node* prev){
        this->prev = prev;
    }
};

class List{
    Node* head , *tail;
    public:
    List(){
        head = NULL;
        tail = NULL;
    }
    void insert(int val){
        
    }
    void remove(int val){
        
    }

};

char* createMem(){
    char* mem = new char[MAXMEM];
    return mem;
}

int createList()
*/

#endif