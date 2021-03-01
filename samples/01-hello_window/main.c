#define RAD2D_IMPL
#include <rad2d.h>

int r2d_init()
{
    return 0;
}

int r2d_shutdown()
{
    return 0;
}

int r2d_update( float delta )
{
    return 0;
}

int r2d_frame( float delta, r2d_frame_buffer_t *frame )
{
    r2d_rgba_t white = { 255, 255, 255, 255 };
    r2d_rgba_t magenta = { 255, 0, 255, 255 };

    r2d_rgba_t palette[] = {
        {0x40, 0x69, 0x78, 0x00},
        {0x49, 0xf3, 0x57, 0x00},
        {0x3e, 0xf2, 0xda, 0x00},
        {0x9e, 0xc0, 0xe9, 0x00},
        {0x99, 0xf9, 0xf7, 0x00},
    };
    
    r2d_draw_clear(frame, palette[3]);
    r2d_draw_point(frame, palette[4], 50, 50);
    r2d_draw_rect(frame, palette[1], 73, 12, 30, 45);
    return 0;
}