/****************************************************************************
	HOOK BOOT path list handling
****************************************************************************/

#include "common.h"

#define __VERBOSE__ 1

// path reprace buffer
char path_list[PATH_HEAP_SIZE];
static path_list_pos;

void init_path_list(void)
{
	path_list[0]  = 0; // EOF
	path_list_pos = 0;
}

int add_path_list(const char *path)
{
	int size = strlen(path)+1;

	// check heap full
	if(path_list_pos+size > (PATH_HEAP_SIZE-1) )
		return 0;

	// copy
	memcpy(&path_list[path_list_pos],path,size);
	path_list_pos += size;

	path_list[path_list_pos] = 0; // EOM

	return 1;
}

int make_path(char *dst,const char *path)
{
	return 1;
}
