#ifndef RAD2D_INCLUDED
#define RAD2D_INCLUDED (1)
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef RAD2D_MALLOC
#define RAD2D_MALLOC(x) malloc(x)
#endif

#ifndef RAD2D_FREE
#define RAD2D_FREE(x) free(x)
#endif

#ifndef RAD2D_REALLOC
#define RAD2D_REALLOC(x, y) realloc(x, y)
#endif

#ifdef RAD2D_IMPL
#ifndef RAD2D_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define RAD2D_PRIVATE __attribute__((unused)) static
    #else
        #define RAD2D_PRIVATE static
    #endif
#endif // RAD2D_PRIVATE

typedef struct {
    uint8_t r, g, b, a;
} r2d_rgba_t;

typedef enum {
    R2D_FORMAT_RGBA = 0,
    R2D_FORMAT_BGRA = 1,
    R2D_FORMAT_ABGR = 2,
    R2D_FORMAT_ARGB = 3,
    R2D_FORMAT_UNKOWN
} r2d_format_t;

typedef struct {
    uint32_t scale;
    uint32_t height;
    uint32_t width;
    r2d_format_t format;
    r2d_rgba_t *pixels;
} r2d_frame_buffer_t;

int r2d_init();
int r2d_shutdown();
int r2d_update( float delta );
int r2d_frame( float delta, r2d_frame_buffer_t *frame );

void r2d_draw_clear( r2d_frame_buffer_t *frame, r2d_rgba_t color );
void r2d_draw_point( r2d_frame_buffer_t *frame, r2d_rgba_t color, uint32_t x, uint32_t y );
void r2d_draw_line( r2d_frame_buffer_t *frame, r2d_rgba_t color, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2 );
void r2d_draw_rect( r2d_frame_buffer_t *frame, r2d_rgba_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height );

// todo(Wynter): for now we only support SDL2 backend, so enable it here
#define RAD2D_BACKEND_SDL2 (1)
#ifdef RAD2D_BACKEND_SDL2
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#endif // RAD2D_BACKEND_SDL2

RAD2D_PRIVATE FILE* rad2d_log_fp = NULL;

#if defined(_WIN32)
#include <windows.h>
INT WinMain(HINSTANCE instance, HINSTANCE previous_instance, PSTR command_line, INT command_show)
#else
int main(int argc, char** argv)
#endif
{

    SDL_SetMainReady();

    SDL_Window* sdl_window;
	SDL_Renderer* sdl_renderer;
	SDL_Texture* sdl_texture;

    r2d_frame_buffer_t frame = {0};
    frame.scale = 4;
    frame.width = 320;
    frame.height = 180;
    frame.pixels = RAD2D_MALLOC(sizeof(r2d_rgba_t) * frame.width * frame.height);

    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}

    r2d_init();

    sdl_window = SDL_CreateWindow( "Rad2D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, frame.width * frame.scale, frame.height * frame.scale, SDL_WINDOW_SHOWN );
    sdl_renderer = SDL_CreateRenderer( sdl_window, -1, SDL_RENDERER_ACCELERATED );
    sdl_texture = SDL_CreateTexture( sdl_renderer, SDL_GetWindowPixelFormat( sdl_window ), SDL_TEXTUREACCESS_STREAMING, frame.width, frame.height );

    SDL_RenderClear( sdl_renderer );
	SDL_RenderPresent( sdl_renderer );

    memset(frame.pixels, 0xFF, sizeof(r2d_rgba_t) * frame.width * frame.height);

    float last_time, current_time;
    last_time = last_time = ((float)SDL_GetTicks()/1000.0f);

    int r2d_run = 1;
    SDL_Event event;
	while(r2d_run)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT: r2d_run = 0;
                    break;
            }
        }

        current_time = ((float)SDL_GetTicks()/1000.0f);

        float time_delta = current_time - last_time;

        r2d_update( time_delta );

        uint32_t format = R2D_FORMAT_UNKOWN;
        SDL_QueryTexture( sdl_texture, &format, NULL, NULL, NULL);

        switch (format)
        {
        case SDL_PIXELFORMAT_BGRA8888:
            frame.format = R2D_FORMAT_RGBA;
            break;
        case SDL_PIXELFORMAT_RGB888:
        case SDL_PIXELFORMAT_RGBA8888:
            frame.format = R2D_FORMAT_ARGB;
            break;
        default:
            frame.format = R2D_FORMAT_UNKOWN;
            break;
        }
        
        r2d_frame( time_delta, &frame );

        void *texture_pixels = NULL;
        int pitch = 0;
        if(SDL_LockTexture( sdl_texture, NULL, &texture_pixels, &pitch )) {
            fprintf( stderr, "Could not unlock texture: %s\n", SDL_GetError() );
            return -1;
        }
        memcpy( texture_pixels, frame.pixels, pitch*frame.height );

        SDL_UnlockTexture( sdl_texture );
        SDL_RenderClear( sdl_renderer );
        SDL_RenderCopy( sdl_renderer, sdl_texture, NULL, NULL );
        SDL_RenderPresent( sdl_renderer );

        last_time = current_time;
	}
    
    r2d_shutdown();

    return 0;
}

