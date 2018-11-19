by cvet:

// Libevent workarounds
// Was bugged for Windows, need retest
#ifndef FO_WINDOWS
# define USE_LIBEVENT
// Linux don't want call write timeouts, need to know why and fix
# define LIBEVENT_TIMEOUTS_WORKAROUND
#endif
