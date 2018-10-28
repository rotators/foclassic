/*
   MIT License

   Copyright (c) 2017-2018 Matthias C. M. Troffaes
   Copyright (c) 2018 Rotators

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

#ifndef __INI__
#define __INI__

#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <string>

#include <scriptstring.h>

typedef std::basic_string<char>         IniString;
typedef std::map<IniString, IniString>  IniSection;
typedef std::map<IniString, IniSection> IniSections;

class Ini
{
protected:
    IniSections          Sections;
    std::list<IniString> Errors;

public:
    Ini();
    ~Ini();

    bool LoadFile( const char* fname );
    void Unload();

    void Parse( std::basic_istream<char>& is );
    void ParseCharPtr( const char* str );
    void ParseScriptString( ScriptString& str );

    bool IsSection( const char* section );
    bool IsSectionKey( const char* section, const char* key );

    int         GetInt( const char* section, const char* key, const int default_value );
    std::string Ini::GetStr( const char* section, const char* key );
    std::string GetStr( const char* section, const char* key, const string& default_value );
};

#endif // __INI__ //
