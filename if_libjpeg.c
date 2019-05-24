
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <if_libjpeg.h>
#include <jpeglib.h>
#include <monitor.h>
#include <scaler.h>

int main_decompress(char *filename)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * infile;
    uint32_t full_stride;
    uint8_t *buffer, *line;
	uint32_t virtual_hor = monitor_hor_pixel();
	uint32_t virtual_ver = monitor_ver_pixel();

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return -1;
    }
    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);
    // printf("image_width = %d\n", cinfo.image_width);
    // printf("image_height = %d\n", cinfo.image_height);
    // printf("num_components = %d\n", cinfo.num_components);
    full_stride = cinfo.image_width * cinfo.image_height * cinfo.num_components;
    buffer = malloc(full_stride);
	if( buffer == NULL ) {
		cinfo.scale_denom = full_stride/(virtual_hor*virtual_ver*cinfo.num_components*2);
		cinfo.scale_num = 1;
	}

    // printf("enter scale M/N:\n");
    // scanf("%d/%d", &cinfo.scale_num, &cinfo.scale_denom);
    // printf("scale to : %d/%d\n", cinfo.scale_num, cinfo.scale_denom);
	
    jpeg_start_decompress(&cinfo);

    // printf("output_width = %d\n", cinfo.output_width);
    // printf("output_height = %d\n", cinfo.output_height);
    // printf("output_components = %d\n", cinfo.output_components);
	if( buffer == NULL ) {
		full_stride = cinfo.output_width * cinfo.output_height * cinfo.output_components;
		buffer = malloc(full_stride);
		if( buffer == NULL ) {
			return -1;
		}
	}

    while (cinfo.output_scanline < cinfo.output_height) 
    {
		line = &buffer[(cinfo.output_scanline)*cinfo.output_components*cinfo.output_width];
		(void) jpeg_read_scanlines(&cinfo, &line, 1);
    }
	// monitor_flush_without_alpha(0, 0, cinfo.output_width-1, cinfo.output_scanline-1, &buffer[0]);
	
	uint8_t *full_screen = NULL;
	bool direction = cinfo.output_width > cinfo.output_height ? (virtual_hor >= virtual_ver) : (virtual_hor <= virtual_ver);
	scaler_init(cinfo.output_width, cinfo.output_height, virtual_hor, virtual_ver, SCALER_AUTO_RATIO, direction?SCALER_ROTATE_0:SCALER_ROTATE_270);
	scaler_process(buffer, &full_screen, cinfo.output_components);
	int alpha = 0xff;
	if( cinfo.output_components == 4 ) {
		monitor_flush(0, 0, virtual_hor-1, virtual_ver-1, (const uint32_t *)&full_screen[0]);
	} else if( cinfo.output_components == 3 ) {
		monitor_flush_without_alpha(0, 0, virtual_hor-1, virtual_ver-1, &full_screen[0], alpha);
	}
	scaler_destroy();

    free(full_screen);
    free(buffer);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	
    return 0;
}
