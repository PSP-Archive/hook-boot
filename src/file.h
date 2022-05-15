#ifndef __FILE_H__
#define __FILE_H__

unsigned char *load_file(char *path,int *size);
int get_file_size(char *path);

char *make_dir_path(char *dir_path,const char *full_path);
char *make_full_path(char *full_path,const char *root_dir,const char *path);

#endif
