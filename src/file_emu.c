////////////////////////////////////////////////////////////////////
// ISO9660 & UMD BLOCK MODE emulation
////////////////////////////////////////////////////////////////////

#include "common.h"

static FILE_EMU_STATE emu_state[MAX_FD_CATCH];

static FILE_EMU_STATE *set_file_mode(FILE_EMU_STATE *state)
{
	int i;

	if(state->fd<0) return NULL;

	// delete old one
	for(i=0;i<MAX_FD_CATCH;i++)
	{
		if(emu_state[i].fd==state->fd)
		{
			emu_state[i].fd   = 0;
			emu_state[i].mode = 0;
		}
	}
	if(state->mode==0) return NULL;

	// new registry
	for(i=0;i<MAX_FD_CATCH;i++)
	{
		if( (emu_state[i].fd==0) || (emu_state[i].mode==0) )
		{
			emu_state[i].fd      = state->fd;
			emu_state[i].isofd   = state->isofd;
			emu_state[i].mode    = state->mode;
			emu_state[i].secsize = state->secsize;
			emu_state[i].offset  = state->offset;
			emu_state[i].filesize= state->filesize;
//			log_printf("EMU-MODE[M:%02X,S:%03X,O:%08X,L:%08X]:",state->mode,state->secsize,state->offset,state->filesize);
			return &emu_state[i];
		}
	}
	log_printf("error:OVERFLOW fd buffer\n");
	return NULL;
}

FILE_EMU_STATE *file_emu_open(const char *file,int target_fd,int iso_fd)
{
	FILE_EMU_STATE state;
	char *ptr;

	if(target_fd<0) return NULL;

	state.fd      = target_fd;
	state.isofd   = iso_fd;
	state.mode    = 0;
	state.secsize = 1;
	state.offset  = 0;
	state.filesize = 0x7fffffff;

	// check emu mode
	if( mode_umd_emu && is_str(file) && (ptr = strstr(file,".ISO")) )
	{
		ptr += 5;
		if(ptr[0])
		{
			// ISO9660 file mode : "fatms0:/PSP/GAME/[gamename]/UMD.ISO\x00[path_in_iso]"
			if(state.isofd==0)
			{
				state.isofd = sceIoOpen(file,PSP_O_RDONLY , 0);
//log_printf("ISO_FD open %08X\n",state.isofd);
			}
			//
			if(state.isofd>=0)
			{
				if( iso9660_get_offset(ptr,&state))
					state.mode = FD_MODE_ISO9660;

				// seek top of file in ISO
				sceIoLseek32(state.isofd,state.offset,PSP_SEEK_SET);
			}
		}
		else
		{
			// ISO9660 file mode : "fatms0:/PSP/GAME/[gamename]/UMD.ISO\x00\x00"
			state.secsize = 2048;
			state.mode = FD_MODE_UMDBLOCK;
		}
	}
	// normal access mode
	return set_file_mode(&state);
}

void file_mode_close(int fd)
{
	FILE_EMU_STATE *state;

	state = get_file_emu_state(fd);
	if(state)
	{
		if(state->fd != state->isofd)
		{
			sceIoClose(state->isofd);
			state->isofd = 0;
		}
		// clear
		state->fd      = 0;
		state->mode    = 0;
		state->secsize = 1;
		state->offset  = 0;
	}
}

FILE_EMU_STATE *get_file_emu_state(int fd)
{
	int i;
	FILE_EMU_STATE *state;

	for(i=0,state=emu_state;i<MAX_FD_CATCH;i++,state++)
	{
		if(state->fd==fd)
		{
			if(state->mode==0) break;
//log_printf("EMU-MODE[%02X]:",state->mode);
			return state;
		}
	}
	return NULL;
}


void init_file_emu(void)
{
	memset(emu_state,0,sizeof(emu_state));
}


