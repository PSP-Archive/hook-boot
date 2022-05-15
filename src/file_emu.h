#ifndef __FILE_EMU_H__
#define __FILE_EMU_H__

#define MAX_FD_CATCH 8

#define FD_MODE_UMDBLOCK  0x01
#define FD_MODE_ISO9660   0x02

typedef struct file_emu_state {
	int fd;
	int isofd;
	int mode;
	int secsize;
	int offset;
	int filesize;
	long long int result;
}FILE_EMU_STATE;

void init_file_emu(void);
FILE_EMU_STATE *file_emu_open(const char *file,int target_fd,int iso_fd);
void file_mode_close(int fd);
FILE_EMU_STATE *get_file_emu_state(int fd);

// iso9660.c
int iso9660_get_offset(const char *path, FILE_EMU_STATE *state);

#endif
