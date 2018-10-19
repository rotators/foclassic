#ifndef __DYNAMIC_LIBRARY__
#define __DYNAMIC_LIBRARY__

#include "Types.h"

#ifdef FO_WINDOWS
# define DLL_Load( name )              (void*)LoadLibrary( name )
# define DLL_Free( h )                 FreeLibrary( (HMODULE)h )
# define DLL_GetAddress( h, pname )    (size_t*)GetProcAddress( (HMODULE)h, pname )
# define DLL_Error()                   Str::ItoA( GetLastError() )
#else
# include <dlfcn.h>
# define DLL_Load( name )              (void*)dlopen( name, RTLD_NOW | RTLD_LOCAL )
# define DLL_Free( h )                 dlclose( h )
# define DLL_GetAddress( h, pname )    (size_t*)dlsym( h, pname )
# define DLL_Error()                   dlerror()
#endif

#endif // __DYNAMIC_LIBRARY__ //
