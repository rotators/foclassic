#ifndef __CMAKE_ANGELSCRIPT__
#define __CMAKE_ANGELSCRIPT__

// This file is updated automagically.
// Any and all hand-made changes will be lost.

#define ASRetCodeLen    (@AS_RETURN_CODES_LENGTH@)

static const char* ASRetCode[@AS_RETURN_CODES_LENGTH@] =
{
    @AS_RETURN_CODES@
};

#endif // __CMAKE_ANGELSCRIPT__ //
