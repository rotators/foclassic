#include "Core.h"
#include "App.h"

#if defined (FO_LINUX)
# include <unistd.h>
#endif

#include <cmath>

#include "CommandLine.h"
#include "FileSystem.h"
#include "GameOptions.h"

using namespace std;

static void StaticAsserts()
{
    STATIC_ASSERT( APP_TYPE_CLIENT == 0 );
    STATIC_ASSERT( APP_TYPE_MAPPER == APP_TYPE_CLIENT + 1 );
    STATIC_ASSERT( APP_TYPE_SERVER == APP_TYPE_MAPPER + 1 );
}

/////
//
// Math stuff
//
/////


int Procent( int full, int peace )
{
    if( !full )
        return 0;
    int procent = peace * 100 / full;

    return CLAMP( procent, 0, 100 );
}

uint NumericalNumber( uint num )
{
    if( num & 1 )
        return num * (num / 2 + 1);
    else
        return num * num / 2 + num / 2;
}

uint DistSqrt( int x1, int y1, int x2, int y2 )
{
    int dx = x1 - x2;
    int dy = y1 - y2;

    return (uint)sqrt( double(dx * dx + dy * dy) );
}

uint DistGame( int x1, int y1, int x2, int y2 )
{
    if( GameOpt.MapHexagonal )
    {
        int dx = (x1 > x2 ? x1 - x2 : x2 - x1);

        if( !(x1 & 1) )
        {
            if( y2 <= y1 )
            {
                int rx = y1 - y2 - dx / 2;

                return dx + (rx > 0 ? rx : 0);
            }
            else
            {
                int rx = y2 - y1 - (dx + 1) / 2;

                return dx + (rx > 0 ? rx : 0);
            }
        }
        else
        {
            if( y2 >= y1 )
            {
                int rx = y2 - y1 - dx / 2;

                return dx + (rx > 0 ? rx : 0);
            }
            else
            {
                int rx = y1 - y2 - (dx + 1) / 2;

                return dx + (rx > 0 ? rx : 0);
            }
        }
    }
    else
    {
        int dx = abs( x2 - x1 );
        int dy = abs( y2 - y1 );

        return MAX( dx, dy );
    }
}

int GetNearDir( int x1, int y1, int x2, int y2 )
{
    int dir = 0;

    if( GameOpt.MapHexagonal )
    {
        if( x1 & 1 )
        {
            if( x1 > x2 && y1 > y2 )
                dir = 0;
            else if( x1 > x2 && y1 == y2 )
                dir = 1;
            else if( x1 == x2 && y1 < y2 )
                dir = 2;
            else if( x1 < x2 && y1 == y2 )
                dir = 3;
            else if( x1 < x2 && y1 > y2 )
                dir = 4;
            else if( x1 == x2 && y1 > y2 )
                dir = 5;
        }
        else
        {
            if( x1 > x2 && y1 == y2 )
                dir = 0;
            else if( x1 > x2 && y1 < y2 )
                dir = 1;
            else if( x1 == x2 && y1 < y2 )
                dir = 2;
            else if( x1 < x2 && y1 < y2 )
                dir = 3;
            else if( x1 < x2 && y1 == y2 )
                dir = 4;
            else if( x1 == x2 && y1 > y2 )
                dir = 5;
        }
    }
    else
    {
        if( x1 > x2 && y1 == y2 )
            dir = 0;
        else if( x1 > x2 && y1 < y2 )
            dir = 1;
        else if( x1 == x2 && y1 < y2 )
            dir = 2;
        else if( x1 < x2 && y1 < y2 )
            dir = 3;
        else if( x1 < x2 && y1 == y2 )
            dir = 4;
        else if( x1 < x2 && y1 > y2 )
            dir = 5;
        else if( x1 == x2 && y1 > y2 )
            dir = 6;
        else if( x1 > x2 && y1 > y2 )
            dir = 7;
    }

    return dir;
}

