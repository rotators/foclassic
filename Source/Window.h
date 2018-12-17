#ifndef __WINDOW__
#define __WINDOW__

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
# include "FL/Fl_Window.H"

# include "Types.h"

# ifdef FO_MACOSX
#  define fl_display    glXGetCurrentDisplay()
#  define fl_window     ( (uint)fl_xid( MainWindow ) )
# endif

class FOWindow : public Fl_Window
{
public:
    FOWindow();
    virtual ~FOWindow() {}
    virtual int handle( int event );
    bool   Focused;
    IntVec KeyboardEvents;
    StrVec KeyboardEventsText;
    IntVec MouseEvents;
};
extern FOWindow* MainWindow; // Initialized and handled in MainClient.cpp / MainMapper.cpp

#endif // FOCLASSIC_CLIENT || FOCLASSIC_MAPPER

extern const char* GetWindowName();

#endif // __WINDOW__ //
