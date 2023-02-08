#ifndef __MALWARE_HPP
#define __MALWARE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <set>
#include <sys/types.h>
#include <signal.h>

using namespace std;

typedef map<string, string> statusMap;
typedef map<int, statusMap> pidMap;
#define SLEEP_DUR_MIN 2
#define NUM_CHILD 5
#define NUM_CHILD_CHILD 10

class squashbug
{
    public:
        squashbug(int, bool);
        ~squashbug();
        void run();
    private:
        pid_t pid;
        bool suggest;
        pidMap pidMap;
        int countChildren(pid_t);
        void returnChildren(pid_t, set<int>&);
};

#endif