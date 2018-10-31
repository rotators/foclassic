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

// This is modified version of IniPP library
// Original source https://github.com/mcmtroffaes/inipp/

#ifndef __INI__
#define __INI__

#include <istream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <scriptstring.h>

typedef std::map<std::string, std::string> IniSection;
typedef std::map<std::string, IniSection>  IniSections;

class Ini
{
protected:
    IniSections            Sections;
    std::list<std::string> Errors;

public:
    Ini();
    ~Ini();

    bool LoadFile( const std::string& fname );
    bool LoadString( const std::string& str );
    void Unload();

    void Parse( std::basic_istream<char>& is );
    void ParseString( const std::string& str );
    void ParseScriptString( const ScriptString& str );

    bool         IsSection( const std::string& section );
    bool         IsSectionKey( const std::string& section, const std::string& key );
    bool         IsSectionKeyEmpty( const std::string& section, const std::string& key );
    unsigned int GetSections( std::vector<std::string>& sections );
    unsigned int GetSectionKeys( const std::string& section, std::vector<std::string>& keys );

    void MergeSections( const std::string& to, const std::string& from, bool override = false );

    bool                     GetBool( const std::string& section, const std::string& key, const bool& default_value );
    int                      GetInt( const std::string& section, const std::string& key, const int& default_value, unsigned char base = 10 );
    int                      GetHex( const std::string& section, const std::string& key, const int& default_value );
    int                      GetOct( const std::string& section, const std::string& key, const int& default_value );
    std::string              GetStr( const std::string& section, const std::string& key );
    std::string              GetStr( const std::string& section, const std::string& key, const std::string& default_value );
    std::vector<std::string> GetStrVec( const std::string& section, const std::string& key, char separator = ' ' );
};

#endif // __INI__ //
