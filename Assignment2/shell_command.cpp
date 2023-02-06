#include "shell_command.h"
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
     * Need more updates here to handle cases
    */
    // Split the command into words using a stringstream
    stringstream ss(command);
    string word;
    while (ss >> word)
    {
        // Check if the word is an input/output redirection symbol
        if (word == "<")
        {
            ss >> input_file;
        }
        else if (word == ">")
        {
            ss >> output_file;
        }
        else
        {
            arguments.push_back(word);
        }
    }

    // The first word in the arguments list is the command to be executed
    command = arguments[0];
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