#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include<glob.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>

#include "delep.h"
#include "delep.cpp"

using namespace std;

static sigjmp_buf env;

size_t job_number = 1;

volatile bool is_background;
pid_t foreground_pid;
set<pid_t> background_pids;

class Command
{
public:
    string command;
    vector<string> arguments;
    int input_fd, output_fd;
    string input_file, output_file;
    pid_t pid;
    bool pipe_mode = false;

    Command(const string &cmd) : command(cmd), input_fd(STDIN_FILENO), output_fd(STDOUT_FILENO), input_file(""), output_file(""), pid(-1)
    {
        parse_command();
    }

    ~Command()
    {
        if (input_fd != STDIN_FILENO)
            close(input_fd);
        if (output_fd != STDOUT_FILENO)
            close(output_fd);
    }

    void parse_command()
    {
        /* Need to do more improvisations here */

        // Parse the command string into the command and arguments
        stringstream ss(command);
        string arg;
        string temp="";
        bool backslash = false;
        while (ss >> arg)
        {
            if (arg == "<")
            {
                ss >> input_file;
                backslash = false;
            }
            else if (arg == ">")
            {
                ss >> output_file;
                backslash = false;
            }else if (arg[arg.size()-1] == '\\')
            {
                temp = temp + arg;
                temp[temp.size()-1] = ' ';
                backslash = true;
                
            }
            else
            {
                if(backslash){
                    temp = temp  + arg;
                    arguments.push_back(temp);
                    temp="";
                    backslash = false;
                }else arguments.push_back(arg);
            }

        }

        // Set the command to the first argument
        command = arguments[0];

        // //Handle wildcards
        vector<string> temp_args;
        for (auto &arg : arguments)
        {
            if (arg.find('*') != string::npos || arg.find('?') != string::npos)
            {
                glob_t glob_result;
                glob(arg.c_str(), GLOB_TILDE, NULL, &glob_result);
                for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
                {
                    arg = glob_result.gl_pathv[i];
                    temp_args.push_back(arg);
                }

                globfree(&glob_result);
            }
            else
                temp_args.push_back(arg);
        }

        arguments.clear();
        arguments = temp_args;
        // Set the input and output file descriptors
        IO_redirection();
    }

