#ifndef __SCRIPT_PRAGMAS__
#define __SCRIPT_PRAGMAS__

#include <preprocessor.h>

#include "Types.h"

#define PRAGMA_UNKNOWN    (0)
#define PRAGMA_CLIENT     (1)
#define PRAGMA_MAPPER     (2)
#define PRAGMA_SERVER     (3)

class IgnorePragma;
class GlobalVarPragma;
class CrDataPragma;
class BindFuncPragma;
class BindFieldPragma;

class ScriptPragmaCallback : public Preprocessor::Pragma::Callback
{
private:
    int              pragmaType;
    set<string>      alreadyProcessed;
    IgnorePragma*    ignorePragma;
    GlobalVarPragma* globalVarPragma;
    CrDataPragma*    crDataPragma;
    BindFuncPragma*  bindFuncPragma;
    BindFieldPragma* bindFieldPragma;

public:
    ScriptPragmaCallback( int pragma_type );
    void CallPragma( const string& name, const Preprocessor::Pragma::Instance& instance );
};

#endif // __SCRIPT_PRAGMAS__
