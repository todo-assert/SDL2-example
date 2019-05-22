#ifndef __FILE_H
#define __FILE_H



struct _file_detail_t;
typedef struct _file_detail_t{
	char *name;
	// ... any file information
	struct _file_detail_t *prev;
	struct _file_detail_t *next;
}file_detail_t;

typedef struct {
	uint32_t count;
	file_detail_t *list;
}file_t;

bool file_scan_folder(char *path, char *suffix);
file_t *file_get(void);

#endif /* __FILE_H */
