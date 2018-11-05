#ifndef __COMMAND_LINE__
#define __COMMAND_LINE__

#include <string>
#include <vector>

class CmdLine
{
protected:
    std::vector<std::string> Cache;

public:
    const std::string App;

    CmdLine( int argc, char** argv );
    ~CmdLine();

    std::string Get();

    bool IsOption( const std::string& option );
    bool IsOptionEmpty( const std::string& option );

    int                      GetInt( const std::string& option, const int& default_value, const unsigned char& base = 10 );
    std::string              GetStr( const std::string& option );
    std::string              GetStr( const std::string& option, const std::string& default_value );
    std::vector<std::string> GetStrVec( const std::string& option, char separator );
};

extern CmdLine* CommandLine;

#endif // __COMMAND_LINE__ //
