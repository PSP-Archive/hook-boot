/*
 *HOOK BOOT debugging tool
 */

#include "common.h"

#define __VERBOSE__ 0

/////////////////////////////////////////////////////////////////////////////
// load file
/////////////////////////////////////////////////////////////////////////////
unsigned char *load_file(char *path,int *size)
{
	int fd;
	int file_size,read_size;
	unsigned char *buf;
	char *ptr;
	FILE_EMU_STATE state;

#if __VERBOSE__
	printf("load_file(%s,%08X)\n",path,(int)size);
#endif
	if(size) *size = 0;

	// get file size first
	file_size = get_file_size(path);

	// aloocate memory
	buf = (unsigned char *)malloc(file_size+1);
#if __VERBOSE__
	printf("malloc %08X\n",(int)buf);
#endif
	if(!buf)
	{
		printf("memory alocate error\n");
		return NULL;
	}

	// open file
	ptr=strchr(path,'@');
	if(ptr) *ptr = 0;     // separate ISO9660 fs
	fd = sceIoOpen(path,PSP_O_RDONLY , 0);
	if(ptr) *ptr++ = '@'; // resotre separater
#if __VERBOSE__
	printf("sceIoOpen %08X\n",fd);
#endif
	if(fd < 0) return NULL;

	if(ptr)
	{
#if __VERBOSE__
		printf("ISO9660 seek\n");
#endif
		state.fd    = fd;
		state.isofd = fd;
		if(iso9660_get_offset(ptr,&state))
		{
			sceIoLseek(fd,state.offset,PSP_SEEK_SET);
		}
	}

	// read file
	read_size = sceIoRead(fd,buf,file_size);
	buf[read_size] = 0x00; // EOF to text

#if __VERBOSE__
	printf("sceIoRead %08X\n",read_size);
#endif
	sceIoClose(fd);

	if(read_size != file_size)
	{
		free(buf);
		return NULL;
	}

	if(size) *size = file_size;

	// OK
	return buf;
}


/////////////////////////////////////////////////////////////////////////////
// get file size
/////////////////////////////////////////////////////////////////////////////
int get_file_size(char *path)
{
	int fd;
	int size;
	char *ptr;
	FILE_EMU_STATE state;

#if __VERBOSE__
	printf("get_file_size(%s)\n",path);
#endif
	// open file
	ptr=strchr(path,'@');
	if(ptr) *ptr = 0;     // separate ISO9660 fs
	fd = sceIoOpen(path,PSP_O_RDONLY , 0);
#if __VERBOSE__
	printf("sceIoOpen %08X\n",fd);
#endif
	if(ptr) *ptr++ = '@'; // resotre separater
	if(fd < 0) return 0;

	if(ptr)
	{
#if __VERBOSE__
		printf("ISO9660 search %s\n",ptr);
#endif
		// ISO9660
		state.fd    = fd;
		state.isofd = fd;
		if(iso9660_get_offset(ptr,&state))
			size = state.filesize;
		else
			size = 0;
	}
	else
	{
		// normal file
		size = sceIoLseek(fd,0,PSP_SEEK_END);
	}
	sceIoClose(fd);
#if __VERBOSE__
	printf("file size %08X\n",size);
#endif
	return size;
}

/////////////////////////////////////////////////////////////////////////////
// get directry path
/////////////////////////////////////////////////////////////////////////////
char *make_dir_path(char *dir_path,const char *full_path)
{
	char *ptr;

	strcpy(dir_path,full_path);
	ptr = strrchr(dir_path,'/');
	if(ptr) *ptr = 0;
	return dir_path;
}

/////////////////////////////////////////////////////////////////////////////
// make full path
/////////////////////////////////////////////////////////////////////////////
char *make_full_path(char *full_path,const char *root_dir,const char *path)
{
	int pos;
	char *ptr;

	strcpy(full_path,root_dir);

	if(strchr(path,':'))
	{
		strcpy(full_path,path);
		return full_path;
	}

	if(path[0]=='/')
	{
		ptr = strchr(root_dir,':');
		if(ptr)
			ptr[1]=0x00;
	}

	if(path[0]=='.' && path[1]=='/')
		path+=2;

	pos = strlen(full_path)-1;
	if(full_path[pos]!='/')
	{
		full_path[++pos] = '/';
		full_path[++pos] = 0;
	}
	strcat(full_path,path);
	return full_path;
}
