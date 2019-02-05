#include "Core.h"

#include <stdlib.h>

#include "ScriptUtils.h"
#include "Text.h"

#pragma TODO("Generate ASRetCode automagically")
static const char* ASRetCode[] =
{
    "SUCCESS",                                          //  0
    "ERROR",                                            // -1
    "CONTEXT_ACTIVE",                                   // -2
    "CONTEXT_NOT_FINISHED",                             // -3
    "CONTEXT_NOT_PREPARED",                             // -4
    "INVALID_ARG",                                      // -5
    "NO_FUNCTION",                                      // -6
    "NOT_SUPPORTED",                                    // -7
    "INVALID_NAME",                                     // -8
    "NAME_TAKEN",                                       // -9
    "INVALID_DECLARATION",                              // -10
    "INVALID_OBJECT",                                   // -11
    "INVALID_TYPE",                                     // -12
    "ALREADY_REGISTERED",                               // -13
    "MULTIPLE_FUNCTIONS",                               // -14
    "NO_MODULE",                                        // -15
    "NO_GLOBAL_VAR",                                    // -16
    "INVALID_CONFIGURATION",                            // -17
    "INVALID_INTERFACE",                                // -18
    "CANT_BIND_ALL_FUNCTIONS",                          // -19
    "LOWER_ARRAY_DIMENSION_NOT_REGISTERED",             // -20
    "WRONG_CONFIG_GROUP",                               // -21
    "CONFIG_GROUP_IS_IN_USE",                           // -22
    "ILLEGAL_BEHAVIOUR_FOR_TYPE",                       // -23
    "WRONG_CALLING_CONV",                               // -24
    "BUILD_IN_PROGRESS",                                // -25
    "INIT_GLOBAL_VARS_FAILED",                          // -26
    "OUT_OF_MEMORY"                                     // -27
};

const char* Script::GetASReturnCode( int return_code )
{
    return Str::Duplicate( ASRetCode[abs( return_code )] );
}
