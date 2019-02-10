#ifndef __TIMER__
#define __TIMER__

#include "Types.h"

struct DateTime
{
    uint16 Year;         // 1601 .. 30827
    uint16 Month;        // 1 .. 12
    uint16 DayOfWeek;    // 0 .. 6
    uint16 Day;          // 1 .. 31
    uint16 Hour;         // 0 .. 23
    uint16 Minute;       // 0 .. 59
    uint16 Second;       // 0 .. 59
    uint16 Milliseconds; // 0 .. 999
};

namespace Timer
{
    void Init();
    void Finish();

    uint   FastTick();
    double AccurateTick();

    uint GameTick();
    void SetGamePause( bool pause );
    bool IsGamePaused();

    void StartAccelerator( int num );
    bool ProcessAccelerator( int num );
    int  GetAcceleratorNum();

    void GetCurrentDateTime( DateTime& dt );
    void DateTimeToFullTime( const DateTime& dt, uint64& ft );
    void FullTimeToDateTime( uint64 ft, DateTime& dt );
    int  GetTimeDifference( const DateTime& dt1, const DateTime& dt2 );
    void ContinueTime( DateTime& dt, int seconds );

    // Game time
    uint     GetFullSecond( uint16 year, uint16 month, uint16 day, uint16 hour, uint16 minute, uint16 second );
    DateTime GetGameTime( uint full_second );
    uint     GameTimeMonthDay( uint16 year, uint16 month );
    void     ProcessGameTime();
};

#endif // __TIMER__
