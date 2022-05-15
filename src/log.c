
#include "common.h"
#include <stdarg.h>

static int log_init = 0;
static int log_mode = 0;
static int log_fd = -1;
static char log_buf[512];

void log_set_device(int device)
{
	if( (~log_init) & device & LOG_DISP)
	{
		// pspDebugScreenInit();
		log_init |= LOG_DISP;
	}

	if( (~log_init) & device & LOG_FILE)
	{
		log_fd = sceIoOpen(OUT_SYSCALL_LOG_FILE,PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
		if(log_fd>=0)
		{
			sceIoClose(log_fd);
			log_init |= LOG_FILE;
		}
	}
	log_mode = device;
}

void log_printf(const char *text,...)
{
	va_list arg;

	if(log_mode==0) return;
	// setup message
	va_start(arg,text);
	vsprintf(log_buf,text,arg);
	va_end(arg);
	log_output();
}

void log_write(const char *data,int size)
{
	if(log_mode==0) return;
	memcpy(log_buf,data,size);
	log_buf[size]=0x00;
	log_output();
}

void log_output(void)
{
	int org_log_mode;

	// !!!!! must be use semaphore!!!!!
	if(log_mode==0) return;
	org_log_mode = log_mode;
	log_mode = 0;

	// to FILE
	if(org_log_mode & LOG_FILE)
	{
		log_fd = sceIoOpen(OUT_SYSCALL_LOG_FILE,PSP_O_WRONLY | PSP_O_APPEND, 0777);
		if(log_fd>=0)
		{
			sceIoWrite(log_fd,log_buf,strlen(log_buf));
			sceIoClose(log_fd);
		}
	}

	// to DISP
	if(org_log_mode & LOG_DISP)
		printf(log_buf);

	if(org_log_mode & LOG_DELAY)
		sceKernelDelayThread(100000);

	log_mode = org_log_mode;
}

#if 0
char kp_buf[2048] = {0};
int log_kprint_handler(int id,const char *format, u32 *args)
{
	sprintf(kp_buf+strlen(kp_buf),"ERR[%08X]:",id);
	vsprintf(kp_buf+strlen(kp_buf),format,args);
	return 0;
}
#endif

