#ifndef __SCALER_H
#define __SCALER_H

typedef struct {
	uint32_t actual_width;
	uint32_t actual_height;
	uint32_t virtual_width;
	uint32_t virtual_height;
	uint32_t display_width;
	uint32_t display_height;
	uint32_t direction;
	uint32_t *map_row;
	uint32_t *map_column;
}scaler_private_t;

#define SCALER_AUTO_RATIO 0
#define SCALER_FULL_SCREEN 1

#define SCALER_ROTATE_0 0
#define SCALER_ROTATE_90 1
#define SCALER_ROTATE_180 2
#define SCALER_ROTATE_270 3

bool scaler_init(uint32_t actual_width, uint32_t actual_height, uint32_t virtual_width, uint32_t virtual_height, uint32_t mode, uint32_t direction);
bool scaler_process(uint8_t *source, uint8_t **target, uint8_t components);
bool scaler_destroy(void);

#endif /* __SCALER_H */