    void IO_redirection()
    {
        // Open the input file for reading, if it exists
        if (!input_file.empty())
        {
            input_fd = open(input_file.c_str(), O_RDONLY);
            if (input_fd == -1)
            {
                cerr << "Error opening input file: " << input_file << endl;
                exit(EXIT_FAILURE);
            }
        }

        // Open the output file for writing, creating it if it does not exist
        if (!output_file.empty())
        {
            output_fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd == -1)
            {
                cerr << "Error opening output file: " << output_file << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
};

int execute_command(Command &command, bool background)
{
    // Execute the command
    // If the command is not found, print an error message
    // If the command is found, execute it
    char **args;
    args = new char *[command.arguments.size() + 1];
    for (int i = 0; i < (int)command.arguments.size(); i++)
    {
        args[i] = strdup(command.arguments[i].c_str());
    }
    args[command.arguments.size()] = NULL;

    // Redirect the input and output
    dup2(command.input_fd, STDIN_FILENO);
    dup2(command.output_fd, STDOUT_FILENO);

    // Execute the command
    int ret = execvp(command.command.c_str(), args);
    if (ret == -1)
    {
        cerr << "Error executing command: " << command.command << endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}

string shell_prompt()
{
    // Print the shell prompt in user@pcname:current_directory$
    char *user = getenv("USER");
    char *pcname = new char[1];
    size_t capacity = 1;
    while (gethostname(pcname, capacity) < 0)
    {
        delete[] pcname;
        capacity *= 2;
        pcname = new char[capacity];
    }
    char *current_directory = new char[1];
    capacity = 1;
    while (!(current_directory = getcwd(current_directory, capacity)))
    {
        delete[] current_directory;
        capacity *= 2;
        current_directory = new char[capacity];
    }
    string prompt = string(user) + "@" + string(pcname) + ":" + string(current_directory) + "$ ";
    delete[] current_directory;
    delete[] pcname;
    return prompt;
}

void delim_remove(string &command)
{
    // Remove the starting and ending spaces
    while (command[0] == ' ')
        command.erase(0, 1);
    while (command[command.length() - 1] == ' ')
        command.pop_back();
}

void ctrl_c_handler(int signum)
{
    if (foreground_pid == 0)
    {
        siglongjmp(env, 42);
    }
    cout << endl;
    kill(foreground_pid, SIGKILL);
    foreground_pid = 0;
}

void ctrl_z_handler(int signum)
{
    if (foreground_pid == 0)
    {
        siglongjmp(env, 42);
    }
    cout << endl
         << "[" << job_number++ << "]+ Stopped " << endl;
    kill(foreground_pid, SIGCONT);
    background_pids.insert(foreground_pid);
    foreground_pid = 0;
}

void child_signal_handler(int signum)
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0)
        background_pids.erase(pid);
}

int main()
{
    char *input;

    // Register the signal handlers
    struct sigaction sa_int;
    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = &ctrl_c_handler;
    sigaction(SIGINT, &sa_int, NULL);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &ctrl_z_handler;
    sigaction(SIGTSTP, &sa, NULL);

    struct sigaction sa_child;
    memset(&sa_child, 0, sizeof(sa_child));
    sa_child.sa_handler = &child_signal_handler;
    sigaction(SIGCHLD, &sa_child, NULL);

    while (1)
    {
        if (sigsetjmp(env, 1) == 42)
        {
            cout << endl;
            continue;
        }

        string prompt = shell_prompt();
        input = readline(prompt.c_str());

        // Exit the shell on CTRL+D
        if (input == NULL)
        {
            cout << "exit" << endl;
            exit(EXIT_SUCCESS);
        }

        string command = string(input);

        // Add the command to the history

        delim_remove(command);
        if (command == "")
        {
            continue;
        }
        vector<string> commands;
        int i = 0;
        while (i < (int)command.length())
        {
            if (command[i] == '|')
            {
                commands.push_back(command.substr(0, i));
                command.erase(0, i + 1);
                i = 0;
            }
            else
                i++;
        }
        delim_remove(command);
        if (command != "")
            commands.push_back(command);
        int pipefd[2];
        for (int i = 0; i < (int)commands.size(); i++)
        {
            try
            {
                delim_remove(commands[i]);
                const string cmd = commands[i];
                Command shell_command(cmd);

                is_background = false;
                if (shell_command.arguments[shell_command.arguments.size() - 1] == "&")
                {
                    is_background = true;
                    shell_command.arguments.pop_back();
                }

                // Pipe the output of one command to the input of the next
                if (i > 0)
                {
                    shell_command.input_fd = pipefd[0];
                }
                if (i < (int)commands.size() - 1)
                {
                    pipe(pipefd);
                    if (pipefd[0] == -1 || pipefd[1] == -1)
                    {
                        cerr << "Error creating pipe" << endl;
                        exit(EXIT_FAILURE);
                    }
                    shell_command.output_fd = pipefd[1];
                }

                // Check if the command is a built-in command
                if (shell_command.command == "exit")
                {
                    cout << "exit" << endl;
                    exit(EXIT_SUCCESS);
                }
                else if (shell_command.command == "cd")
                {
                    if (shell_command.arguments.size() == 1)
                    {
                        chdir(getenv("HOME"));
                    }
                    else if (shell_command.arguments.size() == 2)
                    {
                        chdir(shell_command.arguments[1].c_str());
                    }
                    else
                    {
                        cerr << "Invalid number of arguments" << endl;
                    }
                }
                else if (shell_command.command == "pwd")
                {
                    char *current_directory = new char[1];
                    size_t capacity = 1;
                    while (!(current_directory = getcwd(current_directory, capacity)))
                    {
                        delete[] current_directory;
                        capacity *= 2;
                        current_directory = new char[capacity];
                    }
                    write(shell_command.output_fd, current_directory, strlen(current_directory));
                    write(shell_command.output_fd, "\n", 1);
                    delete[] current_directory;
                }
                else
                {
                    // If not, fork a child process and execute the command
                    foreground_pid = fork();
                    if (foreground_pid == 0)
                    {
                        // Child process
                        // Execute the command
                        if (shell_command.command == "delep")
                        {
                            if (shell_command.arguments.size() == 2)
                            {
                                delep((char *)shell_command.arguments[1].c_str());
                            }
                            else
                            {
                                cerr << "Invalid number of arguments" << endl;
                            }
                        }
                        else
                            execute_command(shell_command, is_background);
                    }
                    else
                    {
                        // Parent process
                        if (is_background)
                        {
                            background_pids.insert(foreground_pid);
                        }
                        else
                        {
                            waitpid(foreground_pid, NULL, 0);
                        }
                        foreground_pid = 0;
                    }
                }
            }
            catch (const char *msg)
            {
                cerr << msg << endl;
            }
        }
    }
    return 0;
}