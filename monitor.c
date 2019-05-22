/**
 * @file monitor.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "monitor.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>

/*********************
 *      DEFINES
 *********************/
#define SDL_REFR_PERIOD     50  /*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int monitor_sdl_refr_thread(void * param);


/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
 static sdl_monitor_t monitor_data[1];
static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static volatile bool sdl_inited = false;
static volatile bool sdl_refr_qry = false;
static volatile bool sdl_quit_qry = false;

int quit_filter(void * userdata, SDL_Event * event);
static void monitor_sdl_clean_up(void);
static void monitor_sdl_init(void);
static void monitor_sdl_refr_core(void);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the monitor
 */
bool monitor_init(uint32_t width, uint32_t height, char *name)
{
	bool ret = false;
	monitor_data->hor_pixel = width;
	monitor_data->ver_pixel = height;
	monitor_data->name = name;
	monitor_data->framebuffer = (uint32_t *)malloc(width * height * sizeof(uint32_t));
	if( monitor_data->framebuffer ) {
		ret = true;
		memset(monitor_data->framebuffer, 0x00, width * height * sizeof(uint32_t));
	} else 
		return ret;

    SDL_CreateThread(monitor_sdl_refr_thread, "sdl_refr", NULL);
    while(sdl_inited == false); /*Wait until 'sdl_refr' initializes the SDL*/
	return ret;
}

uint32_t monitor_hor_pixel(void) {
	return monitor_data->hor_pixel;
}

uint32_t monitor_ver_pixel(void) {
	return monitor_data->ver_pixel;
}

void monitor_flush_ready (void)
{
	sdl_refr_qry = true;
}

/**
 * Flush a buffer to the display. Calls 'monitor_flush_ready()' when finished
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p array of colors to be flushed
 */
void monitor_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint32_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0 || y2 < 0 || x1 > monitor_data->hor_pixel - 1 || y1 > monitor_data->ver_pixel - 1) {
        monitor_flush_ready();
        return;
    }

    int32_t y;
    uint32_t w = x2 - x1 + 1;
    for(y = y1; y <= y2; y++) {
        memcpy(&monitor_data->framebuffer[y * monitor_data->hor_pixel + x1], color_p, w * sizeof(uint32_t));
        color_p += w;
    }

    sdl_refr_qry = true;

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    monitor_flush_ready();
}

/**
 * Flush a buffer to the display. Calls 'monitor_flush_ready()' when finished
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p array of colors to be flushed
 */
void monitor_flush_without_alpha(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint8_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0 || y2 < 0 || x1 > monitor_data->hor_pixel - 1 || y1 > monitor_data->ver_pixel - 1) {
        monitor_flush_ready();
        return;
    }

    int32_t y;
	uint8_t *pixel;
    uint32_t w = x2 - x1 + 1;
    for(y = y1; y <= y2; y++) {
		for(int i=0;i<w;i++) {
			pixel = (uint8_t *)&monitor_data->framebuffer[y * monitor_data->hor_pixel + x1];
			pixel[4*i + 3] = 0xff; 
			pixel[4*i + 2] = color_p[3*i + 0]; 
			pixel[4*i + 1] = color_p[3*i + 1]; 
			pixel[4*i + 0] = color_p[3*i + 2]; 
		}	
        color_p += w*3;
    }

    sdl_refr_qry = true;

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    monitor_flush_ready();
}

/**
 * Fill out the marked area with a color
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color fill color
 */
void monitor_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > monitor_data->hor_pixel - 1) return;
    if(y1 > monitor_data->ver_pixel - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > monitor_data->hor_pixel - 1 ? monitor_data->hor_pixel - 1 : x2;
    int32_t act_y2 = y2 > monitor_data->ver_pixel - 1 ? monitor_data->ver_pixel - 1 : y2;

    int32_t x;
    int32_t y;
    // uint32_t color32 = color; // lv_color_to32(color);

    for(x = act_x1; x <= act_x2; x++) {
        for(y = act_y1; y <= act_y2; y++) {
            monitor_data->framebuffer[y * monitor_data->hor_pixel + x] = color;
        }
    }

    sdl_refr_qry = true;
}

/**
 * Put a color map to the marked area
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p an array of colors
 */
void monitor_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint32_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > monitor_data->hor_pixel - 1) return;
    if(y1 > monitor_data->ver_pixel - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > monitor_data->hor_pixel - 1 ? monitor_data->hor_pixel - 1 : x2;
    int32_t act_y2 = y2 > monitor_data->ver_pixel - 1 ? monitor_data->ver_pixel - 1 : y2;

    int32_t x;
    int32_t y;

    for(y = act_y1; y <= act_y2; y++) {
        for(x = act_x1; x <= act_x2; x++) {
            monitor_data->framebuffer[y * monitor_data->hor_pixel + x] = *color_p;
            color_p++;
        }

        color_p += x2 - act_x2;
    }

    sdl_refr_qry = true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * SDL main thread. All SDL related task have to be handled here!
 * It initializes SDL, handles drawing and the mouse.
 */

static int monitor_sdl_refr_thread(void * param)
{
    (void)param;

	monitor_sdl_init();
    /*Run until quit event not arrives*/
    while(sdl_quit_qry == false) {
        /*Refresh handling*/
        monitor_sdl_refr_core();
    }

    monitor_sdl_clean_up();
    exit(0);

    return 0;
}

int quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;

    if(event->type == SDL_QUIT) {
        sdl_quit_qry = true;
    }

    return 1;
}

static void monitor_sdl_clean_up(void)
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void monitor_sdl_init(void)
{
    /*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

    window = SDL_CreateWindow(monitor_data->name,
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              monitor_data->hor_pixel, monitor_data->ver_pixel, 0);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, monitor_data->hor_pixel, monitor_data->ver_pixel);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    /*Initialize the frame buffer to gray (77 is an empirical value) */
    memset(monitor_data->framebuffer, 77, monitor_data->hor_pixel * monitor_data->ver_pixel * sizeof(uint32_t));
    SDL_UpdateTexture(texture, NULL, monitor_data->framebuffer, monitor_data->hor_pixel * sizeof(uint32_t));
    sdl_refr_qry = true;
    sdl_inited = true;
}

static void monitor_sdl_refr_core(void)
{
    if(sdl_refr_qry != false) {
        sdl_refr_qry = false;
        SDL_UpdateTexture(texture, NULL, monitor_data->framebuffer, monitor_data->hor_pixel * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        /*Test: Draw a background to test transparent screens (LV_COLOR_SCREEN_TRANSP)*/
//        SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff);
//        SDL_Rect r;
//        r.x = 0; r.y = 0; r.w = monitor_data->hor_pixel; r.w = monitor_data->ver_pixel;
//        SDL_RenderDrawRect(renderer, &r);

        /*Update the renderer with the texture containing the rendered image*/
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
	
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if((&event)->type == SDL_WINDOWEVENT) {
            switch((&event)->window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    SDL_UpdateTexture(texture, NULL, monitor_data->framebuffer, monitor_data->hor_pixel * sizeof(uint32_t));
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    break;
                default:
                    break;
            }
        }
    }

    /*Sleep some time*/
    SDL_Delay(SDL_REFR_PERIOD);

}

