#include "goodmalloc.hpp"


int main()
{
    createMem(3e8);
    startScope();
    string s = "hello";
    struct ListElement *list = (struct ListElement *)createList(s, 8);

    assignVal(s, 0, 1);
    assignVal(s, 1, 2);
    assignVal(s, -1, 3);
    assignVal(s, 7, 4);
    assignVal(s, 8, 4);

    printf("%d\n", list->val);
    printf("%d\n", list->next->val);
    printf("%d\n", list->next->next->val);
    printf("%d\n", list->next->next->next->val);
    printf("%d\n", list->next->next->next->next->val);
    printf("%d\n", list->next->next->next->next->next->val);
    printf("%d\n", list->next->next->next->next->next->next->val);
    printf("%d\n", list->next->next->next->next->next->next->next->val);

    
}