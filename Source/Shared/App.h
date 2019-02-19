#ifndef __APP__
#define __APP__

#include "Types.h"

#define APP_TYPE_CLIENT      (0)
#define APP_TYPE_MAPPER      (1)
#define APP_TYPE_SERVER      (2)
#define APP_TYPE_COMPILER    (3)
#define APP_TYPE_UNKNOWN     (4)

#define APP_RENDER_DX        (0)
#define APP_RENDER_GL        (1)
#define APP_RENDER_NONE      (2)

#define APP_OS_LINUX         (0)
#define APP_OS_WINDOWS       (1)
#define APP_OS_UNKNOWN       (2)

struct AppData
{
    const uint8 Type;
    const uint8 Render;
    const uint8 OS;
    const uint8 Bits;

    void        (* WriteLog)( const char* frmt, ... );
    void        (* WriteLogF)( const char* func, const char* frmt, ... );
    void        (* WriteLogX)( const char* frmt, ... );

    AppData();
};

extern struct AppData App;

#endif // __APP__ //