int GetFarDir( int x1, int y1, int x2, int y2 )
{
    if( GameOpt.MapHexagonal )
    {
        float hx = (float)x1;
        float hy = (float)y1;
        float tx = (float)x2;
        float ty = (float)y2;
        float nx = 3 * (tx - hx);
        float ny = (ty - hy) * SQRT3T2_FLOAT - (float(x2 & 1) - float(x1 & 1) ) * SQRT3_FLOAT;
        float dir = 180.0f + RAD2DEG * atan2f( ny, nx );

        if( dir >= 60.0f  && dir < 120.0f )
            return 5;
        if( dir >= 120.0f && dir < 180.0f )
            return 4;
        if( dir >= 180.0f && dir < 240.0f )
            return 3;
        if( dir >= 240.0f && dir < 300.0f )
            return 2;
        if( dir >= 300.0f )
            return 1;
        return 0;
    }
    else
    {
        float dir = 180.0f + RAD2DEG * atan2( (float)(x2 - x1), (float)(y2 - y1) );

        if( dir >= 22.5f  && dir < 67.5f )
            return 7;
        if( dir >= 67.5f  && dir < 112.5f )
            return 0;
        if( dir >= 112.5f && dir < 157.5f )
            return 1;
        if( dir >= 157.5f && dir < 202.5f )
            return 2;
        if( dir >= 202.5f && dir < 247.5f )
            return 3;
        if( dir >= 247.5f && dir < 292.5f )
            return 4;
        if( dir >= 292.5f && dir < 337.5f )
            return 5;
        return 6;
    }
}

int GetFarDir( int x1, int y1, int x2, int y2, float offset )
{
    if( GameOpt.MapHexagonal )
    {
        float hx = (float)x1;
        float hy = (float)y1;
        float tx = (float)x2;
        float ty = (float)y2;
        float nx = 3 * (tx - hx);
        float ny = (ty - hy) * SQRT3T2_FLOAT - (float(x2 & 1) - float(x1 & 1) ) * SQRT3_FLOAT;
        float dir = 180.0f + RAD2DEG * atan2f( ny, nx ) + offset;
        if( dir < 0.0f )
            dir = 360.0f - fmod( -dir, 360.0f );
        else if( dir >= 360.0f )
            dir = fmod( dir, 360.0f );

        if( dir >= 60.0f  && dir < 120.0f )
            return 5;
        if( dir >= 120.0f && dir < 180.0f )
            return 4;
        if( dir >= 180.0f && dir < 240.0f )
            return 3;
        if( dir >= 240.0f && dir < 300.0f )
            return 2;
        if( dir >= 300.0f )
            return 1;
        return 0;
    }
    else
    {
        float dir = 180.0f + RAD2DEG * atan2( (float)(x2 - x1), (float)(y2 - y1) ) + offset;
        if( dir < 0.0f )
            dir = 360.0f - fmod( -dir, 360.0f );
        else if( dir >= 360.0f )
            dir = fmod( dir, 360.0f );

        if( dir >= 22.5f  && dir < 67.5f )
            return 7;
        if( dir >= 67.5f  && dir < 112.5f )
            return 0;
        if( dir >= 112.5f && dir < 157.5f )
            return 1;
        if( dir >= 157.5f && dir < 202.5f )
            return 2;
        if( dir >= 202.5f && dir < 247.5f )
            return 3;
        if( dir >= 247.5f && dir < 292.5f )
            return 4;
        if( dir >= 292.5f && dir < 337.5f )
            return 5;
        return 6;
    }
}

bool CheckDist( uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint dist )
{
    return DistGame( x1, y1, x2, y2 ) <= dist;
}

int ReverseDir( int dir )
{
    int dirs_count = DIRS_COUNT;

    return (dir + dirs_count / 2) % dirs_count;
}

void GetStepsXY( float& sx, float& sy, int x1, int y1, int x2, int y2 )
{
    float dx = (float)abs( x2 - x1 );
    float dy = (float)abs( y2 - y1 );

    sx = 1.0f;
    sy = 1.0f;

    dx < dy ? sx = dx / dy : sy = dy / dx;

    if( x2 < x1 )
        sx = -sx;
    if( y2 < y1 )
        sy = -sy;
}

void ChangeStepsXY( float& sx, float& sy, float deq )
{
    float rad = deq * PI_FLOAT / 180.0f;

    sx = sx * cos( rad ) - sy * sin( rad );
    sy = sx * sin( rad ) + sy * cos( rad );
}

bool MoveHexByDir( uint16& hx, uint16& hy, uint8 dir, uint16 maxhx, uint16 maxhy )
{
    int hx_ = hx;
    int hy_ = hy;

    MoveHexByDirUnsafe( hx_, hy_, dir );

    if( hx_ >= 0 && hx_ < maxhx && hy_ >= 0 && hy_ < maxhy )
    {
        hx = hx_;
        hy = hy_;

        return true;
    }

    return false;
}

