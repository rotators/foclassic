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

// This is modified version of IniPP library
// Original source https://github.com/mcmtroffaes/inipp/

// #ifdef FOCLASSIC_ENGINE
// # include "Core.h"
// #endif

#include <errno.h>

#include <algorithm>
#include <cstring>
#include <cctype>
#include <fstream>
#include <functional>
#include <istream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <scriptstring.h>

#include "Ini.h"
#include "../Log.h"

using namespace std;

#define SECTION_START      '['
#define SECTION_END        ']'
#define KEY_ASSIGN         '='
#define IS_COMMENT( c )    (c == ';' || c == '#')

inline void TrimLeft( string& str )
{
    str.erase( str.begin(), find_if( str.begin(), str.end(), not1( ptr_fun<int, int>( isspace ) ) ) );
}

inline void TrimRight( string& str )
{
    str.erase( find_if( str.rbegin(), str.rend(), not1( ptr_fun<int, int>( isspace ) ) ).base(), str.end() );
}

inline void Cleanup( string& str )
{
    str.erase( remove( str.begin(), str.end(), '\r' ), str.end() );
    str.erase( remove( str.begin(), str.end(), '\n' ), str.end() );
    // TODO '\t' -> ' '
}

//

Ini::Ini()
{}

Ini::~Ini()
{
    Unload();
}

//

bool Ini::LoadFile( const string& fname )
{
    Unload();

    ifstream fstream( fname );
    if( fstream.is_open() )
    {
        Parse( fstream );

        return true;
    }

    return false;
}

bool Ini::LoadString( const string& str )
{
    Unload();
    ParseString( str );

    return true;
}

void Ini::Unload()
{
    Sections.clear();
    Errors.clear();
}

//

void Ini::Parse( basic_istream<char>& stream )
{
    string line;
    string section;

    while( !stream.eof() )
    {
        getline( stream, line );

        Cleanup( line );

        TrimLeft( line );
        TrimRight( line );

        const size_t length = line.length();

        if( !length || line.empty() )
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
        else if( pos != 0 && pos != string::npos )
        {
            string var( line.substr( 0, pos ) );
            string val( line.substr( pos + 1, length ) );

            TrimRight( var );
            TrimLeft( val );

            IniSection& ini_section = Sections[section];
            if( ini_section.find( var ) == ini_section.end() )
            {
                ini_section.insert( make_pair( var, val ) );
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

void Ini::ParseString( const string& str )
{
    stringstream buf;
    buf << str;

    Parse( buf );
}

void Ini::ParseScriptString( const ScriptString& str )
{
    ParseString( str.c_std_str() );
}

//

bool Ini::IsSection( const string& section )
{
    return Sections.find( section ) != Sections.end();
}

bool Ini::IsSectionKey( const string& section, const string& key )
{
    if( !IsSection( section ) )
        return false;

    return Sections[section].find( key ) != Sections[section].end();
}

bool Ini::IsSectionKeyEmpty( const string& section, const string& key )
{
    if( !IsSectionKey( section, key ) )
        return true;

    return Sections[section][key].empty();
}

unsigned int Ini::GetSections( vector<string>& sections )
{
    unsigned int count = 0;
    for( IniSections::iterator it = Sections.begin(); it != Sections.end(); ++it, count++ )
    {
        sections.push_back( it->first );
    }

    return count;
}

unsigned int Ini::GetSectionKeys( const string& section, vector<string>& keys )
{
    unsigned int count = 0;

    if( IsSection( section ) )
    {
        for( IniSection::iterator it = Sections[section].begin(); it != Sections[section].end(); ++it, count++ )
        {
            keys.push_back( it->first );
        }
    }

    return count;
}

//

void Ini::MergeSections( const string& to, const string& from, bool override /* = false */ )
{
    #if 0
    if( from == to || !IsSection( to ) || !IsSection( from ) )
        return;

    IniSection& to_section = Sections[to];
    IniSection& from_section = Sections[from];
    #endif
}

//

bool Ini::GetBool( const string& section, const string& key, const bool& default_value )
{
    bool result = default_value;

    if( !IsSectionKeyEmpty( section, key ) )
    {
        string str = Sections[section][key];
        transform( str.begin(), str.end(), str.begin(), tolower );

        result = (str == "yes" || str == "true" || str == "on" || str == "enable");
    }

    return result;
}

int Ini::GetInt( const string& section, const string& key, const int& default_value )
{
    int result = default_value;

    if( !IsSectionKeyEmpty( section, key ) )
    {
        string str = Sections[section][key];

        // https://stackoverflow.com/a/6154614
        const char* cstr = str.c_str();
        char*       end;
        long        l;
        errno = 0;
        l = strtol( cstr, &end, 0 );
        if( ( (errno == ERANGE && l == LONG_MAX) || l > INT_MAX ) ||
            ( (errno == ERANGE && l == LONG_MIN) || l < INT_MIN ) ||
            (*cstr == '\0' || *end != '\0') )
            result = default_value;
        else
            result = l;
    }

    return result;
}

string Ini::GetStr( const string& section, const string& key )
{
    if( !IsSectionKeyEmpty( section, key ) )
        return string( Sections[section][key] );

    return string();
}

string Ini::GetStr( const string& section, const string& key, const string& default_value )
{
    if( !IsSectionKeyEmpty( section, key ) )
        return string( Sections[section][key] );

    return string( default_value );
}

vector<string> Ini::GetStrVec( const string& section, const string& key, char separator /* = ' ' */ )
{
    string         value = GetStr( section, key );
    vector<string> result;

    if( !value.empty() )
    {
        string        tmp;
        istringstream f( value );
        while( getline( f, tmp, separator ) )
        {
            Cleanup( tmp );
            if( separator != ' ' )
            {
                TrimLeft( tmp );
                TrimRight( tmp );
            }
            result.push_back( tmp );
        }
    }

    return result;
}
