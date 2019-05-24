#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#ifdef CONFIG_PC_SIMULATOR
#include <SDL2/SDL.h>
#endif /* CONFIG_PC_SIMULATOR */
#include <monitor.h>
#include <if_libjpeg.h>
#include <file.h>

#define DISPLAY_HOR_PIXEL 480
#define DISPLAY_VER_PIXEL 272

int main (int argc, char **argv)
{
	monitor_init(DISPLAY_HOR_PIXEL, DISPLAY_VER_PIXEL, "SDL2-example");
	monitor_fill(0, 0, monitor_hor_pixel() - 1, monitor_ver_pixel() - 1, 0xffffffff);
	// main_decompress(argv[1]);
	file_scan_folder(argv[1], "JPG");
	// file_scan_folder(argv[1], "jpg");
	file_t const *file = file_get();
	file_detail_t const *list = file->list;
	while(list) {
		main_decompress(list->name);
		list = list->next;
		printf("%s >> %s\n", list->name, list->next->name);
		sleep(5);
	}
	while(1);
	return 0;
}
