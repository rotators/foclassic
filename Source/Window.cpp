#include "Core.h"

#include "FL/Fl.H"
#include "FL/x.H"

#include "ConfigFile.h"
#include "GameOptions.h"
#include "Ini.h"
#include "SinglePlayer.h"
#include "Text.h"
#include "Window.h"

using namespace std;

#pragma TODO("Move to Text.cpp")
void ReplaceString( string& str, const string& from,     const string& to )
{
    size_t pos = 0;
    while( (pos = str.find( from, pos ) ) != string::npos )
    {
        str.replace( pos, from.length(), to );
        pos += to.length();
    }
}

const char* GetWindowName()
{
    // Default config names
    static char window_name[MAX_FOPATH] =
    {
        #if defined (FOCLASSIC_SERVER)
        "FOClassic Server\0--default-server-name--"
        #elif defined (FOCLASSIC_MAPPER)
        "FOClassic Mapper\0--default-mapper-name--"
        #else// FOCLASSIC_CLIENT and others
        "FOClassic\0--default-client-name--"
        #endif
    };

    static bool processed = false;
    if( !processed )
    {
        // Call once
        processed = true;

        // Take name from config file
        if( !ConfigFile )
            return window_name;

        // 'WindowName'
        string str = ConfigFile->GetStr( SECTION_MAIN, "WindowName" );
        if( str.empty() )
            return window_name;

        ReplaceString( str, "FOCLASSIC_STAGE", FOCLASSIC_STAGE_STRING );
        ReplaceString( str, "FOCLASSIC_VERSION", FOCLASSIC_VERSION_STRING );

        Str::Copy( window_name, str.c_str() );

        // Singleplayer appendix
        if( Singleplayer )
            Str::Append( window_name, " Singleplayer" );

    }

    return window_name;
}

#ifdef FOCLASSIC_SERVER
# pragma TODO("Integrate Server with FOWindow")
#endif

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
FOWindow::FOWindow() : Fl_Window( 0, 0, "" ), Focused( true )
{
    label( GetWindowName() );
    position( (Fl::w() - MODE_WIDTH) / 2, (Fl::h() - MODE_HEIGHT) / 2 );
    size( MODE_WIDTH, MODE_HEIGHT );

    // Icon
    # ifdef FO_WINDOWS
    icon( (char*)LoadIcon( fl_display, MAKEINTRESOURCE( 101 ) ) );
    # else
    // Todo: Linux
    # endif

    // OpenGL parameters
    # ifndef FO_D3D
    Fl::gl_visual( FL_RGB | FL_RGB8 | FL_DOUBLE | FL_DEPTH | FL_STENCIL );
    # endif

    // Fullscreen
    if( GameOpt.FullScreen )
        fullscreen();

    // Show window
    show();
    make_current();

    // Hide cursor
    # ifdef FO_WINDOWS
    ShowCursor( FALSE );
    # else
    char   data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    XColor black;
    black.red = black.green = black.blue = 0;
    Pixmap nodata = XCreateBitmapFromData( fl_display, fl_xid( this ), data, 8, 8 );
    Cursor cur = XCreatePixmapCursor( fl_display, nodata, nodata, &black, &black, 0, 0 );
    XDefineCursor( fl_display, fl_xid( this ), cur );
    XFreeCursor( fl_display, cur );
    # endif

    // Hide menu
    # ifdef FO_WINDOWS
    SetWindowLong( fl_xid( this ), GWL_STYLE, GetWindowLong( fl_xid( this ), GWL_STYLE ) & (~WS_SYSMENU) );
    # endif

    // Place on top
    # ifdef FO_WINDOWS
    if( GameOpt.AlwaysOnTop )
        SetWindowPos( fl_xid( this ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
    # endif
}

int FOWindow::handle( int event )
{
    // Keyboard
    if( event == FL_KEYDOWN || event == FL_KEYUP )
    {
        KeyboardEvents.push_back( event );
        KeyboardEvents.push_back( Fl::event_key() );
        KeyboardEventsText.push_back( Fl::event_text() );
        return 1;
    }
    else if( event == FL_PASTE )
    {
        KeyboardEvents.push_back( FL_KEYDOWN );
        KeyboardEvents.push_back( 0 );
        KeyboardEventsText.push_back( Fl::event_text() );
        KeyboardEvents.push_back( FL_KEYUP );
        KeyboardEvents.push_back( 0 );
        KeyboardEventsText.push_back( Fl::event_text() );
        return 1;
    }
    // Mouse
    else if( event == FL_PUSH || event == FL_RELEASE || (event == FL_MOUSEWHEEL && Fl::event_dy() != 0) )
    {
        MouseEvents.push_back( event );
        MouseEvents.push_back( Fl::event_button() );
        MouseEvents.push_back( Fl::event_dy() );
        return 1;
    }

    // Focus
    if( event == FL_FOCUS )
        Focused = true;
    if( event == FL_UNFOCUS )
        Focused = false;

    return 0;
}

#endif // FOCLASSIC_CLIENT || FOCLASSIC_MAPPER
