
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <scaler.h>

static scaler_private_t scaler[1];

bool scaler_init(uint32_t actual_width, uint32_t actual_height, uint32_t virtual_width, uint32_t virtual_height, uint32_t mode, uint32_t direction)
{
	int i;
	scaler->actual_width = actual_width;
	scaler->actual_height = actual_height;
	if( direction == SCALER_ROTATE_0 || direction == SCALER_ROTATE_180 ) {
		scaler->virtual_width = virtual_width;
		scaler->virtual_height = virtual_height;
	} else {
		scaler->virtual_width = virtual_height;
		scaler->virtual_height = virtual_width;
	}
	scaler->direction = direction;
	switch(mode) {
		case SCALER_AUTO_RATIO:
			scaler->display_width = scaler->virtual_height * scaler->actual_width / scaler->actual_height;
			if( scaler->display_width <= scaler->virtual_width) {
				scaler->display_height = scaler->virtual_height;
			} else {
				scaler->display_height = scaler->virtual_width * scaler->actual_height / scaler->actual_width;
				if( scaler->display_height <= scaler->virtual_height) {
					scaler->display_width = scaler->virtual_width;
				}
			}
		break;
		case SCALER_FULL_SCREEN:
			scaler->display_width = scaler->virtual_width;
			scaler->display_height = scaler->virtual_height;
		break;
		default:
			scaler->display_width = scaler->virtual_width;
			scaler->display_height = scaler->virtual_height;
		break;
	}
	
    scaler->map_row = (uint32_t *)malloc(sizeof(uint32_t) * scaler->display_width);
    if(NULL == scaler->map_row)
    {
        printf("make map_row error\n");
        return false;
    }
    for(i=0; i<scaler->display_width; i++)
    {
        scaler->map_row[i] = (i+0.4999999)*actual_width/scaler->display_width-0.5;
    }
    scaler->map_column = (uint32_t *)malloc(sizeof(uint32_t) * scaler->display_height);
    if(NULL == scaler->map_column)
    {
        printf("make map_column error\n");
        return false;
    }
    for(i=0; i<scaler->display_height; i++)
    {
        scaler->map_column[i] = (i+0.4999999)*actual_height/scaler->display_height-0.5;
    }
    return true;
}

#define CHECK_PRIVATE() do {\
		if( scaler->map_row == NULL || scaler->map_column == NULL ) { \
			printf("please check scaler_init\n"); \
			return false; \
		} \
	}while(0)

bool scaler_process(uint8_t *source, uint8_t **target, uint8_t components)
{
	CHECK_PRIVATE();
	if( source == NULL ) {
		return false;
	}
	if( *target == NULL ) {
		*target = malloc(scaler->virtual_width*scaler->virtual_height*components);
		if( *target == NULL ) {
			printf("error :%s:%d\n", __FILE__, __LINE__);
			return false;
		}
	}
	int h,w,i, offset, end, x, y;
	uint8_t *set;
	int32_t position;
	uint32_t size = scaler->actual_width * scaler->actual_height * components;
	if(scaler->display_width == scaler->virtual_width) {
		offset = scaler->virtual_height - scaler->display_height;
		offset /= 2;
		end = scaler->virtual_height - offset;
	} else {
		offset = scaler->virtual_width - scaler->display_width;
		offset /= 2;
		end = scaler->virtual_width - offset;
	}
	
	for(h=0;h<scaler->virtual_height;h++) {
		for(w=0;w<scaler->virtual_width;w++) {
			for(i=0;i<components;i++) {
				if( scaler->direction == SCALER_ROTATE_90 ) {
					set = &(*target)[(w*scaler->virtual_height+(scaler->virtual_height - h - 1))*components + i];
				} else if( scaler->direction == SCALER_ROTATE_0 )  {
					set = &(*target)[(h*scaler->virtual_width+w)*components + i];
				} else if( scaler->direction == SCALER_ROTATE_180 )  {
					set = &(*target)[((scaler->virtual_height - h - 1)*scaler->virtual_width+(scaler->virtual_width - w - 1))*components + i];
				} else /* if( scaler->direction == 3 ) */  {
					set = &(*target)[((scaler->virtual_width - w - 1)*scaler->virtual_height+h)*components + i];
				}
				if(scaler->display_width == scaler->virtual_width) {
					if( h >= end || h <= offset ) {
						*set = 0;
						continue;
					}
					x = h - offset;
					y = w;
				} else {
					if( w >= end || w <= offset ) {
						*set = 0;
						continue;
					}
					x = h;
					y = w - offset;
				}
				position = (scaler->map_column[x] * scaler->actual_width  + scaler->map_row[y])*components + i;
				if( position > 0 && position < size ) {
					*set = source[position];
				}
			}
		}
	}
	return true;
}

bool scaler_destroy(void)
{
	if( scaler->map_row == NULL ) {
		free(scaler->map_row);
		scaler->map_row = NULL;
	}
	if( scaler->map_row == NULL ) {
		free(scaler->map_column);
		scaler->map_column = NULL;
	}
	return true;
}

