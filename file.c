
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>
#include <file.h>

static file_t file[1];

bool file_add_list (char *name)
{
	file_detail_t *list = (file_detail_t *)malloc(sizeof(file_detail_t));
	
	list->name = name;
	if( file->list == NULL ) {
		file->list = list;
		list->prev = list->next = list;
		file->count = 1;
	} else {
		list->next = file->list;
		list->prev = file->list->prev;
		file->list->prev->next = list;
		file->list->prev = list;
		file->count ++;
	}
	
	return true;
}

bool file_check_list_exist(char *name)
{
	int i = 0;
	bool ret = false;
	file_detail_t const *list = file->list;
	for(i=0;i<file->count;i++) {
		if( strcmp(list->name, name) == 0 ) {
			ret = true;
			break;
		}
		list = list->next;
	}
	return ret;
}

bool file_scan_folder(char *path, char *suffix)
{
	bool ret = false;
	DIR* dp = NULL;
	struct dirent* dirp = NULL;
	dp = opendir( path );
	if ( dp == NULL ) {
		return ret;
	}
	char *filepath;
	struct stat fil_stat;
	while ( ( dirp = readdir( dp ) ) != NULL ) {
		if ( strcmp( dirp->d_name, "." ) == 0 || strcmp( dirp->d_name, ".." ) == 0 ) {
			continue;
		}
		if( strcasecmp(&dirp->d_name[strlen(dirp->d_name)-strlen(suffix)], suffix) ) {
			continue;
		}
		filepath = (char *)malloc(strlen(path) + strlen(dirp->d_name) + 1);
		memset(filepath, 0, strlen(path) + strlen(dirp->d_name) + 1);
		sprintf( filepath, "%s%s%s", path, path[strlen(path)-1] == '/' ? "":"/", dirp->d_name );
		printf(">>> %s\n", filepath);
		if ( stat( filepath, &fil_stat ) == -1 ) {
			free(filepath);
			continue;
		}
		if ( S_ISDIR( fil_stat.st_mode ) ) {
			free(filepath);
			continue;
		}
		if( file_check_list_exist(filepath) ) {
			free(filepath);
			continue;
		}
		file_add_list(filepath);
		// printf(">>> %s\n", filepath);
	}
	printf("file count = %d\n", file->count);
	closedir( dp );
	return ret;
}

file_t *file_get(void)
{
	return file;
}