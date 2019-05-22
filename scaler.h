#ifndef __SCALER_H
#define __SCALER_H

typedef struct {
	uint32_t actual_width;
	uint32_t actual_height;
	uint32_t virtual_width;
	uint32_t virtual_height;
	uint32_t *map_row;
	uint32_t *map_column;
}scaler_private_t;

bool scaler_init(uint32_t actual_width, uint32_t actual_height, uint32_t virtual_width, uint32_t virtual_height);
bool scaler_process(uint8_t *source, uint8_t **target, uint8_t components);
bool scaler_destroy(void);

#endif /* __SCALER_H */
