#ifndef __DEVKIT401__
#define __DEVKIT401__

#include "dev401/psptypes.h"
#include "dev401/pspkerneltypes.h"

// 4.0.1
#define PSP_CTRL_CIRCLE CTRL_CIRCLE
#define PSP_CTRL_TRIANGLE CTRL_TRIANGLE
#define PSP_CTRL_CROSS CTRL_CROSS
#define PSP_CTRL_SQUARE CTRL_SQUARE
#define PSP_CTRL_START CTRL_START
#define PSP_CTRL_SELECT CTRL_SELECT
#define PSP_CTRL_NOTE 0x800000

/* SCE types. */
typedef unsigned char SceUChar8;
typedef short unsigned int SceUShort16;
typedef unsigned int SceUInt32;
typedef long long unsigned int SceUInt64;
typedef long long unsigned int SceULong64;

/** 64-bit system clock type. */
typedef struct SceKernelSysClock {
	SceUInt32   low;
	SceUInt32   hi;
} SceKernelSysClock;

/** Callback function prototype */
typedef int (*SceKernelCallbackFunction)(int arg1, int arg2, void *arg);

/** Structure to hold the status information for a callback */
struct _SceKernelCallbackInfo {
	/** Size of the structure (i.e. sizeof(PspCallbackStatus)) */
	SceSize 	size;
	/** The name given to the callback */
	char 	name[32];
	/** The thread id associated with the callback */
	SceUID 	threadId;
	/** Pointer to the callback function */
	SceKernelCallbackFunction 	callback;
	/** User supplied argument for the callback */
	void * 	common;
	/** Unknown */
	int 	notifyCount;
	/** Unknown */
	int 	notifyArg;
};

typedef struct _SceKernelCallbackInfo SceKernelCallbackInfo;

/**
  * Gets the status of a specified callback.
  *
  * @param cb - The UID of the callback to refer.
  * @param status - Pointer to a status structure. The size parameter should be
  * initialised before calling.
  *
  * @return < 0 on error.
  */
int sceKernelReferCallbackStatus(SceUID cb, SceKernelCallbackInfo *status);

/** Structure to contain the system status returned by ::sceKernelReferSystemStatus */
struct _SceKernelSystemStatus {
	/** Size of the structure (should be set prior to the call) */
	SceSize 	size;
	/** The status ? */
	SceUInt 	status;
	/** The number of cpu clocks in the idle thread */
	SceKernelSysClock 	idleClocks;
	/** Number of times we resumed from idle */
	SceUInt 	comesOutOfIdleCount;
	/** Number of thread context switches */
	SceUInt 	threadSwitchCount;
	/** Number of vfpu switches ? */
	SceUInt 	vfpuSwitchCount;
};

//////////////////////////////////////////////////////////////////////////////

typedef struct ScePspDateTime {
	unsigned short year;
	unsigned short month;
	unsigned short day;
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
	unsigned int   microsecond;
} ScePspDateTime;

typedef struct SceIoStat {
    SceMode             st_mode;        /* file type(file/dir) and mode(RWX) */
    unsigned int        st_attr;        /* device specific attribute */
    SceOff              st_size;        /* file size */
    ScePspDateTime      st_ctime;       /* time of file creation */
    ScePspDateTime      st_atime;       /* time of last access */
    ScePspDateTime      st_mtime;       /* time of last modification */
    unsigned int        st_private[6];  /* reserved */
} SceIoStat;

typedef struct SceIoDirent {
  	SceIoStat   d_stat;         /* file status */
	char        d_name[256];    /* file name (not full pathname) */
    void        *d_private;	    /* reserved */
    int         dummy;
} SceIoDirent;

#endif
