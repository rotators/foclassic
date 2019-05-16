#ifndef __EXTENSION__
#define __EXTENSION__

#include <string>
#include <unordered_map>

#include "Types.h"

namespace ExtensionEvent
{
    enum ExtensionEventId : uint
    {
        // after calling InitExtensions*()
        INIT = 0,

        // before calling FinishExtensions*()
        FINISH,

        // before initializing script system
        SCRIPT_INIT = 10,

        // before loading script modules
        // - script engine has been initialized and configured
        // - scripts config has been parsed and is available via ConfigFile*
        // - script preprocessor has been configured
        SCRIPT_LOAD_MODULES_START,

        // after loading script modules
        // - scripts has been loaded
        // - reserved functions has been binded
        SCRIPT_LOAD_MODULES_END,
    };
};

class Extension
{
private:
    static bool Initialized;

public:
    static std::unordered_map<std::string, Extension*> Map;
    static bool                                        RunEventDebug;

public:
    static void Init();
    static void Finish();

    static void RunEvent( const uint& event );

    ///////

    // copy prevention
public:
    Extension( const Extension& other );
    Extension& operator=( const Extension& other );

public:
    Extension();
    virtual ~Extension() {}

    virtual size_t GetFunctionAddress( const std::string& name );
    virtual void   Event( const uint& id );
};


#endif // __EXTENSION__ //