void r2d_logf(const char* format, ...) {
    char fmt_buffer[256], buffer[2048];
    float time = ((float)SDL_GetTicks()/1000.0f);
    
    va_list args_list;
    va_start(args_list, format);

    #if defined(_WIN32)
    sprintf_s(fmt_buffer, sizeof(fmt_buffer), "[%0-12f] %s\n", time, format);
    vsprintf_s(buffer, sizeof(buffer), fmt_buffer, args_list);
    #else
    sprintf(fmt_buffer, "[%-12s] %s\n", module, format);
    vsprintf(buffer, fmt_buffer, args_list);
    #endif
    fprintf(rad2d_log_fp, "%s", buffer);
    fflush(rad2d_log_fp);
}

uint32_t r2d_color_convert( r2d_rgba_t color, r2d_format_t format )
{
    uint32_t output;

    switch(format)
    {
        case R2D_FORMAT_RGBA:
            output = *((uint32_t*) &color);
            break;
        case R2D_FORMAT_BGRA:
            output =  ((uint32_t)color.r << 24)
                    | ((uint32_t)color.g << 16)
                    | ((uint32_t)color.b << 8)
                    | (color.a);
            break;
        case R2D_FORMAT_ABGR:
            output =  ((uint32_t)color.r << 0)
                    | ((uint32_t)color.g << 8)
                    | ((uint32_t)color.b << 16)
                    | ((uint32_t)color.a << 24);
            break;
        case R2D_FORMAT_ARGB:
            output =  ((uint32_t)color.r << 16)
                    | ((uint32_t)color.g << 8)
                    | ((uint32_t)color.b << 0)
                    | ((uint32_t)color.a << 24);
            break;
        default:
            output = 0x00;
            break;
    }
    return output;
}
void r2d_draw_clear( r2d_frame_buffer_t *frame, r2d_rgba_t color )
{
    uint32_t* ptr = (uint32_t*) frame->pixels;
    uint32_t* end = (uint32_t*) &frame->pixels[frame->width * frame->height];
    uint32_t pixel_color = r2d_color_convert(color, frame->format);

    while(ptr != end) {
        *ptr++ = pixel_color;
    }
}

void r2d_draw_point( r2d_frame_buffer_t *frame, r2d_rgba_t color, uint32_t x, uint32_t y )
{
    uint32_t pixel_color = r2d_color_convert(color, frame->format);

    uint32_t *ptr = (uint32_t*) frame->pixels;

    if( (x >= 0 && x < frame->width) &&
        (y >= 0 && y < frame->width) )
    {
        uint32_t pitch = y * frame->width;
        ptr[pitch + x] = pixel_color;
    }
}

void r2d_draw_line( r2d_frame_buffer_t *frame, r2d_rgba_t color, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2 )
{

}

void r2d_draw_rect( r2d_frame_buffer_t *frame, r2d_rgba_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height )
{
    uint32_t x1 = x;
    uint32_t y1 = y;
    uint32_t w = width;
    uint32_t h = height;

    // no need to draw if the starting point is out of bounds    
    if(x > frame->width) return;
    if(y > frame->height) return;

    // clip origin to 0,0
    if(x < 0) x1 = 0;
    if(y < 0) y1 = 0;

    // clip bounds to width,height
    if((x1 + w) > frame->width) w = frame->width - x1;
    if((y1 + h) > frame->height) h = frame->height - y1;

    uint32_t* ptr = (uint32_t*) &frame->pixels[ y1 * frame->width ];

    uint32_t frame_offset = (frame->width - x1) + (w - frame->width);
    
    uint32_t pixel_color = r2d_color_convert(color, frame->format);

    uint32_t j = 0;
    while(j++ < h)
    {
        uint32_t i = 0;
        while(i++ < w)
        {
            ptr[x1 + i] = pixel_color;
        }
        ptr += frame->width;
    }
}

#endif // RAD2D_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // RAD2D_INCLUDED