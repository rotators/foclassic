#include "Core.h"

uint GetDoubleClickTicks()
{
    #ifdef FO_WINDOWS
    return (uint)GetDoubleClickTime();
    #else
    // Todo: Linux
    return 500;
    #endif
}

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)

const uint8 SELECT_ALPHA = 100;

uint GetColorDay( int* day_time, uint8* colors, int game_time, int* light )
{
    uint8 result[3];
    int   color_r[4] = { colors[0], colors[1], colors[2], colors[3] };
    int   color_g[4] = { colors[4], colors[5], colors[6], colors[7] };
    int   color_b[4] = { colors[8], colors[9], colors[10], colors[11] };

    game_time %= 1440;
    int time, duration;
    if( game_time >= day_time[0] && game_time < day_time[1] )
    {
        time = 0;
        game_time -= day_time[0];
        duration = day_time[1] - day_time[0];
    }
    else if( game_time >= day_time[1] && game_time < day_time[2] )
    {
        time = 1;
        game_time -= day_time[1];
        duration = day_time[2] - day_time[1];
    }
    else if( game_time >= day_time[2] && game_time < day_time[3] )
    {
        time = 2;
        game_time -= day_time[2];
        duration = day_time[3] - day_time[2];
    }
    else
    {
        time = 3;
        if( game_time >= day_time[3] )
            game_time -= day_time[3];
        else
            game_time += 1440 - day_time[3];
        duration = (1440 - day_time[3]) + day_time[0];
    }

    if( !duration )
        duration = 1;
    result[0] = color_r[time] + (color_r[time < 3 ? time + 1 : 0] - color_r[time]) * game_time / duration;
    result[1] = color_g[time] + (color_g[time < 3 ? time + 1 : 0] - color_g[time]) * game_time / duration;
    result[2] = color_b[time] + (color_b[time < 3 ? time + 1 : 0] - color_b[time]) * game_time / duration;

    if( light )
    {
        int max_light = (MAX( MAX( MAX( color_r[0], color_r[1] ), color_r[2] ), color_r[3] ) +
                         MAX( MAX( MAX( color_g[0], color_g[1] ), color_g[2] ), color_g[3] ) +
                         MAX( MAX( MAX( color_b[0], color_b[1] ), color_b[2] ), color_b[3] ) ) / 3;
        int min_light = (MIN( MIN( MIN( color_r[0], color_r[1] ), color_r[2] ), color_r[3] ) +
                         MIN( MIN( MIN( color_g[0], color_g[1] ), color_g[2] ), color_g[3] ) +
                         MIN( MIN( MIN( color_b[0], color_b[1] ), color_b[2] ), color_b[3] ) ) / 3;
        int cur_light = (result[0] + result[1] + result[2]) / 3;
        *light = Procent( max_light - min_light, max_light - cur_light );
        *light = CLAMP( *light, 0, 100 );
    }

    return (result[0] << 16) | (result[1] << 8) | (result[2]);
}

#endif // FOCLASSIC_CLIENT || FOCLASSIC_MAPPER
