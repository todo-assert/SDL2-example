#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <monitor.h>

int main (int argc, char **argv)
{
	monitor_init(640, 480, "TFT Test");
	monitor_fill(0, 0, 639, 479, 0xffffffff);
	while(1);
	return 0;
}
