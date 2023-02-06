#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 1000
using namespace std;

class Command
{
public:
    string command;
    vector<string> arguments;
    int input_fd, output_fd;
    string input_file, output_file;
    pid_t pid;

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
        /*
         * Need to parse the command string into the command and arguments
         * Should support-
         * 1. Run an external command e.g 'cc –o myprog myprog.c'
         * 2. Run an external command by redirecting standard input from a file './a.out < infile.txt'
         * 3. Run an external command by redirecting standard output to a file './a.out > outfile.txt'
         * 4. Combination of input and output redirection './a.out < infile.txt > outfile.txt'
         * 5. Run an external command in the background with possible input and output redirections './a.out < infile.txt > outfile.txt &'
         * 6. Run several external commands in the pipe mode 'cat abc.c | sort | more'
         */

        /* Need to do more improvisations here */

        // Parse the command string into the command and arguments
        stringstream ss(command);
        string arg;
        while (ss >> arg)
        {
            if (arg == "<")
            {
                ss >> input_file;
            }
            else if (arg == ">")
            {
                ss >> output_file;
            }
            else
            {
                arguments.push_back(arg);
            }
        }

        // Set the command to the first argument
        command = arguments[0];

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
            output_fd = open(output_file.c_str(), O_WRONLY | O_CREAT, 0644);
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
    // If the command is not found, print an error message
    // If the command is found, execute it
    char *args[MAX_ARGS];
    for (int i = 0; i < command.arguments.size(); i++)
    {
        args[i] = (char *)command.arguments[i].c_str();
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

void shell_prompt()
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
    cout << user << "@" << pcname << ":" << current_directory << "$ ";
    delete[] current_directory;
    delete[] pcname;
}
void read_command(string &command)
{
    // Read the command from the user
    getline(cin, command);

    // Remove the starting and ending spaces
    while (command[0] == ' ')
        command.erase(0, 1);
    while (command[command.length() - 1] == ' ')
        command.erase(command.length() - 1, 1);
}

int main()
{
    while (1)
    {
        shell_prompt();
        string command;
        read_command(command);
        if (command == "")
        {
            continue;
        }

        // Add to history - to be implemented

        try
        {
            bool is_background = false;
            if (command.back() == '&')
            {
                is_background = true;
                command.pop_back();
            }

            Command shell_command(command);

            // Check if the command is a built-in command
            if (shell_command.command == "exit")
            {
                exit(0);
            }
            else if (shell_command.command == "cd")
            {
                if (shell_command.arguments.size() == 2)
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
                cout<<current_directory<<endl;
                delete[] current_directory;
            }
            else
            {
                // If not, fork a child process and execute the command
                pid_t pid_child = fork();
                if (pid_child == 0)
                {
                    // Child process
                    // Execute the command
                    execute_command(shell_command, is_background);
                }
                else
                {
                    // Parent process
                    // Wait for the child process to finish
                    if (!is_background)
                    {
                        waitpid(pid_child, NULL, 0);
                    }
                }
            }
        }
        catch (const char *msg)
        {
            cerr << msg << endl;
        }
    }
    return 0;
}