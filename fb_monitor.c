
#include "fb_monitor.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/


/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static sdl_monitor_t monitor_data[1];
static volatile bool sdl_refr_qry = false;

static void monitor_sdl_clean_up(void);

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
	struct fb_var_screeninfo vinfo;
	
	monitor_data->fd = open(FBDEV_PATH, O_RDWR);
	if (ioctl(monitor_data->fd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Bad vscreeninfo ioctl\n");
		exit(2);
	}
	size_t fbsize = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/8);
	if ( (monitor_data->framebuffer = (uint32_t *)mmap(0, fbsize, PROT_READ | PROT_WRITE, MAP_SHARED, monitor_data->fd, 0)) == (void *) -1 ) {
		exit(3);
	}
	
	monitor_data->hor_pixel = vinfo.xres;
	monitor_data->ver_pixel = vinfo.yres;
	// monitor_data->name = name;
	// monitor_data->framebuffer = (uint32_t *)malloc(width * height * sizeof(uint32_t));
	if( monitor_data->framebuffer ) {
		ret = true;
		memset(monitor_data->framebuffer, 0x00, monitor_data->hor_pixel * monitor_data->ver_pixel * sizeof(uint32_t));
	} else {
		return ret;
	}

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
void monitor_flush_without_alpha(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint8_t * color_p, uint8_t alpha)
{
    /*Return if the area is out the screen*/
    if(x2 < 0 || y2 < 0 || x1 > monitor_data->hor_pixel - 1 || y1 > monitor_data->ver_pixel - 1) {
        monitor_flush_ready();
        return;
    }

    int32_t y;
    uint8_t *pixel;
    uint32_t w = x2 - x1 + 1;
    int i;
    for(y = y1; y <= y2; y++) {
		for(i=0;i<w;i++) {
			pixel = (uint8_t *)&monitor_data->framebuffer[y * monitor_data->hor_pixel + x1];
			pixel[4*i + 3] = alpha; 
			pixel[4*i + 2] = color_p[3*i + 0]; 
			pixel[4*i + 1] = color_p[3*i + 1]; 
			pixel[4*i + 0] = color_p[3*i + 2]; 
		}	
        color_p += w*3;
    }

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

static __attribute__((unused)) void monitor_sdl_clean_up(void)
{
	munmap(monitor_data->framebuffer, monitor_data->hor_pixel*monitor_data->ver_pixel*4);
	close(monitor_data->fd);
}
