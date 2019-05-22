
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <if_libjpeg.h>
#include <jpeglib.h>
#include <monitor.h>

int main_decompress(char *filename)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * infile;
    int row_stride;
    unsigned char *buffer;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return -1;
    }
    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);
    printf("image_width = %d\n", cinfo.image_width);
    printf("image_height = %d\n", cinfo.image_height);
    printf("num_components = %d\n", cinfo.num_components);

    // printf("enter scale M/N:\n");
    // scanf("%d/%d", &cinfo.scale_num, &cinfo.scale_denom);
    // printf("scale to : %d/%d\n", cinfo.scale_num, cinfo.scale_denom);
	
    jpeg_start_decompress(&cinfo);

    printf("output_width = %d\n", cinfo.output_width);
    printf("output_height = %d\n", cinfo.output_height);
    printf("output_components = %d\n", cinfo.output_components);

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = malloc(row_stride);

    while (cinfo.output_scanline < cinfo.output_height) 
    {
        (void) jpeg_read_scanlines(&cinfo, &buffer, 1);
		monitor_flush_without_alpha(0, cinfo.output_scanline, cinfo.output_width, cinfo.output_scanline, &buffer[0]);
    }

    free(buffer);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return 0;
}
