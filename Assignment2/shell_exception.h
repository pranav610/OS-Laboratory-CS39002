#ifndef SHELL_EXCEPTION_H
#define SHELL_EXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class ShellException : public exception
{
public:
    ShellException(const string &);
    virtual ~ShellException() throw();
    virtual const char *what() const throw();

private:
    string message;
};

#endif
