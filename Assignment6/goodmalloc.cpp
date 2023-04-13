#include "goodmalloc.hpp"

Memory MEM;

void createMem(uint32_t size)
{
    uint64_t mem = (uint64_t)malloc(size);
    if (mem == NULL)
    {
        printf("Error: malloc failed");
        exit(1);
    }
    MEM.setMemStart(mem);
    MEM.setMemSize(size);
}

ssize_t assignVal(string name, int64_t idx, int val)
{   
    if(idx < 0)
        return -1;

    stack<map<string, set<mem_block>::iterator>> temp = MEM.scope_stack;
    while(!temp.empty())
    {
        if(temp.top().find(name) != temp.top().end())
        {   
            // check if index is out of bounds
            if(idx*sizeof(ListElement) >= temp.top()[name]->limit)
                return -1;
            // loop through the list and assign the value
            ListElement *curr = (ListElement*)(temp.top()[name]->base);
            for(int i=0; i<idx; i++)
                curr = curr->next;
            curr->val = val;
            return 0;
        }
        
        temp.pop();
    }
    // if not found in any scope
    printf("Error: variable not found\n");
    return -1;
}


set<mem_block>::iterator Memory::findHole(uint32_t sz, string name)
{
    auto it = blocks.begin();
    auto it2 = ++it;
    it--;
    while(it2 != blocks.end())
    {
        if(it2->base - (it->base+it->limit) >= sz)
            break;
        it++;
        it2++;
    }
    if(it2 == blocks.end())
        return blocks.end();

    blocks.insert({it->base+it->limit, sz, name, true});
    return blocks.find({it->base+it->limit, sz, name, true});
}

void startScope()
{
    MEM.scope_stack.push(map<string, set<mem_block>::iterator>());
}

void endScope()
{   
    map<string, set<mem_block>::iterator> scope = MEM.scope_stack.top();
    for(auto it=scope.begin(); it!=scope.end(); it++)
    {
        freeList(it->first);
    }
    MEM.scope_stack.pop();
}

uint64_t createList(string name, uint32_t sz)
{
    if(sz==0)
        return (uint64_t)NULL;

    auto itr = MEM.findHole(sz*sizeof(ListElement), name);
    uint64_t addr = itr->base;
    if(itr == MEM.blocks.end())
        return (uint64_t)NULL;

    MEM.scope_stack.top()[name] = itr;
    if(sz==1)
        return addr;
    
    ListElement *next = (ListElement*)(addr+sizeof(ListElement)), *curr=(ListElement *)addr;
    curr->prev = NULL;
    for(uint32_t i=0; i<sz-1; i++)
    {
        curr->next = next;
        next->prev = curr;
        curr = next;
        next = (ListElement*)((uint64_t)next+sizeof(ListElement));
    }
    curr->next = NULL;

    return addr;
}


ssize_t freeList(string name="")
{
    if(name=="")
    {
        // free all entries in blocks which are not present in any scope
        set<mem_block> temp = MEM.blocks;
        for(auto it=temp.begin(); it!=temp.end(); it++)
        {
            if(it->name == "_START_" || it->name == "_END_")
                continue;
            if(!it->in_use)
                freeList(it->name);
        }
    }
    else
    {
        // free the entry which occurs first in the scope stack with name as key
        stack<map<string, set<mem_block>::iterator>> temp = MEM.scope_stack;
        while(!temp.empty())
        {
            if(temp.top().find(name) != temp.top().end())
            {
                MEM.blocks.erase(temp.top()[name]);
                return 0;
            }
            temp.pop();
        }
        // if not found in any scope
        printf("Error: variable not found\n");
        exit(EXIT_FAILURE);
    }
}