#ifndef __PATH_H__
#define __PATH_H__

extern char path_list[PATH_HEAP_SIZE];

void init_path_list(void);
int add_path_list(const char *path);

#endif
