#include "shell_command.h"
#include "shell_exception.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>

using namespace std;

Shell_Command::Shell_Command(const string &cmd) : command(cmd), input_fd(STDIN_FILENO), output_fd(STDOUT_FILENO), input_file(""), output_file(""), pid(-1)
{
    parse_command();
}

Shell_Command::~Shell_Command()
{
    if (input_fd != STDIN_FILENO)
        close(input_fd);
    if (output_fd != STDOUT_FILENO)
        close(output_fd);
}

void Shell_Command::parse_command()
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
        else if (arg == "&")
        {
            // Handle background process here
        }
        else
        {
            arguments.push_back(arg);
        }
    }

    // Set the command to the first argument
    command = arguments[0];

    // Remove the command from the arguments
    arguments.erase(arguments.begin());

}

void Shell_Command::IO_redirection()
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

ostream &operator<<(ostream &os, const Shell_Command &cmd)
{
    os << "Command: " << cmd.command << endl;
    os << "Arguments: ";
    for (int i = 0; i < cmd.arguments.size(); i++)
        os << cmd.arguments[i] << " ";
    os << endl;
    os << "Input file: " << cmd.input_file << endl;
    os << "Output file: " << cmd.output_file << endl;
    return os;
}