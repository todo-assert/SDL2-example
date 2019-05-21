/**
 * @file monitor.h
 *
 */

#ifndef MONITOR_H
#define MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

#define USE_MONITOR 1
#define MONITOR_HOR_RES 640
#define MONITOR_VER_RES 480
#define LV_COLOR_DEPTH 32

#if USE_MONITOR

// #include "lvgl/lv_misc/uint32.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
	uint32_t hor_pixel;
	uint32_t ver_pixel;
	uint32_t *framebuffer;
	char *name;
}sdl_monitor_t;
 
/**********************
 * GLOBAL PROTOTYPES
 **********************/
bool monitor_init(uint32_t width, uint32_t height, char *name);
void monitor_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint32_t *color_p);
void monitor_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
void monitor_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const uint32_t * color_p);

/**********************
 *      MACROS
 **********************/

#endif /* USE_MONITOR */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MONITOR_H */
