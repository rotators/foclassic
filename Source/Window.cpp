#include "FL/Fl.H"
#ifdef FO_WINDOWS
# include "FL/win32.H"
#endif
#include "FL/x.H"

#include "Core.h"

#include "GameOptions.h"
#include "Window.h"

#pragma MESSAGE("Better solution for GetWindowName")
extern const char* GetWindowName();

FOWindow::FOWindow() : Fl_Window( 0, 0, "" ), Focused( true )
{
    label( GetWindowName() );
    position( (Fl::w() - MODE_WIDTH) / 2, (Fl::h() - MODE_HEIGHT) / 2 );
    size( MODE_WIDTH, MODE_HEIGHT );

    // Icon
    #ifdef FO_WINDOWS
    icon( (char*)LoadIcon( fl_display, MAKEINTRESOURCE( 101 ) ) );
    #else
    // Todo: Linux
    #endif

    // OpenGL parameters
    #ifndef FO_D3D
    Fl::gl_visual( FL_RGB | FL_RGB8 | FL_DOUBLE | FL_DEPTH | FL_STENCIL );
    #endif

    // Fullscreen
    if( GameOpt.FullScreen )
        fullscreen();

    // Show window
    show();
    make_current();

    // Hide cursor
    #ifdef FO_WINDOWS
    ShowCursor( FALSE );
    #else
    char   data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    XColor black;
    black.red = black.green = black.blue = 0;
    Pixmap nodata = XCreateBitmapFromData( fl_display, fl_xid( this ), data, 8, 8 );
    Cursor cur = XCreatePixmapCursor( fl_display, nodata, nodata, &black, &black, 0, 0 );
    XDefineCursor( fl_display, fl_xid( this ), cur );
    XFreeCursor( fl_display, cur );
    #endif

    // Hide menu
    #ifdef FO_WINDOWS
    SetWindowLong( fl_xid( this ), GWL_STYLE, GetWindowLong( fl_xid( this ), GWL_STYLE ) & (~WS_SYSMENU) );
    #endif

    // Place on top
    #ifdef FO_WINDOWS
    if( GameOpt.AlwaysOnTop )
        SetWindowPos( fl_xid( this ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
    #endif
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
