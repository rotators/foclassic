#ifndef __SHOW_WORLD__
#define __SHOW_WORLD__

#include "Command.h"
#include "Types.h"

class ShowWorld : public Command
{
public:
    ShowWorld();

    virtual std::string UsageArguments() override;
    virtual std::string UsageDescription() override;

    virtual int Run() override;

protected:
    int Stop( int code = EXIT_FAILURE );

};

#endif // __SHOW_WORLD__ //
