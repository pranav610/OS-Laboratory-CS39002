#include "squashbug.hpp"

squashbug::squashbug(int pid, bool suggest = false) : pid(pid), suggest(suggest)
{
    DIR *dirp = opendir("/proc");
    if (!dirp)
    {
        perror("Failed to open /proc");
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL)
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") || entry->d_type != DT_DIR)
            continue;

        char statusFile[300];
        sprintf(statusFile, "/proc/%s/status", entry->d_name);
        std::ifstream fileStream;
        fileStream.open(statusFile);
        std::string line;
        statusMap statusValues;
        while (std::getline(fileStream, line))
        {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, ':'))
            {
                std::string value;
                if (std::getline(is_line, value))
                    statusValues[key] = value;
            }
        }
        pidMap[pid] = statusValues;
    }
    closedir(dirp);
}

squashbug::~squashbug()
{
}

// void getstatus(pid_t pid, statusMap &statusValues)
// {
//     char statusFile[100];
//     sprintf(statusFile, "/proc/%d/status", pid);
//     std::ifstream fileStream;
//     fileStream.open(statusFile);
//     std::string line;
//     while (std::getline(fileStream, line))
//     {
//         std::istringstream is_line(line);
//         std::string key;
//         if (std::getline(is_line, key, ':'))
//         {
//             std::string value;
//             if (std::getline(is_line, value))
//                 statusValues[key] = value;
//         }
//     }
// }

void squashbug::returnChildren(pid_t pid, set<int> &pids)
{   
    for(auto itr = pidMap.begin(); itr != pidMap.end(); itr++)
    {
        if(atoi(itr->second["PPid"].c_str()) == pid)
        {
            pids.insert(itr->first);
            returnChildren(itr->first, pids);
        }
    }
}

int squashbug::countChildren(pid_t pid)
{
    int ret = 0;
    for(auto itr = pidMap.begin(); itr != pidMap.end(); itr++)
    {
        if(atoi(itr->second["PPid"].c_str()) == pid)
        {
            ret++;
            ret += countChildren(itr->first);
        }
    }
    return ret;
}

void squashbug::run()
{
    int count = 3;
    pid_t pids[3];

    pids[0] = pid;
    pids[1] = atoi(pidMap[pids[0]]["PPid"].c_str());
    if (pids[1] == 0)
        count--;
    else
        pids[2] = atoi(pidMap[pids[1]]["PPid"].c_str());


    printf("Process Tree:\n");
    for (int i = 0; i < 3; i++)
    {
        if (i == count)
        {
            printf("Process %d: ", i + 1);
            printf("PID: %d\n, ", pids[i]);
            break;
        }
        printf("Process %d: ", i + 1);
        printf("%s, \t", pidMap[pids[i]]["Name"].c_str());
        printf("PID: %d, \t", pids[i]);
        printf("State: %s, \t", pidMap[pids[i]]["State"].c_str());
        printf("Children: %d\n", countChildren(pids[i]));
    }

    if (suggest)
    {
        printf("Suggested Trojan PID is: ");
        pid_t suggestedPid = -1;

        string guessName = pidMap[pids[0]]["Name"];
        set<int> possPids;
        possPids.insert(0);
        for (int i = 0; i < count; i++)
        {
            if (pidMap[pids[i]]["Name"] == guessName)
                possPids.insert(pids[i]);
        }
        for (int i = 0; i < count; i++)
        {
            char statechar;
            int len = pidMap[pids[i]]["State"].length();
            for (int j = 0; j < len; j++)
            {
                if (pidMap[pids[i]]["State"][j] >= 'A' && pidMap[pids[i]]["State"][j] <= 'Z')
                {
                    statechar = pidMap[pids[i]]["State"][j];
                    break;
                }
            }
            if (statechar == 'S' && possPids.find(pids[i]) != possPids.end())
                suggestedPid = pids[i];
        }

        if (suggestedPid == -1)
        {
            pid_t maxChildrenPID = *possPids.begin();
            int maxChildren = 0;
            for (int i = 0; i < count; i++)
            {
                if (possPids.find(pids[i]) != possPids.end())
                {
                    int children = countChildren(pids[i]);
                    if (children > maxChildren)
                    {
                        maxChildren = children;
                        maxChildrenPID = pids[i];
                    }
                }
            }
            suggestedPid = maxChildrenPID;
        }

        printf("%d\n", suggestedPid);
        printf("Do you want to kill this process and all its children? (y/n): ");
        char c;
        scanf("%c", &c);
        if (c == 'y')
        {
            set<int> children;
            returnChildren(suggestedPid, children);
            for (auto it = children.begin(); it != children.end(); it++)
            {
                kill(*it, SIGKILL);
            }
            kill(suggestedPid, SIGKILL);
            printf("Killed.\n");
        }
        printf("Done.\n");
    }
}