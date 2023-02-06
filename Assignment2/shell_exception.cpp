#include "shell_exception.h"
#include <iostream>
using namespace std;

ShellException::ShellException(const string &message) : message(message)
{
}

ShellException::~ShellException() throw()
{
}

const char *ShellException::what() const throw()
{
    return message.c_str();
}