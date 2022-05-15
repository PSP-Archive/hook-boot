#ifndef __LOG__
#define __LOG__

#define OUT_SYSCALL_LOG_FILE "fatms0:/LOG.txt"

// output devices
#define LOG_NONE  0x0000
#define LOG_DISP  0x0001
#define LOG_FILE  0x0002
#define LOG_DELAY 0x0080

#if 0
#define LOG_WLAN  0x0004 // future
#define LOG_IRDA  0x0008 // future
#define LOG_UART4 0x0010 // future
#endif

void log_set_device(int device);
void log_printf(const char *text,...);
void log_write(const char *data,int size);
void log_output(void);

int log_kprint_handler(int id,const char *format, u32 *args);

#endif
