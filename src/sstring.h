#ifndef __SSTRING__
#define __SSTRING__

//int str_cpy(const char *str1,const char *str2);
int str_cmp(const char *str1,const char *str2);
int str_cmp2(const char *str1,const char *str2);
char *str_chr(const char *str,int chr);

unsigned char *get_string(unsigned char *buf,char **name_str);
int str2num(const char **str_list, const char *str);
unsigned int str2hex(const char *str);

char *get_line_args(char *ptr,int *num_arg,char **arg);

int str_rep(char *path,const char *search,const char *rep);

int is_str(const char *str);

#endif
