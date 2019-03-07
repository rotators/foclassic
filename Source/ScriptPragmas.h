#ifndef __SCRIPT_PRAGMAS__
#define __SCRIPT_PRAGMAS__

#include <preprocessor.h>

#include "Types.h"

class IgnorePragma;
class GlobalVarPragma;
class CrDataPragma;
class BindFuncPragma;
class BindFieldPragma;

class ScriptPragmaCallback : public Preprocessor::Pragma::Callback
{
private:
    uint8            appType;
    StrSet           alreadyProcessed;
    IgnorePragma*    ignorePragma;
    GlobalVarPragma* globalVarPragma;
    CrDataPragma*    crDataPragma;
    BindFuncPragma*  bindFuncPragma;
    BindFieldPragma* bindFieldPragma;

public:
    ScriptPragmaCallback( uint8 app );
    void CallPragma( const std::string& name, const Preprocessor::Pragma::Instance& instance );
};

#endif // __SCRIPT_PRAGMAS__
