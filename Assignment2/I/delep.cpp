void delep(char *path)
{
    DIR *dirp = opendir("/proc");
    if (!dirp) {
        perror("opendir");
        exit(1);
    }

    std::set<int> pids;

    struct dirent *entry;
    char path[1024];
    while ((entry = readdir(dirp)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type != DT_DIR)
            continue;

        sprintf(path, "/proc/%s/fd", entry->d_name);
        DIR *fd_dirp = opendir(path);
        if (!fd_dirp)
            continue;

        struct dirent *fd_entry;
        char link[1024];
        while ((fd_entry = readdir(fd_dirp)) != NULL) {
            if (!strcmp(fd_entry->d_name, ".") || !strcmp(fd_entry->d_name, ".."))
                continue;

            sprintf(link, "/proc/%s/fd/%s", entry->d_name, fd_entry->d_name);
            char buf[1024];
            
            ssize_t len = readlink(link, buf, sizeof buf);
            if (len == -1)
                continue;
            buf[len] = '\0';

            if (!strcmp(buf, path)){
                printf("PID %s has the file open", entry->d_name);
                pids.insert(atoi(entry->d_name));
                char path[1024];
                sprintf(path, "/proc/%s/fdinfo/%s", entry->d_name, fd_entry->d_name);
                FILE *fptemp = fopen(path, "r");
                char line[1024];
                while(fgets(line, sizeof(line), fptemp)){
                    if(strcmp(strtok(line, ":"), "lock")==0){
                        printf("----%s", line);
                    }
                }
                fclose(fptemp);
                printf("\n");
            }
        }
        closedir(fd_dirp);
    }
    closedir(dirp);
    if(pids.size()==0)
        printf("No process has the file open\n");
    else
    {
        // kill all the pids using the file
        printf("Are you want to kill all the processes using the file? (y/n): ");
        char ch;
        ch = getchar();
        getchar();
        if(ch=='y' || ch=='Y' || ch=='\n')
        {
            for(auto it = pids.begin(); it!=pids.end(); it++){
                kill(*it, SIGKILL);
                printf("Killed process %d\n", *it);
            }
            int del = remove(path);
            if(del==0)
                printf("Deleted file %s\n", path);
            else
                printf("Error deleting file %s\n", path);
        }
        else
        {
            printf("Exiting...\n");
        }
    }
}
