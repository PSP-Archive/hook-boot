/*
	ISO9660 file system
*/
#include "common.h"

#define __VERBOSE__ 0

#define SEC_SIZE 2048

#define MAX_DIREC_SIZE (33+64)

#define DIRREC_LEN_DR     0x00
#define DIRREC_EXT_LEN    0x01
#define DIRREC_EXTENT_LE  0x02
#define DIRREC_SIZE_LE    0x0A
#define DIRREC_DATE_TIME  0x12
#define DIRREC_FLAG       0x19
  #define DIRREC_FLAG_VAID     0x01
  #define DIRREC_FLAG_TYPE_DIR 0x02
  #define DIRREC_FLAG_DEPEND   0x03
  #define DIRREC_FLAG_RECOARD  0x04
  #define DIRREC_FLAG_PROTECT  0x08
  #define DIRREC_FLAG_EXTENT   0x80
#define DIRREC_UNIT_SIZE  0x1A
#define DIRREC_INTERLEAVE 0x1B
#define DIRREC_VOLUME_NUM_LE 0x1C
#define DIRREC_LEN_FI     0x20
#define DIRREC_NAME       0x21


#if 0
static unsigned int get_dword(unsigned char *ptr)
{
	return ptr[0] | (ptr[1]*0x100) | (ptr[2]*0x10000) | (ptr[3]*0x1000000);
}
#endif

// search one level of path
static int iso9660_get_directry_one(int fd,unsigned char *dir_rec,const char *path,int path_size)
{
	unsigned int fpos;
#if __VERBOSE__
log_printf("seatch %s\n",path);
#endif
	fpos = get_dword(dir_rec+DIRREC_EXTENT_LE)*SEC_SIZE;

	while(1)
	{
		// read directry recoard one
		sceIoLseek(fd,fpos,PSP_SEEK_SET);
		sceIoRead(fd,dir_rec,MAX_DIREC_SIZE);

		// EOF?
		if(dir_rec[DIRREC_LEN_DR]==0)
		{
#if __VERBOSE__
log_printf("EOF\n");
#endif
			return 0;
		}
		// check name
		if(!memcmp(&dir_rec[DIRREC_NAME],path,path_size))
		{
#if __VERBOSE__
log_printf("found one %s\n",path);
#endif
			return 1;
		}
		// next one
		fpos += dir_rec[DIRREC_LEN_DR];
	}
}

// search path
static int iso9660_search_path(int fd,unsigned char *dir_rec,const char *path)
{
	char *ptr;
	int flag;
	int len;

	// get root-directry
	sceIoLseek(fd,16*SEC_SIZE + 156,PSP_SEEK_SET);
	sceIoRead(fd,dir_rec,MAX_DIREC_SIZE);

	if(path[0]=='/') path++;

	while(path)
	{
		len = strlen(path);
		ptr = strchr(path,'/');
		if(ptr) len = ptr-path;

		if (iso9660_get_directry_one(fd,dir_rec,path,len)==0)
		{
			return 0;
		}
		if(ptr) ptr++;

		// file / directry check
		flag = dir_rec[DIRREC_FLAG] & DIRREC_FLAG_TYPE_DIR;
		int req_flag = ptr ? DIRREC_FLAG_TYPE_DIR : 0;
		if(flag != req_flag)
		{
#if __VERBOSE__
log_printf("flag error %02X != %02X\n",flag , req_flag);
#endif
			return 0;
		}

		// next level
		path = ptr;
	}
	// OK
	return 1;
}

int iso9660_get_offset(const char *path, FILE_EMU_STATE *state)
{
	unsigned char dir_rec[MAX_DIREC_SIZE];

#if __VERBOSE__
log_printf("---------------------------------------------\n");
log_printf("search %s\n",path);
#endif
	state->offset   = 0;
	state->filesize = 0;
	if(iso9660_search_path(state->isofd,dir_rec,path)==0)
		return 0;

	state->offset   = get_dword(dir_rec+DIRREC_EXTENT_LE)*SEC_SIZE;
	state->filesize = get_dword(dir_rec+DIRREC_SIZE_LE);

#if __VERBOSE__
log_printf("offset %08X , size %08X\n",state->offset,state->filesize);
log_printf("---------------------------------------------\n");
#endif

	return 1;
}

