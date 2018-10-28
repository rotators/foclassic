/*
   MIT License

   Copyright (c) 2017-2018 Matthias C. M. Troffaes

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

#ifdef FOCLASSIC_ENGINE
# include "Core.h"
#endif

#include <algorithm>
#include <cstring>
#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>

#include <scriptstring.h>

#include "Ini.h"

#define SECTION_START      '['
#define SECTION_END        ']'
#define KEY_ASSIGN         '='
#define IS_COMMENT( c )    (c == ';' || c == '#')

inline void TrimLeft( IniString& str )
{
    str.erase( str.begin(), std::find_if( str.begin(), str.end(), std::not1( std::ptr_fun<int, int>( std::isspace ) ) ) );
}

inline void TrimRight( IniString& str )
{
    str.erase( std::find_if( str.rbegin(), str.rend(), std::not1( std::ptr_fun<int, int>( std::isspace ) ) ).base(), str.end() );
}

template<typename T>
inline bool Extract( const IniString& value, T& result )
{
    char                           c;
    std::basic_istringstream<char> iss( value );
    T                              res;

    if( (iss >> std::boolalpha >> res) && !(iss >> c) )
    {
        result = res;
        return true;
    }
    else
    {
        return false;
    }
}

//

Ini::Ini()
{}

Ini::~Ini()
{
    Unload();
}

//

bool Ini::LoadFile( const char* fname )
{
    Unload();

    std::ifstream fstream( fname );
    if( fstream.is_open() )
    {
        Parse( fstream );

        return true;
    }

    return false;
}

void Ini::Unload()
{
    Sections.clear();
    Errors.clear();
}

//

void Ini::Parse( std::basic_istream<char>& stream )
{
    IniString line;
    IniString section;

    while( !stream.eof() )
    {
        std::getline( stream, line );
        TrimLeft( line );
        TrimRight( line );

        const size_t length = line.length();

        if( line.empty() )
            continue;

        const size_t pos = line.find_first_of( KEY_ASSIGN );
        const char&  front = line.front();

        if( IS_COMMENT( front ) )
        {
            continue;
        }
        else if( front == SECTION_START )
        {
            if( line.back() == SECTION_END )
                section = line.substr( 1, length - 2 );
            else
                Errors.push_back( line );
        }
        else if( pos != 0 && pos != IniString::npos )
        {
            IniString variable( line.substr( 0, pos ) );
            IniString value( line.substr( pos + 1, length ) );

            TrimRight( variable );
            TrimLeft( value );

            IniSection& sec = Sections[section];
            if( sec.find( variable ) == sec.end() )
            {
                sec.insert( std::make_pair( variable, value ) );
            }
            else
                Errors.push_back( line );
        }
        else
        {
            Errors.push_back( line );
        }
    }
}

void Ini::ParseCharPtr( const char* str )
{
    std::stringstream buf;
    buf << str;

    Parse( buf );
}

void Ini::ParseScriptString( ScriptString& str )
{
    std::stringstream buf;
    buf << str.c_std_str();

    Parse( buf );
}

//

bool Ini::IsSection( const char* section )
{
    return Sections.count( IniString( section ) ) > 0;
}

bool Ini::IsSectionKey( const char* section, const char* key )
{
    if( !IsSection( section ) )
        return false;

    return Sections[section].count( IniString( key ) ) > 0;
}

//

int Ini::GetInt( const char* section, const char* key, int default_value )
{
    int result;

    if( !IsSectionKey( section, key ) || !Extract( Sections[section][key], result ) )
        result = default_value;

    return result;
}

std::string Ini::GetStr( const char* section, const char* key )
{
    std::string result;

    if( !IsSectionKey( section, key ) || !Extract( Sections[section][key], result ) )
        return std::string();

    return result;
}

std::string Ini::GetStr( const char* section, const char* key, const string& default_value )
{
    std::string result;

    if( !IsSectionKey( section, key ) || !Extract( Sections[section][key], result ) )
        result = std::string( default_value );

    return result;
}
/*
   void Ini::Generate( std::basic_ostream<char>&os ) const
   {
                for( const auto& sec : Sections )
                {
                                os << CharSectionStart << sec.first << CharSectionEnd << std::endl;
                                for( auto const& val : sec.second )
                                {
                                                os << val.first << CharAssign << val.second << std::endl;
                                }
                }
   }
 */
