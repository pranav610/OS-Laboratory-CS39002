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

    // check if name present in current scope
    if(MEM.scope_stack.top().find(name) != MEM.scope_stack.top().end())
    {   
        // check if index is out of bounds
        if(idx*sizeof(ListElement) >= MEM.scope_stack.top()[name]->limit)
            return -1;
        // loop through the list and assign the value
        ListElement *curr = (ListElement*)(MEM.scope_stack.top()[name]->base + idx*sizeof(ListElement));
        // for(int i=0; i<idx; i++)
        //     curr = curr->next;
        curr->val = val;
        return 0;
    }
    // check if name present in global scope
    if(MEM.global_scope.find(name) != MEM.global_scope.end())
    {   
        // check if index is out of bounds
        if(idx*sizeof(ListElement) >= MEM.global_scope[name]->limit)
            return -1;
        // loop through the list and assign the value
        ListElement *curr = (ListElement*)(MEM.global_scope[name]->base + idx*sizeof(ListElement));
        // for(int i=0; i<idx; i++)
        //     curr = curr->next;
        curr->val = val;
        return 0;
    }
    // if not found in any scope
    printf("Error: variable not found\n");
    return -1;
}

int getVal(string name, int64_t idx)
{
    if(idx<-1)
        return INT_MIN;

    // check if name present in current scope 
    if(MEM.scope_stack.top().find(name) != MEM.scope_stack.top().end())
    {   
        // check if index is out of bounds
        if(idx*sizeof(ListElement) >= MEM.scope_stack.top()[name]->limit)
            return INT_MIN;
        // loop through the list and assign the value
        ListElement *curr = (ListElement*)(MEM.scope_stack.top()[name]->base + idx*sizeof(ListElement));
        // for(int i=0; i<idx; i++)
        //     curr = curr->next;
        return curr->val;
    }
    // check if name present in global scope
    if(MEM.global_scope.find(name) != MEM.global_scope.end())
    {   
        // check if index is out of bounds
        if(idx*sizeof(ListElement) >= MEM.global_scope[name]->limit)
            return INT_MIN;
        // loop through the list and assign the value
        ListElement *curr = (ListElement*)(MEM.global_scope[name]->base + idx*sizeof(ListElement));
        // for(int i=0; i<idx; i++)
        //     curr = curr->next;
        return curr->val;
    }
    // if not found in any scope
    printf("Error: variable not found\n");
    return INT_MIN;
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
    // for(auto it=scope.begin(); it!=scope.end(); it++)
    // {
    //     MEM.blocks.erase(it->second);
    //     MEM.blocks.insert({it->second->base, it->second->limit, it->second->name, false});
    // }
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
    if((int)MEM.scope_stack.size() == 1)
        MEM.global_scope[name] = itr;
    MEM.scope_stack.top()[name] = itr;
    if(sz==1)
        return addr;
    
    // ListElement *next = (ListElement*)(addr+sizeof(ListElement)), *curr=(ListElement *)addr;
    // curr->prev = NULL;
    // for(uint32_t i=0; i<sz-1; i++)
    // {
    //     curr->next = next;
    //     next->prev = curr;
    //     curr = next;
    //     next = (ListElement*)((uint64_t)next+sizeof(ListElement));
    // }
    // curr->next = NULL;

    return addr;
}


ssize_t freeList(string name="")
{
    if(name=="")
    {
        // free all entries in MEM block which are not in use
        for(auto it=MEM.blocks.begin(); it!=MEM.blocks.end(); it++)
        {
            if(!it->in_use)
            {
                it = MEM.blocks.erase(it);
                it--;
            }
        }
    }
    else
    {
        // free the entry which occurs in the top scope
        if(MEM.scope_stack.top().find(name) != MEM.scope_stack.top().end())
        {
            MEM.blocks.erase(MEM.scope_stack.top()[name]);
            MEM.scope_stack.top().erase(name);
            return 0;
        }
        printf("Error: variable not found\n");
        exit(EXIT_FAILURE);
    }
}