void MoveHexByDirUnsafe( int& hx, int& hy, uint8 dir )
{
    if( GameOpt.MapHexagonal )
    {
        switch( dir )
        {
            case 0:
                hx--;
                if( !(hx & 1) )
                    hy--;
                break;
            case 1:
                hx--;
                if( hx & 1 )
                    hy++;
                break;
            case 2:
                hy++;
                break;
            case 3:
                hx++;
                if( hx & 1 )
                    hy++;
                break;
            case 4:
                hx++;
                if( !(hx & 1) )
                    hy--;
                break;
            case 5:
                hy--;
                break;
            default:
                return;
        }
    }
    else
    {
        switch( dir )
        {
            case 0:
                hx--;
                break;
            case 1:
                hx--;
                hy++;
                break;
            case 2:
                hy++;
                break;
            case 3:
                hx++;
                hy++;
                break;
            case 4:
                hx++;
                break;
            case 5:
                hx++;
                hy--;
                break;
            case 6:
                hy--;
                break;
            case 7:
                hx--;
                hy--;
                break;
            default:
                return;
        }
    }
}

bool IntersectCircleLine( int cx, int cy, int radius, int x1, int y1, int x2, int y2 )
{
    int x01 = x1 - cx;
    int y01 = y1 - cy;
    int x02 = x2 - cx;
    int y02 = y2 - cy;
    int dx = x02 - x01;
    int dy = y02 - y01;
    int a = dx * dx + dy * dy;
    int b = 2 * (x01 * dx + y01 * dy);
    int c = x01 * x01 + y01 * y01 - radius * radius;
    if( -b < 0 )
        return c < 0;
    if( -b < 2 * a )
        return 4 * a * c - b * b < 0;
    return a + b + c < 0;
}

/////
//
// Hex offset
//
/////

#define HEX_OFFSET_SIZE    ( (MAX_HEX_OFFSET * MAX_HEX_OFFSET / 2 + MAX_HEX_OFFSET / 2) * DIRS_COUNT )
int           CurHexOffset = 0; // 0 - none, 1 - hexagonal, 2 - square
static short* SXEven = NULL;
static short* SYEven = NULL;
static short* SXOdd = NULL;
static short* SYOdd = NULL;

void InitializeHexOffsets()
{
    SAFEDELA( SXEven );
    SAFEDELA( SYEven );
    SAFEDELA( SXOdd );
    SAFEDELA( SYOdd );

    if( GameOpt.MapHexagonal )
    {
        CurHexOffset = 1;
        SXEven = new short[HEX_OFFSET_SIZE];
        SYEven = new short[HEX_OFFSET_SIZE];
        SXOdd = new short[HEX_OFFSET_SIZE];
        SYOdd = new short[HEX_OFFSET_SIZE];

        int pos = 0;
        int xe = 0, ye = 0, xo = 1, yo = 0;
        for( int i = 0; i < MAX_HEX_OFFSET; i++ )
        {
            MoveHexByDirUnsafe( xe, ye, 0 );
            MoveHexByDirUnsafe( xo, yo, 0 );

            for( int j = 0; j < 6; j++ )
            {
                int dir = (j + 2) % 6;
                for( int k = 0; k < i + 1; k++ )
                {
                    SXEven[pos] = xe;
                    SYEven[pos] = ye;
                    SXOdd[pos] = xo - 1;
                    SYOdd[pos] = yo;
                    pos++;
                    MoveHexByDirUnsafe( xe, ye, dir );
                    MoveHexByDirUnsafe( xo, yo, dir );
                }
            }
        }
    }
    else
    {
        CurHexOffset = 2;
        SXEven = SXOdd = new short[HEX_OFFSET_SIZE];
        SYEven = SYOdd = new short[HEX_OFFSET_SIZE];

        int pos = 0;
        int hx = 0, hy = 0;
        for( int i = 0; i < MAX_HEX_OFFSET; i++ )
        {
            MoveHexByDirUnsafe( hx, hy, 0 );

            for( int j = 0; j < 5; j++ )
            {
                int dir = 0, steps = 0;
                switch( j )
                {
                    case 0:
                        dir = 2;
                        steps = i + 1;
                        break;
                    case 1:
                        dir = 4;
                        steps = (i + 1) * 2;
                        break;
                    case 2:
                        dir = 6;
                        steps = (i + 1) * 2;
                        break;
                    case 3:
                        dir = 0;
                        steps = (i + 1) * 2;
                        break;
                    case 4:
                        dir = 2;
                        steps = i + 1;
                        break;
                    default:
                        break;
                }

                for( int k = 0; k < steps; k++ )
                {
                    SXEven[pos] = hx;
                    SYEven[pos] = hy;
                    pos++;
                    MoveHexByDirUnsafe( hx, hy, dir );
                }
            }
        }
    }
}

