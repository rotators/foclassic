#ifndef __EXTENSION__
#define __EXTENSION__

#include <string>
#include <unordered_map>

class asIScriptEngine;

class Extension
{
private:
    static bool Initialized;

public:
    static std::unordered_map<std::string, Extension*> Map;

public:
    static void Init();
    static void Finish();

    ///////

    // copy prevention
public:
    Extension( const Extension& other );
    Extension& operator=( const Extension& other );

public:
    Extension();
    virtual ~Extension() {}

    virtual size_t GetFunctionAddress( const std::string& name );
};


#endif // __EXTENSION__ //
