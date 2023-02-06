#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

#include <string>
#include <vector>

using namespace std;

class Shell_Command
{
public:
    string command;
    vector<string> arguments;
    int input_fd;
    int output_fd;
    string input_file;
    string output_file;
    pid_t pid;

    Shell_Command(const string &);
    ~Shell_Command();
    void parse_command();
    void IO_redirection();
    friend ostream &operator<<(ostream &, const Shell_Command &);
};

#endif