void GetHexOffsets( bool odd, short*& sx, short*& sy )
{
    if( CurHexOffset != (GameOpt.MapHexagonal ? 1 : 2) )
        InitializeHexOffsets();
    sx = (odd ? SXOdd : SXEven);
    sy = (odd ? SYOdd : SYEven);
}

void GetHexInterval( int from_hx, int from_hy, int to_hx, int to_hy, int& x, int& y )
{
    if( GameOpt.MapHexagonal )
    {
        int dx = to_hx - from_hx;
        int dy = to_hy - from_hy;
        x = dy * (GameOpt.MapHexWidth / 2) - dx * GameOpt.MapHexWidth;
        y = dy * GameOpt.MapHexLineHeight;
        if( from_hx & 1 )
        {
            if( dx > 0 )
                dx++;
        }
        else if( dx < 0 )
            dx--;
        dx /= 2;
        x += (GameOpt.MapHexWidth / 2) * dx;
        y += GameOpt.MapHexLineHeight * dx;
    }
    else
    {
        int dx = to_hx - from_hx;
        int dy = to_hy - from_hy;
        x = (dy - dx) * GameOpt.MapHexWidth / 2;
        y = (dy + dx) * GameOpt.MapHexLineHeight;
    }
}

/////
//
// Preprocessor output formatting
//
/////

int FormatPreprocessorTabs( string& str, int cur_pos, int level ) // internal
{
    if( cur_pos < 0 || cur_pos >= (int)str.length() )
        return (int)str.size();

    int i = cur_pos;
    for( ; i < (int)str.length() - 1; i++ )
    {
        if( str[i] == '\n' )
        {
            int k = 0;
            if( str[i + 1] == '}' )
                k++;
            for( ; k < level; ++k )
            {
                i++;
                str.insert( str.begin() + i, '\t' );
            }
        }
        else if( str[i] == '{' )
        {
            i = FormatPreprocessorTabs( str, i + 1, level + 1 );
        }
        else if( str[i] == '}' )
        {
            return i;
        }
    }

    return i;
}

void FormatPreprocessorOutput( string& str )
{
    // Combine long line breaks
    for( int i = 0; i < (int)str.length() - 2; ++i )
        if( str[i] == '\n' && str[i + 1] == '\n' && str[i + 2] == '\n' )
            str[i] = ' ';

    // Add tabulations for {}
    for( int i = 0; i < (int)str.length() - 1; ++i )
        if( str[i] == '{' )
            i = FormatPreprocessorTabs( str, i + 1, 1 );
}

/////
//
// Misc
//
/////

string GetExecutableName( bool path /* = true */, bool extension /* = true */ )
{
    static string empty;
    string        executable = CommandLine->App;

    if( executable.empty() )
        return empty;

    if( !path )
    {
        replace( executable.begin(), executable.end(), '\\', '/' );
        size_t pos = executable.find_last_of( '/' );
        if( pos != string::npos )
            executable = executable.substr( pos + 1 );
    }

    if( !extension )
    {
        size_t pos = executable.find_last_of( '.' );
        if( pos != string::npos )
            executable = executable.substr( 0, pos );
    }

    return executable;
}

void RestoreMainDirectory()
{
    #ifdef FO_WINDOWS
    // Get executable file path
    char path[MAX_FOPATH] = { 0 };
    GetModuleFileName( GetModuleHandle( NULL ), path, MAX_FOPATH );

    // Cut off executable name
    int last = 0;
    for( int i = 0; path[i]; i++ )
        if( path[i] == DIR_SLASH_C )
            last = i;
    path[last + 1] = 0;

    // Set executable directory
    SetCurrentDirectory( path );
    #else
    // Read symlink to executable
    char buf[MAX_FOPATH];
    if( readlink( "/proc/self/exe", buf, MAX_FOPATH ) != -1 ||      // Linux
        readlink( "/proc/curproc/file", buf, MAX_FOPATH ) != -1 )   // FreeBSD
    {
        string            sbuf = buf;
        string::size_type pos = sbuf.find_last_of( DIR_SLASH_C );
        if( pos != string::npos )
        {
            buf[pos] = 0;
            chdir( buf );
        }
    }
    #endif
}

// Show dialog box with message
void ShowMessage( const char* message )
{
    #pragma TODO("ShowMessage() should use FLTK/SDL functions?")

    #ifdef FO_WINDOWS
    MessageBox( NULL, message, "FOClassic", MB_OK );
    #else
    // Todo: Linux
    #endif
}
