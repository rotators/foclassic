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
#include <map>
#include <string>
#include <vector>

#if defined (FOCLASSIC_ENGINE)
# include <scriptstring.h>
#endif

typedef std::map<std::string, std::string>              IniSection;
typedef std::map<std::string, IniSection>               IniSections;

typedef std::map<std::string, std::vector<std::string>> IniSectionsRaw;

class Ini
{
protected:
    std::string    LoadedFile;
    IniSections    Sections;
    IniSectionsRaw SectionsRaw;

public:
    bool KeepSectionsRaw;
    bool Lock;

    Ini();
    ~Ini();

    bool LoadFile( const std::string& fname, bool unload = true );
    bool LoadString( const std::string& str, bool unload = true );
    void Unload();

    void Parse( std::basic_istream<char>& is );
    void ParseString( const std::string& str );
    #if defined (FOCLASSIC_ENGINE)
    void ParseScriptString( const ScriptString& str );
    #endif

    std::string  GetLoadedFile();
    bool         IsSection( const std::string& section );
    bool         IsSectionKey( const std::string& section, const std::string& key );
    bool         IsSectionKeyEmpty( const std::string& section, const std::string& key );
    unsigned int GetSections( std::vector<std::string>& sections );
    unsigned int GetSectionKeys( const std::string& section, std::vector<std::string>& keys );

    bool MergeSections( const std::string& to, const std::string& from, bool overwrite = false );
    bool RemoveSection( const std::string& section );

    bool                     IsSectionRaw( const std::string& section );
    std::vector<std::string> GetSectionRaw( const std::string& section );
    std::string              GetSectionRawString( const std::string& section, const std::string& separator );
protected:
    void AddSectionRaw( const std::string& section, const std::string& line );

public:
    bool                     GetBool( const std::string& section, const std::string& key, const bool& default_value );
    int                      GetInt( const std::string& section, const std::string& key, const int& default_value, const unsigned char& base = 10 );
    int                      GetHex( const std::string& section, const std::string& key, const int& default_value );
    int                      GetOct( const std::string& section, const std::string& key, const int& default_value );
    std::string              GetStr( const std::string& section, const std::string& key );
    std::string              GetStr( const std::string& section, const std::string& key, const std::string& default_value );
    std::vector<std::string> GetStrVec( const std::string& section, const std::string& key, char separator = ' ' );

    void SetStr( const std::string& section, const std::string& key, std::string value );
};

#endif // __INI__ //
