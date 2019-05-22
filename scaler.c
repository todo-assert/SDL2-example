
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <scaler.h>

static scaler_private_t scaler[1];

bool scaler_init(uint32_t actual_width, uint32_t actual_height, uint32_t virtual_width, uint32_t virtual_height)
{
    int i;
	scaler->actual_width = actual_width;
	scaler->actual_height = actual_height;
	scaler->virtual_width = virtual_width;
	scaler->virtual_height = virtual_height;
	
    scaler->map_row = (uint32_t *)malloc(sizeof(uint32_t) * virtual_width);
    if(NULL == scaler->map_row)
    {
        printf("make map_row error\n");
        return false;
    }
    for(i=0; i<virtual_width; i++)
    {
        scaler->map_row[i] = (i+0.4999999)*actual_width/virtual_width-0.5;
    }
    scaler->map_column = (uint32_t *)malloc(sizeof(uint32_t) * virtual_height);
    if(NULL == scaler->map_column)
    {
        printf("make map_column error\n");
        return false;
    }
    for(i=0; i<virtual_height; i++)
    {
        scaler->map_column[i] = (i+0.4999999)*actual_height/virtual_height-0.5;
    }
    return true;
}

#define CHECK_PRIVATE() do {\
		if( scaler->map_row == NULL || scaler->map_column == NULL ) { \
			printf("please check scaler_init\n"); \
			return false; \
		} \
	}while(0)

bool scaler_process(uint8_t *source, uint8_t **target, uint8_t components, bool direction)
{
	CHECK_PRIVATE();
	if( source == NULL ) {
		return false;
	}
	if( *target == NULL ) {
		*target = malloc(scaler->virtual_width*scaler->virtual_height*components);
	}
	
	for(int h=0;h<scaler->virtual_height;h++) {
		for(int w=0;w<scaler->virtual_width;w++) {
			for(int i=0;i<components;i++) {
				if( direction ) {
					(*target)[(h*scaler->virtual_width+w)*components + i] = source[(scaler->map_column[h] * scaler->actual_width  + scaler->map_row[w])*components + i];
				} else {
					(*target)[(w*scaler->virtual_height+h)*components + i] = source[(scaler->map_column[h] * scaler->actual_width  + scaler->map_row[w])*components + i];
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

