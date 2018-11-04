#include "Core.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>

#include "CommandLine.h"

using namespace std;

CommandLineOptions* CommandLine;

CommandLineOptions::CommandLineOptions( int argc, char** argv ) : App( argv[0] )
{
    for( int arg = 1; arg < argc; arg++ )
    {
        Cache.push_back( string( argv[arg] ) );
    }
}

CommandLineOptions::~CommandLineOptions()
{
    Cache.clear();
}

//

bool CommandLineOptions::IsOption( const string& option )
{
    return find( Cache.begin(), Cache.end(), "--" + option ) != Cache.end();
}

bool CommandLineOptions::IsOptionEmpty( const string& option )
{
    return GetStr( option ).empty();
}

//

string CommandLineOptions::Get()
{
    static const string empty;

    switch( Cache.size() )
    {
        case 0:
            return empty;
        case 1:
            return Cache[0];
        default:
            ostringstream os;
            copy( Cache.begin(), Cache.end() - 1, ostream_iterator<string>( os, " " ) );
            os << *Cache.rbegin();
            return os.str();
    }
}

int CommandLineOptions::GetInt( const string& option, const int& default_value, const unsigned char& base /* = 10 */ )
{
    int    result = default_value;
    string str = GetStr( option );

    if( !str.empty() )
    {
        const char* cstr = str.c_str();
        char*       end;
        long        l;
        errno = 0;
        l = strtol( cstr, &end, base );
        if( ( (errno == ERANGE && l == LONG_MAX) || l > INT_MAX ) ||
            ( (errno == ERANGE && l == LONG_MIN) || l < INT_MIN ) ||
            (*cstr == '\0' || *end != '\0') )
            result = default_value;
        else
            result = l;
    }

    return result;
}

string CommandLineOptions::GetStr( const string& option )
{
    static const std::string empty;

    return GetStr( option, empty );
}

string CommandLineOptions::GetStr( const string& option, const string& default_value )
{
    vector<string>::const_iterator it = find( Cache.begin(), Cache.end(), "--" + option );

    if( it != Cache.end() && ++it != Cache.end() )
    {
        string value = *it;

        if( value.size() >= 2 && value[0] == '-' && value[1] == '-' )
            return default_value;

        return value;
    }

    return default_value;
}
