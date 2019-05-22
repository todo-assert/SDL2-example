#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <monitor.h>
#include <if_libjpeg.h>

#define DISPLAY_HOR_PIXEL 640
#define DISPLAY_VER_PIXEL 480

int main (int argc, char **argv)
{
	monitor_init(DISPLAY_HOR_PIXEL, DISPLAY_VER_PIXEL, "SDL2-example");
	monitor_fill(0, 0, DISPLAY_HOR_PIXEL - 1, DISPLAY_VER_PIXEL - 1, 0xffffffff);
	main_decompress(argv[1]);
	while(1);
	return 0;
}
