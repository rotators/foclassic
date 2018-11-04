#ifndef __COMMAND_LINE__
#define __COMMAND_LINE__

#include <string>
#include <vector>

class CommandLineOptions
{
protected:
    std::vector<std::string> Cache;

public:
    const std::string App;

    CommandLineOptions( int argc, char** argv );
    ~CommandLineOptions();

    std::string Get();

    bool IsOption( const std::string& option );
    bool IsOptionEmpty( const std::string& option );

    int         GetInt( const std::string& option, const int& default_value, const unsigned char& base = 10 );
    std::string GetStr( const std::string& option );
    std::string GetStr( const std::string& option, const std::string& default_value );
};

extern CommandLineOptions* CommandLine;

#endif // __COMMAND_LINE__ //
