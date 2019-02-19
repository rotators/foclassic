#ifndef __COMMAND__
#define __COMMAND__

#include <string>

class Command
{
public:
    Command() {}
    virtual ~Command() {}

    virtual std::string UsageArguments() = 0;
    virtual std::string UsageDescription() = 0;

    virtual int Run() = 0;
};

#endif // __COMMAND__ //
